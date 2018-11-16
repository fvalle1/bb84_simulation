//
// Created by Filippo Valle on 15/11/2018.
//

#ifndef BB84_SIMULATION_SIMULATOR_H
#define BB84_SIMULATION_SIMULATOR_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TBuddy.h"
#include "TChannel.h"
#include "TPhone.h"
#include <TFile.h>
#endif

class TSimulator {
public:
    TSimulator();
    ~TSimulator();
    TSimulator& RunSimulation();
    TSimulator& ShowResults();

private:
    TChannel** fChannels;
    static const char* fFilename;
    static const char* fTreename;

    ClassDef(TSimulator, 0)
};


#endif //BB84_SIMULATION_SIMULATOR_H
