#include "include.h"
#include "io.h"


char empty[]="";

#ifdef WIN
#include <stdio.h>
#endif

#ifdef WIN
int GetFileSize (char *fname){
	FILE *file=fopen (fname, "rb");
	if (!file) return -1;

	fseek (file, 0, SEEK_END);
	int size=ftell (file);
	fclose (file);

	return size;
}
#else
int GetFileSize (char *fname){
  unsigned int err;
  FSTATS fs;
  GetFileStats (fname, &fs, &err);

  return fs.size;
}
#endif


int FileReadToBuffer (char *fname,  char *buffer, int size){

#ifdef WIN
	 FILE *file=fopen (fname, "rb");

	 fread (buffer, size, 1, file);
	 fclose (file);
#else
	  unsigned int err;
	  int f=_open (fname, A_ReadOnly+A_BIN, P_READ, &err);

	  _read (f, buffer, size, &err);
	  _close (f, &err);
#endif

	  return 0;
}


char dir[128];
char file[128];
char path[128];
char *GetFileName(char* fname)
{
	int len = strlen(fname);
	int ii;
	for(ii = len-1; ii >= 0; ii--) {
		if (fname[ii] == '\\' || fname[ii] == '/') break;
	}
	if (ii>=0) return fname+ii+1;
	return fname;
}

int GetFileDir (char *fname, char *buf){
	int ii;
	int len = strlen(fname);

	for(ii = len-2; ii > 0; ii--)
		if (fname[ii] == '\\' || fname[ii] == '/') break;
	len = ii;

	if (buf)
	{
		for(ii=0; ii<len; ii++) buf[ii] = fname[ii];
		buf[len] = 0;
	}
	return len;
}

void SetCurDir (char *fname){
  GetFileDir (fname, dir);
}

char *GetCurDir (){
  return dir;
}

void SetCurPath (char *fname){
  sprintf (path, "%s", fname);
}

void SetCurFile (char *fname){
  sprintf (file, "%s",  (fname));
//  sprintf (file, "%s", GetFileName (fname));
}

char *GetCurFile (){
  return file;
}

int str2int (char *str){
  int n = 0;
  sscanf(str, "%d", &n);
  return(n);
}


int char16to8 (int c){
  //128 ANSI
  if (c<0x400) return (c);

  c-=0x400;

  if (c==0x01) return 192+5;//Ё ->E c=0;
  if (c==0x51) return 192+32+5;//ё->е c=16;

  if  (c>=0x010 && c<=0x04F) return c-0x010+192;

  return 0;
}


void ws2str (char *str, WSHDR *ws){
   unsigned int sWs=((WSHDR*)ws)->wsbody[0];
   unsigned int cWs=0;

   int p=0;
   for (p=0; p<sWs; p++){
      cWs=(ws)->wsbody[p+1];
      str[p]=char16to8(cWs);
   }
   str[p]='\0';
}

#define LOG_FILE "0:\\Zbin\\ALib\\alib.log"
char tmp[128];
void Log (char *txt)
{
  unsigned int ul;
  int f=_open(LOG_FILE,A_ReadWrite+A_Create+A_Append+A_BIN,P_READ+P_WRITE,&ul);
  if (f!=-1)
  {
    _write(f,txt,strlen(txt),&ul);
    _close(f,&ul);
  }
}
void iLog (unsigned int i)
{
  unsigned int ul;
  int f=_open(LOG_FILE,A_ReadWrite+A_Create+A_Append+A_BIN,P_READ+P_WRITE,&ul);
  if (f!=-1)
  {
    sprintf (tmp, "%d\n", i);
    _write(f,tmp,strlen(tmp),&ul);
    _close(f,&ul);
  }
}



#ifndef WIN
void RunAction (char *s)
{
  if((s)&&strlen(s))
  {
    if (s[0]=='M'){
      //WSHDR  *ws=AllocWS(128);
      int len=strlen(s);
      char *m=new char [len-1];
      for (int i=0; i+1<len; i++) m[i]=s[i+1];
      m[len-1]=0;
      //str_2ws(ws, wayToMC, 128);

      //ExecuteFile(ws, 0, m);
      //FreeWS(ws);
      //OpenInMC (m);
      mfree(m);
    }
    else{
      if (s[0]=='J'){
        int len=strlen(s);
        //LogInt (len+100);
        char *m=new char (len-1);
        for (int i=0; i+1<len; i++) m[i]=s[i+1];
        //m[len-1]='\0';
        //Log (m);
        int mID=str2int (m);
        //LogInt (mID);
        //SetCurPosition (mID);
        mfree(m);
      }
      else{

      if ((s[1]==':')&&(s[2]=='\\')){
        WSHDR  *ws=AllocWS(128);
        str_2ws(ws,s,strlen(s)+1);
        ExecuteFile(ws,0,NULL);
        FreeWS(ws);
   }else
   {
    if ((s[0]!='a')&&(s[0]!='A')&&(s[1]!='0'))
    {unsigned int* addr = (unsigned int*)GetFunctionPointer(s);
      if (addr){
        typedef void (*voidfunc)();
        //voidfunc pp=(voidfunc)*(addr+4);
#ifdef NEWSGOLD
        voidfunc pp=(voidfunc)*(addr+4);
#else
        voidfunc pp=(voidfunc)addr;
#endif
        SUBPROC((void*)pp);
      }
    }

    if (((s[0]=='a')||(s[0]=='A'))&&(s[1]=='0'))
    { int entry; sscanf(s,"%08X",&entry); SUBPROC((void*)entry);}
   }
  }
}
  }
}

#endif
