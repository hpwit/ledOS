#include <SD.h>
#include "SPIFFS.h"
#include "FS.h"
#include <string>
#include <list>
#pragma once
#ifndef __LED_OS__FILESYSTEM
#define __LED_OS__FILESYSTEM
using namespace std;

typedef struct mount_point
{
    FS *fs;
    string name;
};

typedef struct _files
{
    string name;
    bool isDirectory;
    uint16_t file_size;
};

bool compare_nocase(const _files &first, const _files &second)
{
    unsigned int i = 0;
    while ((i < first.name.length()) && (i < second.name.length()))
    {
        if (tolower(first.name[i]) < tolower(second.name[i]))
            return true;
        else if (tolower(first.name[i]) > tolower(second.name[i]))
            return false;
        ++i;
    }
    return (first.name.length() < second.name.length());
}

class FileSystem
{
public:
    FileSystem()
    {
        mount_point spiff;
        if (SPIFFS.begin(true))
        {
            spiff.fs = &SPIFFS;
            spiff.name = "SPIFFS";
            mounts.push_back(spiff);
            current_mount = &mounts.back();
            current_path = "/";
        }
        else
        {
            printf("Unable to open SPIFFS\r\n");
        }
    }

    bool mkdir(string name)
    {
        result.clear();
        name = current_path + name;
        if (current_mount->fs->exists(name.c_str()))
        {

            string res = string_format("Impossible to create directory, %s already exists.", name.c_str());
            result.push_back(res);
            return false;
        }
        else
        {
            if (current_mount->fs->mkdir(name.c_str()))
            {

                string res = string_format("Directory %s created.\r\n", name.c_str());
                result.push_back(res);
                return true;
            }
            else
            {

                string res = string_format("Impossible to create directory %s.", name.c_str());
                result.push_back(res);
                return false;
            }
        }
    }

    bool ls(string opt)
    {
        result.clear();
        dirfiles.clear();
        if (current_path.compare("") == 0)
        {
            for (mount_point g : mounts)
            {
                result.push_back(string_format("%s", g.name.c_str()));
            }
            return true;
        }
        else
        {
            if (current_mount == NULL)
            {
                result.push_back("No Mount !!!");
                return false;
            }
            uint16_t nb_files, total_mem;
            File root = current_mount->fs->open(current_path.c_str());
            File file = root.openNextFile();
            _files f;
            while (file)
            {
                f.isDirectory = false;
                f.file_size = 0;
                if (file.isDirectory())
                {
                    f.isDirectory = true;
                }
                else
                {
                    f.file_size = file.size();
                }
                f.name = file.name();
                dirfiles.push_back(f);
                file = root.openNextFile();
            }
            dirfiles.sort(compare_nocase);

            for (_files fil : dirfiles)
            {
                if (fil.isDirectory)
                {
                    result.push_back(string_format("%s/", fil.name.c_str()));
                }
                else
                {
                    if (opt.compare("-l") == 0)
                    {
                        string f = string_format("%s%30s", fil.name.c_str(), "");
                        f = f.substr(0, 20);
                        f = string_format("%s %6d bytes", f.c_str(), fil.file_size);
                        result.push_back(f);
                    }
                    else if (opt.compare("w") == 0)
                    {
                        string f = fil.name.c_str();

                        result.push_back(f);
                    }
                    else
                    {
                        string f = string_format("%s%30s", fil.name.c_str(), "");
                        f = f.substr(0, 20);
                        result.push_back(f);
                    }

                    nb_files++;
                    total_mem += fil.file_size;
                }
                // file = root.openNextFile();
            }
            result.push_back(string_format("%d file(s) %d bytes", nb_files, total_mem));
            dirfiles.clear();
            return true;
        }
    }

    bool load(string name, string *buff)
    {
        result.clear();
        string _name = current_path + name;
        if (!current_mount->fs->exists(_name.c_str()))
        {
            result.push_back(string_format("File %s does not exists.", name.c_str()));
            return false;
        }
        else
        {
            // open file and read it and push it in the buffer
            File file = current_mount->fs->open(_name.c_str(), FILE_READ);
            if (!file)
            {
                result.push_back(string_format("Impossible to open %s", name.c_str()));
                return false;
            }

            while (file.available())
            {

                (*buff) += file.read();
            }
            result.push_back(string_format("File %s loaded.", name.c_str()));
            return true;
        }
    }

    bool save(string name, string *buff)
    {
        result.clear();
        string _name = current_path + name;
        if (!current_mount->fs->open(_name.c_str()))
        {
            result.push_back(string_format("Impossible to save %s.", name.c_str()));
            return false;
        }
        else
        {
            result.push_back(string_format("File %s saved.", name.c_str()));
            return true;
        }
    }

    bool rm(string name)
    {
        result.clear();
        string _name = current_path + name;
        if (!current_mount->fs->remove(_name.c_str()))
        {
            result.push_back(string_format("Impossible to remove %s.", name.c_str()));
            return false;
        }
        else
        {
            result.push_back(string_format("File %s removed.", name.c_str()));
            return true;
        }
    }

    bool rmdir(string name)
    {
        result.clear();
        string _name = current_path + name;
        if (!current_mount->fs->rmdir(_name.c_str()))
        {
            result.push_back(string_format("Impossible to remove directory %s.", name.c_str()));
            return false;
        }
        else
        {
            result.push_back(string_format("Directory %s removed.", name.c_str()));
            return true;
        }
    }

    string current_path = "";
    list<mount_point> mounts;
    list<_files> dirfiles;
    mount_point *current_mount = NULL;
    list<string> result;
};

FileSystem fileSystem;

void display()
{
    for (string s : fileSystem.result)
    {
        printf("%s\r\n", s.c_str());
    }
}

#endif