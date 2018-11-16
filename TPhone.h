#ifndef TPHONE_H
#define TPHONE_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Riostream.h>
#include <TObject.h>
#include <TTree.h>
#include "TQbit.h"
#endif

class TPhone : public TTree {

public:
    TPhone();
    TPhone(const char *name);
    TPhone(const TPhone& phone);
    virtual ~TPhone();

    void InitResults();
    void SetNewQbit(TQbit *qbitA);
    void Update(TQbit *qbit);
    void AddPoint();

    virtual TBranch* GetBranch();

    struct fStructToSave{
        int Ntot;
        int SameBasisIntercept;
        int SameBasisNoIntercept;
    };

private:
    TQbit* fQbitA;
    fStructToSave fResult;


ClassDef(TPhone,0)
};

#endif
