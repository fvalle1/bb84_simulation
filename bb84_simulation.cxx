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

double gSigma = 0;

double GaussianNoise(){
    return gRandom->Gaus(0, gSigma);
}


void bb84_simulation(bool useLogic = false, int Nqbit = 100, int nSim = 30000, bool withEve = true, double sigma = 0){
    StopWatch watch;
    auto cx = new TCanvas();
    std::function<double(void)> fnoise;
    if(sigma<1e-5){
        fnoise = nullptr;
    }else{
        gSigma = sigma;
        fnoise = std::move(GaussianNoise);
    }

    ConfigSimulation config(useLogic, Nqbit, nSim, withEve, fnoise);

    auto sim = Simulator::Instance(config);
    sim->RunSimulation()->GeneratePlots()->ShowResults(cx);
}
