//
// Created by Filippo Valle on 15/11/2018.
//


#include "Simulator.h"

/// global pointer
Simulator* Simulator::fgSimulator = nullptr;          // global pointer inizializzato a NULL


/// Instance of Simulator, calling the constructor.
Simulator* Simulator::Instance() {
    if(!fgSimulator) fgSimulator = new Simulator();   // se il puntatore globale non è ancora istanziato: lo faccio puntare ad un oggetto Simulator che creo qua
    return fgSimulator;
}


/// Instance of Simulator, passing an object ConfigSimulation
Simulator* Simulator::Instance(ConfigSimulation config) {
    if (!fgSimulator) {
        fgSimulator = new Simulator(config);   // se il puntatore globale non è ancora istanziato: lo faccio puntare ad un oggetto Simulator che creo qui
    }else {
        fgSimulator->~Simulator();              //se ho gia global pointer lo distruggo e ne creo uno con la nuova configurazione
        new(fgSimulator) Simulator(config);
    }
    return fgSimulator;
}


/// Constructor
Simulator::Simulator()
        :fConfiguration()
{
    Buddy::EveIsPresent = fConfiguration.fEveIsPresent;

    fChannels = new Channel *[2];   // creo 2 canali di default.

    fChannels[0] = new Channel();
    fChannels[1] = new Channel();
}


/// Constructor which takes an object ConfigSimulation
Simulator::Simulator(ConfigSimulation config) :                  // definisco il costruttore che verrà chiamato da Instance()
        fConfiguration(config)
{
    Buddy::EveIsPresent = fConfiguration.fEveIsPresent;
    fChannels = new Channel*[2];                                 // di default creo 2 canali. (se non ci fosse Eve ce ne basterebbe uno)
    fChannels[0] = new Channel();
    fChannels[0]->SetNoisy(fConfiguration.fPdfNoise);            // definisco la pdf del noise
    fChannels[1] = new Channel();
}


/// Destructor
Simulator::~Simulator() {
    delete fChannels[0];             // distruggo i due puntatori ai singoli canali e il puntatore di puntatori fchannels
    delete fChannels[1];
    delete[] fChannels;
    printf("\nSimulation ended..\n\n");
}


/// To simulate the transmission:
/// create a Phone to compare qbits of Alice and Bob, create a Qbit. For each length of communication and for each experiment
/// A. prepares the qbit which will be transmitted, intercepted by E. and then measured by B. Phone stores informations and
/// compares qbits of Alice and of Bob. Data are saved on a Tree.
Simulator* Simulator::RunSimulation(){                           // quando lancio la simulatione creo un telefono e la struttura dove salvare i dati
    printf("\nRunning simulation.. \t --> \t # of simulations: %d\n", fConfiguration.fNSimulations);
    auto phone = new Phone();
    static CommunicationInfos currentData;                       //serve al tree per prendere i dati

    TFile file(fFilename, "UPDATE");                             // creo un file
    auto tree = new TTree(fTreeName, fTreeName);                 // creo un tree
    tree->Branch(fBranchName, &currentData.Ntot, "Ntot/I:SameBasisAltered:SameBasisUntouched"); // &currentData.Not -->  indirizzo del primo elemento della sruttura
                                                                                                // "Ntot:...." --> concatenation of all the variable names and types separated by ':'
    tree->SetAutoSave(-10000000); //10MB --> flush every 10MB

    auto qbit = new Qbit(fConfiguration.fUseErrorCorrection);                                   // creo un Qbit che uso di volta in volta

    for(uint32_t experiment = 0; experiment < fConfiguration.fNSimulations ; experiment++) {    // fNSimulations = numero di simulazioni
        printf("\rExperiment %u/%u", experiment+1, fConfiguration.fNSimulations );
        for (uint32_t N = 1; N <= fConfiguration.fNQbits; ++N) {                                // fNqbits = numero di qubit che Alice invia in questa simulazione
            phone->InitResults(currentData);
            for (uint32_t iqbit = 1; iqbit <= N; ++iqbit) {
                Buddy::PrepareQbit(qbit);                           // Alice prepares the Qbit
                phone->SetNewQbit(qbit);                            // Phone stores informations about the qbit
                fChannels[0]->PassQbit(qbit);                       // qbit crosses channel (A. to E.)
                Buddy::InterceptQbit(qbit);                         // Eve measures and reprepares the qbit
                fChannels[1]->PassQbit(qbit);                       // qbit crosses channel (E. to B.)
                Buddy::ReceiveQbit(qbit);                           // Bob measures the qbit
                phone->MakeCallClassicalChannel(qbit, currentData); // Phone compares qbit of Alice and Bob
            }
            tree->Fill();                                           // riempio il tree con le informazioni
        }
        file.Flush();                                               // Salvo i dati della simulazione su file
    }

    printf("\nSimulation ended..\n");
    file.Write();
    file.Close();
    delete phone;
    delete qbit;

    return this;
}


