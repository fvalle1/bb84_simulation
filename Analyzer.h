//
// Created by francescobonacina on 06/12/18.
//

#ifndef BB84_SIMULATION_ANALYZER_H
#define BB84_SIMULATION_ANALYZER_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TCanvas.h>
#include <TFile.h>
#include <TMultiGraph.h>
#include "Simulator.h"
#include "ConfigSimulation.h"
#endif

class Analyzer {
public:
    static Analyzer* Instance(std::vector<ConfigSimulation> VettInfos);
    inline static void Destroy(){fgAnalyzer->~Analyzer();};
    void RunAnalyzer();
    void JoinResults(TCanvas *cx);

private:
    Analyzer(std::vector<ConfigSimulation> VettInfos);
    Analyzer(const Analyzer&); //avoid automatic copy constructor
    ~Analyzer();
    std::vector<ConfigSimulation> fVettInfos;

    static Analyzer* fgAnalyzer;
};


#endif //BB84_SIMULATION_ANALYZER_H
