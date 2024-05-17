#pragma once
#ifndef _LEDS_OS_STRUCT
#define _LEDS_OS_STRUCT

#include <vector>
#include <string>
#define _push(x) Serial.printf("%s", x)

typedef struct
{
  const char *White = "\u001b[37m";
  const char *Black = "\u001b[30m";
  const char *Red = "\u001b[38;5;196m";
  const char *Green = "\u001b[32m";
  const char *Yellow = "\u001b[33m";
  const char *Blue = "\u001b[34m";
  const char *Magenta = "\u001b[35m";
  const char *Cyan = "\u001b[36m";
  const char *BWhite = "\u001b[38;5;255m";
  const char *BBlack = "\u001b[30;1m";
  const char *BRed = "\u001b[31;1m";
  const char *BGreen = "\u001b[38;5;46m";
  const char *BYellow = "\u001b[33;1m";
  const char *BBlue = "\u001b[34;1m";
  const char *BMagenta = "\u001b[35;1m";
  const char *BCyan = "\u001b[36;1m";
  const char *Grey = "\u001b[38;5;240m";
  const char *Orange = "\u001b[38;5;215m";
   const char *LMagenta = "\u001b[38;5;219m";
   const char *LYellow = "\u001b[38;5;191m";
     const char *LCyan = "\u001b[38;5;86m";
      const char *LBlue = "\u001b[38;5;123m";
} _termColor;
_termColor termColor;

typedef struct
{
  const char *White = "\u001b[47m";
  const char *Black = "\u001b[40m";
  const char *Red = "\u001b[41m";
  const char *Green = "\u001b[42m";
  const char *Yellow = "\u001b[43m";
  const char *Blue = "\u001b[44m";
  const char *Magenta = "\u001b[45m";
  const char *Cyan = "\u001b[46m";
  const char *BWhite = "\u001b[47;1m";
  const char *BBlack = "\u001b[40;1m";
  const char *BRed = "\u001b[41;1m";
  const char *BGreen = "\u001b[48;2;0;255;0m";
  const char *BYellow = "\u001b[43;1m";
  const char *BBlue = "\u001b[44;1m";
  const char *BMagenta = "\u001b[45;1m";
  const char *BCyan = "\u001b[46;1m";
} _termBackgroundColor;
_termBackgroundColor termBackgroundColor;

typedef struct
{

  const char *Bold = "\u001b[1m";
  const char *Underline = "\u001b[4m";
  const char *Reversed = "\u001b[7m";
} _termFormat;
_termFormat termFormat;

typedef struct
{
  const char *ENDLINE = "\r\n";
  const char *ESC_RESET = "\u001b[0m";
  const char *DEFAULT_PROMPT = "\u001b[7mLedOS\u001b[1m\u25b6";
  const char *PROGRAM_PROMPT = "";
  const char *DOWN = "\u001b[1000B";
  const char *MOVEUP = "\u001b[0F";
  const char *MOVEDOWN = "\u001b[0E";
  const char *BACK = "\u001b[1D";
  const char *FORWARD = "\u001b[1C";
  const char *LEFT = "\u001b[100D";
  const char *SCROLLUP = "\u001b[0S";
  const char *SCROLLDOWN = "\u001b[0T";
  const char *SAVE = "\u001b[s";
  const char *RESTORE = "\u001b[u";
  const char *SHOWCURSOR = "\u001b[?25h";
  const char *DELINE = "\u001b[2K";
  const char *HIDECURSOR = "\u001b[?25l";
  const char *BEGIN_OF_LINE = "\u001b[0G";
  const char *ERASE_FROM_CURSOR_TO_EOL = "\u001b[0K";
  const char *ENABLE_MOUSE= "\u001b[?1000h";
  const char *SAVESCREEN ="\u001b[?1046h\u001b[?1047h";
  const char *RESTORESCREEN ="\u001b[?1047l";
  const char *HIDESCROLLBAR ="";//\u001b[?30l";
} _config;
_config config;

int width = 300;
int height = 300;
typedef struct
{
  int cursor_x;
  int cursor_y;
  int line_x;
  int line_y;
} coord;
class Console;
typedef struct
{
  uint8_t esc_code;
  string description;
  void (*command)(Console *cons);
} Console_esc_command;

