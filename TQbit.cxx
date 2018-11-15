//
// Created by Filippo Valle on 15/11/2018.
//

#include "TQbit.h"

ClassImp(TQbit)

TQbit::TQbit(bool islogic) : fIsLogic(islogic) {
    if(fIsLogic) fPhysicsQbits = 3;
    else fPhysicsQbits = 1;

    fTheta = new double[fPhysicsQbits];
    for(int qbit = 0; qbit < fPhysicsQbits; qbit++){
        fTheta[qbit] = 0;
    }
}

TQbit::~TQbit() {
    delete[] fTheta;
}
