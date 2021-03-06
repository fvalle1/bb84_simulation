#include "ConfigSimulation.h"

/// constructor
ConfigSimulation::ConfigSimulation(bool IsLogic, uint32_t NQbits, uint32_t NSimulations, bool withEve, double sigma,
                                   std::function<double()> PdfNoise) :
        fUseErrorCorrection(IsLogic),
        fNQbits(NQbits),
        fNSimulations(NSimulations),
        fSigma(sigma),
        fEveIsPresent(withEve){
    if(sigma>1e-5) fPdfNoise = std::move(PdfNoise);
    else fPdfNoise = nullptr;
    fInfos = std::string(TString::Format("%s_N%d_sim%d%s%s_%4.2f", IsLogic?"L":"P", NQbits, NSimulations, withEve?"_Eve_":"_", sigma<=1e-5?"quiet":"noisy", sigma));
}


/// copy constructor
ConfigSimulation::ConfigSimulation(const ConfigSimulation &source):fUseErrorCorrection(source.fUseErrorCorrection),
                                                                   fNQbits(source.fNQbits),
                                                                   fNSimulations(source.fNSimulations),
                                                                   fSigma(source.fSigma),
                                                                   fInfos(source.fInfos),
                                                                   fEveIsPresent(source.fEveIsPresent){
    if(source.fSigma>1e-5) fPdfNoise = source.fPdfNoise;
    else fPdfNoise = nullptr;
}
