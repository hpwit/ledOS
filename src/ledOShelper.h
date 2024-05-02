#include <string>
#include <list>
#include "ledOStruct.h"

void editorleft(Console *cons)
{
  if (cons->internal_coordinates.line_x > 1)
  {
    cons->internal_coordinates.line_x--;
    _push(config.BACK);
  }
}
void editorright(Console *cons)
{
  if (cons->internal_coordinates.line_x <= cons->sentence.size())
  {
    cons->internal_coordinates.line_x++;
    cons->internal_coordinates.cursor_x++;
    _push(config.FORWARD);
  }
}

void promptup(Console *cons)
{
  if (cons->sentence.size() >= 1)
    _push(moveleft(cons->sentence.size()).c_str());
  cons->sentence = cons->commands.getCommandHistoryUp();
  cons->search_sentence = cons->sentence;
  cons->internal_coordinates.line_x = cons->sentence.size() + 1;
  cons->internal_coordinates.cursor_x = cons->sentence.size() + 1;
  _push(cons->sentence.c_str());
  _push(config.ERASE_FROM_CURSOR_TO_EOL);
}

void promptdown(Console *cons)
{
  if (cons->sentence.size() >= 1)
    _push(moveleft(cons->sentence.size()).c_str());
  cons->sentence = cons->commands.getCommandHistoryDown();
  cons->search_sentence = cons->sentence;
  cons->internal_coordinates.line_x = cons->sentence.size() + 1;
  cons->internal_coordinates.cursor_x = cons->sentence.size() + 1;
  _push(cons->sentence.c_str());
  _push(config.ERASE_FROM_CURSOR_TO_EOL);
}


void editorup(Console *cons)
{
  if (cons->internal_coordinates.line_y > 1)
  {
    _push(config.HIDECURSOR);
    list<string>::iterator k = cons->getLineIterator(cons->internal_coordinates.line_y - 1);
    if (k == cons->script.end())
    {
      cons->script.push_back(cons->sentence);
    }
    else
    {
      // printf("jjdd");
      cons->script.erase(k);
      // printf("jj");
      k = cons->getLineIterator(cons->internal_coordinates.line_y - 1);
      // k++;
      cons->script.insert(k, cons->sentence);
      // printf("sdd");
    }
if( cons->internal_coordinates.cursor_y>=2)
{
    cons->internal_coordinates.line_y--;
    cons->internal_coordinates.cursor_y--;
    k = cons->getLineIterator(cons->internal_coordinates.line_y - 1);
    cons->sentence = *k;
    cons->internal_coordinates.cursor_x = (*k).size()+1;
    cons->internal_coordinates.line_x = (*k).size() + 1;
    _push(config.MOVEUP);
    _push(config.BEGIN_OF_LINE);
    _push(cons->prompt(cons).c_str());
    //_push(moveright((*k).size()+4));
    _push(cons->current_hightlight->highLight(cons->sentence).c_str());
    _push(config.ERASE_FROM_CURSOR_TO_EOL);
    _push(config.SHOWCURSOR);
}
else{
      cons->internal_coordinates.line_y--;
    //cons->internal_coordinates.cursor_y--;
    k = cons->getLineIterator(cons->internal_coordinates.line_y - 1);
    cons->sentence = *k;
    cons->internal_coordinates.cursor_x = (*k).size()+1;
    cons->internal_coordinates.line_x = (*k).size() + 1;
    _push(config.SCROLLDOWN);
    //_push(config.MOVEUP);
    _push(config.BEGIN_OF_LINE);
    _push(cons->prompt(cons).c_str());
    //_push(moveright((*k).size()+4));
    _push(cons->current_hightlight->highLight(cons->sentence).c_str());
    _push(config.ERASE_FROM_CURSOR_TO_EOL);
    _push(config.SHOWCURSOR);
}
  }
}

