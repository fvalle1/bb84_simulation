//
// Created by francescobonacina on 06/12/18.
//


#include "Analyzer.h"

Analyzer* Analyzer::fgAnalyzer = nullptr;

Analyzer* Analyzer::Instance(std::vector<ConfigSimulation> VettInfos) {
    if(fgAnalyzer) {
        fgAnalyzer->~Analyzer();
        new(fgAnalyzer) Analyzer(VettInfos);
    }else{
        fgAnalyzer = new Analyzer(VettInfos);
    }
    return fgAnalyzer;
}


Analyzer::Analyzer(std::vector<ConfigSimulation> VettInfos) {
    fVettInfos = std::move(VettInfos);
}

Analyzer::~Analyzer() {
    fVettInfos.clear();
}

void Analyzer::RunAnalyzer(){
    for(const auto &config : fVettInfos){
        auto sim = Simulator::Instance(config);
        sim->RunSimulation()->GeneratePlots();
    }

}

void Analyzer::JoinResults(TCanvas *cx) {
    TFile* file = new TFile(Simulator::fFilename, "READ");
    // show use of logic qbits
    auto mg_LogicPhysics = new TMultiGraph();
    auto mg_ProbabilityVsNsent = new TMultiGraph();

    mg_LogicPhysics->SetTitle("Logic_vs_Physic; Nsent; Naltered");

    mg_ProbabilityVsNsent->SetTitle("Probability_vs_N; Nsent; Probability error");

    TGraphErrors* g_tmpptr;

    for(const auto &config : fVettInfos) {
        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(TString::Format("%s_%s", Simulator::fNPlotName, config.fInfos.c_str())));
        if (g_tmpptr) {
            if (config.fIsLogic) g_tmpptr->SetMarkerStyle(23);
            else g_tmpptr->SetMarkerStyle(20);
            g_tmpptr->SetLineColor(kBlue + static_cast<int>(10 * config.fSigma));
            mg_LogicPhysics->Add(g_tmpptr);
        }else{
            std::cerr << g_tmpptr << "nullptr reading fNPlotName" << std::endl;
        }

        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(TString::Format("%s_%s", Simulator::fProbabilityPlotName, config.fInfos.c_str())));
        if(g_tmpptr){
            mg_ProbabilityVsNsent->Add(g_tmpptr);
        }else{
            std::cerr << g_tmpptr << "nullptr reading fProbabilityPlotName" << std::endl;
        }

        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(TString::Format("%s_%s", Simulator::fProbabilityTeoPlotName, config.fInfos.c_str())));
        if(g_tmpptr){
            mg_ProbabilityVsNsent->Add(g_tmpptr);
        }else{
            std::cerr << g_tmpptr << "nullptr reading fProbabilityTeoPlotName" << std::endl;
        }

    }


    cx->cd();
    cx->Clear();
    cx->SetCanvasSize(1000, 900);
    cx->SetWindowSize(1050, 930);
    cx->SetTitle("bb84");
    cx->SetName("bb84");
    cx->Divide(2, 1);

    auto pad = cx->cd(1);
    mg_LogicPhysics->Draw("ALP");
    TLine line;
    line.SetLineWidth(3);
    line.SetLineColor(kRedBlue);
    line.DrawLine(0,0.25,100,0.25);
    pad->BuildLegend();

    pad = cx->cd(2);
    pad->SetLogy();
    mg_ProbabilityVsNsent->Draw("APL");

    file->Close();
    delete file;

}