/// To generate 3 groups of plots reading data from the Tree
Simulator* Simulator::GeneratePlots() {                        // Genero i grafici che ci servono
    printf("\nGenerating plots..\n");
    TFile file(fFilename, "UPDATE");                           // continuo a scrivere sul file che già esiste
    if (file.IsZombie()) {                                     // NB i grafici possono essere generati anche in un secondo momento
        std::cerr << "Error opening file" << std::endl;
        return this;
    }
    auto tree = dynamic_cast<TTree *>(file.Get(fTreeName));    // leggo il tree dal file
    if (tree && !tree->IsZombie()) {   // se ho letto correttamente il tree chiamo i metodi per fare le varie analisi
        PlotPdfAtFixedNSent(tree);     // fa istogramma a fissato numero di qbit. Utile per stima errori
        PlotNSameBasisVsNSent(tree);   // fa grafico di N stessa base vs inviati (tende a 1/4) e probability (1/4)^n
        HistNaltered(tree);            // istogramma di N altered
    }

    return this;
}


/// To create: 1) NAlteredVsNSent = TGraphErrors: NAltered/Nsent vs length of communication
///            2) probabilityVsN =  TGraphErrors: (Naltered/Nsent)^length of communication vs length of communication --> (~ 0.25^N)
///            3) probabilityVsN_teo = TF1: 1 - 0.75^x
///            4) fractionAlteredToN = (fraction of qbits intercepted)^N sent vs N sent
///            5) fractionAlteredToN_teo = TF1: 0.25^x
/// and save them on the file
void Simulator::PlotPdfAtFixedNSent(TTree *tree) {
    printf("\nExtimating means and sigmas of N altered distributions\n");
    TBranch *data = tree->GetBranch(fBranchName);
    static CommunicationInfos currentData;
    data->SetAddress(&currentData);

    auto NAlteredVsNSent = new TGraphErrors(fConfiguration.fNQbits);  // number of photons intercepted vs. number of photons measured in the same base
    auto probabilityVsN = new TGraphErrors(fConfiguration.fNQbits);   // probability to observe almost one qbit altered in N sent
    auto probabilityVsN_teo = new TF1(fProbabilityTeoPlotName, [](double*x, double*){return 1-TMath::Power(0.75, x[0]);}, 1, fConfiguration.fNQbits, 0);
    auto fractionAlteredToN = new TGraphErrors(fConfiguration.fNQbits);   // log( fraction of intercepted power N sent)
    auto fractionAlteredToN_teo = new TF1(fAlteredToNTeoPlotName, [](double*x, double*){return TMath::Power(0.25, x[0]);}, 1, fConfiguration.fNQbits, 0);

    // *** create histograms
    auto PdfperLenghtCom = new TH1D *[fConfiguration.fNQbits];
    char title[50];
    for (uint32_t i = 0; i < fConfiguration.fNQbits; i++) {
        sprintf(title, "PdfperLenghtCom_%d", i);
        PdfperLenghtCom[i] = new TH1D(title, title, 11, -0.05, 1.05);
    }

    // *** Fill histograms
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

    // extimate means and devs
    for (uint32_t iqbit = 0; iqbit < fConfiguration.fNQbits; iqbit++) {
        double currentMean, currentSigma;

        currentMean = PdfperLenghtCom[iqbit]->GetMean();
        currentSigma = PdfperLenghtCom[iqbit]->GetMeanError(1); // StaDev / sqrt(#effective entries)

        NAlteredVsNSent->SetPoint(iqbit, iqbit+1, currentMean);
        NAlteredVsNSent->SetPointError(iqbit, 0, currentSigma);

        int N = iqbit+1;
        double p = TMath::Power(1-currentMean, N);       // (1-mean) ^N, (~ 0.75^N) = probability to not observe altered qbits during the entire communication
        probabilityVsN->SetPoint(iqbit, N, 1-p);         // (~ 1-0.75^N) = probability to observe almost one altered qbit during the entire communication
        double p_error = N*p/(1-currentMean)*currentSigma; // N * x^(N-1) * sigmaX (aka N * x^N / x * sigmaX)
        probabilityVsN->SetPointError(iqbit, 0., p_error);

        double q = TMath::Power(currentMean,N);             // mean^N (~0.25^N): probability to receive all qbits altered
        fractionAlteredToN->SetPoint(iqbit, N, q);
        double q_error = N*q/currentMean*currentSigma;      // N * x^(N-1) * sigmaX (aka N * x^N / x * sigmaX)
        fractionAlteredToN->SetPointError(iqbit, 0., q_error);

    }

    // delete temporary histograms
    for(uint32_t i=0; i<fConfiguration.fNQbits; i++){
        delete PdfperLenghtCom[i];
        // PdfperLenghtCom[i]->Write();
    }
    delete[] PdfperLenghtCom;

    // ***save to files
    NAlteredVsNSent->SetNameTitle(TString::Format("%s_%s", fAlteredVsSentName, fConfiguration.fInfos.c_str()), TString::Format("%s_%s", fAlteredVsSentName, fConfiguration.fInfos.c_str()));
    NAlteredVsNSent->Write(TString::Format("%s_%s",fAlteredVsSentName, fConfiguration.fInfos.c_str()), TObject::kSingleKey | TObject::kOverwrite);
    delete NAlteredVsNSent;

    probabilityVsN->SetNameTitle(TString::Format("%s_%s", fProbabilityPlotName, fConfiguration.fInfos.c_str()), TString::Format("%s_%s", fProbabilityPlotName, fConfiguration.fInfos.c_str()));
    probabilityVsN->Write(TString::Format("%s_%s", fProbabilityPlotName, fConfiguration.fInfos.c_str()), TObject::kSingleKey | TObject::kOverwrite);  // options: nel caso di collezioni di oggetti tengo una sola key, nel caso ci sia già una key con questo nome la sovrascrivo
    delete probabilityVsN;

    probabilityVsN_teo->SetNameTitle(fProbabilityTeoPlotName, fProbabilityTeoPlotName); //Note that teoric curve is not config dependent
    probabilityVsN_teo->Write(fProbabilityTeoPlotName, TObject::kSingleKey | TObject::kOverwrite);
    delete probabilityVsN_teo;

    fractionAlteredToN->SetNameTitle(TString::Format("%s_%s", fAlteredToNPlotName, fConfiguration.fInfos.c_str()), TString::Format("%s_%s", fAlteredToNPlotName, fConfiguration.fInfos.c_str()));
    fractionAlteredToN->Write(TString::Format("%s_%s", fAlteredToNPlotName, fConfiguration.fInfos.c_str()), TObject::kSingleKey | TObject::kOverwrite);  // options: nel caso di collezioni di oggetti tengo una sola key, nel caso ci sia già una key con questo nome la sovrascrivo
    delete fractionAlteredToN;

    fractionAlteredToN_teo->SetNameTitle(fAlteredToNTeoPlotName, fAlteredToNTeoPlotName); //Note that teoric curve is not config dependent
    fractionAlteredToN_teo->Write(fAlteredToNTeoPlotName, TObject::kSingleKey | TObject::kOverwrite);
    delete fractionAlteredToN_teo;
}


