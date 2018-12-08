#include "ConfigSimulation.h"

ConfigSimulation::ConfigSimulation(bool IsLogic, int NQbits, int NSimulations, bool withEve, double sigma,
                                   std::function<double()> PdfNoise) :
        fIsLogic(IsLogic),
        fNQbits(NQbits),
        fNSimulations(NSimulations),
        fWithEve(withEve),
        fSigma(sigma){
    if(sigma>1e-5) fPdfNoise = std::move(PdfNoise);
    else fPdfNoise = nullptr;
    Buddy::EveIsSleeping = !withEve;
    fInfos = std::string(TString::Format("%s_N%d_sim%d%s%s_%4.2f", IsLogic?"L":"P", NQbits, NSimulations, withEve?"_Eve_":"_", sigma<=1e-5?"quiet":"noisy", sigma));
}

ConfigSimulation::ConfigSimulation(const ConfigSimulation &source):fIsLogic(source.fIsLogic),
                                                                   fNQbits(source.fNQbits),
                                                                   fNSimulations(source.fNSimulations),
                                                                   fWithEve(source.fWithEve),
                                                                   fSigma(source.fSigma),
                                                                   fInfos(source.fInfos){
    if(source.fSigma>1e-5) fPdfNoise = source.fPdfNoise;
    else fPdfNoise = nullptr;
}
