//
// Created by Filippo Valle on 15/11/2018.
//

#include "TBuddy.h"

ClassImp(TBuddy)

void TBuddy::InterceptTQbit(base measureBase) {
    ReceiveTQbit()
    GenerateTQbit(nullptr)
}

void TBuddy::GenerateTQbit(TQbit *qbit) {
    qbit->SetAngle(gRandom->Rndm());
}
