//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Nov 20 07:13:16 2018 by ROOT version 6.14/06
// from TTree bb84/bb84
// found on file: bb84_simulation.root
//////////////////////////////////////////////////////////

#ifndef treebb84_h
#define treebb84_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TH1D.h>

// Headers needed by this particular selector


class treebb84 : public TSelector {
  public :
  TTreeReader     fReader;  //!the tree reader
  TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

  // Readers to access the data (delete the ones you do not need).
  TTreeReaderValue<Int_t> Ntot = {fReader, "dataBranch.Ntot"};
  TTreeReaderValue<Int_t> SameBasisIntercept = {fReader, "dataBranch.SameBasisIntercept"};
  TTreeReaderValue<Int_t> SameBasisNoIntercept = {fReader, "dataBranch.SameBasisNoIntercept"};


  treebb84(TTree * /*tree*/ =0) { }
  virtual ~treebb84() { }
  virtual Int_t   Version() const { return 2; }
  virtual void    Begin(TTree *tree);
  virtual void    SlaveBegin(TTree *tree);
  virtual void    Init(TTree *tree);
  virtual Bool_t  Notify();
  virtual Bool_t  Process(Long64_t entry);
  virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
  virtual void    SetOption(const char *option) { fOption = option; }
  virtual void    SetObject(TObject *obj) { fObject = obj; }
  virtual void    SetInputList(TList *input) { fInput = input; }
  virtual TList  *GetOutputList() const { return fOutput; }
  virtual void    SlaveTerminate();
  virtual void    Terminate();

  TH1D* fHist;

  ClassDef(treebb84,0);

};

#endif

#ifdef treebb84_cxx
void treebb84::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the reader is initialized.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).

  fReader.SetTree(tree);
}

Bool_t treebb84::Notify()
{
  // The Notify() function is called when a new file is opened. This
  // can be either for a new TTree in a TChain or when when a new TTree
  // is started when using PROOF. It is normally not necessary to make changes
  // to the generated code, but the routine can be extended by the
  // user if needed. The return value is currently not used.

  return kTRUE;
}


#endif // #ifdef treebb84_cxx
