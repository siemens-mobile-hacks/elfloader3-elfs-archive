#include "img.h"
#include "io.h"
#include "math.h"
#include "siemens_unicode.h"


color RGBA(unsigned char R, unsigned char G, unsigned char B, unsigned char A){
//#ifdef WIN
	//color t={B,G,R,A};
//#else
	color t={R,G,B,A};
//#endif
  return t;
}

color BGRA(unsigned char R, unsigned char G, unsigned char B, unsigned char A){
//#ifdef WIN
	color t={B,G,R,A};
//#else
	//color t={R,G,B,A};
//#endif
  return t;
}


#ifdef WIN

#else
struct POINT{
  int x;
  int y;
};

/*
int max(int x, int y){ return x > y ? x : y;}

int min(int x, int y){ return x < y ? x : y;}

int abs (int n){ return n>=0 ? n : -n;}
*/

#endif


/////////////////////////RGB16

unsigned short
AIMG::
GetColor16 (int x, int y){
  if (y<h_ && y>=0 && x>=0 && x<w_){
    unsigned short *bm=(unsigned short*)bitmap_;
    return *(bm + x + y*w_);
  }
  else return a16pixel;

}

color RGB16_2_RGBA (unsigned short clr){
  color c;
  c.R=(clr&0xF800)>>8;
  c.G=(clr&0x7E0)>>3;
  c.B=(clr&0x1F)<<3;
  if (clr==a16pixel) c.A=0;
  else c.A=0x64;

  return c;
}


void
AIMG::
SetColor16 (int x, int y, unsigned short clr){
  unsigned short *bm=(unsigned short*)bitmap_;
  if (y<h_ && y>=0 && x>=0 && x<w_) *(bm + x + y*w_)=clr;
}

//////////////////////


//Вычисление цвета при альфа-канале
color CalcColor (color src,  color dst){
	color clr;

	clr.A=src.A;
	clr.R=(src.R*(0xFF-dst.A)+dst.R*dst.A)/0xFF;
	clr.G=(src.G*(0xFF-dst.A)+dst.G*dst.A)/0xFF;
	clr.B=(src.B*(0xFF-dst.A)+dst.B*dst.A)/0xFF;

	return clr;
}

short CalcColor16 (short bg, color clr){
  char r = (((bg&0xF800)>>8)*(0xFF-clr.A)+clr.R*clr.A)/0xFF;
  char g = (((bg&0x7E0)>>3)*(0xFF-clr.A)+clr.G*clr.A)/0xFF;
  char b = (((bg&0x1F)<<3)*(0xFF-clr.A)+clr.B*clr.A)/0xFF;

  return RGB16(r,g,b);
}

color
AIMG::
GetColor (int x, int y){
  if (bpnum_==10){
	if (y<h_ && y>=0 && x>=0 && x<w_){
		color *bm=(color*)bitmap_;
		return *(bm + x + y*w_);
	}
	return RGBA(0, 0, 0, 0);
  }
  if (bpnum_==8){
    unsigned short uclr=GetColor16 (x, y);
    return RGB16_2_RGBA (uclr);
  }

  return RGBA(0, 0, 0, 0);
}

void
AIMG::
SetColor (int x, int y, color clr){
  if (bpnum_==10){
	color *bm=(color*)bitmap_;
	if (y<h_ && y>=0 && x>=0 && x<w_){
		if (y>=0 && y<h_){
			if (clr.A==0xFF) *(bm + x + y*w_)=clr;
			else{
				color src=GetColor (x, y);
				if (src.A==0) *(bm + x + y*w_)=clr;
				else{
					color res=CalcColor (src, clr);
					*(bm + x + y*w_)=res;
				}
			}
		}
	}
  }

  if (bpnum_==8){
            unsigned short *bm=(unsigned short*)bitmap_;
            if (clr.A>0){
              if (clr.A==0xFF) *(bm+ x + y*w_)=RGB16(clr.R, clr.G, clr.B);
              else{
                *(bm+ x + y*w_)=CalcColor16 (*(bm+ x + y*w_), clr);
              }
            }

  }
}



