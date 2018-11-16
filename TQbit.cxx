//
// Created by Filippo Valle on 15/11/2018.
//

#include <TRandom3.h>
#include "TQbit.h"

ClassImp(TQbit)

TQbit::TQbit(bool islogic) : TObject(),
                             fIsLogic(islogic) {
    if(TQbit::DEBUG) printf("\nCreating qbit\n");

    if(fIsLogic) fPhysicsQbits = 3;
    else fPhysicsQbits = 1;

    fTheta = new double[fPhysicsQbits];
    for(int qbit = 0; qbit < fPhysicsQbits; qbit++){
        fTheta[qbit] = 0;
    }

    fBase = ZeroOne;
    fState = false;
}

TQbit::TQbit(const TQbit &qbit) : TObject(qbit),
                                 fPhysicsQbits(qbit.fPhysicsQbits),
                                 fBase(qbit.fBase),
                                 fState(qbit.fState)
{
    if(TQbit::DEBUG) printf("\nCopying qbit\n");
    if(qbit.fTheta){
        fTheta=new double[qbit.fPhysicsQbits];
        for(int iqbit = 0; iqbit < fPhysicsQbits; iqbit++){
            fTheta[iqbit] = qbit.fTheta[iqbit];
        }
    }
}

TQbit &TQbit::operator=(const TQbit &source) {
    if(TQbit::DEBUG) printf("\nCopying qbit with operator=\n");

    if(this == &source) return *this;
    this->~TQbit();
    new(this) TQbit(source);
    return *this;
}


TQbit::~TQbit() {
    if(TQbit::DEBUG) printf("\nDestroying qbit\n");
    delete[] fTheta;
}

void TQbit::PrepareState(base b, polarization pol){
    if(TQbit::DEBUG) printf("\nPreparing qbit\n");
    double angleToSet = 0.;
    if(b == ZeroOne){
        if(pol){
            angleToSet = 0.;
        }else{
            angleToSet = TMath::PiOver2();
        }
    }else{
        if(pol){
            angleToSet = TMath::PiOver4();
        }else{
            angleToSet = -TMath::PiOver4();
        }
    }
    for(int qbit = 0; qbit < fPhysicsQbits; qbit++){fTheta[qbit] = 0;}
}

polarization TQbit::MeasureState(base b){
    if(TQbit::DEBUG) printf("\nMeasuring qbit\n");
    //Return the polarization of th object
    //if basis is zero and one state is cos() |0> + sin() |1>
    //it is zero (aka down, aka false) with probability cos*cos
    if(b == PlusMinus) {
        fTheta[0] -= TMath::PiOver4();
    }
    double cosTheta = TMath::Cos(fTheta[0]);
    fBase = b;
    return gRandom->Rndm() > cosTheta * cosTheta;
}

bool TQbit::operator==(const TQbit &qitCompared) const {
    bool toReturn = (fBase == qitCompared.fBase) &&
            (fPhysicsQbits == qitCompared.fPhysicsQbits);
if(toReturn){
    if(fState) return qitCompared.fState;
    else return !qitCompared.fState;
}else{
    return false;
}
}

bool TQbit::operator!=(const TQbit &qbit) const {
    return !(qbit == *this);
}

