#include "include.h"
#include "font.h"
#include "io.h"


char FNT_HEADER[]="CHFNT";
int SIZE_HEADER=5;


TFont:: ~TFont (){
  for (int i=0; i<glyphCount_; i++)
    if (glyph_[i].buffer) delete [] (glyph_[i].buffer);

  delete [] glyph_;


}

int
TFont::LoadGlyphs ( char *buf, int count){
  int gcount=0;
  int i=0;

  while (i<count && gcount<count_){
    i=i+SIZE_HEADER;
    glyph_[gcount].w=buf[i]; i++;
    glyph_[gcount].h=buf[i]; i++;

    if (buf[i]>127) glyph_[gcount].left=buf[i]-255;
    else glyph_[gcount].left=buf[i];
    i++;
    glyph_[gcount].top=buf[i]; i++;


    int bufsize=glyph_[gcount].w*glyph_[gcount].h;

    glyph_[gcount].buffer=new  char [bufsize];

    for (int k=0; k<bufsize; k++){
      glyph_[gcount].buffer[k]=buf[i];
      i++;
    }

    gcount++;
  }

  return gcount;
}

int
TFont::GetMaxFontYSize (){
  int height=0;
  for (int i=0; i<count_; i++)
	  if (height<glyph_[i].h) height=glyph_[i].h;

  return height;
}

int
TFont::GetStringW (char *str){
  int w=0;
  int cur=0;
  for (int i=0; str[i]!='\0'; i++){
    if (str[i]>=128) cur=str[i]-64;
    else cur=str[i];
    if (cur==' ') w+=glyph_['1'].w;
    else w+=glyph_[cur].left+glyph_[cur].w+ltr_w_;
  }
  return w;
}

void
TFont::Init (char *fname){

  int fsize=GetFileSize(fname);

  if (fsize>0){
	  char *buffer=new char [fsize];

	  FileReadToBuffer (fname, buffer, fsize);


	  glyphCount_=LoadGlyphs (buffer, fsize);

	  delete [] buffer;

	  YMax_=GetMaxFontYSize();
	  ltr_w_=glyph_['A'].h/15;
  }

}
