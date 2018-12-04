//
// Created by Filippo Valle on 15/11/2018.
//

#include <TGraphErrors.h>
#include "Simulator.h"
#include <stdio.h>

Simulator* Simulator::fgSimulator = nullptr;                    // global pointer inizializzato a NULL

Simulator::Simulator(bool useLogicQbits) :                      // definisco il costruttore che verrà chiamato da Instance()
        fNqbits(100),
        fUseLogicQbits(useLogicQbits)
{
    fChannels = new Channel *[2];                                 // di default creo 2 canali. (se non ci fosse Eve ce ne basterebbe uno)

    fChannels[0] = new Channel();
    fChannels[0]->SetNoisy(false);
    fChannels[1] = new Channel();                                 // qua non faccio SetNoisy(False)?

    gStyle->SetOptStat(00000000);                                 // di default non mi stampa nessuna imformazione
    gRandom->SetSeed(42);                                         // The answer to life the universe and everything
}

Simulator::~Simulator() {
    delete fChannels[0];                                          // distruggo i due puntatori ai singoli canali e il puntatore di puntato fchannels
    delete fChannels[1];
    delete[] fChannels;
    printf("\nSimulation ended..\n\n");
}

Simulator* Simulator::Instance(bool useLogicQbits) {
    if(!fgSimulator) fgSimulator = new Simulator(useLogicQbits);   // se il puntatore globale non è ancora istanziato: lo faccio puntare ad un oggetto Simulator che creo qua
    return fgSimulator;
}


