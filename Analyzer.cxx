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
    int current = 0;
    auto max = fVettInfos.size();
    for(const auto &config : fVettInfos){
        auto sim = Simulator::Instance(config);
        printf("\n**************\nSimulation %d/%lu", ++current, max);
        sim->RunSimulation()->GeneratePlots();
    }
}

void Analyzer::JoinResults(TCanvas *cx) {
    auto file = new TFile(Simulator::fFilename, "READ");
    // show use of logic qbits
    auto mg_NalteredVsNsent = new TMultiGraph();
    auto mg_ProbabilityVsNsent = new TMultiGraph();

    mg_NalteredVsNsent->SetTitle("Logic_vs_Physic; Nsent; Naltered");
    mg_ProbabilityVsNsent->SetTitle("Probability_vs_N; Nsent; Probability error");

    FillMultiGraphs(file, mg_NalteredVsNsent, mg_ProbabilityVsNsent);

    cx->cd();
    cx->Clear();
    cx->SetCanvasSize(1000, 600);
    cx->SetWindowSize(1050, 630);
    cx->SetTitle("bb84");
    cx->SetName("bb84");
    cx->Divide(1, 2);

    AddMultiGraphToCanvas(cx, file, mg_NalteredVsNsent, mg_ProbabilityVsNsent);


//    delete mg_NalteredVsNsent;
//    delete mg_ProbabilityVsNsent;
    file->Close();
    delete file;
}

void Analyzer::FillMultiGraphs(TFile *file, TMultiGraph *mg_NalteredVsNsent, TMultiGraph *mg_ProbabilityVsNsent) const {
    TObject* g_tmpptr;

    for(const auto &config : fVettInfos) {
        auto color = kOrange + static_cast<Color_t>(10 * config.fSigma);

        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(TString::Format("%s_%s", Simulator::fNPlotName, config.fInfos.c_str())));
        if (g_tmpptr) {
            if (config.fUseErrorCorrection) Simulator::SetStylesAndDraw(g_tmpptr, "", "", color, 2, 23);
            else Simulator::SetStylesAndDraw(g_tmpptr, "", "", color, 2, 20);
            mg_NalteredVsNsent->Add(dynamic_cast<TGraphErrors*>(g_tmpptr));
        }else{
            std::__1::cerr << std::__1::endl << g_tmpptr << "nullptr reading fNPlotName" << std::__1::endl;
        }

        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(
                TString::Format("%s_%s", Simulator::fProbabilityPlotName, config.fInfos.c_str())));
        if(g_tmpptr){
            if (config.fUseErrorCorrection) Simulator::SetStylesAndDraw(g_tmpptr, "", "", color, 2, 23);
            else Simulator::SetStylesAndDraw(g_tmpptr, "", "", color, 2, 20);
            mg_ProbabilityVsNsent->Add(dynamic_cast<TGraphErrors*>(g_tmpptr));
        }else{
            std::__1::cerr << std::__1::endl << g_tmpptr << "nullptr reading fProbabilityPlotName" << std::__1::endl;
        }

    }
}

void Analyzer::AddMultiGraphToCanvas(TCanvas *cx, TFile *file, TMultiGraph *mg_NalteredVsNsent,
                                     TMultiGraph *mg_ProbabilityVsNsent) const {
    TLegend* leg;

    auto pad = cx->cd(1);
    mg_NalteredVsNsent->Draw("ALP");
    leg = pad->BuildLegend();
    TLine line;
    Simulator::SetStylesAndDraw(&line, "", "", kRedBlue, 6, 0);
    line.DrawLine(0,0.25,100,0.25);
    leg->SetEntrySeparation(0);
    leg->SetTextSize(0.05);

    pad = cx->cd(2);
    pad->SetLogy();
    mg_ProbabilityVsNsent->Draw("APL");
    Simulator::SetStylesAndDraw(dynamic_cast<TF1 *> (file->Get(Simulator::fProbabilityTeoPlotName)), "", "", kRed, 4, 0);
    leg = pad->BuildLegend();
    leg->SetEntrySeparation(0);
    leg->SetTextSize(0.05);
}