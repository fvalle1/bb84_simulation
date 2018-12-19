#ifndef TPHONE_H
#define TPHONE_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Qbit.h"
#endif

struct CommunicationInfos{           //informazioni da tenere
    int Ntot;                    
    int SameBasisAltered;
    int SameBasisUntouched;
};

class Phone {

public:
    Phone();
    Phone(const Phone& phone);
    ~Phone();

    void InitResults(CommunicationInfos &data);                       // per inizializzare la struttura CommunicationInfos
    void SetNewQbit(Qbit *qbitA);                                     // per assegnare a fQbitA il valore del qbit di ALice, per poter fare il confronto
    void MakeCallClassicalChannel(Qbit *qbit, CommunicationInfos &data);   // fa il confronto tra i qbit di Alice e Bob, restituisce un booleano: vero se sono uguali, falso se sono diversi

private:
    Qbit* fQbitA;        // qbit di Alice

};

#endif
