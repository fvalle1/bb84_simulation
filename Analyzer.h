//
// Created by francescobonacina on 06/12/18.
//

#ifndef BB84_SIMULATION_ANALYZER_H
#define BB84_SIMULATION_ANALYZER_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TCanvas.h>
#include <TFile.h>
#include <TMultiGraph.h>
#include <THStack.h>
#include <TLegend.h>
#include "Simulator.h"
#include "ConfigSimulation.h"
#endif

class Analyzer {
public:
    static Analyzer* Instance(std::vector<ConfigSimulation> VettInfos);
    inline static void Destroy(){fgAnalyzer->~Analyzer();};
    void RunAnalyzer();
    void JoinResults(TCanvas *cx, uint32_t fixedN = 0);

private:
    Analyzer(std::vector<ConfigSimulation> VettInfos);
    Analyzer(const Analyzer&);                           // avoid automatic copy constructor
    ~Analyzer();
    std::vector<ConfigSimulation> fVettInfos;            // vector containing all the configurations we want to simulate

    static Analyzer* fgAnalyzer;

    // functions called by RunSimulation and JoinResults
    void FillMultiGraphs(TFile *file, TMultiGraph *mg_NalteredVsNsent, TMultiGraph *mg_ProbabilityVsNsent) const;   // to fill mg_NalteredVsNsent and mg_ProbabilityVsNsent
    void AlteredVsSent(TVirtualPad *cx, TMultiGraph *mg_NalteredVsNsent, uint32_t fixed) const;     // draw the multigraph mg_NalteredVsNsent
    void PlotFunctionOfErrors(TVirtualPad *cx, TFile *file, int Nfixed) const;                      // draw NAlteredVsN at N = Nfixed vs sigma_noise (for each simulation)
    void ProbabilityVsSent(TVirtualPad *cx, TFile *file, TMultiGraph *mg_ProbabilityVsNsent) const; // draw the probability to receive all qbits altered in Nsent qbits (for each simulation)
    void PlotSlopeVsNoise(TVirtualPad *cx, TFile *file) const;              // draw the slope of linear fit vs sigma_noise (for each simulation). The lines fit probability to receive all qbits altered in Nsent qbits, represented in log scale
    void PlotNalteredDistributions(TVirtualPad *cx, TFile *file) const;     // collect the histograms fAlteredDistrName of each simulation on the same THStack
    void PlotFunctionOfAltered(TVirtualPad *cx, TFile *file) const;         // draw sigma_squared of TH1 fAlteredDistrName vs sigma_noise (for each simulation)

    void SetStyleMultiGraph(TGraphErrors *const *mg) const;
    void InitMultiGraph(TGraphErrors **NalteredVsError, const char *name) const;
};


#endif //BB84_SIMULATION_ANALYZER_H
