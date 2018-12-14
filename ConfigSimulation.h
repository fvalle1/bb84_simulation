#ifndef CONFIGSIMULATION_H
#define CONFIGSIMULATION_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <functional>
#include <string>
#include "Buddy.h"
#endif

class ConfigSimulation {

public:
    ConfigSimulation(bool IsLogic = false,
                     uint32_t NQbits = 100,
                     uint32_t NSimulations = 1000,
                     bool withEve = true,
                     double sigma = 0.,
                     std::function<double()> PdfNoise = nullptr);

    ConfigSimulation(const ConfigSimulation&); //creerò dei std::vector<Config>
    ~ConfigSimulation(){};

    //datamember pubblici in modo da essere subito accessibili da simulator
    bool fUseErrorCorrection;
    bool fEveIsPresent;
    uint32_t fNQbits;
    uint32_t fNSimulations;
    double fSigma;
    std::function<double(void)> fPdfNoise;
    std::string fInfos;
};

#endif 
