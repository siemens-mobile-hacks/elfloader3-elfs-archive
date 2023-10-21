#include "include.h"
#include "astring.h"

//#define DEBUG

void Debug (char *str){
#ifdef DEBUG
	cout<<str<<"\n";
#endif
}

AString::
AString(){ 
	s_= NULL;
	lenght_=0;
	Debug("В конструкторе без пар-ов");
}

AString::
AString(char *s){
	lenght_=strlen(s);
	s_=new char[lenght_+1];

    if(!s_) {
        Debug("Ошибка выделения памяти");
        //exit(1);
        return;
    }
	strcpy (s_, s);
	Debug("В конструкторе");
}

AString::
~AString(){ 
	if (s_) delete (s_);  
    Debug("Освобождение памяти"); 
	Debug("В деструкторе");
}


void 
AString::
Set (const char *s){
	lenght_=strlen(s);
	s_=new char[lenght_+1];

    if(!s_) {
        Debug("Ошибка выделения памяти");
        //exit(1);
        return;
    }
	strcpy(s_, s);
	Debug("В конструкторе");
}

bool
AString::
IsEmpty(){
	if (!s_) return true;
	else return false;
}

void
AString::
Insert (char s, int pos){

	int i, j;
	char *newmas=new char[lenght_+1+1];

	for ( i=0,j=0; s_[i]!='\0'; j++){

		if (j<pos) newmas[j]=s_[i], i++;
		else {
			if (j==pos) newmas[j]=s;
			else newmas[j]=s_[i], i++;
		}
	}
	
        if (j==pos){ newmas[j]=s; j++;}
        
	newmas[j]='\0';
	Set(newmas);
	if (newmas) delete newmas;
}

void
AString::
Remove (int pos){
  
  if (!IsEmpty ()){

	if (pos>=0 && pos<lenght_){
		char *newmas=new char[lenght_+1-1];

		int j=0;
		for (int i=0; s_[i]!='\0'; i++){
			if (i!=pos){
				newmas[j]=s_[i]; j++;
			}
		}
	
		newmas[j]='\0';
		Set(newmas);
		if (newmas) delete newmas;
	}
  }
}

int
AString::
Replace (char newSimb, char oldSimb){
	int count=0;
	for ( int i=0; s_[i]!='\0'; i++)
		if ( s_[i]==oldSimb) s_[i]=newSimb, count++;
	
	return count;
}

