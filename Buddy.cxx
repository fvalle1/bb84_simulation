//
// Created by Filippo Valle on 15/11/2018.
//

#include "Buddy.h"

void Buddy::InterceptQbit(Qbit *qbit) {
    if(Qbit::DEBUG) {
        printf("\n\n**********\nIntercepting qbit..\n");
        std::cout << "received qbit: " << *qbit << std::endl;
    }
    ReceiveQbit(qbit);
    qbit->PrepareState(qbit->GetBase(), qbit->GetState());
}

void Buddy::ReceiveQbit(Qbit *qbit){
    if(Qbit::DEBUG) {
        printf("\n\n**********\nReceiving qbit..\n");
        std::cout << "received qbit: " << *qbit << std::endl;
    }
    basis measuringBase = (gRandom->Rndm()<0.5?ZeroOne:PlusMinus);
    qbit->MeasureState(measuringBase);
    if(Qbit::DEBUG) std::cout << "measured qbit: " << *qbit << std::endl;
}


void Buddy::PrepareQbit(Qbit *qbit) {
    if(Qbit::DEBUG) {
        printf("\n\n**********\nPreparing qbit..\n");
        std::cout << "received qbit: " << *qbit << std::endl;
    }
    basis preparingBase = (gRandom->Rndm()<0.5?ZeroOne:PlusMinus);
    polarization preparingPol = (gRandom->Rndm()<0.5);
    qbit->PrepareState(preparingBase, preparingPol);
}
