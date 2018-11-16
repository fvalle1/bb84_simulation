//
// Created by Filippo Valle on 15/11/2018.
//

#include "TBuddy.h"

ClassImp(TBuddy)

void TBuddy::InterceptTQbit(TQbit *qbit, base measureBase) {
    ReceiveTQbit(qbit);
    auto InterceptedBit = qbit->MeasureState(measureBase);
    qbit->PrepareState(measureBase, InterceptedBit);
}

void TBuddy::ReceiveTQbit(TQbit *qbit){
    qbit->MeasureState(ZeroOne);
}


void TBuddy::PrepareTQbit(TQbit *qbit) {
    qbit->PrepareState(ZeroOne, true);
}
