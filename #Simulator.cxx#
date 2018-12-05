//
// Created by Filippo Valle on 15/11/2018.
//


#include "Simulator.h"


Simulator* Simulator::fgSimulator = nullptr;                    // global pointer inizializzato a NULL

Simulator::Simulator(bool useLogicQbits) :                      // definisco il costruttore che verrà chiamato da Instance()
        fNqbits(100),
        fUseLogicQbits(useLogicQbits)
{
    fChannels = new Channel *[2];                                 // di default creo 2 canali. (se non ci fosse Eve ce ne basterebbe uno)

    fChannels[0] = new Channel();
    fChannels[0]->SetNoisy(false);
    fChannels[1] = new Channel();                                 // qua non faccio SetNoisy(False)?

//    gStyle->SetOptStat(00000000);                                 // di default non mi stampa nessuna imformazione
    //gRandom->SetSeed(42);                                         // The answer to life the universe and everything
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
//                Buddy::InterceptQbit(qbit);
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

    auto tree = dynamic_cast<TTree *>(file.Get(fTreename));     // ??? cosa faccio qua?
    if (tree && !tree->IsZombie()) {
        PlotPdfPerLenght(tree);
        PlotNinterceptedVsN(tree);
        HistNintercepted(tree);
    }

    return this;
}

void Simulator::PlotPdfPerLenght(TTree *tree) {
    printf("\nExtimating means and sigmas of N intercepted distributions\n");
    TBranch *data = tree->GetBranch(fBranchName);
    static fStructToSave currentData;
    data->SetAddress(&currentData);

    auto NInteceptedVsNqbitHist = new TGraphErrors(fNqbits);
    auto probVsNHist = new TGraphErrors(fNqbits);
    auto probVsNHist_teo = new TF1(fProbabilityTeoPlotName, [](double*x, double*){return TMath::Power(0.25, x[0]);}, 1, fNqbits, 0);


    // create histograms
    auto PdfperLenghtCom = new TH1D *[fNqbits]; //(fPdfperLenghtCom, fPdfperLenghtCom, 30, 0, 1);
    char title[50];
    for (int i = 0; i < fNqbits; i++) {
        sprintf(title, "PdfperLenghtCom_%d", i);
        PdfperLenghtCom[i] = new TH1D(title, title, 11, -0.05, 1.05);
    }

    // Fill histograms
    auto j=0;
    double fractionOfIntercepted = 0.;
    auto entries = tree->GetEntries();
    for (int entry = 0; entry < entries; ++entry) {
        printf("\r%u/%llu", entry + 1, entries);                 // '\r' mi permette di sovrascrivere la linea stampata ogni volta
        tree->GetEvent(entry);

        int NSamebasis = currentData.SameBasisIntercept + currentData.SameBasisNoIntercept;
        if (NSamebasis != 0)  fractionOfIntercepted = static_cast<double>(currentData.SameBasisIntercept) / NSamebasis;
        else fractionOfIntercepted = 0.;
        j = entry % fNqbits;
        PdfperLenghtCom[j]->Fill(fractionOfIntercepted, 1. / fSimulations);
    }

    // create fit, then create arrays of means and sigmas
    TF1 *fit_gaus = new TF1("fit_gaus", "gaus", 0., 1.);     //TF1 *fa = new TF1("fa","[0]*x*sin([1]*x)",-3,3);

    double p_value = 0;
    for (int i = 0; i < fNqbits; i++) {
        if(Qbit::DEBUG) PdfperLenghtCom[i]->Fit("fit_gaus", "Q0");
        else PdfperLenghtCom[i]->Fit("fit_gaus", "Q");

        double currentMean = 0.;
        double currentSigma = 0.;

        if(TMath::Abs(fit_gaus->GetChisquare()/fit_gaus->GetNDF()-1)<2){
            currentMean = fit_gaus->GetParameter("Mean");
            currentSigma = fit_gaus->GetParameter("Sigma");
        } else{
            currentMean = PdfperLenghtCom[i]->GetMean();
            currentSigma = PdfperLenghtCom[i]->GetStdDev();
        }
        // if (i == 10) {
        std::cout << "mean: " << fit_gaus->GetParameter("Mean") << "\t sigma: "
                  << fit_gaus->GetParameter("Sigma") << std::endl;
        //}
        std::cout << "grafico PdfperLenghtCom " << i << "\t mean: " << currentMean  << std::endl;
        std::cout<<"chi: "<<TMath::Abs(fit_gaus->GetChisquare()/fit_gaus->GetNDF()-1)<<std::endl;
        //        double p_value = fit_gaus->GetProb();
//        if(i<=5||i>=95) std::cout <<fit_gaus->GetProb()<<std::endl;

        NInteceptedVsNqbitHist->SetPoint(i, i+1, currentMean);
        NInteceptedVsNqbitHist->SetPointError(i, 0, currentSigma);


        int N = i+1;
        double p = TMath::Power(currentMean, N); //mean ^N
        probVsNHist->SetPoint(i, N, p);
        double error = N*p/currentMean*currentSigma; //N * x^(N-1) * sigmaX (aka N * x^N / x * sigmaX)
        probVsNHist->SetPointError(i, 0, error);
    }

    // delete histograms
    for(int i=0; i<fNqbits; i++){
        //delete PdfperLenghtCom[i];
        PdfperLenghtCom[i]->Write();
    }
//    delete[] PdfperLenghtCom;
    delete fit_gaus;

    NInteceptedVsNqbitHist->SetTitle(fNPlotName);
    NInteceptedVsNqbitHist->Write(fNPlotName, TObject::kSingleKey | TObject::kOverwrite);
    probVsNHist->SetTitle(fProbabilityTeoPlotName);
    probVsNHist->Write(fProbabilityPlotName, TObject::kSingleKey | TObject::kOverwrite);
    probVsNHist_teo->SetTitle(fProbabilityTeoPlotName);
    probVsNHist_teo->Write(fProbabilityTeoPlotName, TObject::kSingleKey | TObject::kOverwrite);
    delete NInteceptedVsNqbitHist;
    delete probVsNHist;
    delete probVsNHist_teo;
}


