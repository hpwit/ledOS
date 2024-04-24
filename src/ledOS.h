
#pragma once
#define __CONSOLE_ESP32
#include "string_function.h"
#include "ledOStruct.h"

// modules
#include "fileSystem.h"

string defaultPrompt(Console *cons);
string editPrompt(Console *cons);
void insertLine(Console *cons);
void _list(Console *cons, int start, int len);
void initEscCommands(Console *cons);
void manageTabulation(Console *cons);
class Console
{
public:
  string (*prompt)(Console *cons);
  int height = 2000;
  int width = 2000;
  coord internal_coordinates;
  vector<char *> defaultPromptFormatColor;
  vector<char *> editorPromptFormatColor;
  string sentence;
  string search_sentence;
  vector<Console_esc_command> esc_commands;
  vector<Console_keyword_command> keyword_commands;
  vector<highlight_struct> highLighting;
  highlight_struct *current_hightlight;
  ConsoleMode cmode;
  list<string> script;

  bool displayf = true;
  string defaultformat = config.ESC_RESET; //+termBackgroundColor.Black+termColor.BGreen;
  string errorformat = config.ESC_RESET;   //+termBackgroundColor.Black+termColor.Red+termFormat.Bold;
  string editprompt = config.ESC_RESET;    //+termBackgroundColor.Black+termColor.White;
  string editcontent = config.ESC_RESET;   //+termBackgroundColor.Black+termColor.BWhite+termFormat.Bold;
  string footerformat = config.ESC_RESET;
  string currentformat;
  string filename = "";
  CommandHistory commands;
  CommandHistory searchContent;
  bool toBeUpdated = true;

  int index_sentence;

  Console()
  {
    initEscCommands(this);
    __toBeUpdated = true;
    // footerformat += termFormat.Reversed;
    footerformat += termBackgroundColor.BBlue;
    footerformat += termColor.BWhite;

    // defaultformat+=termBackgroundColor.BBlack;
    // defaultformat+="\u001b[49m";
    defaultformat += termColor.BGreen;

    // errorformat+=termBackgroundColor.BBlack;
    errorformat += termColor.Red;

    // editprompt+=termBackgroundColor.BBlack;
    editprompt += termColor.Grey;
    editprompt += "\u001b[48;5;236m";

    // editcontent+=termBackgroundColor.BBlack;
    editcontent += termColor.BWhite;

    cmode = keyword;
    prompt = &defaultPrompt;
    currentformat = defaultformat;
    addHightLightinf("", default_highlightfunction, NULL, NULL);
    current_hightlight = &highLighting[0];
  }

  void addHightLightinf(string ext, string (*highLight)(string), void (*init)(), void (*newline)())
  {
    highlight_struct es;
    es.extension = ext;
    es.highLight = highLight;
    es.init = init;
    es.newLine = newline;
    highLighting.push_back(es);
  }
  bool analyseEscCommand(char c)
  {
    for (int i = 0; i < esc_commands.size(); i++)
    {
      Console_esc_command esc_command = esc_commands[i];
      if (esc_command.esc_code == c)
      {
        esc_command.command(this);
        return true;
      }
    }
    return false;
  }

  bool analyseKeywordCommand(string c, vector<string> args)
  {
    for (int i = 0; i < keyword_commands.size(); i++)
    {
      Console_keyword_command keyword_command = keyword_commands[i];
      if (keyword_command.keyword == c)
      {
        keyword_command.command(this, args);
        return true;
      }
    }
    return false;
  }

  void gotoline()
  {
    internal_coordinates.y++;
    internal_coordinates.internaly++;
    internal_coordinates.x = 1;
    if (cmode == edit)
    {
      // internal_coordinates.y >= height &&
      if (internal_coordinates.internaly >= height - 1)
      {
        internal_coordinates.internaly = height - 2;
        Serial.print("\u001b[0S");
        Serial.print("\u001b[1G");
        Serial.print("\u001b[0K");

        return;
      }
      else
      {
        // internal_coordinates.internaly++;
        Serial.printf("%s", config.ENDLINE);
      }
    }
    else
    {
      Serial.printf("%s", config.ENDLINE);
    }
  }

  void addEscCommand(uint8_t esc_code, void (*command)(Console *cons))
  {
    Console_esc_command es;
    es.esc_code = esc_code;
    es.command = command;
    esc_commands.push_back(es);
  }

