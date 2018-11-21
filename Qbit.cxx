//
// Created by Filippo Valle on 15/11/2018.
//

#include "Qbit.h"

Qbit::Qbit(bool islogic) :    fIsLogic(islogic) {
    if(Qbit::DEBUG) printf("\nCreating qbit..\n");

    if(fIsLogic) fPhysicsQbits = 3;
    else fPhysicsQbits = 1;

    fTheta = new double[fPhysicsQbits];
    for(int qbit = 0; qbit < fPhysicsQbits; qbit++){
        fTheta[qbit] = 0.;
    }

    fBase = ZeroOne;
    fState = false;
}

Qbit::Qbit(const Qbit &qbit) : fPhysicsQbits(qbit.fPhysicsQbits),
                               fBase(qbit.fBase),
                               fState(qbit.fState),
                               fIsLogic(qbit.fIsLogic)
{
    if(Qbit::DEBUG) printf("\nCopying qbit..\n");
    fTheta=new double[qbit.fPhysicsQbits];
    for(int iqbit = 0; iqbit < fPhysicsQbits; iqbit++) {
        if (qbit.fTheta) { //copy values if defined
            fTheta[iqbit] = qbit.fTheta[iqbit];
        }else{
            fTheta[iqbit] = 0;
        }
    }

}

Qbit &Qbit::operator=(const Qbit &source) {
    if(Qbit::DEBUG) printf("\nCopying qbit with operator=..\n");
    if(this == &source) return *this;
    this->~Qbit();
    new(this) Qbit(source);
    return *this;
}


Qbit::~Qbit() {
    if(Qbit::DEBUG) printf("\nDestroying qbit..\n");
    delete[] fTheta;
}

void Qbit::PrepareState(basis b, polarization pol){
    if(Qbit::DEBUG) printf("\nPreparing qbit..\n");
    fBase = b;
    fState = pol;

    PrepareTheta();
}

void Qbit::PrepareTheta() {
    if (Qbit::DEBUG) printf("\nPreparing qbit\n");
    double angleToSet = 0.;
    if(fBase == ZeroOne){
        if(fState){
//preparing state |0>
            angleToSet = 0.;
        }else{
//preparing state |1>
            angleToSet = TMath::PiOver2();
        }
    }else{
        if(fState){
//preparing state |+>
            angleToSet = TMath::PiOver4();
        }else{
//preparing state |->
            angleToSet = - TMath::PiOver4();
        }
    }
    //note: logic qbits have all 3 physical qbits identical after preparation
    for(int qbit = 0; qbit < fPhysicsQbits; qbit++){fTheta[qbit] = angleToSet;}
}

void Qbit::MeasureState(basis b) {
    if (Qbit::DEBUG) printf("\nMeasuring qbit\n");

    //Qbit collapse to the basis used for measuring
    fBase = b;

    //Measuring basis PlusMinus is the same as measuring with a ZeroOne filter
    //the qubit rotated by 45grad anticlockwise
    if (b == PlusMinus) for(int iqbit = 0; iqbit < fPhysicsQbits; iqbit++) fTheta[iqbit] += TMath::PiOver4();
    if (!fIsLogic) {
        fState = MeasurePhisicalqbit(0);
    }else{
        polarization measures[fPhysicsQbits];
        for (int iqbit = 0; iqbit < fPhysicsQbits; iqbit++) measures[iqbit] = MeasurePhisicalqbit(iqbit);
        bool sindrome01 = measures[0] == measures[1];
        bool sindrome12 = measures[1] == measures[2];
        if(sindrome01){//0 e 1 are the same --> measure 0 (eg 001)
            measures[0];
        }else{//0 e 1 are different
            if(sindrome12){ //1 e 2 are the same --> measure 1 (011)
                measures[1];
            }else{//both 0 and 1 , 1 and 2 are different --> measure 0 (010)
                measures[0];
            }
        }
    }
    PrepareTheta();
}

polarization Qbit::MeasurePhisicalqbit(int q) {
    double cosTheta = TMath::Cos(fTheta[q]);
    //Return the polarization of the object
//if basis is ZeroOne state is cos() |0> + sin() |1>
//it is zero (aka down or false) with probability cos*cos
    return fState = gRandom->Rndm() > cosTheta * cosTheta;
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
    os << (q.fIsLogic?"logic":"physic") << " qbit"<< ", state: " <<q.fState << ", basis: "<<q.fBase
       <<std::endl;
    for(int qbit = 0; qbit < q.fPhysicsQbits; qbit++) os <<"\ntheta "<< q.fTheta[qbit]<<std::endl<<std::endl<<std::endl;
    return os;
}

void Qbit::AddNoise() {
    for (int iqbit = 0; iqbit < fPhysicsQbits; iqbit++) fTheta[iqbit]+=gRandom->Gaus(0, 0.2);
}


