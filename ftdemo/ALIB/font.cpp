#include "img.h"
#include "io.h"
#include "math.h"
#include "siemens_unicode.h"
#include <wchar.h>
#include <de/freetype.h>

//fonts

int GetFontH (ft_font *ftf){
    return ftf->fti->h;
}

int GetLetterW (ft_font *ftf, int num){
    if (!ftf )  return 0;

    fte_symbol *fte_s = fte_get_symbol(ftf->fti, num);

    if(fte_s == 0) return -1;

    return fte_s->xadvance;
}

int GetStringW (char *str, ft_font *font){
	if (!font || !str) return 0;

	int str_w=0;

	for (int i=0; str[i]!='\0'; i++){
		int cur=str[i];
        int w=GetLetterW (font, cur);
        if (w>0) str_w+=w;
	}

	return str_w;
}

int
AIMG::
DrawLetter (ft_font *ftf, int num, int x, int y, RECT rc, color clr){
    if (!ftf )  return 0;

    fte_symbol *fte_s = fte_get_symbol(ftf->fti, num);

    if(fte_s == 0) return -1;
    if(!fte_s->bitmap) return fte_s->xadvance;

	int nx=x+fte_s->left;
	int ny=y-fte_s->top+ftf->fti->h; //ftf->fti->h FontH

	for (int j=0; j<fte_s->height; j++){
		for (int i=0; i<fte_s->width; i++){

			if (nx+i>=0 && nx+i<w_ && ny+j>=0 && ny+j<h_){
			    if (nx+i>=rc.x && nx+i<rc.x2 && ny+j>=rc.y && ny+j<rc.y2){
                    clr.A=fte_s->bitmap[i + fte_s->width*j];
                    SetColor (nx+i, ny+j, clr);
                }
			}
		}
	}

	return fte_s->xadvance;
}


int
AIMG::
DrawScrollStringLine (char *str, ft_font *font, int x1, int y1, int x2, int y2, int slide, int TEXT_ALIGN, color clr){
	if (!font || !str) return 0;

	int xy_cur=x1-slide;

	int w=GetStringW (str, font);

    if (TEXT_ALIGN&TEXT_ALIGNMIDDLE){ xy_cur+=((x2-x1)-w)/2;}
    if (TEXT_ALIGN&TEXT_ALIGNRIGHT) { xy_cur+=((x2-x1)-w);}

	for (int i=0; str[i]!='\0'; i++){

		int cur=str[i];
        RECT rc;
        rc.x=x1;
        rc.x2=x2;
        rc.y=y1;
        rc.y2=y2;
        xy_cur+=DrawLetter (font, cur, xy_cur, y1, rc, clr);

	}

}

/*
int
AIMG::
DrawLetter (TFont *font, int num, int x, int y, MyRECT rc, color clr){

	int nx=x+font->glyph_[num].left; //  /2
	int ny=y-font->glyph_[num].top+(font->GetFontH ()+font->glyph_['A'].top)/2;

	for (int j=0; j<font->glyph_[num].h; j++){
		for (int i=0; i<font->glyph_[num].w; i++){

			if (nx+i>=0 && nx+i<w_ && ny+j>=0 && ny+j<h_){
                          if (nx+i>=rc.x && nx+i<rc.x2 && ny+j>=rc.y && ny+j<rc.y2){
				clr.A=font->glyph_[num].buffer[i + font->glyph_[num].w*j];
				SetColor (nx+i, ny+j, clr);
                          }
			}
		}
	}

	return x+font->glyph_[num].left+font->glyph_[num].w+font->glyph_[num].advance_x+font->GetInterval ();
}
void
AIMG::
DrawScrollStringLine (char *str, TFont *font, int x1, int y1, int x2, int y2, int slide, int TEXT_ALIGN, color clr){
	int xy_cur=x1-slide;
	int cntrlSymb=0;
    int cur=0;

    int w=font->GetStringW (str);

    if (TEXT_ALIGN&TEXT_ALIGNMIDDLE){ xy_cur+=((x2-x1)-w)/2;}
    if (TEXT_ALIGN&TEXT_ALIGNRIGHT) { xy_cur+=((x2-x1)-w);}


	for (int i=0; str[i]!='\0'; i++){
		cntrlSymb=0;
        if (str[i]>=128) cur=str[i]-64;
        else cur=str[i];

		if (cur==' '){
			cntrlSymb=1;
			xy_cur+=font->glyph_['1'].w;
		}

		if (!cntrlSymb){
                  RECT rc;
                  rc.x=x1;
                  rc.x2=x2;
                  rc.y=y1;
                  rc.y2=y2;
                  xy_cur=DrawLetter (font, cur, xy_cur, y1, rc, clr);
                }
	}

}

void
AIMG::
DrawScrollString (char *str, TFont *font, int x1, int y1, int x2, int y2, int slide, int TEXT_ALIGN, color clr){
  int nline=0;
  int n=0;
  int s=0;
  for (int i=0; str[i]!='\0'; i++) if (str[i]!='\n') nline++;

  if (nline==0) DrawScrollStringLine (str, font, x1, y1, x2, y2, slide, TEXT_ALIGN, clr);
  else{
    char **lines=new char* [nline];
    for (int i=0; i<nline; i++) lines[i]=new char [128];

    for (int i=0; str[i]!='\0'; i++){
      if (str[i]=='\n'){
        lines[n][s]='\0';

        DrawScrollStringLine (lines[n], font, x1, y1, x2, y2, slide, TEXT_ALIGN, clr);
        y1=y1+font->GetFontH ();
        s=0;
        n++;
      }
      else{
        lines[n][s]=str[i]; s++;
      }
    }
    lines[n][s]='\0';
    DrawScrollStringLine (lines[n], font, x1, y1, x2, y2, slide, TEXT_ALIGN, clr);
    for (int i=0; i<nline; i++) if (lines[i]) delete lines[i];
    if (lines) delete lines;
  }
}

void
AIMG::
DrawString (char *str, TFont *font, int x, int y, int x2, int y2, int TEXT_ALIGN, color clr){
  DrawScrollString (str, font, x, y, x2, y2, 0, TEXT_ALIGN, clr);
}

int SetDecodeColor (color *clr, int rg, int b){
  int rg_shift=(rg>>2);
  int ngreen=rg_shift&0x3F;
  clr->G=ngreen<<2;

  clr->R=((rg_shift-ngreen)>>9)<<3;
  clr->B=((b-100)>>11)<<3;

  return 0;
}
void
AIMG::
WDrawString (WSHDR *ws, TFont *font, int x, int y, int x2, int y2, int TEXT_ALIGN, color clr){
   unsigned int sWs=((WSHDR*)ws)->wsbody[0];
   unsigned int cWs=0;
   int uflag=0;

   char *str=new char [sWs];

   int i=0;
   for (int p=0; p<sWs; p++){
      cWs=(ws)->wsbody[p+1];

      if (cWs==UTF16_INK_RGBA){
          int rg=ws->wsbody[p+1+1];
          int b=ws->wsbody[p+1+2];

          SetDecodeColor (&clr, rg, b);
          p+=2;
          uflag=1;
      }

      if (!uflag){ str[i]=char16to8(cWs); i++; }
   }
   str[i]='\0';

   DrawString (str, font, x, y, x2, y2, TEXT_ALIGN, clr);

   delete str;
}
////////////
*/