void editordown(Console *cons)
{
  if (cons->internal_coordinates.line_y <=cons->script.size() - 1)
  {
    _push(config.HIDECURSOR);
    list<string>::iterator k = cons->getLineIterator(cons->internal_coordinates.line_y - 1);
    if (k == cons->script.end())
    {
      cons->script.push_back(cons->sentence);
    }
    else
    {
      // printf("jjdd");
      cons->script.erase(k);
      // printf("jj");
      k = cons->getLineIterator(cons->internal_coordinates.line_y - 1);
      // k++;
      cons->script.insert(k, cons->sentence);
      // printf("sdd");
    }
if(cons->internal_coordinates.cursor_y<height-1)
{
    cons->internal_coordinates.line_y++;
    cons->internal_coordinates.cursor_y++;
    k = cons->getLineIterator(cons->internal_coordinates.line_y - 1);
    cons->sentence = *k;
    cons->internal_coordinates.cursor_x = (*k).size()+1;
    cons->internal_coordinates.line_x = (*k).size() + 1;
    _push(config.MOVEDOWN);
    _push(config.BEGIN_OF_LINE);
    _push(cons->prompt(cons).c_str());
    //_push(moveright((*k).size()+4));
    _push(cons->current_hightlight->highLight(cons->sentence).c_str());
    _push(config.ERASE_FROM_CURSOR_TO_EOL);
    _push(config.SHOWCURSOR);
  }
  else
  {
     _push(config.SCROLLUP);
     _push(config.DELINE);
        cons->internal_coordinates.line_y++;
   // cons->internal_coordinates.cursor_y++;
    k = cons->getLineIterator(cons->internal_coordinates.line_y - 1);
    cons->sentence = *k;
    cons->internal_coordinates.cursor_x = (*k).size()+1;
    cons->internal_coordinates.line_x = (*k).size() + 1;
    //_push(config.MOVEDOWN);
    _push(config.BEGIN_OF_LINE);
    _push(cons->prompt(cons).c_str());
    //_push(moveright((*k).size()+4));
    _push(cons->current_hightlight->highLight(cons->sentence).c_str());
    _push(config.ERASE_FROM_CURSOR_TO_EOL);
    _push(config.SHOWCURSOR);
  }
  }
}

void mouseMouvement(Console *cons)
{
  char c1 = Serial.read();
  char c2 = Serial.read();
  char c3 = Serial.read();
  //we empty the buffer
  while(Serial.available()>0)
  {
    char f=Serial.read();
  }
  switch (c1)
  {
  case 97:
    if (cons->cmode == edit)
    {
      editorup(cons);
    }
    else
    {

      promptup(cons);
    }
    break;
  case 96:
    if (cons->cmode == edit)
    {
     editordown(cons);
    }
    else
    {

      promptdown(cons);
    }
    break;
case 35:
//printf("click in %d %d\r\n",c2-32,c3-32);
if(cons->cmode==edit)
{
  int newx=c2-32;
  int newy=c3-32;
if(newx<=5)
 return;

  if(newy<cons->height)
  {
  //internal_coordinates.cursor_y = savcy;
  //internal_coordinates.line_y = savly;
    int newline =cons->internal_coordinates.line_y -cons->internal_coordinates.cursor_y+newy;
    if(newline >= cons->script.size())
     return;
    cons->internal_coordinates.line_y=newline;
    cons->internal_coordinates.cursor_y=newy;
    list<string>::iterator k =cons->getLineIterator(cons->internal_coordinates.line_y-1);
    cons->sentence=(*k);
    if(newx-5> (*k).size())
    {
      newx=(*k).size()+6;
    }
    cons->internal_coordinates.line_x=newx-5;
    cons->internal_coordinates.cursor_x=newx-5;
    _push(locate(newx,newy).c_str());

  }
}
break;
  default:
  //printf("%c %c %c\r\n",c1,c2,c3);
    break;
  }
}
void extraEscCommand(Console *cons)
{
  cons->displayf=true;
  char c1 = Serial.read();
  char c2 = Serial.read();
  if (c1 == 91)
  {
    switch (c2)
    {
    case 65:
    {
      if (cons->cmode == edit)
      {
        editorup(cons);
      }
      else
      {

        promptup(cons);
      }
    }
    break;
    case 66:
      if (cons->cmode == edit)
      {
        editordown(cons);
      }
      else
      {
        promptdown(cons);
      }
      break;
    case 67:
      editorright(cons);
      break;
    case 68:
      editorleft(cons);
      break;

    case 'M':
    {
      mouseMouvement(cons);
    }
    break;
    default:
      while (Serial.available() > 0)
      {
        Serial.printf("%c; %d : ", c2, Serial.read());
      }
      _push(config.ENDLINE);
      break;
    }
  }
}
static void cls(Console *cons, vector<string> args)
{

  _push("\u001b[2J\u001b[1000A\u001b[1000D");
  cons->internal_coordinates.cursor_y = 1;
  cons->internal_coordinates.line_y = 1;

  cons->internal_coordinates.line_x = 1;
  cons->internal_coordinates.cursor_x = 1;
}

