//
// Created by Filippo Valle on 15/11/2018.
//

#ifndef BB84_SIMULATION_QBIT_H
#define BB84_SIMULATION_QBIT_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TObject.h"
#include "TMath.h"
#endif

class TQbit : public TObject {
public:
    TQbit(bool islogic = false);
    virtual ~TQbit();

    inline void GetAngle(double* theta) const {for(int qbit = 0; qbit < fPhysicsQbits; qbit++) theta[qbit] = fTheta[qbit];}
    inline void SetBase(base b){fBase = b;}
    int GetNPhysicalQbits() const {return fPhysicsQbits;};

private:
    double* fTheta;
    base fBase;
    bool fIsLogic;
    int fPhysicsQbits;
ClassDef(TQbit, 0)
};


#endif //BB84_SIMULATION_QBIT_H