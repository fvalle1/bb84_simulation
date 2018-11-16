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

typedef bool polarization;

class TQbit : public TObject {
public:
    TQbit(bool islogic = false);
    TQbit(const TQbit &qbit);
    TQbit &operator=(const TQbit &source);
    virtual ~TQbit();

    inline base GetBase() const {return fBase;}
    inline polarization GetState() const {return fState;}
    inline int GetNPhysicalQbits() const {return fPhysicsQbits;}

    inline void SetBase(base b){fBase = b;}
    void SetState(polarization pol) {TQbit::fState = pol;}

    void PrepareState(base b, polarization pol);
    polarization MeasureState(base b);

    bool operator==(const TQbit &qitCompared) const;
    bool operator!=(const TQbit &) const;

    static const bool DEBUG = true;
private:
    double* fTheta;
    base fBase;
    polarization fState;
    bool fIsLogic;
    int fPhysicsQbits;
ClassDef(TQbit, 0)
};


#endif //BB84_SIMULATION_QBIT_H