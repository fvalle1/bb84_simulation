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

void Analyzer::JoinResults(TCanvas *cx, int plotFunctionOfErrors) {
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
    if(plotFunctionOfErrors!=0) cx->Divide(2,1);

    AddMultiGraphToCanvas(cx->cd(1), file, mg_NalteredVsNsent, mg_ProbabilityVsNsent);
    PlotFunctionOfErrors(cx, file, plotFunctionOfErrors);


//    delete mg_NalteredVsNsent;
//    delete mg_ProbabilityVsNsent;
    file->Close();
    delete file;
}

void Analyzer::FillMultiGraphs(TFile *file, TMultiGraph *mg_NalteredVsNsent, TMultiGraph *mg_ProbabilityVsNsent) const {
    TObject* g_tmpptr;

    for(const auto &config : fVettInfos) {
        Color_t color;
        if(config.fEveIsPresent) color = kGreen; //Green(407) and violet(880) should be distant RTypes.h
        else color = kViolet;
        color += static_cast<Color_t>(10 * config.fSigma); //color changes due to error factor

        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(TString::Format("%s_%s", Simulator::fNPlotName, config.fInfos.c_str())));
        if (g_tmpptr) {
            if (config.fUseErrorCorrection) Simulator::SetStylesAndDraw(g_tmpptr, "", "", color, 2, 23);
            else Simulator::SetStylesAndDraw(g_tmpptr, "", "", color, 2, 20);
            mg_NalteredVsNsent->Add(dynamic_cast<TGraphErrors*>(g_tmpptr));
        }else{
            std::cerr << std::endl << g_tmpptr << "nullptr reading fNPlotName" << std::endl;
        }

        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(
                TString::Format("%s_%s", Simulator::fProbabilityPlotName, config.fInfos.c_str())));
        if(g_tmpptr){
            if (config.fUseErrorCorrection) Simulator::SetStylesAndDraw(g_tmpptr, "", "", color, 2, 23);
            else Simulator::SetStylesAndDraw(g_tmpptr, "", "", color, 2, 20);
            mg_ProbabilityVsNsent->Add(dynamic_cast<TGraphErrors*>(g_tmpptr));
        }else{
            std::cerr << std::endl << g_tmpptr << "nullptr reading fProbabilityPlotName" << std::endl;
        }

    }
}

void Analyzer::AddMultiGraphToCanvas(TVirtualPad *cx, TFile *file, TMultiGraph *mg_NalteredVsNsent,
                                     TMultiGraph *mg_ProbabilityVsNsent) const {


    TLegend* leg;
    cx->Divide(1, 2);
    auto pad = cx->cd(1);
    mg_NalteredVsNsent->Draw("APL");
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

void Analyzer::PlotFunctionOfErrors(TCanvas *cx, TFile *file, int Nfixed) {
    if(Nfixed!=0) {
        auto leftPad = cx->cd(1);
        auto rightPad = cx->cd(2);
        rightPad->Divide(1, 2);

        TVirtualPad *pad;
        pad = leftPad->cd(1);
        TLine line;
        Simulator::SetStylesAndDraw(&line, "", "", kBlue, 5);
        line.DrawLine(Nfixed, -0.1, Nfixed, 0.6);

        pad = rightPad->cd(1);


        TGraphErrors* NalteredVsError[4]; //[0] P [1] L [2] P_Eve [3]L_Eve
        for (auto &graph : NalteredVsError) {
            graph = new TGraphErrors(static_cast<int>(fVettInfos.size()));
            graph->SetNameTitle("NAlteredVsError_Physical_qbits");
        }
        int pointCntr[4] = {0};
        TGraph* g_tmpptr = nullptr;
        double x , y;

        for(const auto &config : fVettInfos) {
            g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(TString::Format("%s_%s", Simulator::fNPlotName, config.fInfos.c_str())));
            if (g_tmpptr) {
                g_tmpptr->GetPoint(Nfixed - 1, x, y); //Get value of NAlteredVsN at N = Nfixed
                int iGraph = config.fUseErrorCorrection?1:0;
                if(config.fEveIsPresent) iGraph+=2;
                NalteredVsError[iGraph]->SetPoint(pointCntr[iGraph], config.fSigma, y);
                NalteredVsError[iGraph]->SetPointError(pointCntr[iGraph]++, 0, dynamic_cast<TGraphErrors *>(g_tmpptr)->GetEY()[Nfixed-1]);
            }
        }

        Simulator::SetStylesAndDraw(NalteredVsError[0], "", "", kCyan - 2, 1, 24);
        Simulator::SetStylesAndDraw(NalteredVsError[1], "", "", kViolet, 1, 32);
        Simulator::SetStylesAndDraw(NalteredVsError[2], "", "", kCyan - 9, 1, 24);
        Simulator::SetStylesAndDraw(NalteredVsError[3], "", "", kViolet + 4, 1, 32);

        auto mg_NAlteredVsNoise = new TMultiGraph();
        mg_NAlteredVsNoise->SetTitle("Altered_vs_Noise; #sigma noise; Naltered");
        for (int i = 0; i < 4 ; i++) {
            NalteredVsError[i]->Set(pointCntr[i]);
            mg_NAlteredVsNoise->Add(NalteredVsError[i]);
        }

        mg_NAlteredVsNoise->Draw("APL");
        auto leg = pad->BuildLegend();
        leg->SetEntrySeparation(0);
        leg->SetTextSize(0.05);
    }
}
