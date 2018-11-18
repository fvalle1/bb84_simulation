//
// Created by Filippo Valle on 15/11/2018.
//

#include "Qbit.h"

ClassImp(Qbit)

Qbit::Qbit(bool islogic) :    fIsLogic(islogic) {
    if(Qbit::DEBUG) printf("\nCreating qbit\n");

    if(fIsLogic) fPhysicsQbits = 3;
    else fPhysicsQbits = 1;

    fTheta = new double[fPhysicsQbits];
    for(int qbit = 0; qbit < fPhysicsQbits; qbit++){
        fTheta[qbit] = 0;
    }

    fBase = ZeroOne;
    fState = false;
}

Qbit::Qbit(const Qbit &qbit) : fPhysicsQbits(qbit.fPhysicsQbits),
                               fBase(qbit.fBase),
                               fState(qbit.fState)
{
    if(Qbit::DEBUG) printf("\nCopying qbit\n");
    if(qbit.fTheta){
        fTheta=new double[qbit.fPhysicsQbits];
        for(int iqbit = 0; iqbit < fPhysicsQbits; iqbit++){
            fTheta[iqbit] = qbit.fTheta[iqbit];
        }
    }
}

Qbit &Qbit::operator=(const Qbit &source) {
    if(Qbit::DEBUG) printf("\nCopying qbit with operator=\n");

    if(this == &source) return *this;
    this->~Qbit();
    new(this) Qbit(source);
    return *this;
}


Qbit::~Qbit() {
    if(Qbit::DEBUG) printf("\nDestroying qbit\n");
    delete[] fTheta;
}

void Qbit::PrepareState(base b, polarization pol){
    if(Qbit::DEBUG) printf("\nPreparing qbit\n");
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
    fBase = b;
    fState = pol;
    for(int qbit = 0; qbit < fPhysicsQbits; qbit++){fTheta[qbit] = angleToSet;}
}

void Qbit::MeasureState(base b){
    if(Qbit::DEBUG) printf("\nMeasuring qbit\n");
    //Return the polarization of th object
    //if basis is zero and one state is cos() |0> + sin() |1>
    //it is zero (aka down, aka false) with probability cos*cos
    if(b == PlusMinus) {
        fTheta[0] += TMath::PiOver4();
    }
    double cosTheta = TMath::Cos(fTheta[0]);
    fBase = b;
    fState = gRandom->Rndm() > cosTheta * cosTheta;
}

bool Qbit::operator==(const Qbit &qitCompared) const {
    bool toReturn = (fBase == qitCompared.fBase);
    if(toReturn){
        if(fState) return qitCompared.fState;
        else return !qitCompared.fState;
    }else{
        return false;
    }
}

bool Qbit::operator!=(const Qbit &qbit) const {
    return !(qbit == *this);
}

ostream& operator<<(ostream& os, const Qbit q){
    os << (q.fIsLogic?"logic":"physic") << " qbit"<< ", state: " <<q.fState << ", base: "<<q.fBase
       <<std::endl;
    return os;
}