void cls(Console *cons)
{
  vector<string> args;
  cls(cons, args);
}

string defaultPrompt(Console *cons)
{
  return string_format("%s%sLedOS>", config.ENABLE_MOUSE, cons->defaultformat.c_str());
}

string editPrompt(Console *cons)
{
  return string_format("%s%3d %s ", cons->editprompt.c_str(), cons->internal_coordinates.line_y, cons->editcontent.c_str());
}

void ls(Console *cons, vector<string> args)
{
  string opt = "";
  if (args.size() > 0)
    opt = args[0];
  _push(config.ENDLINE);
  bool er = fileSystem.ls(opt);

  if (!er)
  {
    printf("%s", termColor.Red);
    printf("%s", fileSystem.result.back().c_str());
    printf("%s\r\n", config.ESC_RESET);
  }
  else
  {

    if (opt.compare("-l") == 0)
    {

      for (string h : fileSystem.result)
      {
        printf("%s\r\n", h.c_str());
      }
    }

    else
    {
      getConsoleSize();
      int nb_col = width / 20;
      int j = 0;
      // list<string>::iterator it=fileSystem.result.begin();
      for (list<string>::iterator it = fileSystem.result.begin(); it != fileSystem.result.end(); ++it)
      {
        list<string>::iterator it2 = fileSystem.result.end();
        it2--;
        if (it2 != it)
        {
          printf("%s", (*it).c_str());
          j++;
          if (j % nb_col == 0)
          {
            printf("\r\n");
          }
        }
      }
      list<string>::iterator it = fileSystem.result.end();
      it--;
      if (j % nb_col != 0)
      {
        printf("\r\n");
      }
      printf("%s\r\n", (*it).c_str());
    }
  }
  __toBeUpdated = true;
}

void rm(Console *cons, vector<string> args)
{
  if (args.size() > 0)
  {
    string name = args[0];
    bool er = fileSystem.rm(name);
    if (!er)
    {
      printf("%s", termColor.Red);
      printf("Error:%s", fileSystem.result.back().c_str());
      printf("%s\r\n", config.ESC_RESET);
    }
    else
    {
      display();
    }
    // cons->searchContent.clear();
    __toBeUpdated = true;
  }
  else
  {

    printf("%s", termColor.Red);
    printf("Error:%s", "No argument provided");
    printf("%s\r\n", config.ESC_RESET);
  }
}

void manageTabulation(Console *cons)
{
  if (cons->sentence.size()<1)
  return;
  vector<string> j;
  string search_sentence;
  j = split(cons->sentence, " ");

  search_sentence = j[j.size() - 1];
  if (search_sentence.size() < 1)
  {
    return;
  }
  else
  {
    if (__toBeUpdated)
    {

      fileSystem.ls("w");
      cons->searchContent.clear();

      for (string h : fileSystem.result)
      {
        string l = h.substr(0, search_sentence.size());
        if (l.compare(search_sentence) == 0)
        {

          cons->searchContent.addCommandToHistory(h);
        }
      }
      cons->searchContent.current_read = 0;
      __toBeUpdated = false;
      // Serial.printf("%d",cons->searchContent.history.size());
    }
  }
  string f = cons->searchContent.getCommandHistoryCircular();
  if (f.compare("") != 0)
  {
    _push(moveleft(search_sentence.size()).c_str());
    if (j.size() > 1)
    {
      cons->sentence = j[0];
      for (int i = 1; i < j.size() - 1; i++)
      {
        cons->sentence = cons->sentence + " " + j[i];
      }
      cons->sentence = cons->sentence + " " + f;
    }
    else
    {
      cons->sentence = f;
    }
    cons->internal_coordinates.cursor_x = cons->sentence.size() + 1;
    cons->internal_coordinates.line_x = cons->sentence.size() + 1;
    _push(f.c_str());
    _push(config.ERASE_FROM_CURSOR_TO_EOL);
  }
}