void
AIMG::
Clean(){

  if (bpnum_==10){
	color *bm=(color*)bitmap_;
	for (int i=0; i<w_; i++)
	  for (int j=0; j<h_; j++)
		  *(bm + i + j*w_)=RGBA(0,0,0,0);
  }

  if (bpnum_==8){
        unsigned short *bm=(unsigned short*)bitmap_;
        for (int i=0; i<w_; i++)
	  for (int j=0; j<h_; j++)
		  *(bm + i + j*w_)=a16pixel;
  }
}

void
AIMG::
Create (int w, int h, int bpnum){
	w_=w;
	h_=h;
	bpnum_=bpnum;

	if (bpnum_==10) bitmap_=new unsigned char [h*w*sizeof(color)]; //RGBA
	if (bpnum_==8) bitmap_=new unsigned char [h*w*2];	//RGB16

	//Clean ();
}

void
AIMG::
FlipHoriz (){
	for (int j=0; j<h_; j++){
		for (int i=0; i<w_/2; i++){
			color left=GetColor (i, j);
			color right=GetColor (w_-i-1, j);
			SetColor (i, j, right);
			SetColor (w_-i-1, j, left);
		}
	}
}

void
AIMG::
FlipVertic (){
	for (int i=0; i<w_; i++){
		for (int j=0; j<h_/2; j++){
			color up=GetColor (i, j);
			color down=GetColor (i, h_-j-1);
			SetColor (i, j, down);
			SetColor (i, h_-j-1, up);
		}
	}
}
void
AIMG::
SetAlpha (unsigned char alpha){
	color *bm=(color*)bitmap_;
	for (int i=0; i<w_; i++){
		for (int j=0; j<h_; j++){

			color clr=GetColor (i, j);
			if (clr.A){
				clr.A=alpha;
				*(bm + i + j*w_)=clr;
			}
		}
	}
}

void
AIMG::
DrawOldCircle (int xc, int yc, int R, color brush, color pen){
	int b=0;
	int x=0;
	int y=R;

	xc=xc+R;
	yc=yc+R;

	int delta=2*(1-R);
	int limit=0;

M1:
	SetColor (xc-x, yc-y, pen);
	SetColor (xc+x, yc-y, pen);
	if (brush.A && y<R){
		for (int i=0; i<x; i++){
			SetColor (xc-i, yc-y, brush);
			SetColor (xc+i, yc-y, brush);
		}
	}
	SetColor (xc-x, yc+y, pen);
	SetColor (xc+x, yc+y, pen);
	if (brush.A && y<R){
		for (int i=0; i<x; i++){
			SetColor (xc-i, yc+y, brush);
			SetColor (xc+i, yc+y, brush);
		}
	}

	if (y<=limit) return;

	if (delta<0) goto M2;
	if (delta>0) goto M3;
	if (delta==0) goto M20;

M2:
	b=2*delta+2*y-1;
	if (b<=0) goto M10;
	else goto M20;

M3:
	b=2*delta+2*x-1;
	if (b<=0) goto M20;
	else goto M30;

M10:
	x++;
	delta=delta+2*x+1;
	goto M1;

M20:
	x++;
	y--;
	delta=delta+2*x-2*y+2;
	goto M1;

M30:
	y--;
	delta=delta-2*y+1;
	goto M1;
}

void
AIMG::
DrawCircle (int xc, int yc, int R, color brush, color pen){
	int balpha=brush.A;
	brush.A=255;
	AIMG img;
	img.Create (2*R+1, 2*R+1, 10);

	img.DrawOldCircle (0, 0, R, brush, pen);
	img.SetAlpha (balpha);

	DrawLayer (&img, xc, yc);

}

int signf (double arg){
	if (arg>0) return 1;
	if (arg<0) return -1;

	return 0;
}
int sign (int arg){
	if (arg>0) return 1;
	if (arg<0) return -1;

	return 0;
}

int Integer (double arg){
	return (int)(arg+0.5);
}

