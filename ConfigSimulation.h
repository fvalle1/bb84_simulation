#ifndef CONFIGSIMULATION_H
#define CONFIGSIMULATION_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Buddy.h"
#include "Channel.h"
#include "Phone.h"
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH1D.h>
#include <TGraphErrors.h>
#include <stdio.h>
#include <TLine.h>
#endif

class ConfigSimulation {

 public:
  bool fIsLogic;
  int fNQbits;
  int fNSimulations;
  double fSigmaNoise;
  TF1* fPdfNoise;
  
  ConfigSimulation(bool fIsLogic=true, int fNQbits=100, int fNSimulations=2000, double fSigmaNoise=0.1, TF1* fPdfNoise=NULL);
  ~ConfigSimulation();
  
};

#endif 