void save(Console *cons, vector<string> args)
{
  _push(config.ENDLINE);

  if ((args.size() < 1) && (cons->filename.compare("")==0) && args[0].size()>0)
  {
    _push(termColor.Red);
    _push("Error: No argument provided");
    _push(config.ESC_RESET);
    _push(config.ENDLINE);
    return;
  }
  // string buff;
  string filename=cons->filename;
  // cons->script.clear();
  if(args.size()>0)
  {
    filename=args[0];
   
  }

  bool er = fileSystem.save(filename, &cons->script);
  if (!er)
  {
    printf("%s", termColor.Red);
    printf("%s", fileSystem.result.back().c_str());
    printf("%s\r\n", config.ESC_RESET);
  }
  else
  {
    cons->filename = filename;
     cons->scriptModified=false;
    cons->setHighlight(filename);
    display();
  }
}

void load(Console *cons, vector<string> args)
{
  _push(config.ENDLINE);
  if (args.size() < 1)
  {
    printf("%s", termColor.Red);
    printf("Error:%s", "No argument provided");
    printf("%s\r\n", config.ESC_RESET);
    return;
  }
  // string buff;
  cons->script.clear();
  bool er = fileSystem.load(args[0], &cons->script);
 
  if (!er)
  {
    printf("%s", termColor.Red);
    printf("%s", fileSystem.result.back().c_str());
    printf("%s\r\n", config.ESC_RESET);
  }
  else
  {
     cons->scriptModified=false;
    cons->filename = args[0];
    cons->setHighlight(args[0]);
    display();
  }
}

void clear(Console *cons, vector<string> args)
{
  cons->script.clear();
  cons->filename = "";
  cons->setHighlight("");
  _push(config.ENDLINE);
   cons->scriptModified=false;
}
void type(Console *cons, vector<string> args)
{
  _push(config.ENDLINE);
  _push(termColor.Cyan);
  if (cons->current_hightlight->init != NULL)
    cons->current_hightlight->init();
  if (args.size() < 1)
  {
    for (string j : cons->script)
    {
      // Òcons->current_hightlight->highLight(sentence)
      if (cons->current_hightlight->newLine != NULL)
        cons->current_hightlight->newLine();
      _push(cons->current_hightlight->highLight(j).c_str());
      _push(config.ENDLINE);
    }
    _push(config.ESC_RESET);
    return;
  }

  else
  {
    load(cons, args);
    _push(config.ENDLINE);
    _push(termColor.Cyan);

    for (string j : cons->script)
    {
      if (cons->current_hightlight->newLine != NULL)
        cons->current_hightlight->newLine();
      _push(cons->current_hightlight->highLight(j).c_str());
      _push(config.ENDLINE);
    }
    _push(config.ESC_RESET);
  }
}

