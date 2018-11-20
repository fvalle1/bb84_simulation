//
// Created by Filippo Valle on 14/11/2018.
//

#include "Simulator.h"
#include "TelegramWatch.h"
#include "StopWatch.h"

int main(){
    TelegramWatch twatch("bb84");
    auto sim = Simulator::Instance(false);
    sim->RunSimulation()->GeneratePlots();
    return 0;
}