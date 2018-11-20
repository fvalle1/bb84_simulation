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
    static Simulator *Instance(bool useLogicQbits);

    Simulator* RunSimulation();
    Simulator* GeneratePlots();
    Simulator* ShowResults(TCanvas *cx);

private:
    Simulator(bool useLogicQbits);
    ~Simulator();
    Simulator(const Simulator& source); //impedisco a compilatore di creare copy constructor

    Channel** fChannels;
    int fNqbits;
    bool fUseLogicQbits;
    static constexpr const char* fFilename = "bb84_simulation.root";
    static constexpr const char* fTreename = "bb84";
    static constexpr const char* fBranchName = "dataBranch";
    static constexpr const char* fProbabilityPlotName = "probability_vs_N";
    static constexpr const char* fProbabilityTeoPlotName = "probability_vs_N_teo";
    static constexpr const char* fNPlotName = "N_vs_N";
    static constexpr const char* fNDistrName = "N_distr";
    static constexpr const char* fUsefulPlotName = "useful_distr";
    static constexpr const char* fPdfperLenghtCom = "PdfperLenghtCom";
    static const int fSimulations = 2000;

    static Simulator* fgSimulator;

    void SetStylesAndDraw(TH1D *hist, const char *xLabel, const char *ylabel, Color_t color, Width_t linewidth) const;

ClassDef(Simulator, 0)
};


#endif //BB84_SIMULATION_SIMULATOR_H
