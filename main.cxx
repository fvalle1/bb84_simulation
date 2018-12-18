//
// Created by Filippo Valle on 14/11/2018.
//

#include "Simulator.h"
#include "ConfigSimulation.h"
#include "StopWatch.h"
#include "Analyzer.h"

int main(){
    StopWatch sw;
    std::vector<ConfigSimulation> configs;

    configs.emplace_back(ConfigSimulation(false, 100, 5000, false, 0., nullptr));
    configs.emplace_back(ConfigSimulation(false, 100, 5000, true, 0.2, [&](){return gRandom->Gaus(0, 0.2);}));
    configs.emplace_back(ConfigSimulation(false, 100, 5000, true, 0.8, [&](){return gRandom->Gaus(0, 0.8);}));
    configs.emplace_back(ConfigSimulation(true, 100, 5000, true, 0., nullptr));
    configs.emplace_back(ConfigSimulation(true, 100, 5000, true, 0.2, [&](){return gRandom->Gaus(0, 0.2);}));
    configs.emplace_back(ConfigSimulation(true, 100, 5000, true, 0.8, [&](){return gRandom->Gaus(0, 0.8);}));


    auto MyAnalizer = Analyzer::Instance(configs);
    MyAnalizer->RunAnalyzer();
    Analyzer::Destroy();
    return 0;
}