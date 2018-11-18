#ifndef TCHANNEL_H
#define TCHANNEL_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TF1.h>
#include "Qbit.h"
#endif

class Channel{
  
 private:
  TF1* fPdf;
  bool fIsNoisy;
    
 public:
  Channel(bool isNoisy = false);
  ~Channel();
  void PassQbit(Qbit* qbit);
  inline void SetNoisy(bool isNoisy){fIsNoisy = isNoisy;};

  ClassDef(Channel, 0)
};

#endif
