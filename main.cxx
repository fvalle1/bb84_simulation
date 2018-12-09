//
// Created by Filippo Valle on 14/11/2018.
//

#include "Simulator.h"
#include "ConfigSimulation.h"
#include "TelegramWatch.h"
#include "StopWatch.h"
#include "Analyzer.h"

int main(){
    TelegramWatch twatch("bb84");
    auto cx = new TCanvas();
    std::vector<ConfigSimulation> configs;
    //3.1
    configs.emplace_back(ConfigSimulation(false, 100, 2000, false, 0., nullptr));
    configs.emplace_back(ConfigSimulation(false, 100, 10000, false, 0.3, [&](){return gRandom->Gaus(0, 0.3);}));
    configs.emplace_back(ConfigSimulation(false, 100, 10000, false, 0.4, [&](){return gRandom->Gaus(0, 0.4);}));
    configs.emplace_back(ConfigSimulation(false, 100, 10000, false, 0.8, [&](){return gRandom->Gaus(0, 0.8);}));
    configs.emplace_back(ConfigSimulation(false, 100, 10000, false, 1, [&](){return gRandom->Gaus(0, 1);}));

    //3.2
    configs.emplace_back(ConfigSimulation(true, 100, 10000, true, 0, nullptr));
    configs.emplace_back(ConfigSimulation(true, 100, 10000, true, 0.3, [&](){return gRandom->Gaus(0, 0.3);}));
    configs.emplace_back(ConfigSimulation(true, 100, 10000, true, 0.4, [&](){return gRandom->Gaus(0, 0.4);}));
    configs.emplace_back(ConfigSimulation(true, 100, 10000, true, 0.8, [&](){return gRandom->Gaus(0, 0.8);}));
    configs.emplace_back(ConfigSimulation(true, 100, 10000, true, 1, [&](){return gRandom->Gaus(0, 1);}));
    configs.emplace_back(ConfigSimulation(true, 100, 10000, true, 0.4, [&](){return gRandom->Rndm()*0.8-0.4;}));

    auto MyAnalizer = Analyzer::Instance(configs);
    MyAnalizer->RunAnalyzer();
    MyAnalizer->JoinResults(cx);

    Analyzer::Destroy();
    return 0;
}