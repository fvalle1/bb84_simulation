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
    configs.emplace_back(ConfigSimulation(true, 100, 2000, false, 0., nullptr));
    configs.emplace_back(ConfigSimulation(false, 100, 50000, true, 0.3, [&](){return gRandom->Gaus(0, 0.1);}));

    Analyzer * MyAnalizer;
    MyAnalizer = new Analyzer(configs);
    MyAnalizer->RunAnalyzer();
    MyAnalizer->JoinResults(cx);

    return 0;
}