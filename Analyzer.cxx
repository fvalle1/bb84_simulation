//
// Created by francescobonacina on 06/12/18.
//


#include <THStack.h>
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

void Analyzer::JoinResults(TCanvas *cx, uint32_t fixedN) {
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
    cx->Divide(2,3); //separe leftRight

    AlteredVsSent(cx->cd(1), mg_NalteredVsNsent, fixedN);
    ProbabilityVsSent(cx->cd(3), file, mg_ProbabilityVsNsent);
    PlotFunctionOfErrors(cx->cd(2), file, fixedN);
    PlotSlopeVsNoise(cx->cd(4), file);
    PlotNalteredDistributions(cx->cd(5), file);
    PlotFunctionOfAltered(cx->cd(6), file);


//    delete mg_NalteredVsNsent;
//    delete mg_ProbabilityVsNsent;
    file->Close();
    delete file;
}

void Analyzer::AlteredVsSent(TVirtualPad *cx, TMultiGraph *mg_NalteredVsNsent, uint32_t fixed) const {
    auto pad = cx->cd();
    mg_NalteredVsNsent->Draw("APL");
    auto leg = pad->BuildLegend();
    TLine line25;
    Simulator::SetStylesAndDraw(&line25, "", "", kRedBlue, 6, 0);
    line25.DrawLine(0,0.25,100,0.25);
    leg->SetEntrySeparation(0);
    leg->SetTextSize(0.05);

    TLine line;
    Simulator::SetStylesAndDraw(&line, "", "", kBlue, 4);
    line.DrawLine(fixed, -0.1, fixed, 0.6);
}

