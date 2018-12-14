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
//    //3.1
//    configs.emplace_back(ConfigSimulation(false, 100, 2000, false, 0., nullptr));
//    for(double sigma = 0; sigma < 2.5; sigma+=0.1) configs.emplace_back(ConfigSimulation(false, 100, 10000, false, sigma, [&](){return gRandom->Gaus(0, sigma);}));
//
//    //3.2
//    configs.emplace_back(ConfigSimulation(true, 100, 10000, false, 0, nullptr));
//    for(double sigma = 0; sigma < 2.5; sigma+=0.1) configs.emplace_back(ConfigSimulation(true, 100, 10000, false, sigma, [&](){return gRandom->Gaus(0, sigma);}));
//
//    configs.emplace_back(ConfigSimulation(false, 100, 2000, true, 0., nullptr));
//    for(double sigma = 0; sigma < 2.5; sigma+=0.1) configs.emplace_back(ConfigSimulation(false, 100, 10000, true, sigma, [&](){return gRandom->Gaus(0, sigma);}));
//
    configs.emplace_back(ConfigSimulation(true, 100, 100, true, 0, nullptr));
//    for(double sigma = 0; sigma < 2.5; sigma+=0.1) configs.emplace_back(ConfigSimulation(true, 100, 10000, true, sigma, [&](){return gRandom->Gaus(0, sigma);}));


    auto MyAnalizer = Analyzer::Instance(configs);
    MyAnalizer->RunAnalyzer();
    MyAnalizer->JoinResults(cx, 0);

    Analyzer::Destroy();
    return 0;
}