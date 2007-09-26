#include "../inc/swilib.h"
#include "view.h"
#include "string_works.h"
#include "destructors.h"

void FreeRawText(VIEWDATA *vd)
{
  mfree(vd->rawtext);
  vd->rawtext=NULL;
  vd->rawtext_size=0;
}

void FreeDynImgList(VIEWDATA *vd)
{
  OMS_DYNPNGLIST *dpl=vd->dynpng_list;
  vd->dynpng_list=NULL;
  while(dpl)
  {
    OMS_DYNPNGLIST *p=dpl;
    dpl=dpl->dp.next;
    if (p->dp.img)
    {
      mfree(p->dp.img->bitmap);
      mfree(p->dp.img);
    }
    mfree(p);
  }
}

void FreeViewData(VIEWDATA *vd)
{
  if (vd->zs)
  {
    inflateEnd(vd->zs);
    mfree(vd->zs);
  }
  if (vd->ws) FreeWS(vd->ws);
  mfree(vd->ref_cache);
  vd->ref_cache=NULL;
  vd->ref_cache_size=0;
  mfree(vd->lines_cache);
  vd->lines_cache=NULL;
  mfree(vd->rawtext);
  vd->rawtext=NULL;
  vd->rawtext_size=0;
  mfree(vd->oms);
  vd->oms=NULL;
  vd->oms_size=0;
//  mfree(vd->I_cache);
  mfree(vd->S_cache);
  vd->S_cache=NULL;
  FreeDynImgList(vd); 
  mfree(vd);
}
