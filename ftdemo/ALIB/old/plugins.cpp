/*#include "include.h"
#include "plugin.h"

#include "io.h"

AIBAR obj[10];

int count=0;

char per[]="\\";

void _StartPLG (char *way, AIBAR *plg){

  if (GetFileSize(way)>0){

    WSHDR *wsbuf=AllocWS(256);
    str_2ws(wsbuf, way, 256);

    ExecuteFile(wsbuf, NULL, plg);

    FreeWS(wsbuf);
  }
}


void DrawPLG (){

  for (int i=0; i<count; i++){
    if (obj[i].InitPLG==1) obj[i].OnRedraw();
  }
}

void _SearchPLG (char *path){
  unsigned int err;
  DIR_ENTRY de;
  char name[256];
  char way[256];

  strcpy(name, path);
  strcat(name, "\\*.elf");

  if (FindFirstFile(&de,name,&err))
  {
    do
    {
       strcpy (way, path);
       strcat (way, "\\");
       strcat (way, de.file_name);

       //obj[count]=(AIBAR*)malloc(sizeof(AIBAR));

       _StartPLG (way, &obj[count]);

       count++;
    }

    while(FindNextFile(&de,&err));

#ifdef NEWSGOLD
    FindClose(&de, &err);
#endif
   }
#ifndef NEWSGOLD
   FindClose(&de, &err);
#endif
}


void SearchPLG (char *way){
  unsigned int err;

  char *buf=(char*)malloc(256);
  strcpy(buf, way);
  strcat (buf, per);
  strcat(buf, "Plugins");
  if (!isdir(buf, &err)){
    mkdir(buf, &err);
  }

  _SearchPLG (buf);

  mfree(buf);
}

void ClosePLG(){
  //for (int i=0; i<count; i++) mfree(obj[i]);
}
*/
