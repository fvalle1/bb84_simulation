//
// Created by Filippo Valle on 15/11/2018.
//


#include "Simulator.h"


Simulator* Simulator::fgSimulator = nullptr;                    // global pointer inizializzato a NULL

Simulator::Simulator()                      // definisco il costruttore di default
        :fConfiguration(){
    Buddy::EveIsPresent = fConfiguration.fEveIsPresent;

    fChannels = new Channel *[2];                                 // di default creo 2 canali di default. (se non ci fosse Eve ne basterebbe uno)

    fChannels[0] = new Channel();
    fChannels[1] = new Channel();
}

Simulator::Simulator(ConfigSimulation config) :                      // definisco il costruttore che verrà chiamato da Instance()
        fConfiguration(config)
{
    Buddy::EveIsPresent = fConfiguration.fEveIsPresent;
    fChannels = new Channel*[2];                                 // di default creo 2 canali. (se non ci fosse Eve ce ne basterebbe uno)
    fChannels[0] = new Channel();
    fChannels[0]->SetNoisy(fConfiguration.fPdfNoise);                     // definisco la pdf del noise
    fChannels[1] = new Channel();
}

Simulator::~Simulator() {
    delete fChannels[0];                                          // distruggo i due puntatori ai singoli canali e il puntatore di puntato fchannels
    delete fChannels[1];
    delete[] fChannels;
    printf("\nSimulation ended..\n\n");
}

Simulator* Simulator::Instance() {
    if(!fgSimulator) fgSimulator = new Simulator();   // se il puntatore globale non è ancora istanziato: lo faccio puntare ad un oggetto Simulator che creo qua
    return fgSimulator;
}

Simulator* Simulator::Instance(ConfigSimulation config) {
    if (!fgSimulator) {
        fgSimulator = new Simulator(config);   // se il puntatore globale non è ancora istanziato: lo faccio puntare ad un oggetto Simulator che creo qua
    }else {
        fgSimulator->~Simulator();              //se ho gia global pointer lo distruggo e ne creo uno con la nuova configurazione
        new(fgSimulator) Simulator(config);
    }
    return fgSimulator;
}


