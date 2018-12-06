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
  std::string fInfos;
  
  explicit ConfigSimulation(bool IsLogic = false,
          int NQbits = 100,
          int NSimulations = 10,
          bool withEve = true,
          double sigma = 0,
          std::function<double()> PdfNoise = nullptr) {
    fIsLogic = IsLogic;
    fNQbits = NQbits;
    fNSimulations = NSimulations;
    if(PdfNoise) fPdfNoise = std::move(PdfNoise);
    else fPdfNoise = nullptr;
    Buddy::EveIsSleeping = !withEve;
    fInfos = std::string(TString::Format("N%d_sim%d%s%s_%f",NQbits, NSimulations, withEve?"_Eve_":"_", sigma<=1e-5?"noisy":"quiet", sigma));
  };

  ~ConfigSimulation() = default;
  
};

#endif 
