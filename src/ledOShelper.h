#include <string>

void editorleft(Console *cons)
{
  if (cons->internal_coordinates.x > 1)
  {
    cons->internal_coordinates.x--;
    _push(config.BACK);
  }
}
void editorright(Console *cons)
{
  if (cons->internal_coordinates.x <= cons->sentence.size())
  {
    cons->internal_coordinates.x++;
    _push(config.FORWARD);
  }
}
void extraEscCommand(Console *cons)
{
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
        // editorup(cons);
      }
      else
      {
        if (cons->sentence.size() >= 1)
          _push(moveleft(cons->sentence.size()).c_str());
        cons->sentence = cons->commands.getCommandHistoryUp();
        cons->search_sentence = cons->sentence;
        cons->internal_coordinates.x = cons->sentence.size() + 1;
        _push(cons->sentence.c_str());
        _push(config.ERASE_FROM_CURSOR_TO_EOL);
        // Serial.printf("%d",cons->internal_coordinates.x);
      }
    }
    break;
    case 66:
      if (cons->cmode == edit)
      {
        // editordown(cons);
      }
      else
      {
        if (cons->sentence.size() >= 1)
          _push(moveleft(cons->sentence.size()).c_str());
        cons->sentence = cons->commands.getCommandHistoryDown();
        cons->search_sentence = cons->sentence;
        cons->internal_coordinates.x = cons->sentence.size() + 1;
        _push(cons->sentence.c_str());
        _push(config.ERASE_FROM_CURSOR_TO_EOL);
      }
      break;
    case 67:
      editorright(cons);
      break;
    case 68:
      editorleft(cons);
      break;
    default:
      break;
    }
  }
}
static void cls(Console *cons, vector<string> args)
{

  Serial.print("\u001b[2J\u001b[0;0H");
  cons->internal_coordinates.y = 1;
  cons->internal_coordinates.internaly = 0;
  cons->internal_coordinates.x = 1;
}

void cls(Console *cons)
{
  vector<string> args;
  cls(cons, args);
}

string defaultPrompt(Console *cons)
{
  return string_format("%sLedOS>", cons->defaultformat.c_str(), cons->internal_coordinates.x, cons->internal_coordinates.y);
}

string editPrompt(Console *cons)
{
  return string_format("%s%3d %s ", cons->editprompt.c_str(), cons->internal_coordinates.y, cons->editcontent.c_str());
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
  __toBeUpdated=true;
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
  vector<string> j;
  string search_sentence;
   j=split(cons->sentence," ");
      
       search_sentence=j[j.size()-1];
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
      cons->searchContent.current_read=0;
      __toBeUpdated = false;
      // Serial.printf("%d",cons->searchContent.history.size());
    }
  }
  string f = cons->searchContent.getCommandHistoryCircular();
  if (f.compare("") != 0)
  {
    _push(moveleft(search_sentence.size()).c_str());
    if(j.size()>1)
    {
     cons->sentence=j[0];
    for(int i=1;i<j.size()-1;i++)
    {
      cons->sentence=cons->sentence+" "+j[i];
    }
     cons->sentence=cons->sentence+" "+f;
    }
    else
    {
    cons->sentence=f;
    }
    cons->internal_coordinates.x = cons->sentence.size() + 1;
    _push(f.c_str());
    _push(config.ERASE_FROM_CURSOR_TO_EOL);
  }
}

void load(Console *cons, vector<string> args)
{
  _push(config.ENDLINE);
  if(args.size()<1)
  {
        printf("%s", termColor.Red);
    printf("Error:%s", "No argument provided");
    printf("%s\r\n", config.ESC_RESET);
    return;
  }
  //string buff;
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
    display();
  }
}



void type(Console *cons, vector<string> args)
{
  _push(config.ENDLINE);
  _push(termColor.Cyan);
  if(args.size()<1)
  {
   for(string j:cons->script)
   {
    _push(j.c_str());
    _push(config.ENDLINE);
   }
   _push(config.ESC_RESET);
    return;
  }
  
  else{
    load(cons,args);
     _push(config.ENDLINE);
  _push(termColor.Cyan);

   for(string j:cons->script)
   {
    _push(j.c_str());
    _push(config.ENDLINE);
   }
   _push(config.ESC_RESET);

  }
}


void initEscCommands(Console *cons)
{
  // cons->addEscCommand(16, enterProgMode);
  cons->addEscCommand(27, extraEscCommand);
  // cons->addEscCommand(15, test);
  // cons->addEscCommand(5, scrollup);
  // cons->addEscCommand(18, compilerun);
  // cons->addEscCommand(22, switchfooter);
  cons->addKeywordCommand("ls", ls);
  cons->addKeywordCommand("cls", cls);
   cons->addKeywordCommand("load", load);
   cons->addKeywordCommand("type", type);
  //  cons->addEscCommand(27,top);
}