  void addKeywordCommand(string keyword, void (*command)(Console *cons, vector<string> args))
  {
    Console_keyword_command es;
    es.keyword = keyword;
    es.command = command;
    keyword_commands.push_back(es);
  }

  void run()
  {
    char c;
    // getConsoleSize();
    printf("welcome\r\n");

    // Serial.printf("%d:%d:%s",width,height,config.DEFAULT_PROMPT);
    _push(prompt(this).c_str());
    // internal_coordinates=getCursorPos();
    internal_coordinates.x = 1;
    internal_coordinates.y = 1;
    internal_coordinates.internaly = 0;
    displayf = false;
    // displayfooter();

    while (1)
    {

      // while(!(Serial.available()>0));
      if (Serial.available() > 0)
      {
        c = Serial.read();
        bool res = analyseEscCommand(c);

        if (res == false)
        {
          switch (c)
          {

          case 127:
            if (internal_coordinates.x > 1)
            {
              if (internal_coordinates.x > sentence.size())
              {
                sentence = sentence.substr(0, sentence.length() - 1);
                search_sentence = search_sentence.substr(0, search_sentence.length() - 1);
                __toBeUpdated = true;
                internal_coordinates.x--;
                Serial.print("\u001b[1D");
                Serial.printf("\u001b[0K");
              }
            }

            break;
          case '\t':
            if (cmode == keyword)
            {
              manageTabulation(this);
            }
            else if (cmode == edit)
            {
              addCharacterEditor(c);
            }

            break;

          case ' ':

            if (cmode == keyword)
            {
              search_sentence = "";
              __toBeUpdated = true;
              sentence += c;
              Serial.write(c);
              internal_coordinates.x++;
            }
            else if (cmode == edit)
            {
              addCharacterEditor(c);
            }

            break;

          case '\r':
          {

            if (cmode == keyword)
            {
              if (sentence.size() < 1)
              {

                gotoline();
                _push(prompt(this).c_str());
                continue;
              }
              commands.addCommandToHistory(sentence);
              currentformat = defaultformat;
              vector<string> cmd_line = split(trim(sentence), " ");

              if (trim(cmd_line[0]).size() > 0)
              {

                vector<string> args;
                if (cmd_line.size() > 1)
                {
                  for (int i = 1; i < cmd_line.size(); i++)
                  {
                    if (trim(cmd_line[i]).size() > 0)
                    {
                      args.push_back(cmd_line[i]);
                    }
                  }
                }
                bool res = analyseKeywordCommand(cmd_line[0], args);
                if (res == false)
                {
                  gotoline();
                  Serial.printf("%sLedOS commande not found: %s", errorformat.c_str(), sentence.c_str());
                  gotoline();
                }
                else
                {
                  //  gotoline();
                }
              }
              else
              {
                gotoline();
              }
              _push(prompt(this).c_str());
              sentence = "";
              search_sentence = "";
              __toBeUpdated = true;
              delay(10);
            }
            else if (cmode == edit)
            {
              sentence = "";
              if (current_hightlight->newLine)
              current_hightlight->newLine();

              _push(config.HIDECURSOR);
              _push(config.MOVEDOWN);
              _push(config.BEGIN_OF_LINE);
             
              internal_coordinates.x = 1;
              internal_coordinates.y++;
              internal_coordinates.internaly++;
              _push(prompt(this).c_str());
               _push(config.SHOWCURSOR);
            }
          }
          break;
          default:
            if (cmode == keyword)
            {
              sentence += c;
              search_sentence += c;
              __toBeUpdated = true;
              Serial.printf("%c", c);

              internal_coordinates.x++;
            }
            else if (cmode == edit)
            {
              // sentence += c;
              addCharacterEditor(c);
            }
            break;
          }
        }
      }
    }
  }

  void addCharacterEditor(char c)
  {
    _push(config.HIDECURSOR);
    _push(config.SAVE);
    _push(config.BEGIN_OF_LINE);
    _push(moveright(5).c_str());
    sentence = sentence.substr(0, internal_coordinates.x - 1) + c + sentence.substr(internal_coordinates.x - 1, sentence.size());
    _push(current_hightlight->highLight(sentence).c_str());
    _push(config.RESTORE);
    _push(config.FORWARD);
   _push(config.SHOWCURSOR);

    internal_coordinates.x++;
  }
};
#include "ledOShelper.h"
Console LedOS;
