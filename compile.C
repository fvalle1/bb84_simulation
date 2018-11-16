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
        gSystem->SetFlagsOpt("-Wunused-parameter -Winconsistent-missing-override");
    }else{
        gSystem->SetFlagsOpt("-Wunused-parameter -Winconsistent-missing-override -O3");
        opt = "kg0";
    }

    gSystem->CompileMacro("TQbit.cxx", opt.Data());
    gSystem->CompileMacro("TPhone.cxx", opt.Data());
    gSystem->CompileMacro("TBuddy.cxx", opt.Data());
    gSystem->CompileMacro("TChannel.cxx", opt.Data());
    gSystem->CompileMacro("TGate.cxx", opt.Data());
    gSystem->CompileMacro("TPhone.cxx", opt.Data());
    gSystem->CompileMacro("TSimulator.cxx", opt.Data());
    return 0;
}