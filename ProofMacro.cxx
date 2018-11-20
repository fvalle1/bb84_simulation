#include <TFile.h>
#include <TTree.h>
#include <TProof.h>
#include "StopWatch.h"

void ProofMacro(bool parallel = true){
  if(parallel) TProof::Open("workers=2");
  auto file = new TFile("bb84_simulation.root");
  auto tree = (TTree*)file->Get("bb84");
  StopWatch watch;
  tree->Process("treebb84.C");

  return;
}