void
AIMG::DrawLine (int x1, int y1, int x2, int y2, color clr){
	int len=0;
	int lenx=abs(x2-x1);
	int leny=abs(y2-y1);

	if (lenx>=leny) len=lenx;
	else len=leny;

	double deltax=(x2-x1)*1./len;
	double deltay=(y2-y1)*1./len;

	double x=x1+0.5*signf (deltax);
	double y=y1+0.5*signf (deltay);

	for (int i=0; i<len; i++){
		SetColor (Integer(x), Integer(y), clr);
		x+=deltax;
		y+=deltay;
	}
}

void
AIMG::DrawRect (int x1, int y1, int x2, int y2, color clr){
	for (int i=x1; i<x2; i++)
	  for (int j=y1; j<y2; j++) SetColor (i, j, clr);
}

void
AIMG::DrawLayer (AIMG *img, int x, int y){

  if (bpnum_==10){
	for (int i=0; i<img->GetW(); i++){
	  for (int j=0; j<img->GetH(); j++){
		  color dst=img->GetColor (i, j);
                  SetColor (x+i, y+j, dst);
	  }
	}
  }

  if (bpnum_==8){
    if (img->GetBtype ()==8){
	for (int i=0; i<img->GetW(); i++){
	  for (int j=0; j<img->GetH(); j++){
                  SetColor16 (x+i, y+j, img->GetColor16 (i, j));
	  }
	}
    }
    if (img->GetBtype ()==10){
	for (int i=0; i<img->GetW(); i++){
	  for (int j=0; j<img->GetH(); j++){
                  SetColor (x+i, y+j, img->GetColor (i, j));
	  }
	}
    }
  }
}

void
AIMG::DrawLayerRECT (AIMG *img, int x, int y, MyRECT rc){
  if (bpnum_==8){
      int nx=max (x, rc.x);
      int nx2=min (x+img->GetW(), rc.x2);

      int ny=max (y, rc.y);
      int ny2=min (y+img->GetH(), rc.y2);

	for (int i=nx; i<nx2; i++){
	  for (int j=ny; j<ny2; j++){
            if (img->GetBtype ()==8) SetColor16 (i, j, img->GetColor16 (i-x, j-y));
            if (img->GetBtype ()==10) SetColor (i, j, img->GetColor (i-x, j-y));
	  }
	}


  }


}



void
AIMG::Rotate90 (){
	color clr;

	AIMG img;
	img.Create (h_, w_, bpnum_);


	for (int i=0; i<h_; i++){
		for (int j=0; j<w_; j++){
			clr=GetColor (j, i);
			img.SetColor (h_-1-i, j, clr);
		}
	}

	w_=img.w_;
	h_=img.h_;

	for (int i=0; i<h_; i++){
		for (int j=0; j<w_; j++){
			clr=img.GetColor (i, j);
			SetColor (i, j, clr);
		}
	}
}

int
AIMG::Rotate (int ang){
	if (ang==90) Rotate90 ();
	if (ang==180) FlipVertic ();
	if (ang==270){
		//use rotate matrix
		Rotate90 ();
		Rotate90 ();
		Rotate90 ();
	}

	return 0;
}

