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
  
  ConfigSimulation(bool IsLogic=true, int NQbits=100, int NSimulations=2000, double SigmaNoise=0.1, TF1* PdfNoise=NULL){
    fIsLogic = IsLogic;
    fNQbits = NQbits;
    NSimulations = fNSimulations;
    SigmaNoise = fSigmaNoise;
    PdfNoise = fPdfNoise;
  };
  ~ConfigSimulation();
  
};

#endif 
