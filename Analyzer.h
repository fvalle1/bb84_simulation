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
    void JoinResults(TCanvas *cx, int plotFunctionOfErrors = 0);

private:
    Analyzer(std::vector<ConfigSimulation> VettInfos);
    Analyzer(const Analyzer&); //avoid automatic copy constructor
    ~Analyzer();
    std::vector<ConfigSimulation> fVettInfos;

    static Analyzer* fgAnalyzer;

    void AddMultiGraphToCanvas(TVirtualPad *cx, TFile *file, TMultiGraph *mg_NalteredVsNsent,
                               TMultiGraph *mg_ProbabilityVsNsent) const;
    void FillMultiGraphs(TFile *file, TMultiGraph *mg_NalteredVsNsent, TMultiGraph *mg_ProbabilityVsNsent) const;

    void PlotFunctionOfErrors(TCanvas *cx, TFile *file, int Nfixed);
};


#endif //BB84_SIMULATION_ANALYZER_H
