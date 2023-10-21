#include "platform.h"


//const char per_s[]="%s";
extern char empty[];

int GetFileSize (char *fname);

int FileReadToBuffer (char *fname, char *buffer, int size);

int GetFileDir (char *fname, char *buf);
void SetCurDir (char *fname);
char *GetCurDir ();
void SetCurFile (char *fname);
char *GetCurFile ();

int char16to8 (int c);
void ws2str (char *str, WSHDR *ws);

void iLog (unsigned int i);
void Log (char *txt);

int str2int (char *str);

void RunAction (char *s);

class AFILE{
  char *buf_;
  int count_;
  int cur_;
public:
	AFILE (){buf_=NULL; count_=0; cur_=0;}
	~AFILE (){}

	int GetCount (){ return count_;}
	int GetCur (){ return cur_;}
        unsigned char GetCurCh (){ cur_++; return buf_[cur_-1];}

	int Open (char *fname){
		count_=GetFileSize(fname);
		if (count_>0){
			buf_=new  char [count_];
			FileReadToBuffer (fname, buf_, count_);

			return count_;
		}
		else return -1;
	}

	void Close (){
		if (buf_){
			delete buf_;
		}
	}

	int GoToSymb (char s){
		while (cur_<count_){
			if (buf_[cur_]!=s) cur_++;
			else return ++cur_;
		}
		return -1;
	}

	int CopyToSymb ( char *str,  char s){
		int i=0;
		while (cur_<count_){
			if (buf_[cur_]!=s){ str[i]=buf_[cur_]; cur_++; i++;}
			else{ str[i]='\0'; return ++cur_;}
		}
		str[i]='\0';
		return -1;
	}
};
