#include "img.h"
#include "menu.h"


////////////////////Config///////////////////////
extern int CFG_SCROLL_H;
 
extern int CFG_HEADER_X;
extern int CFG_HEADER_Y;

extern int CFG_SOFTKEY_X;

extern int CFG_PANEL_SIZE_UP;
extern int CFG_PANEL_SIZE_DOWN;
extern int CFG_PANEL_SIZE_HEAD;


extern int CFG_DISABLE_IBAR;
//////////////////

//#define ELF_NAME "RadioPlayer"


struct ConfigInfo{
  AIMG *buffer;
  AIMG *bg;
  TFont *small;
  TFont *normal;
  TFont *large;
  
  color background;
  color panel;
  color panel2;

  color header;
  color header2;

  color scrollbg;
  color scrollbg2;

  color scroll;
  color scroll2;
  
  color cursor;
  color cursor2;
  
  int size_panel_up;
  int size_panel_down;
  int size_header;
  
};

void LoadTheme (ConfigInfo *cfg);

extern ConfigInfo *config;

#ifdef WIN
	#define WindowW 600
	#define WindowH 600

	void OnRedraw (HDC hDC);
#else
      void UI_OnRedraw ();
      void UI_OnCreate ();
      void UI_OnClose ();
      void UI_OnKey (int mess, int key);
      void UI_OnFocus ();
      void UI_OnUnFocus ();
      
      void StartUI ();


#endif



//////////////////////////////UI////////////////////////
int GetNonDispH ();

class UI{
public:
  char hname[64];
  char lname[64];
  char rname[64];
  
  int hfont;
  int mfont;
  int sfont;
  
  color bgclr;
  
  color panel_clr;
  color panel_clr2;
  
  color header_clr;
  color header_clr2;
  
  color scroll_clr;
  color scroll_clr2;
  
  color sel_scroll_clr;
  color sel_scroll_clr2;

   int NonDispH;
   color clr;
   
   bool focus;
   
   UI (){ 
     focus=0;
     clr=RGBA (255, 255, 255,255);
     int w=ScreenW();
     int h=ScreenH();
     
     hfont=0;
     mfont=0;
     sfont=0;

     NonDispH=GetNonDispH ();
     
     bgclr=config->background;//RGBA (90, 100, 110,255);
     
     panel_clr=config->panel;//RGBA (100, 115, 130,180);
     
     panel_clr2=config->panel2;//RGBA (40, 50, 60,180);
     
     header_clr=config->header;//RGBA (120, 130, 140,180);
     header_clr2=config->header2;//RGBA (40, 50, 60,180);
     
     scroll_clr2=config->scrollbg2;//RGBA (80, 90, 100,180);
     scroll_clr=config->scrollbg;//RGBA (160, 170, 180,180);
     
     sel_scroll_clr2=config->scroll2;//RGBA (30, 40, 50,255);
     sel_scroll_clr=config->scroll;//RGBA (80, 90, 100,255);
   }
   
   ~UI (){ 

   }
 
   void DrawBG ();
   void DrawBG2 (RECT rc);
   void DrawUpPanel ();
   void DrawDownPanel ();
   
   void DrawHeaderPanel ();
   
   void DrawHeader ();
   void DrawHeaderText (char *str, int align);
   void DrawScroll (int cur, int show, int max);
   void DrawSoft ();
   void DrawSoftName (char *left, char *right);
   void DrawIndex (int cur, int max);
   
   void DrawAbout (char *name, char *author, char *ver);
   
   void SetHeader (char *name){
     strcpy (hname, name);
   }
   void SetHeaderFont (int font){
     hfont=font-1;
   }
   
   void SetSofts (char *name1, char *name2){
     strcpy (lname, name1);
     strcpy (rname, name2);
   }
   void SetSoftsFont (int font){
     sfont=font-1;
   }
   
   void SetMainFont (int font){
     mfont=font-1;
   }
   
   void DrawIconbar ();
   
   //void OnRedraw ();
   //void OnKey (int mess, int key);
   void OnFocus (){ focus=1;}
   void UnFocus (){ focus=0;}
   bool IsFocus (){ return focus; }
  
   void SwitchFocus (){
    if (focus) focus=0;
    else focus=1;
  }
  
   void SetBGColor (color clr){ bgclr=clr;} 
 };


/////////////////////////Menu///////////////////////////      
      
//input
//char *names[]={"1", "2"};
//KeyHandler (int mes, int key)
struct Item{
  int type;
  int IsColor;
  char str[128];
  WSHDR *ws;
  color clr;
};
class Menu{
private:

  bool IsAlloc_;
  bool InitShow_;
  int MenuType_;
  int curPosMenu_;
  int curShowPos_;
  int slide_;
  int MAX_SHOW_ITEM_;
  
  int MAX_ITEM_;
  
  int confNum_;
  
  bool editActive_;
  int countEdit_;
  
