#ifndef CONFIGSIMULATION_H
#define CONFIGSIMULATION_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TF1.h>
#include "Buddy.h"
#endif

class ConfigSimulation {

public:
    ConfigSimulation(bool IsLogic = false,
                              int NQbits = 100,
                              int NSimulations = 10,
                              bool withEve = true,
                              double sigma = 0,
                              std::function<double()> PdfNoise = nullptr);

    ConfigSimulation(const ConfigSimulation&);
    ~ConfigSimulation() = default;

    bool fIsLogic;
    int fNQbits;
    int fNSimulations;
    bool fWithEve;
    double fSigma;
    std::function<double(void)> fPdfNoise;
    std::string fInfos;
};

#endif 
