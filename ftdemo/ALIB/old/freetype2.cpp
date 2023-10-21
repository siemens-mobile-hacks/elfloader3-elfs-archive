#include "include.h"
#include "font.h"
#include "io.h"

#ifdef WIN32
#pragma comment(lib, "freetype244.lib")
#endif

//FreeType заголовочные файлы
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <freetype/ttnameid.h>



#define FT_LIB_NAME "libft-2.4.6-1.so"


//ANSI
const unsigned short win2unicode[128]=
{
  0x0402,0x0403,0x201A,0x0453,0x201E,0x2026,0x2020,0x2021,
  0x20AC,0x2030,0x0409,0x2039,0x040A,0x040C,0x040B,0x040F,
  0x0452,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
  0x0020,0x2122,0x0459,0x203A,0x045A,0x045C,0x045B,0x045F,
  0x00A0,0x040E,0x045E,0x0408,0x00A4,0x0490,0x00A6,0x00A7,
  0x0401,0x00A9,0x0404,0x00AB,0x00AC,0x00AD,0x00AE,0x0407,
  0x00B0,0x00B1,0x0406,0x0456,0x0491,0x00B5,0x00B6,0x00B7,
  0x0451,0x2116,0x0454,0x00BB,0x0458,0x0405,0x0455,0x0457,
  0x0410,0x0411,0x0412,0x0413,0x0414,0x0415,0x0416,0x0417,
  0x0418,0x0419,0x041A,0x041B,0x041C,0x041D,0x041E,0x041F,
  0x0420,0x0421,0x0422,0x0423,0x0424,0x0425,0x0426,0x0427,
  0x0428,0x0429,0x042A,0x042B,0x042C,0x042D,0x042E,0x042F,
  0x0430,0x0431,0x0432,0x0433,0x0434,0x0435,0x0436,0x0437,
  0x0438,0x0439,0x043A,0x043B,0x043C,0x043D,0x043E,0x043F,
  0x0440,0x0441,0x0442,0x0443,0x0444,0x0445,0x0446,0x0447,
  0x0448,0x0449,0x044A,0x044B,0x044C,0x044D,0x044E,0x044F
};

unsigned int char8to16_v2(int c)
{
  if (c>=128)
  {
    return(win2unicode[c-128+64]);
  }
  return(c);
}

//CONFIG
//#define resolution 96
//#define height 12

//hinting
//FT_LOAD_DEFAULT
//FT_LOAD_TARGET_MONO
//FT_LOAD_TARGET_LIGHT
/*
#define FT_LOAD_DEFAULT                      0x0
#define FT_LOAD_NO_SCALE                     0x1
#define FT_LOAD_NO_HINTING                   0x2
#define FT_LOAD_RENDER                       0x4
#define FT_LOAD_NO_BITMAP                    0x8
#define FT_LOAD_VERTICAL_LAYOUT              0x10
#define FT_LOAD_FORCE_AUTOHINT               0x20
#define FT_LOAD_CROP_BITMAP                  0x40
#define FT_LOAD_PEDANTIC                     0x80
#define FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH  0x200
#define FT_LOAD_NO_RECURSE                   0x400
#define FT_LOAD_IGNORE_TRANSFORM             0x800
#define FT_LOAD_MONOCHROME                   0x1000
#define FT_LOAD_LINEAR_DESIGN                0x2000
#define FT_LOAD_NO_AUTOHINT                  0x8000U
*/

//ft_render_mode_normal
//FT_RENDER_MODE_MONO
//FT_RENDER_MODE_LIGHT
/*
  typedef enum  FT_Render_Mode_
  {
    FT_RENDER_MODE_NORMAL = 0,
    FT_RENDER_MODE_LIGHT,
    FT_RENDER_MODE_MONO,
    FT_RENDER_MODE_LCD,
    FT_RENDER_MODE_LCD_V,

    FT_RENDER_MODE_MAX

  } FT_Render_Mode;
  */

  int GetLadModeByCfg (int cfg){
      int mode=0;
      switch (cfg){
          case 0: mode=FT_LOAD_DEFAULT; break;
          case 1: mode=FT_LOAD_TARGET_MONO; break;
          case 2: mode=FT_LOAD_TARGET_LIGHT; break;
      }

      return mode;
  }


int InitGlyphByCH (FT_Face face, int ch, Glyph *gObj, int LoadMode_, int RenderMode_) {

  if (FT_Load_Glyph (face, FT_Get_Char_Index (face, ch), GetLadModeByCfg(LoadMode_) )) return -1;

  FT_Glyph glyph;

  if (FT_Get_Glyph (face->glyph, &glyph)) return -1;

  FT_Glyph_To_Bitmap (&glyph, (FT_Render_Mode)RenderMode_, 0, 1);

  FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;


  gObj->top=bitmap_glyph->top;
  gObj->left=bitmap_glyph->left;
  gObj->advance_x=0;

  //(bitmap_glyph->root.advance.x)>>6>>6>>6;

  FT_Bitmap& bitmap=bitmap_glyph->bitmap;

  gObj->w=bitmap.width;
  gObj->h=bitmap.rows;

  int size=gObj->h*gObj->w;

  gObj->buffer=new char [size];

  for (int k=0; k<size; k++){
	  gObj->buffer[k]=bitmap.buffer[k];
  }

  return 0;
}

int
TFont::InitFT (char *fname){

  FT_Library library;
  if (FT_Init_FreeType (&library)) return -1;

  FT_Face face;
  //if (FT_New_Face (library, fname, 0, &face)) return -1;

  //mem
  int size=GetFileSize (fname);
  if (size<=0) return -1;
  char *buf=new char [size];

  FileReadToBuffer (fname, buf, size);

  if (FT_New_Memory_Face (library, (FT_Byte*)buf, size, 0, &face )) return -1;


  FT_Set_Char_Size (face, FT_FontHeight_ << 6, FT_FontHeight_ << 6, FT_Resolution_, FT_Resolution_);

  for (int i=0; i<count_; i++){
	  int s=char8to16_v2 (i);
	  InitGlyphByCH (face, s, &glyph_[i], FT_LoadMode_, FT_RenderMode_);
  }

  FT_Done_Face(face);
  FT_Done_FreeType(library);

  delete [] buf;

  glyphCount_=count_;

  YMax_=GetMaxFontYSize();
  ltr_w_=glyph_['A'].h/15;

  return 0;

}
