#ifndef TCHANNEL_H
#define TCHANNEL_H

#include <TObject.h>
#include <TF1.h>
#include "TQbit.h"

class TChannel : public TObject {
  
 private:
  TF1* fPdf;
  bool fIsNoisY;
    
 public:
  TChannel(bool fIsNoisy = false);
  virtual ~TChannel();
  void PassQbit(TQbit* qbit);	
  void SetNoisy(bool fIsNoisy);
  
  ClassDef(TChannel, 0)  
};

#endif
