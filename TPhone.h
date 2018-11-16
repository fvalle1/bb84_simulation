#ifndef TPHONE_H
#define TPHONE_H

#include <TObject.h>

class TPhone : public TTree {
  
 private:
  double fQbitA;
  double fQbitB;		
 
 public:
  TPhone();
  virtual ~TPhone();
  virtual Fill(TQbit* qbitB);
  void SetQbitA(TQbit* qbitA);
  
  ClassDef(TPhone,0)
};

#endif
