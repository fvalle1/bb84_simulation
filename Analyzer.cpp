//
// Created by francescobonacina on 06/12/18.
//

#include <TMultiGraph.h>
#include "Analyzer.h"
#include "Simulator.h"

double gSigma = 0;

double GaussianNoise(){
    return gRandom->Gaus(0, gSigma);
}

Analyzer::Analyzer(std::vector<ConfigSimulation> VettInfos) {
fVettInfos = std::move(VettInfos);
}

Analyzer::~Analyzer() {

}

void Analyzer::RunAnalyzer(){
    for(auto config:fVettInfos){

        std::function<double(void)> fnoise;
        if(config.fSigma<1e-5){
            fnoise = nullptr;
        }else{
            gSigma = config.fSigma;
            fnoise = std::move(GaussianNoise);
        }

        auto sim = Simulator::Instance(config);
        sim->RunSimulation()->GeneratePlots();
    }

}

void Analyzer::JoinResults(TCanvas *cx) {
    TFile* file = new TFile(Simulator::fFilename, "READ");
    cx->cd();
    cx->Clear();
    cx->SetCanvasSize(1800, 1000);
    cx->SetWindowSize(1850, 1030);
    cx->SetTitle("bb84");
    cx->SetName("bb84");

    // show use of logic qbits
    auto g_LorP = new TMultiGraph();
    g_LorP->SetTitle("Logic_vs_Physic; Nsent; Naaltered");
    TGraphErrors* g_tmp;

    for(auto config:fVettInfos) {
        g_tmp = dynamic_cast<TGraphErrors *> (file->Get(
                TString::Format("%s_%s", Simulator::fNPlotName, config.fInfos.c_str())));
        if (g_tmp) {
            if (!config.fWithEve) {
                if (config.fIsLogic) g_tmp->SetMarkerStyle(23);
                else g_tmp->SetMarkerStyle(20);
                g_tmp->SetLineColor(kBlue + 10 * config.fSigma);
                g_LorP->Add(g_tmp);
            }
        }
        else std::cerr << g_tmp << std::endl;
    }


    g_LorP->Draw("ALP");
    cx->BuildLegend();

    file->Close();
    delete file;

}
