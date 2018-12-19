//
// Created by Filippo Valle on 15/11/2018.
//

#ifndef BB84_SIMULATION_SIMULATOR_H
#define BB84_SIMULATION_SIMULATOR_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH1D.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TLine.h>
#include <TTree.h>
#include <TBranch.h>
#include "Buddy.h"
#include "Channel.h"
#include "Phone.h"
#include "ConfigSimulation.h"
#endif

class Simulator {
public:
    static Simulator* Instance();
    static Simulator* Instance(ConfigSimulation config);
    inline static void Destroy(){fgSimulator->~Simulator();};


    Simulator* RunSimulation(); //Simulate data and store on tree
    Simulator* GeneratePlots(); //Save plots on file
    Simulator* ShowResults(TCanvas *cx);


    //Lascio i nomi con cui salvo sui file pubblici per essere facilmente identificati ovunque
    //static constexpr const char* per avere dei const char* dichiarati e definiti nello stesso posto
    static constexpr const char* fFilename = "bb84_simulation.root";
    static constexpr const char* fAlteredVsSentName = "Naltered_vs_Nsent";
    static constexpr const char* fSameBasisHistName = "NsameBase_vs_Nsent";
    static constexpr const char* fTreeName = "tree_bb84";
    static constexpr const char* fBranchName = "branch_data_bb84";
    static constexpr const char* fProbabilityPlotName = "probability_vs_N";
    static constexpr const char* fProbabilityTeoPlotName = "probability_vs_N_teo";
    static constexpr const char* fAlteredDistrName = "Naltered_distr";
    static constexpr const char* fUsefulPlotName = "Nsame_Base_distr";

    static void SetStylesAndDraw(TObject *obj, const char *xLabel, const char *ylabel, Color_t color = kRedBlue, Width_t linewidth = 4, Style_t markerStyle = 20); //Styles on TH1, TGraph...

private:
    Simulator();                  // costruttore privato --> non si può usare! Il Simulatpr verrà creato (una volta sola) solo con Instance()
    Simulator(ConfigSimulation config);                  // costruttore privato --> non si può usare! Il Simulatpr verrà creato (una volta sola) solo con Instance()
    ~Simulator();
    Simulator(const Simulator& source);             // impedisco a compilatore di creare copy constructor


    void PlotPdfAtFixedNSent(TTree *tree);    // all distribution of N altered one per N sent (cd(5))
    void PlotNSameBasisVsNSent(TTree *tree);  // TH1D Nsamebasis/Nsent vs. length of communication (cd(3))
    void HistNaltered(TTree *);               // TH1D NAltered/NSameBasis vs. length of communication (cd(2))

    Channel** fChannels;
    ConfigSimulation fConfiguration;

    static Simulator* fgSimulator;   // global pointer
};


#endif //BB84_SIMULATION_SIMULATOR_H
