#ifndef CONFIGSIMULATION_H
#define CONFIGSIMULATION_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TF1.h>
#endif

class ConfigSimulation {

 public:
  bool fIsLogic;
  int fNQbits;
  int fNSimulations;
  double fSigmaNoise;
  TF1* fPdfNoise;
  
  ConfigSimulation(bool IsLogic=false, int NQbits=100, int NSimulations=10, double SigmaNoise=0.1, TF1* PdfNoise=NULL){
    fIsLogic = IsLogic;
    fNQbits = NQbits;
    fNSimulations = NSimulations;
    fSigmaNoise = SigmaNoise;
    fPdfNoise = PdfNoise;
  };
  ~ConfigSimulation(){};
  
};

#endif 
