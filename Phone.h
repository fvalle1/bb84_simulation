#ifndef TPHONE_H
#define TPHONE_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Riostream.h>
#include <TTree.h>
#include "Qbit.h"
#endif

struct fStructToSave{           //informazioni da tenere
    int Ntot;                    
    int SameBasisIntercept;
    int SameBasisNoIntercept;
};

class Phone {

public:
    Phone();
    Phone(const Phone& phone);
    virtual ~Phone();

    void InitResults(fStructToSave &data);                            // per salvare le informazioni nella struttura fStructToSave                            
    void SetNewQbit(Qbit *qbitA);                                     // per assegnare a fQbitA il valore del qbit di ALice, per poter fare il confronto
    bool MakeCallClassicalChannel(Qbit *qbit, fStructToSave &data);   // fa il confronto tra i qbit di Alice e Bob, restituisce un booleano: vero se sono uguali, falso se sono diversi

private:
    Qbit* fQbitA;        // qbit di Alice

};

#endif
