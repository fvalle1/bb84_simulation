//
// Created by Filippo Valle on 18/11/2018.
//

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Simulator.h"
#include "StopWatch.h"
#include "ConfigSimulation.h"
#include "Analyzer.h"
#include <TCanvas.h>
#include <TF1.h>
#endif

void bb84_simulation(){
  StopWatch watch;
  auto cx = new TCanvas();
  std::vector<ConfigSimulation> configs;
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.1, [&](){return gRandom->Poisson(0.1);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.2, [&](){return gRandom->Poisson(0.2);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.3, [&](){return gRandom->Poisson(0.3);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.4, [&](){return gRandom->Poisson(0.4);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.5, [&](){return gRandom->Poisson(0.5);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.6, [&](){return gRandom->Poisson(0.6);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.7, [&](){return gRandom->Poisson(0.7);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.8, [&](){return gRandom->Poisson(0.8);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.9, [&](){return gRandom->Poisson(0.9);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 1.0, [&](){return gRandom->Poisson(1.0);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 1.1, [&](){return gRandom->Poisson(1.1);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 1.2, [&](){return gRandom->Poisson(1.2);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 1.3, [&](){return gRandom->Poisson(1.3);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 1.4, [&](){return gRandom->Poisson(1.4);}));


  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.1, [&](){return gRandom->Gaus(0, 0.1);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.2, [&](){return gRandom->Gaus(0, 0.2);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.3, [&](){return gRandom->Gaus(0, 0.3);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.4, [&](){return gRandom->Gaus(0, 0.4);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.5, [&](){return gRandom->Gaus(0, 0.5);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.6, [&](){return gRandom->Gaus(0, 0.6);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.7, [&](){return gRandom->Gaus(0, 0.7);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.8, [&](){return gRandom->Gaus(0, 0.8);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 0.9, [&](){return gRandom->Gaus(0, 0.9);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 1.0, [&](){return gRandom->Gaus(0, 1.0);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 1.1, [&](){return gRandom->Gaus(0, 1.1);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 1.2, [&](){return gRandom->Gaus(0, 1.2);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 1.3, [&](){return gRandom->Gaus(0, 1.3);}));
  configs.emplace_back(ConfigSimulation(true, 100, 1000, true, 1.4, [&](){return gRandom->Gaus(0, 1.4);}));

  //Instance of Analyzer, run all simulations, put results on the canvas and delete the Analyzer
  auto MyAnalizer = Analyzer::Instance(configs);
  //MyAnalizer->RunAnalyzer();
  MyAnalizer->JoinResults(cx, 50);
  Analyzer::Destroy();
}
