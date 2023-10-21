
#include "include.h"

#include "img.h"
#include "io.h"

extern "C" {

};

#ifdef WIN

#include "include/jpeglib.h"
#include "include/jerror.h"
#pragma comment(lib, "lib/jpeg.lib")

#else

#include "jpeglib.h"

#endif

/*
int
AIMG::
CreateFromJPEG (char *fname){

	//JDIMENSION num_scanlines;
	JSAMPARRAY buffer=0;
	unsigned char *row;

	int size=GetFileSize (fname);

	if (size<=0) return -1;

	unsigned char *buf=new unsigned char [size];

	FileReadToBuffer (fname, buf, size);
        //iLog (size);

      jpeg_decompress_struct cinfo;
      jpeg_error_mgr jerr;

      cinfo.err = jpeg_std_error(&jerr);
      jpeg_create_decompress(&cinfo);
      //Log ("jpeg_create_decompress\n");

      jpeg_mem_src(&cinfo, buf, size);

      //Log ("jpeg_mem_src\n");

      jpeg_read_header(&cinfo, TRUE);
      //Log ("jpeg_read_header\n");


      //cinfo.dct_method = JDCT_FASTEST;

      jpeg_start_decompress(&cinfo);
      //Log ("jpeg_start_decompress\n");

	unsigned int img_w=cinfo.output_width;
	unsigned int img_h=cinfo.output_height;

        //iLog (img_w);
        //iLog (img_h);

	Create (img_w, img_h, 8);

	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, img_w*cinfo.output_components, 1);

	while (cinfo.output_scanline < img_h)
	{
		JDIMENSION num_scanlines=jpeg_read_scanlines(&cinfo, buffer, 1);
		row=(unsigned char*)buffer[0];
		unsigned short clr;
		//color *bm=(color*)bitmap_;
		for (int x=0; x<img_w; x++){
			clr=RGB16 (row[0], row[1], row[2]);
			SetColor16 (x,cinfo.output_scanline-1, clr);
			row+=3;
		}
	}

	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	delete [] buf;


	return 0;
}
*/
/*
int
AIMG::
CreateFromJPEG (char *filename){
  unsigned int err=0;
  int msz,uid;
  IMGHDR *tmpimg;//*myimg=NULL;
  WSHDR *ext;
  HObj  mypicObj;
  short pos;
  int len;

  short w=0;
  short h=0;

  WSHDR *path=AllocWS(256);
  str_2ws (path, filename, 256);

  if (!path) goto exit0;
  len=wstrlen(path);
  pos= wstrrchr(path,len,'.');
  if (!pos) goto exit0;

  ext=AllocWS(len-pos);
  wstrcpybypos(ext,path,pos+1,len-pos);
  uid=GetExtUid_ws(ext);
  FreeWS(ext);
  mypicObj=Obs_CreateObject(uid,0x2d,0x02,0x80A8,1,1,&err);
  if (err)  return 0;

  err=Obs_SetInput_File(mypicObj,0,path);
  if (err)  goto exit1;

  FreeWS(path);


  err=Obs_GetInfo(mypicObj,0);
  if (err)  goto exit1;
  err=Obs_GetInputImageSize(mypicObj,&w,&h);
  if (err)  goto exit1;



  err=Obs_SetOutputImageSize(mypicObj,w,h);
  if (err)  goto exit1;


  err=Obs_SetScaling(mypicObj,5);
  if (err)  goto exit1;

  err=Obs_Start(mypicObj);
  if (err)  goto exit1;

  err=Obs_Output_GetPictstruct(mypicObj,&tmpimg);
  if (err)  goto exit1;

  msz=CalcBitmapSize (tmpimg->w,tmpimg->h,tmpimg->bpnum);

  w_=tmpimg->w;
  h_=tmpimg->h;
  bpnum_=tmpimg->bpnum;
  bitmap_=(unsigned char*)malloc(msz);
  memcpy(bitmap_,tmpimg->bitmap,msz);
exit1:
  Obs_DestroyObject(mypicObj);
exit0:
  return 0;
}
*/
/*
#define CFG_JPEG_CLEVEL 100
void
AIMG::
SaveInJPEG (char *file){
  unsigned int err;
  //Tlog("write jpeg...");
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;
        int row_stride;
        JSAMPROW row_pointer;
        int i;

        int outfile = fopen(file,A_WriteOnly+A_BIN+A_Create,P_WRITE,0);

        cinfo.err = jpeg_std_error(&jerr);
        //Tlog("err ok");
        jpeg_create_compress(&cinfo);
        //Tlog("create compress ok");
        unsigned long size=(w_*h_*4);
        jpeg_mem_dest(&cinfo, &bitmap_, &size);
        //Tlog("stdio dest ok");
        cinfo.image_width = w_;
        cinfo.image_height = h_;
        cinfo.input_components = 4; //3
        cinfo.in_color_space = JCS_RGB;
        jpeg_set_defaults(&cinfo);
        //Tlog("defafults ok");
        jpeg_set_quality(&cinfo,CFG_JPEG_CLEVEL,TRUE);
        //Tlog("quality ok");
        jpeg_start_compress(&cinfo, TRUE);
        //Tlog("compress ok");
        row_stride = 4*w_;  //3
        for (i=0; i<h_; i++)
        {
                row_pointer =(char*)&bitmap_[i*row_stride];
                jpeg_write_scanlines(&cinfo, &row_pointer, 1);
        }
        //Tlog("scanlines writed");
        jpeg_finish_compress(&cinfo);
        //Tlog("compress finished");
        jpeg_destroy_compress(&cinfo);
        //Tlog("compress destroyed");
        fclose(outfile, &err);
        //Tlog("all ok");
}

*/