int
AIMG::
FRotate (float angle){

  double ang = -angle*acos(0.)/90;
  int newWidth, newHeight;
  int nWidth = w_;
  int nHeight= h_;
  double cos_angle = cos(ang);
  double sin_angle = sin(ang);

  POINT p1={0,0};
  POINT p2={nWidth,0};
  POINT p3={0,nHeight};
  POINT p4={nWidth-1,nHeight};
  POINT newP1,newP2,newP3,newP4, leftTop, rightTop, leftBottom, rightBottom;

  newP1.x = p1.x;
  newP1.y = p1.y;
  newP2.x = (long)(p2.x*cos_angle - p2.y*sin_angle);
  newP2.y = (long)(p2.x*sin_angle + p2.y*cos_angle);
  newP3.x = (long)(p3.x*cos_angle - p3.y*sin_angle);
  newP3.y = (long)(p3.x*sin_angle + p3.y*cos_angle);
  newP4.x = (long)(p4.x*cos_angle - p4.y*sin_angle);
  newP4.y = (long)(p4.x*sin_angle + p4.y*cos_angle);

  leftTop.x = min(min(newP1.x,newP2.x),min(newP3.x,newP4.x));
  leftTop.y = min(min(newP1.y,newP2.y),min(newP3.y,newP4.y));
  rightBottom.x = max(max(newP1.x,newP2.x),max(newP3.x,newP4.x));
  rightBottom.y = max(max(newP1.y,newP2.y),max(newP3.y,newP4.y));
  leftBottom.x = leftTop.x;
  leftBottom.y = rightBottom.y;
  rightTop.x = rightBottom.x;
  rightTop.y = leftTop.y;

  newWidth = rightTop.x - leftTop.x;
  newHeight= leftBottom.y - leftTop.y;

  AIMG rimg;
  rimg.Create(newWidth, newHeight, bpnum_);

  int x,y,newX,newY,oldX,oldY;
  for (y = leftTop.y, newY = 0; y<=leftBottom.y; y++,newY++){
    for (x = leftTop.x, newX = 0; x<=rightTop.x; x++,newX++){
      oldX = (long)(x*cos_angle + y*sin_angle);
      oldY = (long)(y*cos_angle - x*sin_angle);
      rimg.SetColor (newX,newY, GetColor(oldX,oldY));

    }
  }

  delete [] bitmap_;
  Create (rimg.w_, rimg.h_, bpnum_);

  for (int i=0; i<h_; i++){
	  for (int j=0; j<w_; j++){
		  color clr=rimg.GetColor (j, i);
		  SetColor (j, i, clr);
	  }
  }

  return 0;

}

int
AIMG::
Resize (int px, int py)
{
  if (w_==px && h_==py){
    //original
    return 1;
  }
  else{

    long newx = px,
    newy = py;

    float xScale, yScale, fX, fY;
    xScale = (float)w_  / (float)newx;
    yScale = (float)h_ / (float)newy;

    AIMG simg;
	simg.Create (newx, newy, 10);

    long ifX, ifY, ifX1, ifY1, xmax, ymax;
    float ir1, ir2, ig1, ig2, ib1, ib2, ia1, ia2, dx, dy;
    char r,g,b,a;
    color rgb1, rgb2, rgb3, rgb4;
    xmax = w_-1;
    ymax = h_-1;
    for(long y=0; y<newy; y++){
      fY = y * yScale;
      ifY = (int)fY;
      ifY1 = min(ymax, ifY+1);
      dy = fY - ifY;
      for(long x=0; x<newx; x++){
        fX = x * xScale;
        ifX = (int)fX;
        ifX1 = min(xmax, ifX+1);
        dx = fX - ifX;
        rgb1= GetColor(ifX,ifY);
        rgb2= GetColor(ifX1,ifY);
        rgb3= GetColor(ifX,ifY1);
        rgb4= GetColor(ifX1,ifY1);

        ir1 = rgb1.R   * (1 - dy) + rgb3.R   * dy;
        ig1 = rgb1.G * (1 - dy) + rgb3.G * dy;
        ib1 = rgb1.B  * (1 - dy) + rgb3.B  * dy;
        ia1 = rgb1.A  * (1 - dy) + rgb3.A  * dy;
        ir2 = rgb2.R   * (1 - dy) + rgb4.R   * dy;
        ig2 = rgb2.G * (1 - dy) + rgb4.G * dy;
        ib2 = rgb2.B  * (1 - dy) + rgb4.B  * dy;
        ia2 = rgb2.A  * (1 - dy) + rgb4.A  * dy;

        r = (char)(ir1 * (1 - dx) + ir2 * dx);
        g = (char)(ig1 * (1 - dx) + ig2 * dx);
        b = (char)(ib1 * (1 - dx) + ib2 * dx);
        a = (char)(ia1 * (1 - dx) + ia2 * dx);

        simg.SetColor (x,y,RGBA(b,g,r,a));
      }
    }

	delete [] bitmap_;
	Create (simg.w_, simg.h_, bpnum_);

	for (int i=0; i<h_; i++){
		for (int j=0; j<w_; j++){
			color clr=simg.GetColor (j, i);
			SetColor (j, i, clr);
		}
	}

  }

  return 0;
}

