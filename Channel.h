#ifndef TCHANNEL_H
#define TCHANNEL_H

#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Qbit.h"
#include <functional>
#endif

class Channel{

private:
    std::function<double(void)> fPdf;

public:
    Channel();
    ~Channel();
    void PassQbit(Qbit* qbit);                                  //trasmissione del qbit: prende il qbit, lo altera se il canale è rumoroso
    inline void SetNoisy(std::function<double(void)> pdfNoise) {fPdf=std::move(pdfNoise);};
    inline bool IsNoisy() const {return fPdf? true: false;};
};

#endif