/// To create: 1) NSameBasisVsNsent = TH1D: NSameBasis/Nsent vs length of communication
///            2) Useful_distr = TH1D: distribution of NSameBasis/Nsent
void Simulator::PlotNSameBasisVsNSent(TTree *tree) {
    printf("\nPlotting N with same base vs total N sent\n");
    TBranch *data = tree->GetBranch(fBranchName);
    static CommunicationInfos currentData;
    data->SetAddress(&currentData);

    auto NSameBasisVsNsent = new TH1D(TString::Format("%s_%s", fSameBasisHistName, fConfiguration.fInfos.c_str()), TString::Format("%s_%s", fSameBasisHistName, fConfiguration.fInfos.c_str()), fConfiguration.fNQbits, 0.5, fConfiguration.fNQbits+0.5);  //number of photons intercepted vs. number of photons measured in the same base
    auto Useful_distr = new TH1D(TString::Format("%s_%s", fUsefulPlotName, fConfiguration.fInfos.c_str()), TString::Format("%s_%s", fUsefulPlotName, fConfiguration.fInfos.c_str()), 11, -0.05, 1.05);  //number of photons intercepted vs. number of all photons sent

    double distrNormFactor = 1. / fConfiguration.fNSimulations / fConfiguration.fNQbits;  // normalization

    auto entries = tree->GetEntries();
    for (Long64_t entry = 0; entry < entries; ++entry) {
        printf("\r%llu/%llu", entry + 1, entries);
        tree->GetEvent(entry);

        int NSamebasis = currentData.SameBasisAltered + currentData.SameBasisUntouched;
        NSameBasisVsNsent->Fill(currentData.Ntot, static_cast<double>(NSamebasis) /(fConfiguration.fNSimulations * currentData.Ntot));

        if (currentData.Ntot == fConfiguration.fNQbits / 2) Useful_distr->Fill(static_cast<double>(NSamebasis) / currentData.Ntot, distrNormFactor);
    }

    // ***write to file
    NSameBasisVsNsent->Write(TString::Format("%s_%s", fSameBasisHistName, fConfiguration.fInfos.c_str()), TObject::kSingleKey | TObject::kOverwrite);
    NSameBasisVsNsent->SetDirectory(nullptr);
    Useful_distr->Write(TString::Format("%s_%s", fUsefulPlotName, fConfiguration.fInfos.c_str()), TObject::kSingleKey | TObject::kOverwrite);
    Useful_distr->SetDirectory(nullptr);

    delete Useful_distr;
    delete NSameBasisVsNsent;
}