Simulator* Simulator::RunSimulation(){                           // quando lancio la simulation creo un telefono e la struttura dove salvare i dati
    //hists/file
    printf("\nRunning simulation..\n");
    auto phone = new Phone();
    static fStructToSave currentData;


    TFile file(fFilename, "RECREATE");                             // creo un file
    auto tree = new TTree(fTreename, fTreename);                   // creo un tree
    tree->Branch(fBranchName, &currentData.Ntot, "Ntot/I:SameBasisIntercept:SameBasisNoIntercept"); // &currentData.Not -->  indirizzo del primo elemento della sruttura
    // "Ntot:...." --> concatenation of all the variable names and types separated by ':'
    tree->SetAutoSave(-10000000); //10MB --> This function may be called at the start of a program to change the default value for fAutoSave

    auto qbit = new Qbit(fUseLogicQbits);                                // creo un Qbit

    for(int simulation = 0; simulation < fSimulations; simulation++) {   // fSimulations = numero di simulazioni
        printf("\rSimulation %u/%u", simulation+1, fSimulations);
        for (int N = 1; N <= fNqbits; ++N) {                               // fNqbits = numero di qubit che Alice invia in ogni simulazione
            phone->InitResults(currentData);

            for (int i = 1; i <= N; ++i) {                         // ogni qbit della comunicazione viene creato da A, trasmesso, intercettato da E,  ritrasmesso, ricevuto da B, controllato dalla telefonata
                Buddy::PrepareQbit(qbit);
                phone->SetNewQbit(qbit);
                fChannels[0]->PassQbit(qbit);
                Buddy::InterceptQbit(qbit);
                fChannels[1]->PassQbit(qbit);
                Buddy::ReceiveQbit(qbit);
                phone->MakeCallClassicalChannel(qbit, currentData);
            }
            tree->Fill();                                          // riempio il tree con le informazioni
        }
        file.Flush();                                            // Synchronize a file's in-memory and on-disk states
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
    TFile file(fFilename, "UPDATE");                           // continuo a scrivere sul file che già esiste
    if (file.IsZombie()) {
        std::cerr << "Error opening file" << std::endl;
        return this;
    }

    auto probVsNHist = new TH1D(fProbabilityPlotName, fProbabilityPlotName, fNqbits, 0.5, fNqbits + 0.5);
    auto probVsNHist_teo = new TH1D(fProbabilityTeoPlotName, fProbabilityTeoPlotName, fNqbits, 0.5, fNqbits + 0.5);


    auto tree = dynamic_cast<TTree *>(file.Get(fTreename));     // ??? cosa faccio qua?
    if (tree && !tree->IsZombie()) {
        HistNintercepted(tree);
        PlotNinterceptedVsN(tree);

    }

    return this;
}

void Simulator::PlotPdfPerLenght(TTree *, double *, double *) {

}

void Simulator::PlotNinterceptedVsN(TTree* tree) {
    printf("\nPlotting plot of N intercepted vs N\n");
    TBranch *data = tree->GetBranch(fBranchName);
    static fStructToSave currentData;
    data->SetAddress(&currentData);

    auto NVsNHist = new TH1D(fNPlotName, fNPlotName, fNqbits, 0.5, fNqbits + 0.5);
    auto Useful_distr = new TH1D(fUsefulPlotName, "Useful", 10, 0, 1);

    double fractionOfIntercepted = 0.;
    double distrNormFactor = 1. / fSimulations / fNqbits;

    auto entries = tree->GetEntries();
    for (int entry = 0; entry < entries; ++entry) {
        printf("\r%u/%llu", entry + 1, entries);
        tree->GetEvent(entry);

        int NSamebasis = currentData.SameBasisIntercept + currentData.SameBasisNoIntercept;
        if (NSamebasis != 0) fractionOfIntercepted = static_cast<double>(currentData.SameBasisIntercept) / NSamebasis;
        else fractionOfIntercepted = 0.;

        NVsNHist->Fill(currentData.Ntot, fractionOfIntercepted / fSimulations);

        Useful_distr->Fill(static_cast<double>(NSamebasis) / currentData.Ntot, distrNormFactor);
    }

    Useful_distr->Write();
    NVsNHist->Write();

    Useful_distr->SetDirectory(nullptr);
    NVsNHist->SetDirectory(nullptr);

    delete Useful_distr;
    delete NVsNHist;
}

void Simulator::HistNintercepted(TTree *tree) {
    printf("\nPlotting hist of N intercepted\n");
    TBranch *data = tree->GetBranch(fBranchName);
    static fStructToSave currentData;
    data->SetAddress(&currentData);

    double fractionOfIntercepted = 0.;
    double distrNormFactor = 1. / fSimulations / fNqbits;

    auto NVsNHist_distr = new TH1D(fNDistrName, fNDistrName, 10, 0, 1);
    auto entries = tree->GetEntries();
    for (int entry = 0; entry < entries; ++entry) {
        printf("\r%u/%llu", entry + 1, entries);
        tree->GetEvent(entry);

        int NSamebasis = currentData.SameBasisIntercept + currentData.SameBasisNoIntercept;
        if (NSamebasis != 0)
            fractionOfIntercepted = static_cast<double>(currentData.SameBasisIntercept) / NSamebasis;
        else fractionOfIntercepted = 0.;

        NVsNHist_distr->Fill(fractionOfIntercepted, distrNormFactor);
    }

    NVsNHist_distr->Write();
    NVsNHist_distr->SetDirectory(nullptr);
    delete NVsNHist_distr;
}




Simulator *Simulator::ShowResults(TCanvas *cx) {
    printf("\nShowing results..\n");
    if (cx) {
        TFile file(fFilename, "READ");

        if (file.IsZombie()) {
            std::cerr << "Error opening file" << std::endl;
            return this;
        }

        auto probVsNHist = dynamic_cast<TH1D *>(file.Get(fProbabilityPlotName));
        if (probVsNHist) probVsNHist->SetDirectory(nullptr);
        auto probVsNHist_teo = dynamic_cast<TH1D *>(file.Get(fProbabilityTeoPlotName));
        if (probVsNHist_teo) probVsNHist_teo->SetDirectory(nullptr);
        auto NVsNHist = dynamic_cast<TGraphErrors *>(file.Get(fNPlotName));
        auto NVsNHist_distr = dynamic_cast<TH1D *>(file.Get(fNDistrName));
        if (NVsNHist_distr) NVsNHist_distr->SetDirectory(nullptr);
        auto UsefulHist = dynamic_cast<TH1D *>(file.Get(fUsefulPlotName));
        if (UsefulHist) UsefulHist->SetDirectory(nullptr);


        file.Close();

        cx->cd();
        cx->Clear();
        cx->SetCanvasSize(900, 600);
        cx->SetWindowSize(910, 610);
        cx->SetTitle("bb84");
        cx->SetName("bb84");
        cx->Divide(3, 2);
        cx->cd(1);
        SetStylesAndDraw(probVsNHist, "Number_of_sent_qbits", "Percentage_of_wrong_qbits", kCyan - 3, 6);
        SetStylesAndDraw(probVsNHist_teo, "Number_of_sent_qbits_teo", "Percentage_of_wrong_qbits_teo", kOrange, 5);
        cx->cd(2);
        SetStylesAndDraw(NVsNHist, "Number_of_sent_qbits", "Percentage_of_intercepted_qbits", kBlue, 2);
        cx->cd(4)->SetLogy();
        SetStylesAndDraw(probVsNHist, "Number_of_sent_qbits", "Percentage_of_wrong_qbits", kCyan - 3, 6);
        SetStylesAndDraw(probVsNHist_teo, "Number_of_sent_qbits_teo", "Percentage_of_wrong_qbits_teo", kOrange, 5);
        cx->cd(5);
        SetStylesAndDraw(NVsNHist_distr, "Number_of_sent_qbits", "Percentage_of_intercepted_qbits", kBlue, 5);
        cx->cd(6);
        SetStylesAndDraw(UsefulHist, "Number_of_useful_qbits", "#", kYellow - 3, 5);

    } else {
        std::cerr << "TCanvas is nullptr" << std::endl;
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
            dynamic_cast<TGraph*>(hist)->Draw("APE same");
            dynamic_cast<TGraph*>(hist)->GetXaxis()->SetTitle(xLabel);
            dynamic_cast<TGraph*>(hist)->GetYaxis()->SetTitle(ylabel);
            dynamic_cast<TGraph*>(hist)->SetLineWidth(linewidth);
            dynamic_cast<TGraph*>(hist)->SetMarkerSize(0.8);
            dynamic_cast<TGraph*>(hist)->SetMarkerStyle(20);
            dynamic_cast<TGraph*>(hist)->SetLineColor(color);
            dynamic_cast<TGraph*>(hist)->SetMarkerColor(color);
        }
    }
}
