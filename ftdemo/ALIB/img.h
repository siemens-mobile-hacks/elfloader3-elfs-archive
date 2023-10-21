#pragma once

#include "platform.h"
#include "include.h"
//#include "font.h"
#include <de/freetype.h>


#define RGB16(R, G, B) (((B>>3)&0x1F) | ((G<<3)&0x7E0) | ((R<<8)&0xF800))
#define a16pixel 0xE000 //- transparent pixel

typedef struct{
  unsigned char R;
  unsigned char G;
  unsigned char B;
  unsigned char A;
}color;

struct MyRECT{
	int x;
	int y;
	int x2;
	int y2;
};


color RGBA(unsigned char R, unsigned char G, unsigned char B, unsigned char A);
color BGRA(unsigned char R, unsigned char G, unsigned char B, unsigned char A);

class AIMG{
	int w_;
	int h_;
	int bpnum_;
	unsigned char *bitmap_;
public:
	AIMG (){ w_=0; h_=0; bpnum_=0; bitmap_=0;}
    AIMG (int w, int h, int bpnum, unsigned char *bitmap){ w_=w; h_=h; bpnum_=bpnum; bitmap_=bitmap;}
	~AIMG (){
		w_=0; h_=0; bpnum_=0;
		if (bitmap_){ delete [] bitmap_; bitmap_=NULL;}
	}

	int GetW (){ return w_;}
	int GetH (){ return h_;}
	int GetBtype (){ return bpnum_;}
	unsigned char **GetBitmap (){ return &bitmap_;}
    void FreeBitmap (){ if (bitmap_){ delete [] bitmap_; bitmap_=NULL;}}
	color GetColor (int x, int y);
	void SetColor (int x, int y, color clr);

    unsigned short GetColor16 (int x, int y);
    void SetColor16 (int x, int y, unsigned short clr);

	void Clean ();
	void Create (int w, int h, int bpnum);
	int CreateFromJPEG (char *fname);
    void SaveInJPEG (char *file);
	int CreateFromPNG (char *fname);

	void Draw (int x, int y);
	void DrawLayer (AIMG *img, int x, int y);
    void DrawLayerRECT (AIMG *img, int x, int y, MyRECT rc);
	void DrawLine (int x, int y, int x2, int y2, color clr);
	void DrawRect (int x, int y, int x2, int y2, color clr);
	void DrawOldCircle (int x, int y, int R, color brush, color pen);
	void DrawCircle (int x, int y, int R, color brush, color pen);

	void FlipHoriz ();
	void FlipVertic ();
	void Rotate90 ();
	int Rotate (int ang);
	int FRotate (float angle);
	int Resize (int px, int py);
	int FResize (float k);
	void SetAlpha (unsigned char alpha);

	int DrawLetter (ft_font *ftf, int num, int x, int y, RECT rc, color clr);
	int DrawScrollStringLine (char *str, ft_font *font, int x1, int y1, int x2, int y2, int slide, int TEXT_ALIGN, color clr);
    /*void DrawScrollStringLine (char *str, TFont *font, int x1, int y1, int x2, int y2, int slide, int TEXT_ALIGN, color clr);
	void DrawString (char *str, TFont *font, int x, int y, int x2, int y2, int TEXT_ALIGN, color clr);
    void DrawScrollString (char *str, TFont *font, int x, int y, int x2, int y2, int slide, int TEXT_ALIGN, color clr);
    void WDrawString (WSHDR *ws, TFont *font, int x, int y, int x2, int y2, int TEXT_ALIGN, color clr);
    */

	void DrawGradient (int x, int y, int x2, int y2, color clr);
	void DrawGradient2 (int x, int y, int x2, int y2, color clr1, color clr2, int rot);

};


int GetFontH (ft_font *ftf);
int GetLetterW (ft_font *ftf, int num);
int GetStringW (char *str, ft_font *font);
