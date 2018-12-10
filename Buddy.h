//
// Created by Filippo Valle on 15/11/2018.
//

#ifndef BB84_SIMULATION_BUDDY_H
#define BB84_SIMULATION_BUDDY_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Qbit.h"
#endif


class Buddy{
public:
    static void PrepareQbit(Qbit *qbit);    //Preparo il qbit con base e polarizzazione
    static void ReceiveQbit(Qbit *qbit);    //cambia basis del qbit
    static void InterceptQbit(Qbit *qbit);  //intercetto qbit: Eve

    static bool EveIsSleeping;              //decido se eve deve intervenire o meno
};


#endif //BB84_SIMULATION_TBUDDY_H
