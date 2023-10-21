#include "ALIB/img.h"
#include "ALIB/io.h"
#include "ALIB/freetype.h"
#include <de/freetype.h>

///System draw
#ifdef WIN
#else
void DrwImg(IMGHDR *img, int x, int y,  char *pen, char *brush){
  RECT rc;
  DRWOBJ drwobj;
  StoreXYWHtoRECT(&rc,x,y,img->w,img->h);
  SetPropTo_Obj5(&drwobj,&rc,0,img);
  SetColor(&drwobj,pen,brush);
  DrawObject(&drwobj);
}

void DrwImg2(IMGHDR * onse, int x, int y, int xRect, int yRect,int xxRect, int yyRect)
{
  RECT rc;
  DRWOBJ drwobj;
  StoreXYWHtoRECT(&rc,x,y,xxRect,yyRect);
  SetPropTo_Obj5(&drwobj,&rc,0,onse);
  SetProp2ImageOrCanvas(&drwobj, &rc, 0, onse, xRect, yRect);
  //SetColor(&drwobj,NULL,NULL);
  DrawObject(&drwobj);
}


void DrawAIMG (AIMG *aimg, int x, int y){
	IMGHDR img;
	img.w=aimg->GetW();
	img.h=aimg->GetH();
	img.bpnum=aimg->GetBtype ();
	img.bitmap=(unsigned char*)*aimg->GetBitmap();

    DrwImg (&img, x, y,  0, 0);
}

void DrawAIMG2 (AIMG *aimg, int x, int y, MyRECT rc){
	IMGHDR img;
	img.w=aimg->GetW();
	img.h=aimg->GetH();
	img.bpnum=aimg->GetBtype ();
	img.bitmap=(unsigned char*)*aimg->GetBitmap();

    DrwImg2 (&img, x, y, rc.x, rc.y, rc.x2, rc.y2);
}
#endif

#include <ft2build.h>
#include <freetype/freetype.h>

extern unsigned int CFG_FONT_SIZE;
extern int CFG_LOAD_MODE;
extern int CFG_RENDER_MODE;
extern unsigned int CFG_RESOLUTION;

bool IsWinStarted=0;
bool IsFontCfgUpdate=1;
AIMG buffer;
//TFont font;
ft_font *ftf = NULL;

void DrawWindow (){
    if (!IsWinStarted){
        IsWinStarted=1;
        int scrH=ScreenH ();
        int scrW=ScreenW ();
        buffer.Create (scrW, scrH, 8);
        ftf = _ft_open(GetCurFile (), CFG_FONT_SIZE, 0);
    }

    color clr=RGBA (128, 128, 128, 255);
    color white=RGBA (255, 255, 255,255);


    if (IsFontCfgUpdate){
        //font.SetFTsettings (CFG_LOAD_MODE, CFG_RENDER_MODE, CFG_RESOLUTION, CFG_FONT_SIZE);
        //font.InitFT ("4:\\Zbin\\fonts\\Ubuntu-R.ttf");

        //font.Init ("0:\\Zbin\\fonts\\normal.atf");

        ftf = _ft_open(GetCurFile (), CFG_FONT_SIZE, 0);

        IsFontCfgUpdate=0;
    }

    buffer.DrawRect (0, 0, buffer.GetW(), buffer.GetH(), clr);

	if (ftf) {
		buffer.DrawScrollStringLine ("Hello world!", ftf, 0, 50, buffer.GetW(), 50+GetFontH (ftf), 0, TEXT_ALIGNMIDDLE, white);
	}

    DrawAIMG (&buffer, 0, 0);
}

void OnCloseWin (){
    _ft_close(ftf);
}