void Analyzer::ProbabilityVsSent(TVirtualPad *cx, TFile *file, TMultiGraph *mg_ProbabilityVsNsent) const {
    auto pad = cx->cd();
    pad->SetLogy();
    mg_ProbabilityVsNsent->Draw("APL");
    Simulator::SetStylesAndDraw(dynamic_cast<TF1 *> (file->Get(Simulator::fProbabilityTeoPlotName)), "", "", kRed, 4, 0);
    auto leg = pad->BuildLegend();
    leg->SetEntrySeparation(0);
    leg->SetTextSize(0.05);
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


void Analyzer::PlotFunctionOfErrors(TVirtualPad *cx, TFile *file, int Nfixed) {

    auto pad = cx->cd();
    TGraphErrors* NalteredVsError[4]; //[0] P [1] L [2] P_Eve [3]L_Eve
    InitMultiGraph(NalteredVsError, "alteredError");
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


    SetStyleMultiGraph(NalteredVsError);

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


void Analyzer::PlotFunctionOfAltered(TVirtualPad *cx, TFile *file) {
    auto pad = cx->cd();

    TObject* g_tmpptr = nullptr;


    TGraphErrors* devstVsNoise[4]; //[0] P [1] L [2] P_Eve [3]L_Eve
    InitMultiGraph(devstVsNoise, "devst_noise");
    int devstVsNoiseCntr[4] = {0};


    for(const auto &config : fVettInfos) {
        Color_t color = kRed + static_cast<Color_t>(10*config.fSigma);

        g_tmpptr = dynamic_cast<TH1D *> (file->Get(TString::Format("%s_%s", Simulator::fAlteredDistrName, config.fInfos.c_str())));
        if (g_tmpptr) {
            int iGraph = config.fUseErrorCorrection?1:0;
            if(config.fEveIsPresent) iGraph+=2;
            double devstNuseful = dynamic_cast<TH1 *>(g_tmpptr)->GetStdDev();
            double errorSigmaSquare = 2 * devstNuseful * dynamic_cast<TH1 *>(g_tmpptr)->GetStdDevError();
            Simulator::SetStylesAndDraw(g_tmpptr, "", "", color);
            devstVsNoise[iGraph]->SetPoint(devstVsNoiseCntr[iGraph], config.fSigma, devstNuseful * devstNuseful);
            devstVsNoise[iGraph]->SetPointError(devstVsNoiseCntr[iGraph]++, 0, errorSigmaSquare);
        }else{
            std::cerr << std::endl << "nullptr reading fAlteredDistrName" << std::endl;
        }
    }


    SetStyleMultiGraph(devstVsNoise);

    auto mg_Devst_sigma = new TMultiGraph();
    mg_Devst_sigma->SetTitle("Useful_vs_Noise; #sigma noise; #devSt^2 useful");
    for (int i = 0; i < 4 ; i++) {
        devstVsNoise[i]->Set(devstVsNoiseCntr[i]);
        mg_Devst_sigma->Add(devstVsNoise[i]);
    }

    mg_Devst_sigma->Draw("APL");
    auto leg = pad->BuildLegend();
    leg->SetEntrySeparation(0);
    leg->SetTextSize(0.05);
}

void Analyzer::PlotNalteredDistributions(TVirtualPad *cx, TFile *file) {
    TObject* g_tmpptr = nullptr;

    auto st_Nusefuldistr = new THStack();

    for(const auto &config : fVettInfos) {
        g_tmpptr = dynamic_cast<TH1D *> (file->Get(TString::Format("%s_%s", Simulator::fAlteredDistrName, config.fInfos.c_str())));
        if (g_tmpptr) {
            dynamic_cast<TH1D*>(g_tmpptr)->SetDirectory(nullptr);
            st_Nusefuldistr->Add(dynamic_cast<TH1D *>(g_tmpptr));
        }else{
            std::cerr << std::endl << "nullptr reading fProbabilityPlotName" << std::endl;
        }
    }

    cx->cd();
    st_Nusefuldistr->SetNameTitle("Distrs of altered","Distrs of altered");
    st_Nusefuldistr->Draw("nostack");
}

void Analyzer::PlotSlopeVsNoise(TVirtualPad *cx, TFile *file) {
    auto pad = cx->cd();

    TObject* g_tmpptr = nullptr;


    TGraphErrors* slopeVsNoise[4]; //[0] P [1] L [2] P_Eve [3]L_Eve
    InitMultiGraph(slopeVsNoise, "slope_noise");
    int slopeVsNoiseCntr[4] = {0};


    for(const auto &config : fVettInfos) {
        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(TString::Format("%s_%s", Simulator::fProbabilityPlotName, config.fInfos.c_str())));
        if (g_tmpptr) {
            int iGraph = config.fUseErrorCorrection?1:0;
            if(config.fEveIsPresent) iGraph+=2;
            auto fit = new TF1("lin", "TMath::Power([0],x)", 1, config.fNQbits);
            dynamic_cast<TGraphErrors*>(g_tmpptr)->Fit(fit,"Q0M");
            slopeVsNoise[iGraph]->SetPoint(slopeVsNoiseCntr[iGraph], config.fSigma, fit->GetParameter(0));
            slopeVsNoise[iGraph]->SetPointError(slopeVsNoiseCntr[iGraph]++, 0, fit->GetParError(0));
            delete fit;
        }else{
            std::cerr << std::endl << "nullptr reading fProbabilityPlotName" << std::endl;
        }
    }


    SetStyleMultiGraph(slopeVsNoise);

    auto mg_slope_sigma = new TMultiGraph();
    mg_slope_sigma->SetTitle("slope_vs_Noise; #sigma noise; probability slope");
    for (int i = 0; i < 4 ; i++) {
        slopeVsNoise[i]->Set(slopeVsNoiseCntr[i]);
        mg_slope_sigma->Add(slopeVsNoise[i]);
    }

    mg_slope_sigma->Draw("APL");
    auto leg = pad->BuildLegend();
    leg->SetEntrySeparation(0);
    leg->SetTextSize(0.05);
}

void Analyzer::InitMultiGraph(TGraphErrors **NalteredVsError, const char *name) const {
    NalteredVsError[0] = new TGraphErrors(static_cast<int>(fVettInfos.size()));
    NalteredVsError[0] -> SetNameTitle(TString::Format("%s_Physical_qbits", name), TString::Format("%s_Physical_qbits", name));
    NalteredVsError[1] = new TGraphErrors(static_cast<int>(fVettInfos.size()));
    NalteredVsError[1] -> SetNameTitle(TString::Format("%s_Logical_qbits", name), TString::Format("%s_Logical_qbits", name));
    NalteredVsError[2] = new TGraphErrors(static_cast<int>(fVettInfos.size()));
    NalteredVsError[2] -> SetNameTitle(TString::Format("%s_Physical_qbits_Eve", name), TString::Format("%s_Physical_qbits_Eve", name));
    NalteredVsError[3] = new TGraphErrors(static_cast<int>(fVettInfos.size()));
    NalteredVsError[3] -> SetNameTitle(TString::Format("%s_Logical_qbits_Eve", name), TString::Format("%s_Logical_qbits_Eve", name));
}

void Analyzer::SetStyleMultiGraph(TGraphErrors *const *mg) const {
    Simulator::SetStylesAndDraw(mg[0], "", "", kCyan - 2, 1, 24);
    Simulator::SetStylesAndDraw(mg[1], "", "", kViolet, 1, 32);
    Simulator::SetStylesAndDraw(mg[2], "", "", kCyan - 9, 1, 24);
    Simulator::SetStylesAndDraw(mg[3], "", "", kViolet + 4, 1, 32);
}
