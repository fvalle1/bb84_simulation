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


    Simulator* RunSimulation();
    Simulator* GeneratePlots();
    Simulator* ShowResults(TCanvas *cx);


    //Lascio i nomi con cui salvo sui file pubblici per essere facilmente identificati ovunque
    //static constexpr const char* per avere dei const char* dichiarati e definiti nello stesso posto
    static constexpr const char* fFilename = "bb84_simulation.root";
    static constexpr const char* fNPlotName = "Naltered_vs_Nsent";
    static constexpr const char* fUsefulHistName = "NSameBase_vs_Nsent";
    static constexpr const char* fTreename = "bb84";
    static constexpr const char* fBranchName = "dataBranch";
    static constexpr const char* fProbabilityPlotName = "probability_vs_N";
    static constexpr const char* fProbabilityTeoPlotName = "probability_vs_N_teo";
    static constexpr const char* fNDistrName = "N_distr";
    static constexpr const char* fUsefulPlotName = "useful_distr";
    static constexpr const char* fPdfperLenghtCom = "PdfperLenghtCom";

private:
    Simulator();                  // costruttore privato --> non si può usare! Il Simulatpr verrà creato (una volta sola) solo con Instance()
    Simulator(ConfigSimulation config);                  // costruttore privato --> non si può usare! Il Simulatpr verrà creato (una volta sola) solo con Instance()
    ~Simulator();
    Simulator(const Simulator& source);             // impedisco a compilatore di creare copy constructor


    void PlotPdfAtFixedNSent(TTree *tree);
    void PlotNSameBasisVsNSent(TTree *tree);
    void HistNintercepted(TTree*);

    Channel** fChannels;
    ConfigSimulation fConfiguration;

    void SetStylesAndDraw(TObject *hist, const char *xLabel, const char *ylabel, Color_t color, Width_t linewidth) const; //Styles on TH1, TGraph...

    static Simulator* fgSimulator;                  // global pointer
};


#endif //BB84_SIMULATION_SIMULATOR_H
