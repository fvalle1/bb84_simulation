//
// Created by Filippo Valle on 15/11/2018.
//

#include "Buddy.h"

bool Buddy::EveIsPresent = false;

void Buddy::InterceptQbit(Qbit *qbit) {
    if (EveIsPresent) {
        if (Qbit::DEBUG) {
            printf("\nIntercepting qbit..\n");
            std::cout << "received qbit: " << *qbit << std::endl;
        }
        ReceiveQbit(qbit);
        qbit->PrepareState(qbit->GetBase(), qbit->GetState());
    }
}

void Buddy::ReceiveQbit(Qbit *qbit){
    if(Qbit::DEBUG) {
        printf("\n\n**********\nReceiving qbit..\n");
        std::cout << "received qbit: " << *qbit << std::endl;              // stampo lo stato del qbit ricevuto
    }
    basis measuringBase = (gRandom->Rndm()<0.5?ZeroOne:PlusMinus);         // estraggo a caso una base
    qbit->MeasureState(measuringBase);                                     // misuro lo stato del qbit, proiettandolo con la base scelta
    if(Qbit::DEBUG) std::cout << "measured qbit: " << *qbit << std::endl;  // stampo lo stato del qbit trasformato 
}


void Buddy::PrepareQbit(Qbit *qbit) {                                      
    if(Qbit::DEBUG) {
        printf("\n\n**********\nPreparing qbit..\n");
        std::cout << "received qbit: " << *qbit << std::endl;
    }
    basis preparingBase = (gRandom->Rndm()<0.5?ZeroOne:PlusMinus);          // estraggo la base da usare
    polarization preparingPol = (gRandom->Rndm()<0.5);                      // estraggo lo stato 
    qbit->PrepareState(preparingBase, preparingPol);                        // preparo lo stato
}
