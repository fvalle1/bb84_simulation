//
// Created by Filippo Valle on 15/11/2018.
//

#include "TQbit.h"

ClassImp(TQbit)

TQbit::TQbit(bool islogic) : TObject(),
                             fIsLogic(islogic) {
    if(fIsLogic) fPhysicsQbits = 3;
    else fPhysicsQbits = 1;

    fTheta = new double[fPhysicsQbits];
    for(int qbit = 0; qbit < fPhysicsQbits; qbit++){
        fTheta[qbit] = 0;
    }
}

TQbit::TQbit(const TQbit &bit) : TObject(bit),
                           fPhysicsQbits(bit.fPhysicsQbits)
{
    if(bit.fTheta){
        fTheta=new double[bit.fPhysicsQbits];
        for(int qbit = 0; qbit < fPhysicsQbits; qbit++){
            fTheta[qbit] = bit.fTheta[qbit];
        }
    }
}

TQbit &TQbit::operator=(const TQbit &source) {
    if(this == &source) return *this;
    this->~TQbit();
    new(this) TQbit(source);
    return *this;
}


TQbit::~TQbit() {
    delete[] fTheta;
}

void TQbit::SetAngle(double * angle){
    for(int qbit = 0; qbit < fPhysicsQbits; qbit++)  fTheta[qbit] = angle[qbit];
}

void TQbit::SetAngle(double angle) {
    for(int qbit = 0; qbit < fPhysicsQbits; qbit++)  fTheta[qbit] = angle;
};
