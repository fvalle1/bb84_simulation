//
// Created by Filippo Valle on 15/11/2018.
//

#ifndef BB84_SIMULATION_SIMULATOR_H
#define BB84_SIMULATION_SIMULATOR_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Buddy.h"
#include "Channel.h"
#include "Phone.h"
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH1D.h>
#endif

class Simulator {
public:
    static Simulator* Instance();

    Simulator* RunSimulation();
    Simulator* GeneratePlots();
    Simulator* ShowResults(TCanvas *cx);

private:
    Simulator();
    ~Simulator();
    Simulator(const Simulator& source); //impedisco a compilatore di creare copy constructor

    Channel** fChannels;
    int fNqbits;
    static const char* fFilename;
    static const char* fTreename;
    static const char* fBranchName;
    static const char* fProbabilityPlotName;
    static const char* fProbabilityTeoPlotName;
    static const char* fNPlotName;
    static const char* fNDistrName;
    static const char* fUsefulPlotName;
    static const int fSimulations = 2000;

    static Simulator* fgSimulator;

    void SetStylesAndDraw(TH1D *hist, const char *xLabel, const char *ylabel, Color_t color, Width_t linewidth) const;

ClassDef(Simulator, 0)
};


#endif //BB84_SIMULATION_SIMULATOR_H
