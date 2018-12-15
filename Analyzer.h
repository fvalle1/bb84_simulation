//
// Created by francescobonacina on 06/12/18.
//

#ifndef BB84_SIMULATION_ANALYZER_H
#define BB84_SIMULATION_ANALYZER_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TCanvas.h>
#include <TFile.h>
#include <TMultiGraph.h>
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
    void FillMultiGraphs(TFile *file, TMultiGraph *mg_NalteredVsNsent, TMultiGraph *mg_ProbabilityVsNsent) const;

    void PlotFunctionOfErrors(TVirtualPad *cx, TFile *file, int Nfixed);

    void PlotFunctionOfAltered(TVirtualPad *cx, TFile *file);

    void ProbabilityVsSent(TVirtualPad *cx, TFile *file, TMultiGraph *mg_ProbabilityVsNsent) const;

    void AlteredVsSent(TVirtualPad *cx, TMultiGraph *mg_NalteredVsNsent, uint32_t fixed) const;

    void PlotNalteredDistributions(TVirtualPad *cx, TFile *file);

    void PlotSlopeVsNoise(TVirtualPad *cx, TFile *file);

    void SetStyleMultiGraph(TGraphErrors *const *mg) const;

    void InitMultiGraph(TGraphErrors **NalteredVsError, const char *name) const;
};


#endif //BB84_SIMULATION_ANALYZER_H
