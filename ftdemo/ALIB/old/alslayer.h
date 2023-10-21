#ifndef _ALSLAYER_H
#define _ALSLAYER_H

#define A_SMALL_FONT 1
#define A_NORMAL_FONT 2
#define A_LARGE_FONT 3

#define ALIB_SERVER "ALibServer"
#define ALIB_CLIENT "ALibClient"

#define IPC_GET_BG 0
#define IPC_SET_BG 1

#define IPC_GET_FUNC 3
#define IPC_SET_FUNC 4

typedef struct {
	int x;
	int y;
	int x2;
	int y2;
}MyRECT;

void SendIPC (char *from, char *to, int ipc, void* data);

typedef struct {
  void (*ALS_CreateUI) ();
  void (*ALS_DrawBG) ();
  int (*ALS_DrawHeader) (char *hname);
  void (*ALS_DrawSoft) (char *lsoft, char *rsoft);
  void (*ALS_CloseUI) ();
  void (*OnPaint) ();
  void (*ALS_WDrawString) (WSHDR *ws,  int x, int y, int x2, int y2, int nfont, int TEXT_ALIGN, char *clr);
  void (*ALS_DrawProgressBar) (MyRECT rc, int percent);
  void (*ALS_DrawLayer) (int x, int y, int w, int h, int bpnum, unsigned char *bitmap);
}ALSFunc;


void GetIPC_Client (GBS_MSG *msg);
#endif
