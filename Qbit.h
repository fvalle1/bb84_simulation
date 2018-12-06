//
// Created by Filippo Valle on 15/11/2018.
//

#ifndef BB84_SIMULATION_QBIT_H
#define BB84_SIMULATION_QBIT_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TMath.h>
#include <TF1.h>
#include <TRandom3.h>
#include <Riostream.h>
#endif

using std::ostream;

typedef enum basis{              // definisco il tipo "basis", cheè un enumeration. Cioè qualcosa che può assumere solo i valori Plusminuse ZeroOne
    ZeroOne,
    PlusMinus
} basis;

typedef bool polarization;       // definisco il tipo "polarization", che è un booleano. Sarà 0 o 1

class Qbit {
public:
    Qbit(bool islogic = false);
    Qbit(const Qbit &qbit);
    Qbit &operator=(const Qbit &source);
    virtual ~Qbit();

    inline basis GetBase() const {return fBase;}
    inline polarization GetState() const {return fState;}
    inline int GetNPhysicalQbits() const {return fPhysicsQbits;}

    inline void SetBase(basis b){fBase = b;}

    void SetState(polarization pol) {Qbit::fState = pol;}
    void PrepareState(basis b, polarization pol);
    void MeasureState(basis b);

    void AddNoise(std::function<double()>);

    bool operator==(const Qbit &qitCompared) const;
    bool operator!=(const Qbit &) const;
    friend ostream& operator<<(ostream& os, Qbit q);

    static const bool DEBUG = false;

private:
    double* fTheta;
    basis fBase;
    polarization fState;
    bool fIsLogic;
    int fPhysicsQbits;

    void PrepareTheta();
    polarization MeasurePhisicalqbit(int q);
};


#endif //BB84_SIMULATION_QBIT_H