int
AIMG::
FResize (float k){
	return Resize (w_*k, h_*k);
}



void
AIMG::
DrawGradient (int x, int y, int x2, int y2, color clr){
	x=x-1;x2=x2-1; y=y-1; y2=y2-1;

  int h=y2-y;

  color MyColor1=RGBA (127,127,127,0x64);
  color MyColor2=RGBA (1,1,1,0x64);

  MyColor1.R=MyColor1.R+clr.R/2;
  MyColor1.G=MyColor1.G+clr.G/2;
  MyColor1.B=MyColor1.B+clr.B/2;

  MyColor2.R=MyColor2.R+clr.R/3;
  MyColor2.G=MyColor2.G+clr.G/3;
  MyColor2.B=MyColor2.B+clr.B/3;

  color MyColor=RGBA (0,0,0,0x64);

  int r=MyColor1.R-MyColor2.R;
  int g=MyColor1.G-MyColor2.G;
  int b=MyColor1.B-MyColor2.B;

  for (int i=0; i<h; i++){
    MyColor.R=MyColor1.R-r/h*(i+1);
    MyColor.G=MyColor1.G-g/h*(i+1);
    MyColor.B=MyColor1.B-b/h*(i+1);

    MyColor.A=clr.A;
    DrawRect (x, y+i, x2, y+i+1, MyColor);
  }
}


void RotateCoordinates(int *x,int *y){
  int r=0;

  r = *y;
  *y = *x;
  *x = ScreenW()-r;

}

void
AIMG::
DrawGradient2 (int x, int y, int x2, int y2, color clr1, color clr2, int rot){

	//int wdh=x2-x;
    int hgt=0;
    if (rot){
      hgt=x2-x;
    }
    else hgt=y2-y;

	for (int i=0; i<hgt; i++){
		int r=clr1.R+(i-hgt+1)*(clr2.R-clr1.R)/(hgt-1)+(clr2.R-clr1.R);
		int g=clr1.G+(i-hgt+1)*(clr2.G-clr1.G)/(hgt-1)+(clr2.G-clr1.G);
		int b=clr1.B+(i-hgt+1)*(clr2.B-clr1.B)/(hgt-1)+(clr2.B-clr1.B);

		if (rot){
                  DrawRect (x+i, y, x+i+1, y2, RGBA (r, g, b, (clr1.A+clr2.A)/2));
                }
                else DrawRect (x, y+i, x2, y+i+1, RGBA (r, g, b, (clr1.A+clr2.A)/2));

	}


}

