//
// Created by Filippo Valle on 15/11/2018.
//

#include "Simulator.h"


ClassImp(Simulator)

const char* Simulator::fFilename = "bb84_simulation.root";
const char* Simulator::fTreename = "bb84";
const char* Simulator::fBranchName = "dataBranch";
const char* Simulator::fProbabilityPlotName = "probability_vs_N";
const char* Simulator::fProbabilityTeoPlotName = "probability_vs_N_teo";


Simulator::Simulator(): fNqbits(100){
    fChannels = new Channel*[2];

    fChannels[0] = new Channel();
    //fChannels[0]->SetNoisy(TF1* pdf);
    fChannels[1] = new Channel();

    gStyle->SetOptStat(00000000);
    gRandom->SetSeed(42);
}

Simulator::~Simulator() {
    delete fChannels[0];
    delete fChannels[1];
    delete[] fChannels;
    printf("\nSimulation ended\n\n");
}

Simulator& Simulator::RunSimulation(){
//hists/file
    printf("\nRunning simulation..\n");
    auto phone = new Phone();
    static fStructToSave currentData;


    TFile file(fFilename, "RECREATE");
    auto tree = new TTree(fTreename, fTreename);
    tree->Branch(fBranchName, &currentData.Ntot, "Ntot/I:SameBasisIntercept:SameBasisNoIntercept");

    auto qbit = new Qbit(true);

    for(int simulation = 0; simulation < fSimulations; simulation++) {
        printf("\rSimulation %u/%u", simulation+1, fSimulations);
        for (uint32_t N = 1; N <= fNqbits; ++N) {
            phone->InitResults(currentData);

            for (uint32_t i = 1; i <= N; ++i) {
                Buddy::PrepareQbit(qbit);
                phone->SetNewQbit(qbit);
                fChannels[0]->PassQbit(qbit);
                Buddy::InterceptQbit(qbit);
                fChannels[1]->PassQbit(qbit);
                Buddy::ReceiveQbit(qbit);
                phone->MakeCallClassicalChannel(qbit, currentData);
            }
            tree->Fill();
        }
    }

    printf("\nSimulation ended..\n");
    file.Write();
    file.Close();
    delete phone;
    delete qbit;
//    delete tree;

    return *this;
}

Simulator & Simulator::GeneratePlots() {
    printf("\nSaving results..\n");
    TFile file(fFilename, "UPDATE");
    auto probVsNHist = new TH1D(fProbabilityPlotName, "pN", fNqbits, 0.5, fNqbits+0.5);
    auto probVsNHist_teo = new TH1D(fProbabilityTeoPlotName, "pN_teo", fNqbits, 0.5, fNqbits+0.5);

    double probVsNHist_integral = 0.;
    double probVsNHist_teo_integral = 0.;


    auto tree = dynamic_cast<TTree*>(file.Get(fTreename));
    if(tree) {
        TBranch *data = tree->GetBranch(fBranchName);
        static fStructToSave currentData;
        data->SetAddress(&currentData);
        double simValue = 1.;
        for (int entry = 0; entry < tree->GetEntries(); ++entry) {
            tree->GetEvent(entry);
            if ((currentData.SameBasisIntercept + currentData.SameBasisNoIntercept) != 0) {
                simValue *= static_cast<double>(currentData.SameBasisIntercept) / (currentData.SameBasisIntercept + currentData.SameBasisNoIntercept);
                probVsNHist->Fill(currentData.Ntot,simValue) ;
                probVsNHist_integral+=simValue;
            }else{
                probVsNHist->Fill(currentData.Ntot, 0);
            }

        }

        for(uint32_t n = 1; n<=fNqbits; n++){
            double teoValue=TMath::Power(0.25, static_cast<double>(n));
            probVsNHist_teo->Fill(n, teoValue);
            probVsNHist_teo_integral+=teoValue;
        }

        probVsNHist->Scale(1. / fSimulations);
        probVsNHist->Write();
        probVsNHist_teo->Scale(1.);
        probVsNHist_teo->Write();
        file.Close();

        // delete probVsNHist;
    }else{
        std::cerr<<"Tree not found on file"<<std::endl;
    }
//    delete tree;
//    delete probVsNHist;

    return *this;
}

Simulator & Simulator::ShowResults(TCanvas *cx) {
    printf("\nShowing results..\n");
    if(cx) {
        TFile file(fFilename, "READ");
        auto probVsNHist = dynamic_cast<TH1D*>(file.Get(fProbabilityPlotName));
        probVsNHist->SetDirectory(nullptr);
        auto probVsNHist_teo = dynamic_cast<TH1D*>(file.Get(fProbabilityTeoPlotName));
        probVsNHist_teo->SetDirectory(nullptr);

        file.Close();

        cx->cd();
        cx->Clear();
        SetStylesAndDraw(probVsNHist, "Number_of_sent_qbits", "Percentage_of_wrong_qbits", kCyan - 3, 6);
        SetStylesAndDraw(probVsNHist_teo, "Number_of_sent_qbits_teo", "Percentage_of_wrong_qbits_teo", kOrange, 5);

    }else{
        std::cerr<<"TCanvas is nullptr"<<std::endl;
    }
    return *this;
}

void Simulator::SetStylesAndDraw(TH1D *hist, const char *xLabel, const char *ylabel, Color_t color,
                                 Width_t linewidth) const {
    if (hist) {
        hist->Draw("hist same c");
        hist->GetXaxis()->SetTitle(xLabel);
        hist->GetYaxis()->SetTitle(ylabel);
        hist->SetLineWidth(linewidth);
        hist->SetLineColor(color);
    }
}