///  To create: 1) NVsNHist_distr = TH1D: distribution of NSameBasisAltered/NSameBasis
void Simulator::HistNaltered(TTree * tree) {
    printf("\nPlotting hist of N altered\n");
    TBranch *data = tree->GetBranch(fBranchName);
    static CommunicationInfos currentData;
    data->SetAddress(&currentData);

    double fractionOfAltered = 0.;
    double distrNormFactor = 1. / fConfiguration.fNSimulations / fConfiguration.fNQbits; //normalization

    auto NVsNHist_distr = new TH1D(fAlteredDistrName, fAlteredDistrName, 10, 0, 1);
    auto entries = tree->GetEntries();
    for (Long64_t entry = 0; entry < entries; ++entry) {
        printf("\r%llu/%llu", entry + 1, entries);
        tree->GetEvent(entry);

        if (currentData.Ntot == fConfiguration.fNQbits / 2) { //fix one single length
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


/// To draw plots on a Canvas
Simulator* Simulator::ShowResults(TCanvas *cx) {  // gets things from file and plots them on cx
    printf("\nShowing results..\n");
    if (cx) {
        TFile file(fFilename, "READ");

        if (file.IsZombie()) {
            std::cerr << "Error opening file" << std::endl;
            return this;
        }

        auto probVsNHist = dynamic_cast<TGraphErrors*>(file.Get(TString::Format("%s_%s", fProbabilityPlotName, fConfiguration.fInfos.c_str())));
        auto probVsNHist_teo = dynamic_cast<TF1*>(file.Get(fProbabilityTeoPlotName));
        auto AlteredToNVsN = dynamic_cast<TGraphErrors*>(file.Get(TString::Format("%s_%s", fAlteredToNPlotName, fConfiguration.fInfos.c_str())));
        auto AlteredToNVsN_teo = dynamic_cast<TF1*>(file.Get(fAlteredToNTeoPlotName));
        auto NalteredVsNsent = dynamic_cast<TGraphErrors*>(file.Get(TString::Format("%s_%s", fAlteredVsSentName, fConfiguration.fInfos.c_str())));
        auto NVsNHist_distr = dynamic_cast<TH1D*>(file.Get(TString::Format("%s_%s", fAlteredDistrName, fConfiguration.fInfos.c_str())));
        if (NVsNHist_distr) NVsNHist_distr->SetDirectory(nullptr);
        auto UsefulHist = dynamic_cast<TH1D *>(file.Get(TString::Format("%s_%s", fUsefulPlotName, fConfiguration.fInfos.c_str())));
        if (UsefulHist) UsefulHist->SetDirectory(nullptr); // se sono istogrammi li faccio diventare miei (altrimenti poi spariscono quando chiudo il file)
        auto NSameBasisVsNqbit = dynamic_cast<TH1D*>(file.Get(TString::Format("%s_%s", fSameBasisHistName, fConfiguration.fInfos.c_str())));
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
        if(probVsNHist) probVsNHist->GetYaxis()->SetRangeUser(0,1.1);
        TLine line1;
        Simulator::SetStylesAndDraw(&line1, "", "", kRed, 2);
        line1.DrawLine(0., 1., fConfiguration.fNQbits, 1.);

        cx->cd(2);
        SetStylesAndDraw(NalteredVsNsent, "Number_of_sent_qbits", "Percentage_of_intercepted_qbits", kBlue, 2);
        TLine line2;
        Simulator::SetStylesAndDraw(&line2, "", "", kRed, 4);
        line2.DrawLine(fConfiguration.fNQbits/2., 0.18, fConfiguration.fNQbits/2., 0.28);

        cx->cd(3);
        SetStylesAndDraw(NSameBasisVsNqbit, "Number_of_sent_qbits", "Percentage_of_qbits_with_same_base", kBlue, 2);
        TLine line3;
        Simulator::SetStylesAndDraw(&line3, "", "", kRedBlue, 4);
        line3.DrawLine(fConfiguration.fNQbits/2., 0.495, fConfiguration.fNQbits/2., 0.505);
        TLine line05;
        Simulator::SetStylesAndDraw(&line05, "", "", kRed, 4);
        line05.DrawLine(0, 0.5, fConfiguration.fNQbits,0.5);

        cx->cd(4)->SetLogy();
        //SetStylesAndDraw(probVsNHist, "Number_of_sent_qbits", "Percentage_of_wrong_qbits", kCyan - 3, 2);
        //SetStylesAndDraw(probVsNHist_teo, "Number_of_sent_qbits_teo", "Percentage_of_wrong_qbits_teo", kOrange, 2);
        SetStylesAndDraw(AlteredToNVsN, "Number_of_sent_qbits", "Prob_to_receive_all_qbits_altered", kCyan - 3, 2);
        SetStylesAndDraw(AlteredToNVsN_teo, "Number_of_sent_qbits_teo", "Prob_to_receive_all_qbits_altered_teo", kRed, 2);

        cx->cd(5);
        SetStylesAndDraw(NVsNHist_distr, "Number_of_sent_qbits", "Percentage_of_intercepted_qbits", kBlue, 2);
        cx->cd(6);
        SetStylesAndDraw(UsefulHist, "Number_of_useful_qbits", "#", kYellow - 3, 2, 0);
    } else {
        std::cerr << "TCanvas is nullptr" << std::endl;
    }
    return this;
}


/// Set style and draw
void Simulator::SetStylesAndDraw(TObject *obj, const char *xLabel, const char *ylabel, Color_t color, Width_t linewidth,
                                 Style_t markerStyle) {
    //draw objects based of what class they inherits from
    if (obj) {
        std::cout<<std::endl<<"Plotting "<<obj->GetName()<<std::endl;  // print name of this object
        if(obj->InheritsFrom(TH1::Class())){                           // if the object is of a class which inherits from TH1
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