/*
//////////////////////////////////png//////////////////

void* xmalloc(int x,int n)
{
  return malloc(n);
}

void xmfree(int x,void* ptr)
{
  mfree(ptr);
}

void read_data_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
  unsigned int err;
  int f;
  f=(int)png_get_io_ptr(png_ptr);
  fread(f, data, length, &err);
}

#define number 8
#define DEFAULT_COLOR 2
#define ALPHA_THRESHOLD 128
#define PNG_1 0xFF
#define PNG_8 1
#define PNG_16 2
#define PNG_24 3

int
AIMG::CreateFromPNG (char *fname){
  int type=0;
  int f;
  char buf[number];
  unsigned int err;
  struct PP
  {
    char *row;
    char *img;
    //EIMGHDR * img_h;
  } pp;
  //EIMGHDR * img_hc;
  png_structp png_ptr=NULL;
  png_infop info_ptr=NULL;
  png_uint_32 rowbytes;

  if ((f=fopen(fname, A_ReadOnly+A_BIN, P_READ, &err))==-1) return 0;
  pp.row=NULL;
  pp.img=NULL;
  //pp.img_h=NULL;

  if (fread(f, &buf, number, &err)!=number) goto L_CLOSE_FILE;
  if  (!png_check_sig((png_bytep)buf,number)) goto  L_CLOSE_FILE;

  png_ptr = png_create_read_struct_2("1.2.5", (png_voidp)0, 0, 0, (png_voidp)0,(png_malloc_ptr)xmalloc,(png_free_ptr)xmfree);
  if (!png_ptr) goto L_CLOSE_FILE;

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    goto L_CLOSE_FILE;
  }
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    goto L_CLOSE_FILE;
  }

  png_set_read_fn(png_ptr, (void *)f, read_data_fn);

  png_set_sig_bytes(png_ptr, number);

  png_read_info(png_ptr, info_ptr);

  png_uint_32 width, height;
  int bit_depth, color_type;

  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);

  if (type==0)
  {
    if (color_type == PNG_COLOR_TYPE_GRAY)
      type=PNG_1;
    else type=DEFAULT_COLOR+1;
  }

  if (bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);

  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png_ptr);

  if (bit_depth == 16) png_set_strip_16(png_ptr);

  if (bit_depth < 8) png_set_packing(png_ptr);

  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png_ptr);

  if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA || color_type == PNG_COLOR_TYPE_GRAY)
    png_set_gray_to_rgb(png_ptr);

  png_set_filler(png_ptr,0xFF,PNG_FILLER_AFTER);
  png_read_update_info(png_ptr, info_ptr);

  rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  pp.row=(char*)malloc(rowbytes);
  //pp.img_h=img_hc=malloc(sizeof(EIMGHDR));


  if (type==PNG_1)
  {
    int rowc_w=(width+7)>>3;
    int size=height*rowc_w;
    unsigned char *iimg=(unsigned char *)(pp.img=(char*)malloc(size));
    zeromem(iimg,size);
    for (unsigned int y = 0; y<height; y++)
    {
      png_read_row(png_ptr, (png_bytep)pp.row, NULL);
      for (unsigned int x = 0; x<width; x++)
      {
        if (!pp.row[x*4+0] && !pp.row[x*4+1] && !pp.row[x*4+2])
          iimg[x>>3]|=(0x80>>(x&7));
      }
      iimg+=rowc_w;
    }
    bpnum_=1;
  }
  else
  {
    switch (type)
    {
    case PNG_8:
      {
        unsigned char *iimg=(unsigned char *)(pp.img=(char*)malloc(width*height));
        for (unsigned int y = 0; y<height; y++)
        {
          png_read_row(png_ptr, (png_bytep)pp.row, NULL);
          for (unsigned int x = 0; x<width; x++)
          {
            if (pp.row[x*4+3]<ALPHA_THRESHOLD)
              *iimg++=0xC0;
            else
            {
              unsigned char c=(pp.row[x*4+0] & 0xE0);
              c|=((pp.row[x*4+1]>>3)&0x1C);
              c|=((pp.row[x*4+2]>>6)&0x3);
              *iimg++=c;
            }
          }
        }
        bpnum_=5;
        break;
      }
    case PNG_16:
      {
        unsigned short *iimg=(unsigned short *)(pp.img=(char*)malloc(width*height*2));
        for (unsigned int y = 0; y<height; y++)
        {
          png_read_row(png_ptr, (png_bytep)pp.row, NULL);
          for (unsigned int x = 0; x<width; x++)
          {
            if (pp.row[x*4+3]<ALPHA_THRESHOLD)
              *iimg++=0xE000;
            else
            {
              unsigned int c=((pp.row[x*4+0]<<8)&0xF800);
              c|=((pp.row[x*4+1]<<3)&0x7E0);
              c|=((pp.row[x*4+2]>>3)&0x1F);
              *iimg++=c;
            }
          }
        }
        bpnum_=8;
        break;
      }

    case PNG_24:
      {
        unsigned char *iimg=(unsigned char *)(pp.img=(char*)malloc((width*height)<<2));
        for (unsigned int y = 0; y<height; y++)
        {
          png_read_row(png_ptr, (png_bytep)pp.row, NULL);
          for (unsigned int x = 0; x<width; x++)
          {
	    unsigned int c;

	    *iimg++=pp.row[x*4+0];
            *iimg++=pp.row[x*4+1];
            *iimg++=pp.row[x*4+2];

            c=pp.row[x*4+3];
//	    if (c>=128) c++;
//	    c*=100;
//	    c>>=8;
            *iimg++=c;//(pp.row[x*4+3]*100)/0xFF;
          }
        }
        bpnum_=0xA;
        break;
      }
    }
  }
  w_=width;
  h_=height;
  //pp->img_h->zero=0;
  bitmap_=(unsigned char*)pp.img;

  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
  if (!pp.img)
  {
  L_CLOSE_FILE:
    mfree(pp.row);
    mfree(pp.img);
    //mfree(pp.img_h);
    fclose(f, &err);
    return -1;
  }
  mfree(pp.row);
  fclose(f, &err);
  //return (img_hc);

  return 0;
}

*/

