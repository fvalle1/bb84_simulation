//
// Created by Filippo Valle on 14/11/2018.
//

#include "Simulator.h"
#include "ConfigSimulation.h"
#include "TelegramWatch.h"
#include "StopWatch.h"

int main(){
    TelegramWatch twatch("bb84");
    ConfigSimulation config(true, 100, 100000, 0.1, nullptr);
    auto sim = Simulator::Instance(config);
    sim->RunSimulation()->GeneratePlots();
    return 0;
}