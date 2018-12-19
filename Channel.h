#ifndef TCHANNEL_H
#define TCHANNEL_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Qbit.h"
#endif

class Channel{
public:
    Channel();
    ~Channel();
    void PassQbit(Qbit* qbit);     //trasmissione del qbit: prende il qbit, lo altera se il canale è rumoroso
    inline void SetNoisy(std::function<double(void)> pdfNoise) {if(pdfNoise) fPdf=std::move(pdfNoise);};
    inline bool IsNoisy() const {return fPdf? true: false;}; //se fPDF è nullptr il canale è non noisy

private:
    std::function<double(void)> fPdf;

};

#endif
