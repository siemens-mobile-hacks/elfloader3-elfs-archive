#include "platform.h"

typedef struct{
  int w;
  int h;
  int left;
  int top;
  int advance_x;
  int advance_y;
  char *buffer;
}Glyph;

#define MAX_COUNT 192
#define FONT_COUNT 4

class  TFont{

	int count_;

	int YMax_;
	int glyphCount_;
	int ltr_w_;

	int FT_LoadMode_;
	int FT_RenderMode_;
	int FT_Resolution_;
	int FT_FontHeight_;

public:
    Glyph *glyph_;
    TFont (){ count_=MAX_COUNT; YMax_=0; glyphCount_=0; ltr_w_=0; glyph_=new Glyph [count_]; }
	~TFont ();

    int LoadGlyphs ( char *buf, int count);
	int GetMaxFontYSize();

	void Init (char *fname);
	int InitFT (char *fname);

    int GetFontH (){ return YMax_;}
    int GetInterval (){ return ltr_w_; }
    int GetStringW (char *str);

    void SetFTsettings (int load, int render, int res, int h){
        FT_LoadMode_=load;
        FT_RenderMode_=render;
        FT_Resolution_=res;
        FT_FontHeight_=h;
    }
};

