//
// Created by Filippo Valle on 15/11/2018.
//

#ifndef BB84_SIMULATION_SIMULATOR_H
#define BB84_SIMULATION_SIMULATOR_H

#include "TBuddy.h"
#include "TChannel.h"
#include "TPhone.h"

bool DEBUG = true;

class TSimulator {
public:
    TSimulator();
    ~TSimulator();

    TSimulator &TRunSimulation();
private:

    TBuddy* fAlice;
    TBuddy* fEve;
    TBuddy* fBob;
    TChannel** fChannels;
    TPhone* fPhone;

    ClassDef(TSimulator, 0)
};


#endif //BB84_SIMULATION_SIMULATOR_H
