#ifndef TGATE_H
#define TGATE_H

#include <TObject.h>

enum base{
    Plusminus;
    ZeroOne;
};

class TGate : public TObject {
  
 private:
  double fQbit;
  base   fBase;
  
 public:
  TGate(base fBase);
  virtual ~TGate();
  TGate Proiect(TQbit fQbit, base fBase);
  void GetQbit(double fQbit);
  void GetBase(base fBase);
  
  ClassDef(TProva,1)  
};

#endif
