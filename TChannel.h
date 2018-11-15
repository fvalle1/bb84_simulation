#ifndef TCHANNEL_H
#define TCHANNEL_H

#include <TObject.h>

class TChannel : public TObject {
  
 private:
  TF1* fPdf;
  bool fIsNoisY;
    
 public:
  TChannel(bool fIsNoisy);
  virtual ~TChannel();
  void PassQbit(TQbit* qbit);	
  void SetNoisy(bool fIsNoisy);
  
  ClassDef(TProva,1)  
};

#endif
