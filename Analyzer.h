//
// Created by francescobonacina on 06/12/18.
//

#ifndef BB84_SIMULATION_ANALYZER_H
#define BB84_SIMULATION_ANALYZER_H


#include <TCanvas.h>
#include "ConfigSimulation.h"
#include <TFile.h>

class Analyzer {

public:
Analyzer(std::vector<ConfigSimulation> VettInfos);
~Analyzer();
void RunAnalyzer();
void JoinResults(TCanvas *cx);

private:
std::vector<ConfigSimulation> fVettInfos;

};


#endif //BB84_SIMULATION_ANALYZER_H
