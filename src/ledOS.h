
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
  bool scriptModified=false;
  bool toBeUpdated = true;

  int index_sentence;

  Console() { init(); }
  void init()
  {
    initEscCommands(this);
    fileSystem.init();
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
    filename = "";
    sentence = "";
    script.clear();
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

  list<string>::iterator getLineIterator(int pos)
  {
    int i = 0;
    if (pos >= script.size() || pos < 0)
    {
      return script.end();
    }
    for (list<string>::iterator it = script.begin(); it != script.end(); it++)
    {
      if (i == pos)
      {

        return it;
      }
      i++;
    }
    return script.end();
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
    internal_coordinates.cursor_y++;
    internal_coordinates.line_y++;
    internal_coordinates.cursor_x = 1;
    internal_coordinates.line_x = 1;
    if (cmode == edit)
    {
      // internal_coordinates.y >= height &&
      if (internal_coordinates.cursor_y >= height - 1)
      {
        internal_coordinates.cursor_y = height - 2;
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
  void displayfooter()
  {

    Serial.printf(config.SAVE);
    Serial.printf(config.HIDECURSOR);
    Serial.printf(config.DOWN);
    Serial.printf(config.LEFT);
    
    Serial.printf("%s\r", footerformat.c_str());
    // Serial.print("\u001b[0K");

    Serial.printf("%sPos %d,%d width:%d  height:%d curpos:%d,%d script size:%d  filename:%s%s", footerformat.c_str(), internal_coordinates.line_x, internal_coordinates.line_y, width, height, internal_coordinates.cursor_x, internal_coordinates.cursor_y, script.size(),scriptModified ? "*":"" ,filename.c_str());
    Serial.print("\u001b[0K");
    Serial.printf("\u001b[%dC", width);
    Serial.printf("\u001b[%dD", 11);
    Serial.printf(termBackgroundColor.BYellow);
    Serial.printf(" mem:%6d %s", esp_get_free_heap_size(), currentformat.c_str());
    Serial.printf(config.RESTORE);
    Serial.printf(config.SHOWCURSOR);
  }

  void _list()
  {
    // je peux en mettre height-internaly+1
    // int end = start + len - 1;
    // int sl =internal_coordinates.line_y;
    // int sc=  internal_coordinates.cursor_y;
    list<string>::iterator k = getLineIterator(internal_coordinates.line_y - 1);
    list<string>::iterator k2 = getLineIterator(internal_coordinates.line_y);
    while (k != script.end() and (internal_coordinates.cursor_y <= height - 1))
    {

      _push(config.DELINE);
      _push(config.BEGIN_OF_LINE);
      //
      Serial.printf("%s%3d %s %s", editprompt.c_str(), internal_coordinates.line_y, editcontent.c_str(), current_hightlight->highLight(*k).c_str());

      sentence = (*k);
      internal_coordinates.cursor_x = (*k).size() + 1;
      internal_coordinates.line_x = (*k).size() + 1;

      // if ((internal_coordinates.cursor_y < height - 1) )
      // {

      //
      // gotoline();
      k++;

      if (internal_coordinates.cursor_y != height - 1)
      {
        if (k != script.end())
        {
          if (current_hightlight->newLine)
            current_hightlight->newLine();
          internal_coordinates.cursor_y++;
          internal_coordinates.line_y++;
          _push(config.ENDLINE);
          _push(config.BEGIN_OF_LINE);
        }
        else

        {

          _push(config.SAVE);
          _push(config.MOVEDOWN);
          _push(config.DELINE);
          _push(config.RESTORE);
          return;
          // internal_coordinates.cursor_y++;
          // internal_coordinates.line_y--;
        }
      }
      else
      {

        _push(config.SAVE);
        _push(config.MOVEDOWN);
        _push(config.DELINE);
        _push(config.RESTORE);

        return;
      }

      // }
    }

    // k++;
    // k2=getLineIterator(internal_coordinates.line_y+1);
    //  Serial.printf("%s%s", cons->prompt(cons).c_str(), cons->script[i].c_str());
  }

  void setHighlight(string str)
  {
    // vector<highlight_struct> highLighting;
    int count = 0;
    vector<string> k = split(str, ".");
    if (k.size() > 0)
    {
      for (highlight_struct h : highLighting)
      {

        if (h.extension.compare(k[k.size() - 1]) == 0)
        {
          current_hightlight = &highLighting[count];
          return;
        }
        count++;
      }
    }
    // LedOS.addHightLightinf(".sc", formatLine,formatInit,formatNewLine);
    current_hightlight = &highLighting[0];
  }
  void run()
  {
    _push(config.HIDESCROLLBAR);
    char c;
    // getConsoleSize();
    printf("welcome\r\n");

    // Serial.printf("%d:%d:%s",width,height,config.DEFAULT_PROMPT);
    _push(prompt(this).c_str());
    // internal_coordinates=getCursorPos();
    internal_coordinates.cursor_x = 1;
    internal_coordinates.cursor_y = 1;
    internal_coordinates.line_x = 1;
    internal_coordinates.line_y = 0;
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
          displayf ==true;
          switch (c)
          {

          case 127:
            if (cmode == keyword)
            {
              if (internal_coordinates.line_x > 1)
              {
                if (internal_coordinates.line_x > sentence.size())
                {
                  sentence = sentence.substr(0, sentence.length() - 1);
                  internal_coordinates.line_x--;
                  Serial.print("\u001b[1D");
                  Serial.printf("\u001b[0K");
                }
                else
                {
                  string save = sentence.substr(internal_coordinates.line_x - 1, sentence.size());
                  sentence = sentence.substr(0, internal_coordinates.line_x - 2) + sentence.substr(internal_coordinates.line_x - 1, sentence.size());

                  internal_coordinates.line_x--;

                  Serial.printf("\u001b[1D");
                  _push(config.SAVE);
                  Serial.printf("\u001b[0K%s", save.c_str());
                  _push(config.RESTORE);
                }
              }
            }
            else if (cmode == edit)
            {
              if (internal_coordinates.line_x > 1)
              {
                scriptModified=true;
                if (internal_coordinates.line_x > sentence.size())
                {
                  sentence = sentence.substr(0, sentence.length() - 1);
                  internal_coordinates.line_x--;
                }
                else
                {
                  string save = sentence.substr(internal_coordinates.line_x - 1, sentence.size());
                  sentence = sentence.substr(0, internal_coordinates.line_x - 2) + sentence.substr(internal_coordinates.line_x - 1, sentence.size());
                  internal_coordinates.line_x--;
                }
                _push(config.HIDECURSOR);
                Serial.printf("\u001b[1D");
                _push(config.SAVE);
                _push(config.BEGIN_OF_LINE);
                _push(prompt(this).c_str());
                _push(current_hightlight->highLight( sentence).c_str());
                _push(config.ERASE_FROM_CURSOR_TO_EOL);
                _push(config.SHOWCURSOR);
                _push(config.RESTORE);
              }

              else if (internal_coordinates.line_x == 1 && internal_coordinates.line_y > 1)
              {
                list<string>::iterator k = getLineIterator(internal_coordinates.line_y - 1);
                if (k != script.end())
                {
                  script.erase(k);
                }
                k = getLineIterator(internal_coordinates.line_y - 2);
                sentence = (*k) + sentence;
                string sav = sentence;
                _push(config.HIDECURSOR);
                _push(config.DELINE);
                _push(config.MOVEUP);
                //_push(config.BEGIN_OF_LINE);
                // _push(prompt(this).c_str());
                // _push((*k).c_str());
                // _push(config.SAVE);
                _push(config.BEGIN_OF_LINE);
                internal_coordinates.line_y--;
                internal_coordinates.cursor_y--;
                int sl = internal_coordinates.line_y;
                int sc = internal_coordinates.cursor_y;
                _push(prompt(this).c_str());
                _push(current_hightlight->highLight( sentence).c_str());
                _push(config.MOVEDOWN);
                internal_coordinates.line_y++;
                internal_coordinates.cursor_y++;
                _list();
                // _push(config.RESTORE);
                internal_coordinates.line_x = (*k).size() + 1;

                internal_coordinates.line_y = sl;
                internal_coordinates.cursor_y = sc;
                _push(config.BEGIN_OF_LINE);
                _push(locate(5 + internal_coordinates.line_x, sc).c_str());
                sentence = sav;
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
              //ok it's ugly but for now it's ok
              scriptModified=true;
              //addCharacterEditor(' ');
              //addCharacterEditor(' ');
              //addCharacterEditor(' ');
              //addCharacterEditor(' ');
              //addCharacterEditor(' ');
              addStringEditor("     ");
              _push(moveright(4).c_str());
                            internal_coordinates.cursor_x+=4;
              internal_coordinates.line_x+=4;

            }
            else if (cmode == paste)
            {
               scriptModified=true;
              sentence += c;
            }
            break;

          case ' ':

            if (cmode == keyword)
            {
              search_sentence = "";
              __toBeUpdated = true;
              sentence += c;
              Serial.write(c);
              internal_coordinates.cursor_x++;
              internal_coordinates.line_x++;
            }
            else if (cmode == edit)
            {
              scriptModified=true;
              addCharacterEditor(c);
            }
            else if (cmode == paste)
            {
               scriptModified=true;
              sentence += c;
              Serial.write(c);
            }

            break;
          case '(':
          if(cmode==keyword)
          {
              sentence += c;
              search_sentence+=c;
              Serial.write(c);
              internal_coordinates.cursor_x++;
              internal_coordinates.line_x++;
          }
          else if(cmode==edit)
          {
            scriptModified=true;
            addStringEditor("()");
          }
           else if (cmode == paste)
            {
               scriptModified=true;
              sentence += c;
              Serial.write(c);
            }
          break;
           case '[':
          if(cmode==keyword)
          {
              sentence += c;
              search_sentence+=c;
              Serial.write(c);
              internal_coordinates.cursor_x++;
              internal_coordinates.line_x++;
          }
          else if(cmode==edit)
          {
             scriptModified=true;
            addStringEditor("[]");
          }
           else if (cmode == paste)
            {
               scriptModified=true;
              sentence += c;
              Serial.write(c);
            }
          break;
        case '{':
          if(cmode==keyword)
          {
              sentence += c;
              search_sentence+=c;
              Serial.write(c);
              internal_coordinates.cursor_x++;
              internal_coordinates.line_x++;
          }
          else if(cmode==edit)
          {
             scriptModified=true;
            string left=sentence+"{";
            string right=leadSpace(left)+"   ";
            list<string>::iterator k = getLineIterator(internal_coordinates.line_y - 1);
              if (k == script.end())
              {
                script.push_back(left);
              }
              else
              {
                // printf("jjdd");
                script.erase(k);
                // printf("jj");
                k = getLineIterator(internal_coordinates.line_y - 1);
                // k++;
                script.insert(k, left);
                // printf("sdd");
              }
              displayline(internal_coordinates.line_y - 1);
              k = getLineIterator(internal_coordinates.line_y);
               if (k == script.end())
              {
                script.push_back(right);
              }
              else
              {
                // printf("jjdd");
                // k++;
                script.insert(k, right);
                // printf("sdd");
              }
              sentence = right;
               k = getLineIterator(internal_coordinates.line_y+1);
                if (k == script.end())
              {
                script.push_back(leadSpace(left)+"}");
              }
              else
              {
                // k++;
                script.insert(k, leadSpace(left)+"}");
                // printf("sdd");
              }
                              _push(config.HIDECURSOR);
                _push(config.MOVEDOWN);
                _push(config.BEGIN_OF_LINE);
                internal_coordinates.cursor_y++;
                internal_coordinates.line_y++;
                int savcy = internal_coordinates.cursor_y;
                int savly = internal_coordinates.line_y;
              //  _push(prompt(this).c_str());
               // _push(config.ERASE_FROM_CURSOR_TO_EOL);
               // _push(config.SAVE);
               // _push(config.MOVEDOWN);
                _push(config.BEGIN_OF_LINE);
                // _push(config.ENDLINE);

               // internal_coordinates.cursor_y++;
               // internal_coordinates.line_y++;
                _list();
                sentence = right;
                internal_coordinates.cursor_x = right.size()+1;
                internal_coordinates.line_x =right.size()+ 1;

                internal_coordinates.cursor_y = savcy;
                internal_coordinates.line_y = savly;
              //displayline(internal_coordinates.line_y - 1);
              _push(locate(5 + internal_coordinates.line_x, internal_coordinates.cursor_y).c_str());
              _push(config.SHOWCURSOR);
           
          }
           else if (cmode == paste)
            {
               scriptModified=true;
              sentence += c;
              Serial.write(c);
            }
          break;
          case '\r':
          {
            if (cmode == paste)
            {
               scriptModified=true;
              script.push_back(sentence);
              sentence = "";
              _push(config.ENDLINE);
            }

            else if (cmode == keyword)
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
                  Serial.printf("%sLedOS commande not found: %s", errorformat.c_str(), cmd_line[0].c_str());
                  gotoline();
                }
                else
                {
                   gotoline();
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
               scriptModified=true;
              string left = sentence.substr(0, internal_coordinates.line_x - 1);
              string right = sentence.substr(internal_coordinates.line_x - 1, sentence.size());
              list<string>::iterator k = getLineIterator(internal_coordinates.line_y - 1);
              if (k == script.end())
              {
                script.push_back(left);
              }
              else
              {
                // printf("jjdd");
                script.erase(k);
                // printf("jj");
                k = getLineIterator(internal_coordinates.line_y - 1);
                // k++;
                script.insert(k, left);
                // printf("sdd");
              }
              displayline(internal_coordinates.line_y - 1);
              k = getLineIterator(internal_coordinates.line_y);
              //printf("%s :%d",left.c_str(),leadSpace(left).size());
              right=leadSpace(left)+right;
              if (k != script.end())
              {
                script.insert(k, right);
              }
              else{
                script.push_back(right);
              }
              sentence = right;
              if (current_hightlight->newLine)
                current_hightlight->newLine();

              if (internal_coordinates.cursor_y < height - 2)
              {
                _push(config.HIDECURSOR);
                _push(config.MOVEDOWN);
                _push(config.BEGIN_OF_LINE);
                internal_coordinates.cursor_y++;
                internal_coordinates.line_y++;
                int savcy = internal_coordinates.cursor_y;
                int savly = internal_coordinates.line_y;
                _push(prompt(this).c_str());
                _push(config.ERASE_FROM_CURSOR_TO_EOL);
                _push(config.SAVE);
                //_push(config.MOVEDOWN);
                //_push(config.BEGIN_OF_LINE);
                // _push(config.ENDLINE);

                //internal_coordinates.cursor_y++;
                //internal_coordinates.line_y++;
                _list();
                sentence = right;
                internal_coordinates.cursor_x = leadSpace(left).size()+1;
                internal_coordinates.line_x =leadSpace(left).size()+ 1;

                internal_coordinates.cursor_y = savcy;
                internal_coordinates.line_y = savly;
                _push(config.RESTORE);
               // _push(locate(5 + internal_coordinates.line_x, internal_coordinates.cursor_y).c_str());
               // _push(config.BEGIN_OF_LINE);
               // _push(prompt(this).c_str());
               // _push(current_hightlight->highLight(sentence).c_str());
               // _push(locate(5 + internal_coordinates.line_x, internal_coordinates.cursor_y).c_str());
               // _push(config.SHOWCURSOR);
              }
              else
              {
                _push(config.HIDECURSOR);
                _push(config.SAVE);
                _push(config.DOWN);
                _push(config.DELINE);
                _push(config.RESTORE);
                _push(config.SCROLLUP);
                _push(config.BEGIN_OF_LINE);
                internal_coordinates.line_y++;
                int savcy = internal_coordinates.cursor_y;
                int savly = internal_coordinates.line_y;
               // _push(prompt(this).c_str());
               // _push(config.ERASE_FROM_CURSOR_TO_EOL);

                _push(config.SAVE);
               // _push(config.MOVEDOWN);
                _push(config.BEGIN_OF_LINE);
                //internal_coordinates.cursor_y++;
               // internal_coordinates.line_y++;
                _list();
                sentence = right;
                internal_coordinates.cursor_x = leadSpace(left).size()+1;
                internal_coordinates.line_x = leadSpace(left).size()+1;
                internal_coordinates.cursor_y = savcy;
                internal_coordinates.line_y = savly;
               //_push(config.RESTORE);
               /// _push(locate(5 + internal_coordinates.line_x, internal_coordinates.cursor_y).c_str());
               // _push(config.SHOWCURSOR);
              }
             // displayline(internal_coordinates.line_y - 1);
              _push(locate(5 + internal_coordinates.line_x, internal_coordinates.cursor_y).c_str());
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

              internal_coordinates.cursor_x++;
              internal_coordinates.line_x++;
            }
            else if (cmode == edit)
            {
               scriptModified=true;
              addCharacterEditor(c);
            }
            else if (cmode == paste)
            {
               scriptModified=true;
              sentence += c;
              Serial.write(c);
            }
            break;
          }

        }
        if (cmode == edit && displayf ==true)
        {
          displayfooter();
        }
        
      }
    }
  }
  void displayline(int pos)
  {
    list<string>::iterator k = getLineIterator(pos);
    if (k != script.end())
    {
      //_push(config.SAVE);
      _push(config.HIDECURSOR);
      _push(config.BEGIN_OF_LINE);
      _push(prompt(this).c_str());
      _push(current_hightlight->highLight((*k)).c_str());
      _push(config.ERASE_FROM_CURSOR_TO_EOL);
      _push(config.BEGIN_OF_LINE);
      _push(prompt(this).c_str());
      _push(config.SHOWCURSOR);
    }
  }

  void addCharacterEditor(char c)
  {
    _push(config.HIDECURSOR);
    _push(config.SAVE);
    _push(config.BEGIN_OF_LINE);
    _push(moveright(5).c_str());
    if (sentence.size() >= 1)
    {
      sentence = sentence.substr(0, internal_coordinates.line_x - 1) + c + sentence.substr(internal_coordinates.line_x - 1, sentence.size());
    }
    else
      sentence += c;
    _push(current_hightlight->highLight(sentence).c_str());
    // _push(sentence.c_str());
    _push(config.RESTORE);
    _push(config.FORWARD);
    _push(config.SHOWCURSOR);

    internal_coordinates.cursor_x++;
    internal_coordinates.line_x++;
  }
  void addStringEditor(string c)
  {
    _push(config.HIDECURSOR);
    _push(config.SAVE);
    _push(config.BEGIN_OF_LINE);
    _push(moveright(5).c_str());
    if (sentence.size() >= 1)
    {
      sentence = sentence.substr(0, internal_coordinates.line_x - 1) + c + sentence.substr(internal_coordinates.line_x - 1, sentence.size());
    }
    else
      sentence = sentence+c;
    _push(current_hightlight->highLight(sentence).c_str());
    // _push(sentence.c_str());
    _push(config.RESTORE);
    _push(config.FORWARD);
    _push(config.SHOWCURSOR);

    internal_coordinates.cursor_x++;
    internal_coordinates.line_x++;
  }


  void pushToConsole(string str)
  {
    if(cmode==keyword)
    {
      _push(str.c_str());
      _push(config.ENDLINE);
    }
    else if (cmode ==edit)
    {
         _push(config.SAVE);
    _push(config.HIDECURSOR);
    _push(config.DOWN);
    _push(config.LEFT);
    _push(config.DELINE);
  _push(str.c_str());
   _push(config.RESTORE);
    _push(config.SHOWCURSOR);
    }
  }


  void storeCurrentLine()
{
    list<string>::iterator k = getLineIterator(internal_coordinates.line_y - 1);
    if (k == script.end())
    {
     script.push_back(sentence);
    }
    else
    {
      // printf("jjdd");
      script.erase(k);
      // printf("jj");
      k = getLineIterator(internal_coordinates.line_y - 1);
      // k++;
      script.insert(k, sentence);
    }
}
};
#include "ledOShelper.h"
Console LedOS;
