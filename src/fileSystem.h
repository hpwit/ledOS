#include <SD.h>
#include "SPIFFS.h"
#include "FS.h"
#include <string>
#include <list>
#pragma once
#ifndef __LED_OS
#define __LED_OS
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

bool compare_nocase (const _files& first, const _files& second)
{
  unsigned int i=0;
  while ( (i<first.name.length()) && (i<second.name.length()) )
  {
    if (tolower(first.name[i])<tolower(second.name[i])) return true;
    else if (tolower(first.name[i])>tolower(second.name[i])) return false;
    ++i;
  }
  return ( first.name.length() < second.name.length() );
}

class FileSystem
{
public:
    FileSystem()
    {
        mount_point spiff;
        if(SPIFFS.begin(true)){
        spiff.fs = &SPIFFS;
        spiff.name = "SPIFFS";
        mounts.push_back(spiff);
        current_mount = &mounts.back();
        current_path = "/";
        }
        else{
            printf("Unable to open SPIFFS\r\n");
        }
    }

    list<string> mkdir(string name)
    {
        name=name;
        printf("trying to create %s\r\n",name.c_str());
        if(current_mount->fs->exists(name.c_str()))
        {
            printf("impossible to create directory\r\n");
            return;
        }
        else
        {
          current_mount->fs->mkdir(name.c_str());
          return;
         }
         
    }
    void ls()
    {
        dirfiles.clear();
        if (current_path.compare("")==0)
        {
            for (mount_point g : mounts)
            {
                printf("%s\n", g.name);
            }
        }
        else
        {
            if (current_mount == NULL)
            {
                printf("no Mount !!!\r\n");
                return;
            }
            uint16_t nb_files,total_mem;
            File root = current_mount->fs->open(current_path.c_str());
            File file = root.openNextFile();
            _files f;
            while (file)
            {
                f.isDirectory=false;
                f.file_size=0;
                                if (file.isDirectory())
                {
                    f.isDirectory=true;
                }
                else{
                    f.file_size=file.size();
                }
                f.name=file.name();
                dirfiles.push_back(f);
                file = root.openNextFile();
            }
            dirfiles.sort(compare_nocase);
            Serial.printf("\r\n");
            for(_files fil:dirfiles)
            {
                if (fil.isDirectory)
                {
                    Serial.printf("%s/\r\n", fil.name.c_str());
                }
                else
                {
                    // Serial.print(file.name());
                    // Serial.print("\r\n");
                    string f = string_format("%s%30s", fil.name.c_str(), "");
                    f = f.substr(0, 20);
                    Serial.printf("%s %6d bytes\r\n", f.c_str(), fil.file_size);
                    nb_files++;
                    total_mem += fil.file_size;
                }
                file = root.openNextFile();
            }
            Serial.printf("%d file(s) %d bytes \r\n", nb_files, total_mem);
        }
        
    }
    string current_path = "";

    list<mount_point> mounts;
    list<_files> dirfiles;

    mount_point *current_mount = NULL;
};

FileSystem fileSystem;

#endif