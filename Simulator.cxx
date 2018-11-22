//
// Created by Filippo Valle on 15/11/2018.
//

#include <TGraphErrors.h>
#include "Simulator.h"

Simulator* Simulator::fgSimulator = nullptr;

Simulator::Simulator(bool useLogicQbits) :
        fNqbits(100),
        fUseLogicQbits(useLogicQbits)
{
    fChannels = new Channel *[2];

    fChannels[0] = new Channel();
    fChannels[0]->SetNoisy(true);
    fChannels[1] = new Channel();

    gStyle->SetOptStat(00000000);
    gRandom->SetSeed(42);
}

Simulator::~Simulator() {
    delete fChannels[0];
    delete fChannels[1];
    delete[] fChannels;
    printf("\nSimulation ended..\n\n");
}

Simulator * Simulator::Instance(bool useLogicQbits) {
    if(!fgSimulator) fgSimulator = new Simulator(useLogicQbits);
    return fgSimulator;
}


Simulator* Simulator::RunSimulation(){
//hists/file
    printf("\nRunning simulation..\n");
    auto phone = new Phone();
    static fStructToSave currentData;


    TFile file(fFilename, "RECREATE");
    auto tree = new TTree(fTreename, fTreename);
    tree->Branch(fBranchName, &currentData.Ntot, "Ntot/I:SameBasisIntercept:SameBasisNoIntercept");
    tree->SetAutoSave(-10000000); //10MB

    auto qbit = new Qbit(fUseLogicQbits);

    for(int simulation = 0; simulation < fSimulations; simulation++) {
        printf("\rSimulation %u/%u", simulation+1, fSimulations);
        for (int N = 1; N <= fNqbits; ++N) {
            phone->InitResults(currentData);

            for (int i = 1; i <= N; ++i) {
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
        file.Flush();
    }

    printf("\nSimulation ended..\n");
    file.Write();
    file.Close();
    delete phone;
    delete qbit;

    return this;
}

Simulator* Simulator::GeneratePlots() {
    printf("\nGenerating plots..\n");
    TFile file(fFilename, "UPDATE");
    if (file.IsZombie()) {
        std::cerr << "Error opening file" << std::endl;
        return this;
    }

    auto probVsNHist = new TH1D(fProbabilityPlotName, fProbabilityPlotName, fNqbits, 0.5, fNqbits+0.5);
    auto probVsNHist_teo = new TH1D(fProbabilityTeoPlotName, fProbabilityTeoPlotName, fNqbits, 0.5, fNqbits+0.5);

    auto NVsNPlot = new TGraphErrors(fNqbits);
    int NVsHPlotiPoint = 0;
    auto NVsNHist_distr = new TH1D(fNDistrName, fNPlotName, 10, 0, 1);

    auto Useful_distr = new TH1D(fUsefulPlotName, "Useful", 10, 0, 1);

    auto PdfperLenghtCom = new TH1D(fPdfperLenghtCom, fPdfperLenghtCom, 10, 0, 1); // da scrivere

    auto tree = dynamic_cast<TTree*>(file.Get(fTreename));
    if(tree && !tree->IsZombie()) {
        TBranch *data = tree->GetBranch(fBranchName);
        static fStructToSave currentData;
        data->SetAddress(&currentData);

        double fractionOfIntercepted = 0.;
        double distrNormFactor = 1./ fSimulations / fNqbits;

        auto entries = tree->GetEntries();
        for (int entry = 0; entry < entries; ++entry) {
            printf("\r%u/%llu", entry+1, entries);
            tree->GetEvent(entry);

            int NSamebasis = currentData.SameBasisIntercept + currentData.SameBasisNoIntercept;
            if (NSamebasis != 0)
                fractionOfIntercepted = static_cast<double>(currentData.SameBasisIntercept) / NSamebasis;
            else fractionOfIntercepted = 0.;

            NVsNHist_distr->Fill(fractionOfIntercepted, distrNormFactor);

            Useful_distr->Fill(static_cast<double>(NSamebasis)/currentData.Ntot, distrNormFactor);
            if(currentData.Ntot == 50){                                                 //togliere l'if, farlo per tutti i giri del ciclo, fare il fit, salvarsi in un vettore tutte le sigma
                PdfperLenghtCom -> Fill(fractionOfIntercepted, 10./fSimulations);
            }

        }

        for(int nqbit = 1; nqbit <= fNqbits; nqbit++){
            double teoValue=TMath::Power(0.25, nqbit);
            probVsNHist_teo->Fill(nqbit, teoValue);

            double simulatedFrac, fakeN;


            NVsNPlot->SetPoint(NVsHPlotiPoint, nqbit, 1);
            NVsNPlot->SetPointError(NVsHPlotiPoint++, 0, 0.25);

            NVsNPlot->GetPoint(nqbit, fakeN, simulatedFrac);
            probVsNHist->SetBinContent(nqbit, TMath::Power(simulatedFrac, nqbit));
            if(Qbit::DEBUG) printf("\nn:%20.15f pow:%20.15f", simulatedFrac, TMath::Power(simulatedFrac, nqbit));
        }

        NVsNPlot->Write(fNPlotName, TObject::kOverwrite | TObject::kSingleKey); //salvo solo ultima versione
//        probVsNHist->Write();
//        probVsNHist_teo->Write();
//        NVsNPlot->Write();
//        NVsNHist_distr->Write();
//        Useful_distr->Write();
        file.Write();
        file.Close();

    }else{
        std::cerr<<"Tree not found on file"<<std::endl;
    }

//    delete probVsNHist;
//    delete probVsNHist_teo;
    delete NVsNPlot;
//    delete NVsNHist_distr;
//    delete Useful_distr;

    return this;
}

Simulator* Simulator::ShowResults(TCanvas *cx) {
    printf("\nShowing results..\n");
    if(cx) {
        TFile file(fFilename, "READ");

        if (file.IsZombie()) {
            std::cerr << "Error opening file" << std::endl;
            return this;
        }

        auto probVsNHist = dynamic_cast<TH1D*>(file.Get(fProbabilityPlotName));
        probVsNHist->SetDirectory(nullptr);
        auto probVsNHist_teo = dynamic_cast<TH1D*>(file.Get(fProbabilityTeoPlotName));
        probVsNHist_teo->SetDirectory(nullptr);
        auto NVsNHist = dynamic_cast<TGraphErrors*>(file.Get(fNPlotName));
        auto NVsNHist_distr = dynamic_cast<TH1D*>(file.Get(fNDistrName));
        NVsNHist_distr->SetDirectory(nullptr);
        auto UsefulHist = dynamic_cast<TH1D*>(file.Get(fUsefulPlotName));
        UsefulHist->SetDirectory(nullptr);
        auto PdfperLenghtCom = dynamic_cast<TH1D*>(file.Get(fPdfperLenghtCom));
        PdfperLenghtCom->SetDirectory(nullptr);

        file.Close();

        cx->cd();
        cx->Clear();
        cx->SetCanvasSize(900, 600);
        cx->SetWindowSize(910, 610);
        cx->SetTitle("bb84");
        cx->SetName("bb84");
        cx->Divide(3,2);
        cx->cd(1);
        SetStylesAndDraw(probVsNHist, "Number_of_sent_qbits", "Percentage_of_wrong_qbits", kCyan - 3, 6);
        SetStylesAndDraw(probVsNHist_teo, "Number_of_sent_qbits_teo", "Percentage_of_wrong_qbits_teo", kOrange, 5);
        cx->cd(2);
        SetStylesAndDraw(NVsNHist, "Number_of_sent_qbits", "Percentage_of_intercepted_qbits", kBlue, 5);
        cx->cd(4)->SetLogy();
        SetStylesAndDraw(probVsNHist, "Number_of_sent_qbits", "Percentage_of_wrong_qbits", kCyan - 3, 6);
        SetStylesAndDraw(probVsNHist_teo, "Number_of_sent_qbits_teo", "Percentage_of_wrong_qbits_teo", kOrange, 5);
        cx->cd(5);
        SetStylesAndDraw(NVsNHist_distr, "Number_of_sent_qbits", "Percentage_of_intercepted_qbits", kBlue, 5);
        cx->cd(6);
        SetStylesAndDraw(UsefulHist, "Number_of_useful_qbits", "#", kYellow-3, 5);
        cx->cd(3);
        SetStylesAndDraw(PdfperLenghtCom, "frac_of_intercepted", "#Simulations",  kYellow-3, 5);

    }else{
        std::cerr<<"TCanvas is nullptr"<<std::endl;
    }
    return this;
}

void Simulator::SetStylesAndDraw(TObject *hist, const char *xLabel, const char *ylabel, Color_t color,
                                 Width_t linewidth) const {
    if (hist) {
        if(hist->InheritsFrom("TH1")){
            dynamic_cast<TH1*>(hist)->Draw("hist same c");
            dynamic_cast<TH1*>(hist)->GetXaxis()->SetTitle(xLabel);
            dynamic_cast<TH1*>(hist)->GetYaxis()->SetTitle(ylabel);
            dynamic_cast<TH1*>(hist)->SetLineWidth(linewidth);
            dynamic_cast<TH1*>(hist)->SetLineColor(color);
        }
        if(hist->InheritsFrom("TGraph")){
            dynamic_cast<TGraph*>(hist)->Draw("APL same");
            dynamic_cast<TGraph*>(hist)->GetXaxis()->SetTitle(xLabel);
            dynamic_cast<TGraph*>(hist)->GetYaxis()->SetTitle(ylabel);
            dynamic_cast<TGraph*>(hist)->SetLineWidth(linewidth);
            dynamic_cast<TGraph*>(hist)->SetLineColor(color);
        }
    }
}

