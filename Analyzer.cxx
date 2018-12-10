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

    gStyle->SetLegendBorderSize(1);
    gStyle->SetLegendFillColor(0);
    gStyle->SetLegendFont(42);
    gStyle->SetOptStat(00000000);                                 // di default non mi stampa nessuna informazione

    gRandom->SetSeed(42);
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
    auto file = new TFile(Simulator::fFilename, "READ");
    // show use of logic qbits
    auto mg_LogicPhysics = new TMultiGraph();
    auto mg_ProbabilityVsNsent = new TMultiGraph();

    mg_LogicPhysics->SetTitle("Logic_vs_Physic; Nsent; Naltered");
    mg_ProbabilityVsNsent->SetTitle("Probability_vs_N; Nsent; Probability error");

    TObject* g_tmpptr;

    for(const auto &config : fVettInfos) {
        auto color = kOrange + static_cast<Color_t>(10 * config.fSigma);

        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(TString::Format("%s_%s", Simulator::fNPlotName, config.fInfos.c_str())));
        if (g_tmpptr) {
            if (config.fUseErrorCorrection) dynamic_cast<TGraphErrors*>(g_tmpptr)->SetMarkerStyle(23);
            else dynamic_cast<TGraphErrors*>(g_tmpptr)->SetMarkerStyle(20);
            dynamic_cast<TGraphErrors*>(g_tmpptr)->SetLineColor(color);
            dynamic_cast<TGraphErrors*>(g_tmpptr)->SetMarkerColor(color);
            dynamic_cast<TGraphErrors*>(g_tmpptr)->SetLineWidth(2);
            mg_LogicPhysics->Add(dynamic_cast<TGraphErrors*>(g_tmpptr));
        }else{
            std::cerr << std::endl << g_tmpptr << "nullptr reading fNPlotName" << std::endl;
        }

        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(TString::Format("%s_%s", Simulator::fProbabilityPlotName, config.fInfos.c_str())));
        if(g_tmpptr){
            Simulator::SetStylesAndDraw(g_tmpptr, "","", color);
            mg_ProbabilityVsNsent->Add(dynamic_cast<TGraphErrors*>(g_tmpptr));
        }else{
            std::cerr << std::endl << g_tmpptr << "nullptr reading fProbabilityPlotName" << std::endl;
        }

    }

    TLegend* leg;
    cx->cd();
    cx->Clear();
    cx->SetCanvasSize(1000, 600);
    cx->SetWindowSize(1050, 630);
    cx->SetTitle("bb84");
    cx->SetName("bb84");
    cx->Divide(1, 2);

    auto pad = cx->cd(1);
    mg_LogicPhysics->Draw("ALP");
    leg = pad->BuildLegend();
    TLine line;
    Simulator::SetStylesAndDraw(&line, "", "", kRedBlue, 5, 0);
    line.DrawLine(0,0.25,100,0.25);
    leg->SetEntrySeparation(0);
    leg->SetTextSize(0.05);

    pad = cx->cd(2);
    pad->SetLogy();
    mg_ProbabilityVsNsent->Draw("APL");
    g_tmpptr = dynamic_cast<TF1 *> (file->Get(Simulator::fProbabilityTeoPlotName));
    if(g_tmpptr) Simulator::SetStylesAndDraw(g_tmpptr, "", "", kRed, 1, 0);
    else std::cerr << std::endl << g_tmpptr << "nullptr reading fProbabilityTeoPlotName" << std::endl;
    leg = pad->BuildLegend();

    file->Close();
    delete file;

}