void Simulator::PlotNinterceptedVsN(TTree *tree) {
    printf("\nPlotting plot of N intercepted vs N\n");
    TBranch *data = tree->GetBranch(fBranchName);
    static fStructToSave currentData;
    data->SetAddress(&currentData);

    auto Useful_distr = new TH1D(fUsefulPlotName, "Useful", 10, 0, 1);

    double fractionOfIntercepted = 0.;
    double distrNormFactor = 1. / fSimulations / fNqbits;
//
//    auto entries = tree->GetEntries();
//    for (int entry = 0; entry < entries; ++entry) {
//        printf("\r%u/%llu", entry + 1, entries);
//        tree->GetEvent(entry);
//
//        int NSamebasis = currentData.SameBasisIntercept + currentData.SameBasisNoIntercept;
//        if (NSamebasis != 0) fractionOfIntercepted = static_cast<double>(currentData.SameBasisIntercept) / NSamebasis;
//        else fractionOfIntercepted = 0.;
//
//        NVsNHist->Fill(currentData.Ntot, fractionOfIntercepted / fSimulations);
//
//        Useful_distr->Fill(static_cast<double>(NSamebasis) / currentData.Ntot, distrNormFactor);
//    }

    Useful_distr->Write();

    Useful_distr->SetDirectory(nullptr);

    delete Useful_distr;
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

Simulator* Simulator::ShowResults(TCanvas *cx) {
    printf("\nShowing results..\n");
    if (cx) {
        TFile file(fFilename, "READ");

        if (file.IsZombie()) {
            std::cerr << "Error opening file" << std::endl;
            return this;
        }

        auto probVsNHist = dynamic_cast<TGraphErrors*>(file.Get(fProbabilityPlotName));
        auto probVsNHist_teo = dynamic_cast<TF1*>(file.Get(fProbabilityTeoPlotName));
        auto NVsNHist = dynamic_cast<TGraphErrors*>(file.Get(fNPlotName));
        auto NVsNHist_distr = dynamic_cast<TH1D*>(file.Get(fNDistrName));
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
        SetStylesAndDraw(probVsNHist, "Number_of_sent_qbits", "Percentage_of_wrong_qbits", kCyan - 3, 2);
        SetStylesAndDraw(probVsNHist_teo, "Number_of_sent_qbits_teo", "Percentage_of_wrong_qbits_teo", kOrange, 2);
        probVsNHist->GetYaxis()->SetRangeUser(0,0.25);

        cx->cd(2);
        SetStylesAndDraw(NVsNHist, "Number_of_sent_qbits", "Percentage_of_intercepted_qbits", kBlue, 2);
        TLine line;
        line.SetLineWidth(3);
        line.SetLineColor(kRedBlue);
        line.DrawLine(0,0.25,fNqbits,0.25);

        cx->cd(4)->SetLogy();
        SetStylesAndDraw(probVsNHist, "Number_of_sent_qbits", "Percentage_of_wrong_qbits", kCyan - 3, 2);
        SetStylesAndDraw(probVsNHist_teo, "Number_of_sent_qbits_teo", "Percentage_of_wrong_qbits_teo", kOrange, 2);
        probVsNHist->GetYaxis()->SetRangeUser(0,0.25);

        cx->cd(5);
        SetStylesAndDraw(NVsNHist_distr, "Number_of_sent_qbits", "Percentage_of_intercepted_qbits", kBlue, 2);
        cx->cd(6);
        SetStylesAndDraw(UsefulHist, "Number_of_useful_qbits", "#", kYellow - 3, 2);

    } else {
        std::cerr << "TCanvas is nullptr" << std::endl;
    }
    return this;
}



void Simulator::SetStylesAndDraw(TObject *hist, const char *xLabel, const char *ylabel, Color_t color,
                                 Width_t linewidth) const {
    if (hist) {
        if(hist->InheritsFrom("TH1")){
            dynamic_cast<TH1*>(hist)->SetLineWidth(linewidth);
            dynamic_cast<TH1*>(hist)->SetLineColor(color);
            dynamic_cast<TH1*>(hist)->Draw("hist same c");
            dynamic_cast<TH1*>(hist)->GetXaxis()->SetTitle(xLabel);
            dynamic_cast<TH1*>(hist)->GetYaxis()->SetTitle(ylabel);
        }
        if(hist->InheritsFrom("TF1")){
            dynamic_cast<TF1*>(hist)->SetLineWidth(linewidth);
            dynamic_cast<TF1*>(hist)->SetLineColor(color);
            dynamic_cast<TF1*>(hist)->GetXaxis()->SetTitle(xLabel);
            dynamic_cast<TF1*>(hist)->GetYaxis()->SetTitle(ylabel);
            dynamic_cast<TF1*>(hist)->Draw("same c");
        }
        if(hist->InheritsFrom("TGraph")){
            dynamic_cast<TGraph*>(hist)->SetLineWidth(linewidth);
            dynamic_cast<TGraph*>(hist)->SetMarkerSize(0.2);
            dynamic_cast<TGraph*>(hist)->SetMarkerStyle(20);
            dynamic_cast<TGraph*>(hist)->SetLineColor(color);
            dynamic_cast<TGraph*>(hist)->SetMarkerColor(color);
            dynamic_cast<TGraph*>(hist)->GetXaxis()->SetTitle(xLabel);
            dynamic_cast<TGraph*>(hist)->GetYaxis()->SetTitle(ylabel);
            dynamic_cast<TGraph*>(hist)->Draw("APE same c");
        }
    }
}


