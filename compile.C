//
// Created by Filippo Valle on 15/11/2018.
//

#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TString.h>
#include <TSystem.h>
#endif

int compile(TString myopt = "fast"){
    TString opt;
    if(myopt.Contains("force")) {
        opt = "kfO";
        gSystem->SetFlagsOpt("");
    }else{
        gSystem->SetFlagsOpt("-O3");
        opt = "kg0";
    }
    gSystem->CompileMacro("Qbit.cxx", opt.Data());
    gSystem->CompileMacro("Phone.cxx", opt.Data());
    gSystem->CompileMacro("Buddy.cxx", opt.Data());
    gSystem->CompileMacro("Channel.cxx", opt.Data());
    gSystem->CompileMacro("TGate.cxx", opt.Data());
    gSystem->CompileMacro("Phone.cxx", opt.Data());
    gSystem->CompileMacro("ConfigSimulation.cxx", opt.Data());
    gSystem->CompileMacro("Simulator.cxx", opt.Data());
    gSystem->CompileMacro("Analyzer.cxx", opt.Data());

    return 0;
}
