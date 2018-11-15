//
// Created by Filippo Valle on 15/11/2018.
//

#ifndef BB84_SIMULATION_TBUDDY_H
#define BB84_SIMULATION_TBUDDY_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TRandom3.h>
#include <TObject.h>
#include "TQbit.h"
#include "TGate.h"
#endif


class TBuddy : public TObject{
public:

    void GenerateTQbit(TQbit *qbit);
    TQbit GenerateTQbit(double angle);

    void ReceiveTQbit(TQbit qbit, base measureBase); //cambia base del qbit
    void InterceptTQbit(base measureBase);

private:
    TGate fGate;
ClassDef(TBuddy, 0)
};


#endif //BB84_SIMULATION_TBUDDY_H
