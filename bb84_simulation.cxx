//
// Created by Filippo Valle on 18/11/2018.
//

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Simulator.h"
#include "StopWatch.h"
#include <TCanvas.h>
#endif

void bb84_simulation(){
    StopWatch watch;
    auto cx = new TCanvas();
    auto sim = Simulator::Instance(false);
    sim->RunSimulation()->GeneratePlots()->ShowResults(cx);
}
