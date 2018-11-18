#ifndef TPHONE_H
#define TPHONE_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Riostream.h>
#include <TTree.h>
#include "Qbit.h"
#endif

struct fStructToSave{
    int Ntot;
    int SameBasisIntercept;
    int SameBasisNoIntercept;
};

class Phone {

public:
    Phone();
    Phone(const Phone& phone);
    virtual ~Phone();

    void InitResults(fStructToSave &data);
    void SetNewQbit(Qbit *qbitA);
    bool MakeCallClassicalChannel(Qbit *qbit, fStructToSave &data);

private:
    Qbit* fQbitA;

ClassDef(Phone,0)
};

#endif
