//
// Created by Filippo Valle on 15/11/2018.
//

#include "TSimulator.h"
#include "TPhone.h"
#include "TChannel.h"

TSimulator::TSimulator(){
    fAlice = new TBuddy();
    fEve = new TBuddy();
    fBob = new TBuddy();
    fPhone = new TPhone();

    fChannels = new TChannel*[2];

    fChannels[0] = new TChannel();
    //fChannels[0]->SetNoisy(TF1* pdf);
    fChannels[1] = new TChannel();

}

TSimulator::~TSimulator() {
    delete fAlice;
    delete fEve;
    delete fBob;
    delete fPhone;
    delete fChannels[0];
    delete fChannels[1];
    delete[] fChannels;
}

TSimulator& TSimulator::TRunSimulation(){
//hists/file
    //for N
//i-->N
    auto qbit = new TQbit(true);
    fAlice->GenerateTQbit(qbit);
    fPhone->SetQbitA(qbit);
    fChannels[0]->PassQbit(qbit);
    fEve->InterceptQbit(qbit);
    fChannels[1]->PassQbit(qbit);
    fBob->Receive();
    fPhone->Fill(qbit);
    delete qbit;

    return *this;
}
