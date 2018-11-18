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
    Simulator();
    ~Simulator();
    Simulator& RunSimulation();
    Simulator &GeneratePlots();
    Simulator &ShowResults(TCanvas *cx);

private:
    Channel** fChannels;
    uint32_t fNqbits;
    static const char* fFilename;
    static const char* fTreename;
    static const char* fBranchName;
    static const char* fProbabilityPlotName;
    static const char* fProbabilityTeoPlotName;
    static const int fSimulations = 1000;

    void SetStylesAndDraw(TH1D *hist, const char *xLabel, const char *ylabel, Color_t color, Width_t linewidth) const;

ClassDef(Simulator, 0)
};


#endif //BB84_SIMULATION_SIMULATOR_H