Simulator* Simulator::RunSimulation(){                           // quando lancio la simulatione creo un telefono e la struttura dove salvare i dati
    printf("\nRunning simulation.. \t --> \t # of simulations: %d\n", fConfiguration.fNSimulations);
    auto phone = new Phone();
    static CommunicationInfos currentData; //serve al tree per prendere i dati

    TFile file(fFilename, "UPDATE");                             // creo un file
    auto tree = new TTree(fTreename, fTreename);                   // creo un tree
    tree->Branch(fBranchName, &currentData.Ntot, "Ntot/I:SameBasisAltered:SameBasisUntouched"); // &currentData.Not -->  indirizzo del primo elemento della sruttura
    // "Ntot:...." --> concatenation of all the variable names and types separated by ':'
    tree->SetAutoSave(-10000000); //10MB --> This function may be called at the start of a program to change the default value for fAutoSave

    auto qbit = new Qbit(fConfiguration.fUseErrorCorrection);                                // creo un Qbit che uso di volta in volta

    for(uint32_t experiment = 0; experiment < fConfiguration.fNSimulations ; experiment++) {   // fNSimulations = numero di simulazioni
        printf("\rExperiment %u/%u", experiment+1, fConfiguration.fNSimulations );
        for (uint32_t N = 1; N <= fConfiguration.fNQbits; ++N) {                               // fNqbits = numero di qubit che Alice invia in ogni simulazione
            phone->InitResults(currentData);
            for (uint32_t iqbit = 1; iqbit <= N; ++iqbit) {                         // ogni qbit della comunicazione viene creato da A, trasmesso, intercettato da E,  ritrasmesso, ricevuto da B, controllato dalla telefonata
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
        file.Flush();                                            // Salvo i dati della simulazione su file
    }

    printf("\nSimulation ended..\n");
    file.Write();
    file.Close();
    delete phone;
    delete qbit;

    return this;
}

Simulator* Simulator::GeneratePlots() { //Genero i grafici che i servono
    printf("\nGenerating plots..\n");
    TFile file(fFilename, "UPDATE");                           // continuo a scrivere sul file che già esiste
    if (file.IsZombie()) {
        std::cerr << "Error opening file" << std::endl;
        return this;
    }
    auto tree = dynamic_cast<TTree *>(file.Get(fTreename));     // leggo il tree dal file
    if (tree && !tree->IsZombie()) { //se ho letto correttamente il tree chiamo i metodi per fare le varie analisi
        PlotPdfAtFixedNSent(tree);     //fa istogramma a fissato numero di qbit. Utile per stima errori
        PlotNSameBasisVsNSent(tree); //fa grafico di stessa base vs inviati (tende a 1/4) e probability (1/4)^n
        HistNintercepted(tree);     //
    }

    return this;
}

void Simulator::PlotPdfAtFixedNSent(TTree *tree) {
    printf("\nExtimating means and sigmas of N altered distributions\n");
    TBranch *data = tree->GetBranch(fBranchName);
    static CommunicationInfos currentData;
    data->SetAddress(&currentData);

    auto NAlteredVsNSent = new TGraphErrors(fConfiguration.fNQbits);  //number of photons intercepted vs. number of photons measured in the same base
    auto probabilityVsN = new TGraphErrors(fConfiguration.fNQbits);
    auto probabilityVsN_teo = new TF1(fProbabilityTeoPlotName, [](double*x, double*){return TMath::Power(0.25, x[0]);}, 1, fConfiguration.fNQbits, 0);


    // create histograms
    auto PdfperLenghtCom = new TH1D *[fConfiguration.fNQbits];
    char title[50];
    for (uint32_t i = 0; i < fConfiguration.fNQbits; i++) {
        sprintf(title, "PdfperLenghtCom_%d", i);
        PdfperLenghtCom[i] = new TH1D(title, title, 11, -0.05, 1.05);
    }

    // Fill histograms
    Long64_t currentN = 0;
    double fractionOfAltered = 0.;
    auto entries = tree->GetEntries();
    for (Long64_t entry = 0; entry < entries; ++entry) {
        printf("\r%llu/%llu", entry + 1, entries);
        tree->GetEvent(entry);

        int NSamebasis = currentData.SameBasisAltered + currentData.SameBasisUntouched;
        if (NSamebasis != 0)  fractionOfAltered = static_cast<double>(currentData.SameBasisAltered) / NSamebasis;
        else fractionOfAltered = 0.;
        currentN = entry % fConfiguration.fNQbits;
        PdfperLenghtCom[currentN]->Fill(fractionOfAltered, 1. / fConfiguration.fNSimulations);
    }

    for (uint32_t iqbit = 0; iqbit < fConfiguration.fNQbits; iqbit++) {
        double currentMean, currentSigma;

        currentMean = PdfperLenghtCom[iqbit]->GetMean();
        currentSigma = PdfperLenghtCom[iqbit]->GetStdDev();

        NAlteredVsNSent->SetPoint(iqbit, iqbit+1, currentMean);
        NAlteredVsNSent->SetPointError(iqbit, 0, currentSigma);

        int N = iqbit+1;
        double p = TMath::Power(currentMean, N); //mean ^N
        probabilityVsN->SetPoint(iqbit, N, p);
        double error = N*p/currentMean*currentSigma; //N * x^(N-1) * sigmaX (aka N * x^N / x * sigmaX)
        probabilityVsN->SetPointError(iqbit, 0., error);
    }

    // delete temporary histograms
    for(uint32_t i=0; i<fConfiguration.fNQbits; i++){
        delete PdfperLenghtCom[i];
//        PdfperLenghtCom[i]->Write();
    }
    delete[] PdfperLenghtCom;

    NAlteredVsNSent->SetNameTitle(TString::Format("%s_%s", fNPlotName, fConfiguration.fInfos.c_str()), TString::Format("%s_%s", fNPlotName, fConfiguration.fInfos.c_str()));
    NAlteredVsNSent->Write(TString::Format("%s_%s",fNPlotName, fConfiguration.fInfos.c_str()), TObject::kSingleKey | TObject::kOverwrite);
    delete NAlteredVsNSent;

    probabilityVsN->SetNameTitle(TString::Format("%s_%s", fProbabilityPlotName, fConfiguration.fInfos.c_str()), TString::Format("%s_%s", fProbabilityPlotName, fConfiguration.fInfos.c_str()));
    probabilityVsN->Write(TString::Format("%s_%s", fProbabilityPlotName, fConfiguration.fInfos.c_str()), TObject::kSingleKey | TObject::kOverwrite);
    delete probabilityVsN;

    probabilityVsN_teo->SetNameTitle(fProbabilityTeoPlotName, fProbabilityTeoPlotName); //Note that teoric curve is not config dependent
    probabilityVsN_teo->Write(fProbabilityTeoPlotName, TObject::kSingleKey | TObject::kOverwrite);
    delete probabilityVsN_teo;
}


void Simulator::PlotNSameBasisVsNSent(TTree *tree) {
    printf("\nPlotting N with same basis vs total N sent\n");
    TBranch *data = tree->GetBranch(fBranchName);
    static CommunicationInfos currentData;
    data->SetAddress(&currentData);

    auto NSameBasisVsNsent = new TH1D(TString::Format("%s_%s", fUsefulHistName, fConfiguration.fInfos.c_str()), TString::Format("%s_%s", fUsefulHistName, fConfiguration.fInfos.c_str()), fConfiguration.fNQbits, 0.5, fConfiguration.fNQbits+0.5);  //number of photons intercepted vs. number of photons measured in the same base
    auto Useful_distr = new TH1D(TString::Format("%s_%s", fUsefulPlotName, fConfiguration.fInfos.c_str()), TString::Format("%s_%s", fUsefulPlotName, fConfiguration.fInfos.c_str()), 11, -0.05, 1.05);  //number of photons intercepted vs. number of all photons sent

    double distrNormFactor = 1. / fConfiguration.fNSimulations / fConfiguration.fNQbits;

    auto entries = tree->GetEntries();
    for (Long64_t entry = 0; entry < entries; ++entry) {
        printf("\r%llu/%llu", entry + 1, entries);
        tree->GetEvent(entry);

        int NSamebasis = currentData.SameBasisAltered + currentData.SameBasisUntouched;
        NSameBasisVsNsent->Fill(currentData.Ntot, static_cast<double>(NSamebasis) /(fConfiguration.fNSimulations * currentData.Ntot));

        if (currentData.Ntot == fConfiguration.fNQbits / 2) Useful_distr->Fill(static_cast<double>(NSamebasis) / currentData.Ntot, distrNormFactor);
    }

    NSameBasisVsNsent->Write(TString::Format("%s_%s", fUsefulHistName, fConfiguration.fInfos.c_str()), TObject::kSingleKey | TObject::kOverwrite);
    NSameBasisVsNsent->SetDirectory(nullptr);
    Useful_distr->Write(TString::Format("%s_%s", fUsefulPlotName, fConfiguration.fInfos.c_str()), TObject::kSingleKey | TObject::kOverwrite);
    Useful_distr->SetDirectory(nullptr);

    delete Useful_distr;
    delete NSameBasisVsNsent;
}

void Simulator::HistNintercepted(TTree *tree) {
    printf("\nPlotting hist of N intercepted\n");
    TBranch *data = tree->GetBranch(fBranchName);
    static CommunicationInfos currentData;
    data->SetAddress(&currentData);

    double fractionOfAltered = 0.;
    double distrNormFactor = 1. / fConfiguration.fNSimulations / fConfiguration.fNQbits;

    auto NVsNHist_distr = new TH1D(fAlteredDistrName, fAlteredDistrName, 10, 0, 1);
    auto entries = tree->GetEntries();
    for (Long64_t entry = 0; entry < entries; ++entry) {
        printf("\r%llu/%llu", entry + 1, entries);
        tree->GetEvent(entry);

        if (currentData.Ntot == fConfiguration.fNQbits / 2) {
            int NSamebasis = currentData.SameBasisAltered + currentData.SameBasisUntouched;
            if (NSamebasis != 0)
                fractionOfAltered = static_cast<double>(currentData.SameBasisAltered) / NSamebasis;
            else fractionOfAltered = 0.;

            NVsNHist_distr->Fill(fractionOfAltered, distrNormFactor);
        }
    }

    NVsNHist_distr->Write(TString::Format("%s_%s", fAlteredDistrName, fConfiguration.fInfos.c_str()), TObject::kSingleKey| TObject::kOverwrite);
    NVsNHist_distr->SetDirectory(nullptr);
    delete NVsNHist_distr;
}

Simulator* Simulator::ShowResults(TCanvas *cx) {
    printf("\nShowing results..\n");
    if (cx) {
        TFile file(fFilename, "READ");

        if (file.IsZombie()) {
            std::cerr << "Error opening file" << std::endl;
            return this;
        }

        auto probVsNHist = dynamic_cast<TGraphErrors*>(file.Get(TString::Format("%s_%s", fProbabilityPlotName, fConfiguration.fInfos.c_str())));
        auto probVsNHist_teo = dynamic_cast<TF1*>(file.Get(fProbabilityTeoPlotName));
        auto NVsNHist = dynamic_cast<TGraphErrors*>(file.Get(TString::Format("%s_%s", fNPlotName, fConfiguration.fInfos.c_str())));
        auto NVsNHist_distr = dynamic_cast<TH1D*>(file.Get(TString::Format("%s_%s", fAlteredDistrName, fConfiguration.fInfos.c_str())));
        if (NVsNHist_distr) NVsNHist_distr->SetDirectory(nullptr);
        auto UsefulHist = dynamic_cast<TH1D *>(file.Get(TString::Format("%s_%s", fUsefulPlotName, fConfiguration.fInfos.c_str())));
        if (UsefulHist) UsefulHist->SetDirectory(nullptr);
        auto NSameBasisVsNqbit = dynamic_cast<TH1D*>(file.Get(TString::Format("%s_%s", fUsefulHistName, fConfiguration.fInfos.c_str())));
        if (NSameBasisVsNqbit) NSameBasisVsNqbit->SetDirectory(nullptr);

        file.Close();

        cx->cd();
        cx->Clear();
        cx->SetCanvasSize(1800, 1000);
        cx->SetWindowSize(1850, 1030);
        cx->SetTitle("bb84");
        cx->SetName("bb84");
        cx->Divide(3, 2);
        cx->cd(1);
        SetStylesAndDraw(probVsNHist, "Number_of_sent_qbits", "Percentage_of_wrong_qbits", kCyan - 3, 2);
        SetStylesAndDraw(probVsNHist_teo, "Number_of_sent_qbits_teo", "Percentage_of_wrong_qbits_teo", kOrange, 2);
        if(probVsNHist) probVsNHist->GetYaxis()->SetRangeUser(0,0.25);

        cx->cd(2);
        SetStylesAndDraw(NVsNHist, "Number_of_sent_qbits", "Percentage_of_intercepted_qbits", kBlue, 2);
        TLine line2;
        Simulator::SetStylesAndDraw(&line2, "", "", kRed, 4);
        line2.DrawLine(fConfiguration.fNQbits/2., -0.1, fConfiguration.fNQbits/2., 0.6);

        cx->cd(3);
        SetStylesAndDraw(NSameBasisVsNqbit, "Number_of_sent_qbits", "Percentage_of_qbits_with_same_base", kBlue, 2);
        TLine line3;
        Simulator::SetStylesAndDraw(&line3, "", "", kRedBlue, 4);
        line3.DrawLine(fConfiguration.fNQbits/2., 0.495, fConfiguration.fNQbits/2, 0.505);
        TLine line05;
        Simulator::SetStylesAndDraw(&line05, "", "", kRed, 4);
        line05.DrawLine(0, 0.5, fConfiguration.fNQbits,0.5);

        cx->cd(4)->SetLogy();
        SetStylesAndDraw(probVsNHist, "Number_of_sent_qbits", "Percentage_of_wrong_qbits", kCyan - 3, 2);
        SetStylesAndDraw(probVsNHist_teo, "Number_of_sent_qbits_teo", "Percentage_of_wrong_qbits_teo", kOrange, 2);

        cx->cd(5);
        SetStylesAndDraw(NVsNHist_distr, "Number_of_sent_qbits", "Percentage_of_intercepted_qbits", kBlue, 2);
        cx->cd(6);
        SetStylesAndDraw(UsefulHist, "Number_of_useful_qbits", "#", kYellow - 3, 2, 0);
    } else {
        std::cerr << "TCanvas is nullptr" << std::endl;
    }
    return this;
}



void Simulator::SetStylesAndDraw(TObject *obj, const char *xLabel, const char *ylabel, Color_t color, Width_t linewidth,
                                 Style_t markerStyle) {
    if (obj) {
        std::cout<<std::endl<<"Plotting "<<obj->GetName()<<std::endl;
        if(obj->InheritsFrom(TH1::Class())){
            dynamic_cast<TH1*>(obj)->SetLineWidth(linewidth);
            dynamic_cast<TH1*>(obj)->SetLineColor(color);
            dynamic_cast<TH1*>(obj)->Draw("hist same c");
            dynamic_cast<TH1*>(obj)->GetXaxis()->SetTitle(xLabel);
            dynamic_cast<TH1*>(obj)->GetYaxis()->SetTitle(ylabel);
        }
        if(obj->InheritsFrom(TF1::Class())){
            dynamic_cast<TF1*>(obj)->SetLineWidth(linewidth);
            dynamic_cast<TF1*>(obj)->SetLineColor(color);
            dynamic_cast<TF1*>(obj)->Draw("same");
            dynamic_cast<TF1*>(obj)->GetXaxis()->SetTitle(xLabel);
            dynamic_cast<TF1*>(obj)->GetYaxis()->SetTitle(ylabel);
        }
        if(obj->InheritsFrom(TGraph::Class())){
            dynamic_cast<TGraph*>(obj)->SetMarkerSize(1.2);
            dynamic_cast<TGraph*>(obj)->SetMarkerStyle(markerStyle);
            dynamic_cast<TGraph*>(obj)->SetMarkerColor(color);
            dynamic_cast<TGraph*>(obj)->SetLineWidth(linewidth);
            dynamic_cast<TGraph*>(obj)->SetLineColor(color);
            dynamic_cast<TGraph*>(obj)->Draw("APE same c");
            dynamic_cast<TGraph*>(obj)->GetXaxis()->SetTitle(xLabel);
            dynamic_cast<TGraph*>(obj)->GetYaxis()->SetTitle(ylabel);
        }
        if(obj->InheritsFrom(TLine::Class())){
            dynamic_cast<TLine*>(obj)->SetLineWidth(linewidth);
            dynamic_cast<TLine*>(obj)->SetLineColor(color);
        }
    }
}

