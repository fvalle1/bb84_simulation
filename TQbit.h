//
// Created by Filippo Valle on 15/11/2018.
//

#ifndef BB84_SIMULATION_QBIT_H
#define BB84_SIMULATION_QBIT_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TObject.h"
#include "TMath.h"
#endif

enum base{
    PlusMinus,
    ZeroOne
};


class TQbit : public TObject {
public:
    TQbit(bool islogic = false);
    TQbit(const TQbit &bit);
    TQbit &operator=(const TQbit &source);
    virtual ~TQbit();

    inline void GetAngle(double* theta) const {for(int qbit = 0; qbit < fPhysicsQbits; qbit++) theta[qbit] = fTheta[qbit];}
    inline base GetBase() const {return fBase;}
    int GetNPhysicalQbits() const {return fPhysicsQbits;}

    void SetAngle(double * angle);
    void SetAngle(double angle);
    inline void SetBase(base b){fBase = b;}

private:
    double* fTheta;
    base fBase;
    bool fIsLogic;
    int fPhysicsQbits;
ClassDef(TQbit, 0)
};


#endif //BB84_SIMULATION_QBIT_H