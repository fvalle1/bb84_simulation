//
// Created by Filippo Valle on 18/11/2018.
//

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Simulator.h"
#include "StopWatch.h"
#include "ConfigSimulation.h"
#include <TCanvas.h>
#include <TF1.h>
#endif

void bb84_simulation(bool useLogic = false, int Nqbit = 100, int nSim = 30000, double sigmaNoise = 0, TF1* fNoise = nullptr){
    StopWatch watch;
    auto cx = new TCanvas();
    ConfigSimulation config(useLogic, Nqbit, nSim, sigmaNoise, fNoise);
    auto sim = Simulator::Instance(config);
    sim->RunSimulation()->GeneratePlots()->ShowResults(cx);
}
