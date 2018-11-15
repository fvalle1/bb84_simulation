#ifndef TCHANNEL_H
#define TCHANNEL_H

#include <TObject.h>

class TChannel : public TObject {
  
 private:
  double fQbit;
  double fNoise;
    
 public:
  TChannel(TQbit fQbit);
  virtual ~TChannel();
  TChannel AddNoise(double theta, TF1 *PdfNoise);
  void GetQbit(double fQbit);
  
  ClassDef(TProva,1)  
};

#endif
