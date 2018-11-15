//
// Created by Filippo Valle on 15/11/2018.
//

#include "TSimulator.h"

TSimulator::Simulator(){
    fAlice = new TBuddy();
    fEve = new TBuddy();
    fBob = new TBuddy();
    fPhone = new TPhone();

    fChannel1 = new TChannel();
    Channel->SetNoisy(TF1* pdf);
    fChannel2 = new TChannel();

}

void TSimulator::TRunSimulation(){
//hists/file
    //for N
//i-->N
    TQbit qbit = new TQbit(true);
    fAlice->GenerateTQbit(qbit, TF1* pdf);
    fPhone->SetQbit(qbit);
    Channel1->PassQbit(qbit);
    fEve->InterceptQbit(qbit, TF1* pdf);
    Channel2->SetQbit(qbit)->TChannel(qbit);
    fBob->Receive();
    fPhone->Fill(qbit);
    delete qbit;

}
