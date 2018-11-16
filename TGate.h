#ifndef TGATE_H
#define TGATE_H

#include <TObject.h>

class TGate : public TObject {
  
 private:
  
 public:
  TGate(base fBase);
  virtual ~TGate();
  double Proiect(TQbit* fQbit, base fBase);
  void GetQbit();
  void GetBase(base fBase);
  
  ClassDef(TGate,1)
};

#endif
