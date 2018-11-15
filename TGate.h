#ifndef TGATE_H
#define TGATE_H

#include <TObject.h>

enum base{
    Plusminus;
    ZeroOne;
};

class TGate : public TObject {
  
 private:
  
 public:
  TGate(base fBase);
  virtual ~TGate();
  void Proiect(TQbit* fQbit, base fBase);
  void GetQbit();
  void GetBase(base fBase);
  
  ClassDef(TProva,1)  
};

#endif
