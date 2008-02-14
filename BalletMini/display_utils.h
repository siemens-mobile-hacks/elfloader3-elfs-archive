unsigned int SearchNextDisplayLine(VIEWDATA *vd, LINECACHE *p, unsigned int *max_h);
int LineDown(VIEWDATA *vd);
int LineUp(VIEWDATA *vd);
int RenderPage(VIEWDATA *vd, int do_draw);
REFCACHE *FindReference(VIEWDATA *vd, unsigned int ref);
int FindReferenceById(VIEWDATA *vd, unsigned int id, int i);
int ChangeMenuSelection(VIEWDATA *vd, REFCACHE *rf);
int CreateInputBox(VIEWDATA *vd, REFCACHE *rf);
