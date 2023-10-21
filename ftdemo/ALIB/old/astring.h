
class AString{
  private:
    char *s_;
    int lenght_;
  public:
     AString ();
     AString (char *s);
    ~AString (); 
    
    char *Get (){ return s_;}
    int GetLen (){ return lenght_;}
   
    void Set (const char *s);
    
    bool IsEmpty (); 
    
    void Insert (char s, int pos);  
    
    void Remove (int pos);
    
    //заменяет в строке все симв. oldSimb  на  newSimb  и  определят число замен
    int Replace (char newSimb, char oldSimb); 
};