///////////////system///////////////////////
#ifdef WIN
void DrawAIMG (HDC hDC, int w, int h, unsigned char **bitmap, int x, int y){

	int LayerW=w;
	int LayerH=h;
	BITMAPINFO bi;
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = LayerW;
	bi.bmiHeader.biHeight = LayerH;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = 0;
	bi.bmiHeader.biXPelsPerMeter = 0;
	bi.bmiHeader.biYPelsPerMeter = 0;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;
	void* pBufPixels;
	HBITMAP hBuf = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pBufPixels, NULL, 0);

	color *bm = (color*)(pBufPixels);
	color *bm2 = (color*)(*bitmap);
	for (int i=0; i<LayerW && i<WindowW; i++){
	  for (int j=0; j<LayerH && j<WindowH; j++){
		  color clr=*(bm2 + i + j*LayerW);
		  *(bm + i + j*LayerW)=clr;
	  }
	}

	HDC hBufDC = CreateCompatibleDC(NULL);
	SelectObject(hBufDC, hBuf);

    //рисуем содержимое буфера в указанный DC, со смешиванием (blending) цветов
	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;
	AlphaBlend(hDC, x, y, min(LayerW, WindowW), min(LayerH, WindowH), hBufDC, 0, 0, min(LayerW, WindowW), min(LayerH, WindowH), bf);

	//освобождаем временные GDI-объекты
	DeleteDC(hBufDC);
	DeleteObject(hBuf);
}
#else



#endif




int init=0;
void Key (int c){ init=init+c;}

#ifdef WIN
void OnRedraw (HDC hDC){
	AIMG img;
	AIMG img2;

	img.Create (WindowW, WindowH, 10);
	img.DrawRect (0,0, WindowW, WindowH, RGBA (128, 128, 128,255));

	img2.CreateFromJPEG ("4.jpg");
	//img2.DrawCircle (10, 10, 50, RGBA (255, 0, 0,100), RGBA (0, 0, 255,100));

	//img2.FResize (0.8);

	TFont font;
	font.Init ("Ubuntu-R.ttf12.atf");

	color white=RGBA (255, 255, 255,255);
	//img.DrawLetter (&font, 'A', 50, 50, white);
	img2.DrawString ("Siemens is alive", &font, 20, 20, 0, white);
	img2.DrawString ("ALib (c) ANDRE \ncrossplatform library SIEMENS/WIN32 \nwork with image, fonts", &font, 50, 50, 0, white);

	//img2.DrawGradient (0, img2.GetH()/2-30, img2.GetW(), img2.GetH()/2, RGBA (100, 150, 200,150));

	/*color clr1=RGBA (220, 200, 180,100);
	color clr2=RGBA (20, 20, 20,100);
	img2.DrawGradient2 (0, 0, img2.GetW(), img2.GetH(), clr1, clr2);*/

	img2.Rotate90();

	//img2.FlipHoriz ();

	img.DrawLayer (&img2, 10, 10);

	img.FlipVertic();
	DrawAIMG (hDC, img.GetW(), img.GetH(), img.GetBitmap(), 0, 0);
}
#else

#endif