typedef struct
{
  string keyword;
  string description;
  void (*command)(Console *cons, vector<string> args);
} Console_keyword_command;


typedef struct
{
  string extension;
  string (*highLight)(string str);
  void(*init)();
  void(*newLine)();
} highlight_struct;

enum ConsoleMode
{
  keyword,
  edit,
  execute,
  paste
};



string getEscapeSequence()
{
  bool first = true;
  char c;
  string res;
  res="";
 // while (1)
  //{
    while(!(Serial.available()>0));
    while (Serial.available() > 0)
    {
      c = Serial.read();
      res += c;
      
      /*
      if (c >= 65 and c <= 90)
      {
        Serial.printf("size:%d\n",res.size());
        return res;
     }*/
    }
  //}
  // Serial.printf("size:%d\n",res.size());
return res;
}

void getMousePos()
{
    coord co;
  char t,x, y;
  
    _push(config.ENABLE_MOUSE);
    string res = getEscapeSequence();
    sscanf(res.c_str(), "\033[M%c%c%c",&t, &y, &x);
  

 Serial.printf("mouse: %d %d %d\n",t,x-32,y-32);
  return ;
}
coord getCursorPos()
{
  coord co;
  int x, y;


  do
  {
    Serial.print("\u001b[6n");
    string res = getEscapeSequence();
    sscanf(res.c_str(), "\033[%d;%dR", &y, &x);
  } while (x < 0 or x > 2000 or y < 0 or y > 2000);

  co.cursor_x = x;
  co.cursor_y = y;
  return co;
}
void getConsoleSize()
{
  // getMousePos();
  Serial.printf("%s", config.SAVE);
  Serial.printf("%s", config.DOWN);
  Serial.printf("\u001b[10000C");
  coord co = getCursorPos();
  width = co.cursor_x;
  height = co.cursor_y;
  Serial.printf("%s", config.RESTORE);
}

class CommandHistory
{
public:
  CommandHistory()
  {
 clear();
  }
void clear()
{
      history.clear();
    current_save = 0;
    current_read = 0;
}
  void addCommandToHistory(string command)
  {
    if (current_save >= MAX)
    {
      for(int i=1;i<MAX;i++)
      {
        history[i-1]=history[i];
      }
    
      current_save=MAX-1;
      history[current_save] = command;
    }
    else
    {
      history.push_back(command);
    }
    current_save = (current_save + 1) ;
    current_read = current_save;
  }
  string getCommandHistoryCircular()
  {
    
    if (history.size() > 0)
    {
      string s = history[current_read];
      
      current_read++;
      if (current_read >=current_save)
      {
        current_read = 0;
      }
     
      return s;
    }
    else
    {
      return "";
    }
  }

  string getCommandHistoryUp()
  {
    
    if (history.size() > 0)
    {
     
      current_read--;
      if (current_read <0)
      {
        current_read = 0;
      }
      string s = history[current_read];
      
      return s;
    }
    else
    {
      return "";
    }
  }
  string getCommandHistoryDown()
  {
     current_read++;
    if (history.size() > 0 && current_read < current_save)
    {
            //current_read++;
    /*
      if (current_read >= current_save)
      {
        current_read = current_save-1;
      } 
      */
      string s = history[current_read];

      return s;
    }
    else
    {
      current_read--;
      return "";
    }
  }

  vector<string> history;
  int MAX = 10;
  int current_save, current_read;
};

string moveleft(int i)
{
  return string_format("\u001b[%dD", i);
}

string moveright(int i)
{
  return string_format("\u001b[%dC", i);
}
string moveup(int i)
{
  return string_format("\u001b[%dA", i);
}
string movedown(int i)
{
  return string_format("\u001b[%dB", i);
}
string locate(int x,int y)
{
  return string_format("\u001b[%d;%dH", y,x);
}
bool __toBeUpdated;

string default_highlightfunction(string str)
{
  return str;
}

highlight_struct default_highlight={
  .extension = " ",
  .highLight =  default_highlightfunction
};
#endif