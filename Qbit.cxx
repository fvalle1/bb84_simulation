//
// Created by Filippo Valle on 15/11/2018.
//

#include "Qbit.h"

Qbit::Qbit(bool islogic) :    fIsLogic(islogic) {
    if(Qbit::DEBUG) printf("\nCreating qbit..\n");

    if(fIsLogic) fPhysicsQbits = 3;
    else fPhysicsQbits = 1;

    fTheta = new double[fPhysicsQbits];
    for(int qbit = 0; qbit < fPhysicsQbits; qbit++) fTheta[qbit] = 0.;

    fBase = ZeroOne;
    fState = false;            // il Qbit di default è costruito con polarizzazione orizzontale: base=ZeroOne, polarizzazione=0
}

Qbit::Qbit(const Qbit &qbit) : fPhysicsQbits(qbit.fPhysicsQbits),
                               fBase(qbit.fBase),
                               fState(qbit.fState),
                               fIsLogic(qbit.fIsLogic)
{
    if(Qbit::DEBUG) printf("\nCopying qbit..\n");
    fTheta=new double[qbit.fPhysicsQbits];
    PrepareTheta();
}

Qbit &Qbit::operator=(const Qbit &source) {
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
    if (fBase == ZeroOne) {
        if (fState) {
            //preparing state |1>
            angleToSet = TMath::PiOver2();
        }else {
            //preparing state |0>
            angleToSet = 0.;
        }
    }else{
        if (fState) {
            //preparing state |+>
            angleToSet = TMath::PiOver4();
        }else {
            //preparing state |->
            angleToSet = -TMath::PiOver4();
        }
    }
    //note: logic qbits have all 3 physical qbits identical after preparation
    for (int qbit = 0; qbit < fPhysicsQbits; qbit++) fTheta[qbit] = angleToSet;
}


void Qbit::MeasureState(basis b) {
    if (Qbit::DEBUG) printf("\nMeasuring qbit\n");

    //Qbit collapse to the basis used for measure
    fBase = b;

    //Measuring basis PlusMinus is the same as measuring with a ZeroOne filter
    //on a qubit rotated by 45grad anticlockwise
    if (b == PlusMinus) for(int iqbit = 0; iqbit < fPhysicsQbits; iqbit++) fTheta[iqbit] += TMath::PiOver4();
    if (!fIsLogic) { //if only have one qbit I measure it
        fState = MeasurePhisicalqbit(0);
    }else{ //if qbit is logic I measure all qbits and then extimate syndrome
        polarization measures[fPhysicsQbits];
        for (int iqbit = 0; iqbit < fPhysicsQbits; iqbit++) measures[iqbit] = MeasurePhisicalqbit(iqbit);
        bool syndrome01 = measures[0] == measures[1];
        bool syndrome12 = measures[1] == measures[2];
        if(syndrome01){//0 e 1 are the same --> measure 0 (eg 001)
            fState = measures[0];
        }else{//0 e 1 are different
            if(syndrome12){ //1 e 2 are the same --> measure 1 (011)
                fState = measures[1];
            }else{//both 0 and 1 , 1 and 2 are different --> measure 0 (010)
                fState = measures[0];
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
    return gRandom->Rndm() > cosTheta * cosTheta;
}

bool Qbit::operator==(const Qbit &qitCompared) const {
    bool toReturn = (fBase == qitCompared.fBase);          // confronto due qbit: se hanno la stessa base --> guardo lo stato
    if(toReturn){
        if(fState) return qitCompared.fState;               //se primo è vero, sono uguali se secondo è vero
        else return !qitCompared.fState;                    //se primo è falso sono uguali se secondo è !vero
    }else{
        return false;
    }
}


ostream& operator<<(ostream& os, const Qbit q){
    os << (q.fIsLogic?"logic":"physic") << " qbit"<< ", state: " <<q.fState << ", basis: "<<q.fBase                    // (q.fUseErrorCorrection?"logic":"physic") --> se fUseErrorCorrection = 1, è logico, altrimenti è fisico
       <<std::endl;
    for(int qbit = 0; qbit < q.fPhysicsQbits; qbit++) os <<"\ntheta "<< q.fTheta[qbit]<<std::endl<<std::endl<<std::endl;
    return os;
}

void Qbit::AddNoise(std::function<double()> pdfNoise) {
    // la pdf per aggiungere rumore indipendentemente a ogni qbit fisico
    for (int iqbit = 0; iqbit < fPhysicsQbits; iqbit++) fTheta[iqbit]+=pdfNoise();
}


