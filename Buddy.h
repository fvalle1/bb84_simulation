//
// Created by Filippo Valle on 15/11/2018.
//

#ifndef BB84_SIMULATION_TBUDDY_H
#define BB84_SIMULATION_TBUDDY_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TRandom3.h>
#include "Qbit.h"
#include "TGate.h"
#endif


class Buddy{
public:
    static void PrepareQbit(Qbit *qbit);
    static void ReceiveQbit(Qbit *qbit); //cambia base del qbit
    static void InterceptQbit(Qbit *qbit);

ClassDef(Buddy, 0)
};


#endif //BB84_SIMULATION_TBUDDY_H