  bool IsSetHook_;
  int (*EnterFunc)(int, int, int);
  

  //CFG
  int SUBNAME_FONT_;
  int MENU_FONT_;
  
  int OFFSET_ITEM_X;
  int OFFSET_ITEM_Y;
  int OFFSET_ICON_X;
  int OFFSET_ICON_Y; 
  int OFFSET_ITEM_NAME_Y;
  int OFFSET_ITEM_SUBNAME_Y;
  
  color mclr;
  
    
public:
  RECT MenuCoord;
  bool LongPress;
  Item *nameList;
  Item *nameList2;
  
  int scrollSlide;
  Menu (){
    MAX_ITEM_=0;
    IsAlloc_=0;
    MenuType_=0;
    InitShow_=0;
    curPosMenu_=0;
    curShowPos_=0;
    slide_=0;
    MAX_SHOW_ITEM_=0;
    editActive_=0;
    countEdit_=0;


    scrollSlide=0;
    LongPress=0;
    
    IsSetHook_=0;
    //cursor_=NULL;
    mclr=RGBA (255, 255, 255, 255);
    
  }
  
  void Release  (){
    MAX_SHOW_ITEM_=0;
    InitShow_=0;
    if (nameList && IsAlloc_){
      /*for (int i=0; i<MAX_ITEM_; i++){
        if (nameList[i].type==1){
          if (nameList[i].ws) FreeWS(nameList[i].ws);
        }
      }*/
      delete nameList; nameList=NULL;
    }
    
    if (nameList2 && IsAlloc_){
      /*for (int i=0; i<MAX_ITEM_; i++){
        if (nameList2[i].type==1){
          if (nameList2[i].ws) FreeWS(nameList2[i].ws);
        }
      }*/
      delete nameList2; nameList2=NULL;
    }
    
    IsAlloc_=0;
  }
  
   ~Menu (){
     Release ();
   }
  
  int GetItemCount (){ return MAX_ITEM_;}
  void SetItemCount (int count){ MAX_ITEM_=count;}
  
  int GetPos (){ return curPosMenu_; }
  void SetPos (){ curPosMenu_=0; curShowPos_=0;}

  void SetAddKeyHook (int (*f)(int, int, int)){
    IsSetHook_=1;
    EnterFunc=f;
  }
  
  
  void AllocItemsName (){
    IsAlloc_=1;
    nameList=new Item [MAX_ITEM_];
    /*
    for (int i=0; i<MAX_ITEM_; i++){
      if (nameList[i].type==1) nameList[i].ws=AllocWS(64);
    }
    */
    nameList2=new Item [MAX_ITEM_];
    /*
    for (int i=0; i<MAX_ITEM_; i++){
      if (nameList2[i].type==1) nameList2[i].ws=AllocWS(64);
    }
    */
  }
  void SetItemName (int num, char *name, char *name2){
    strcpy (nameList[num].str, name);
    strcpy (nameList2[num].str, name2);
  }
  
  void SetItemColor (int num, color clr){
    nameList[num].IsColor=1;
    nameList[num].clr=clr;
  }
  
  void SetMenuRect (int x, int y, int x2, int y2){
    MenuCoord.x=x;
    MenuCoord.y=y;
    MenuCoord.x2=x2;
    MenuCoord.y2=y2;  
  }
  
  void SetCoordinates (int NOFFSET_ITEM_X, int NOFFSET_ITEM_Y,
                       int NOFFSET_ICON_X, int NOFFSET_ICON_Y, 
                       int NOFFSET_ITEM_NAME_Y,
                       int NOFFSET_ITEM_SUBNAME_Y){
                         
    OFFSET_ITEM_X=NOFFSET_ITEM_X;
    OFFSET_ITEM_Y=NOFFSET_ITEM_Y;
    OFFSET_ICON_X=NOFFSET_ICON_X;
    OFFSET_ICON_Y=NOFFSET_ICON_Y; 
    OFFSET_ITEM_NAME_Y=NOFFSET_ITEM_NAME_Y;
    OFFSET_ITEM_SUBNAME_Y=NOFFSET_ITEM_SUBNAME_Y;
              }
  
  void UpKey();
  void DownKey();
  
  void DrawMenuList (UI *ui);
  
  void MenuListKey (int mess, int key);  
  
  void DownKeyList ();
  void UpKeyList ();
  void AddOnKey (int mess, int key){
    if (IsSetHook_) EnterFunc (mess, key, curPosMenu_);
  }
  
  void MenuOnKey (int mess, int key);
};


extern bool IsStarted;


#define A_SMALL_FONT 1
#define A_NORMAL_FONT 2
#define A_LARGE_FONT 3


extern AIMG *buffer;
extern AIMG *bg;
   
extern TFont *fonts[3];

void DrawAIMG (AIMG *aimg, int x, int y);
void DrawAIMG2 (AIMG *aimg, int x, int y, RECT rc);
void CloseTimer ();



