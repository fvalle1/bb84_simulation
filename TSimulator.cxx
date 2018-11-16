//
// Created by Filippo Valle on 15/11/2018.
//

#include "TSimulator.h"


ClassImp(TSimulator)

const char* TSimulator::fFilename = "BB84_simulation.root";
const char* TSimulator::fTreename = "Phone";


TSimulator::TSimulator(){
    fChannels = new TChannel*[2];

    fChannels[0] = new TChannel();
    //fChannels[0]->SetNoisy(TF1* pdf);
    fChannels[1] = new TChannel();

}

TSimulator::~TSimulator() {
    delete fChannels[0];
    delete fChannels[1];
    delete[] fChannels;
    if(TQbit::DEBUG) printf("Simulation ended\n");
}

TSimulator& TSimulator::RunSimulation(){
//hists/file
    printf("Running simulation..");
    TFile file(fFilename, "RECREATE");
    auto phone = new TPhone(fTreename);
    auto qbit = new TQbit(true);

    for (int N = 0; N < 10; ++N) {
        phone->InitResults();

        for (int i = 0; i < N; ++i) {
            printf("\rSimulating qbits %u/%u", i + 1 , N);
            TBuddy::PrepareTQbit(qbit);
            phone->SetNewQbit(qbit);
//    fChannels[0]->PassQbit(qbit);
//    TBuddy::InterceptQbit(qbit);
//    fChannels[1]->PassQbit(qbit);
            TBuddy::ReceiveTQbit(qbit);
            phone->Update(qbit);
        }
        phone->AddPoint();
    }

    printf("\nSaving results..\n");
    file.Write();
    file.Close();


    return *this;
}

TSimulator& TSimulator::ShowResults(){
    TFile file(fFilename);


    file.Close();
    return *this;
}

