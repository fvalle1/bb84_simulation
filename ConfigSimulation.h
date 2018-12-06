#ifndef CONFIGSIMULATION_H
#define CONFIGSIMULATION_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TF1.h>
#include "Buddy.h"
#endif

class ConfigSimulation {

 public:
  bool fIsLogic;
  int fNQbits;
  int fNSimulations;
  std::function<double(void)> fPdfNoise;
  
  explicit ConfigSimulation(bool IsLogic = false, int NQbits = 100, int NSimulations = 10, bool withEve = true,
                              std::function<double()> PdfNoise = nullptr) {
    fIsLogic = IsLogic;
    fNQbits = NQbits;
    fNSimulations = NSimulations;
    if(PdfNoise) fPdfNoise = std::move(PdfNoise);
    else fPdfNoise = nullptr;
    Buddy::EveIsSleeping = !withEve;
  };

  ~ConfigSimulation() = default;
  
};

#endif 
