//
// Created by Filippo Valle on 15/11/2018.
//

#ifndef BB84_SIMULATION_QBIT_H
#define BB84_SIMULATION_QBIT_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Riostream.h>
#include <TMath.h>
#include <TRandom3.h>
#include <functional>
#endif

using std::ostream;

typedef enum base{              // definisco il tipo "base", cheè un enumeration. può assumere solo i valori Plusminuse ZeroOne
    ZeroOne,
    PlusMinus
} base;

typedef bool polarization;       // definisco il tipo "polarization", che è un booleano. Sarà 0 o 1

class Qbit {
public:
    Qbit(bool islogic = false);
    Qbit(const Qbit &qbit);
    Qbit &operator=(const Qbit &source);
    ~Qbit();

    inline base GetBase() const {return fBase;}
    inline polarization GetState() const {return fState;}
    inline int GetNPhysicalQbits() const {return fPhysicsQbits;}

    void PrepareState(base b, polarization pol);
    void MeasureState(base b);

    void AddNoise(std::function<double()>);

    bool operator==(const Qbit &qitCompared) const;
    friend ostream& operator<<(ostream& os, Qbit q);

    static const bool DEBUG = false;

private:
    double* fTheta;
    base fBase;
    polarization fState;
    bool fIsLogic;
    int fPhysicsQbits;

    void PrepareTheta();
    polarization MeasurePhisicalqbit(int q);
};


#endif //BB84_SIMULATION_QBIT_H