void exitProgMode(Console *cons)
{
  if (cons->cmode == edit or cons->cmode==paste)
  {

    // if (cons->internal_coordinates.line_y <= cons->script.size())
    //{
    if (cons->sentence.size() > 0)
    {
      list<string>::iterator k = cons->getLineIterator(cons->internal_coordinates.line_y - 1);
      if (k == cons->script.end())
      {
        cons->script.push_back(cons->sentence);
      }
      else
      {
        cons->script.erase(k);
        // printf("jj");
        k = cons->getLineIterator(cons->internal_coordinates.line_y - 1);
        if(k==cons->script.end())
        {
          cons->script.push_back(cons->sentence);
        }
else
        {
          cons->script.insert(k, cons->sentence);
        }
      }
      // }

      cons->sentence = "";
      // cons->gotoline();
    }
    else
    {
      // cons->script[cons->internal_coordinates.y - 1] = cons->sentence;
      cons->sentence = "";
    }
    cons->cmode = keyword;
    cons->prompt = &defaultPrompt;
    cons->currentformat = cons->defaultformat;
    /*
    Serial.printf("%s",config.ESC_RESET);
    Serial.printf("%s",termColor.Magenta);
    Serial.printf("Exiting edit mode to re enter Ctrl+p");
    */
    // cls(cons);
    cons->displayf = false;
    // cons->gotoline();
    // Serial.printf("%s",config.ESC_RESET);
    //_push(cons->prompt(cons).c_str());
    // cons->gotoline();
  }
}
void enterProgMode(Console *cons)
{
  // getConsoleSize();
  if (cons->cmode == keyword)
  {
    _push(config.SAVESCREEN);
    cons->displayf = true;
    cons->cmode = edit;
    cons->height = 200;
    cons->width = 200;
    cons->prompt = &editPrompt;
    cons->sentence = "";
    cons->currentformat = cons->editcontent;
    getConsoleSize();
    cons->height = height;
    cons->width = width;
    cons->internal_coordinates.cursor_y = 1;
    cons->internal_coordinates.line_y = 1;
    cons->internal_coordinates.cursor_x = 1;
    cons->internal_coordinates.line_x = 1;
    cons->setHighlight(cons->filename);
    cls(cons);
    cons->displayf = true;
    if (cons->current_hightlight->init)
      cons->current_hightlight->init();
    // _list(cons, 1, cons->height - 1);
    if (cons->script.size() == 0)
    {
      if (cons->script.size() < cons->height - 1)
      {
        _push(cons->prompt(cons).c_str());

      }
    }
    else
    {
      cons->_list();
    }
  }
  else
  {
    
    exitProgMode(cons);
    _push(config.RESTORESCREEN);
  }
}


void saveFromEditor(Console *cons)
{
  if(cons->filename.size()<1)
  {
    cons->pushToConsole(string_format("%s%s%s",termColor.Red,"Error: No argument provided",config.ESC_RESET));
   // vTaskDelay(600);
  }
   else if(cons->script.size()<1)
  {
    cons->pushToConsole(string_format("%s%s%s",termColor.Red,"Empty file",config.ESC_RESET));
    //vTaskDelay(600);
  }
  else
  {
    bool er = fileSystem.save(cons->filename, &cons->script);
    if (!er)
  {
        cons->pushToConsole(string_format("%s%s%s",termColor.Red,"Impossible to save file",config.ESC_RESET));
    
  }
  else{
    cons->scriptModified=false;
     cons->pushToConsole("File saved");
  }
  }
  vTaskDelay(600);
  cons->pushToConsole("");
}

void modePaste(Console *cons)
{
  cons->displayf=true;
  if(cons->cmode==edit)
  {
    cons->cmode=paste;
       _push(config.SAVE);
   _push(config.HIDECURSOR);
   _push(config.DOWN);
   _push(config.LEFT);
   _push(config.DELINE);
   _push("PASTE MODE ACTIVATED");
        _push(config.RESTORE);
    _push(config.SHOWCURSOR);

  }
  else if(cons->cmode==paste)
  {
    //cls(cons);
   cons->cmode=keyword;
   
    enterProgMode(cons);
   
  }
}

void initEscCommands(Console *cons)
{
  cons->addEscCommand(5, enterProgMode);
  cons->addEscCommand(27, extraEscCommand);
 cons->addEscCommand(16, modePaste);
  cons->addEscCommand(19, saveFromEditor);
  // cons->addEscCommand(5, scrollup);
  // cons->addEscCommand(18, compilerun);
  // cons->addEscCommand(22, switchfooter);
  cons->addKeywordCommand("ls", ls);
  cons->addKeywordCommand("cls", cls);
  cons->addKeywordCommand("load", load);
  cons->addKeywordCommand("save", save);
  cons->addKeywordCommand("type", type);
  cons->addKeywordCommand("clear", clear);
  //  cons->addEscCommand(27,top);
}
