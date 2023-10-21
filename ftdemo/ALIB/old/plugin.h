

typedef struct{
  char InitPLG;
  void (*OnRedraw)(); 
    
  //void *next;
}AIBAR;

void DrawPLG ();
void ClosePLG();
void SearchPLG (char *way);
