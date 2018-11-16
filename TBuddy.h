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
    static void PrepareTQbit(TQbit *qbit);

    static void ReceiveTQbit(TQbit *qbit); //cambia base del qbit
    static void InterceptTQbit(TQbit *qbit, base measureBase);

ClassDef(TBuddy, 0)
};


#endif //BB84_SIMULATION_TBUDDY_H
