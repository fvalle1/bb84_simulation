//
// Created by francescobonacina on 06/12/18.
//


#include "Analyzer.h"

/// global pointer set to nullptr
Analyzer* Analyzer::fgAnalyzer = nullptr;


/// to instance an object Analyzer, it ensures that there is only one object at a time.
/// It calls the private constructor.
Analyzer* Analyzer::Instance(std::vector<ConfigSimulation> VettInfos) {
    if(fgAnalyzer) {
        fgAnalyzer->~Analyzer();              // destry old analyzer
        new(fgAnalyzer) Analyzer(VettInfos);  // create new analyzer in same space of memory, fgAnalyzer will point to it
    }else{
        fgAnalyzer = new Analyzer(VettInfos);
    }
    return fgAnalyzer;
}


/// constructor. It takes the vector of configurations
Analyzer::Analyzer(std::vector<ConfigSimulation> VettInfos) {
    fVettInfos = std::move(VettInfos);

    gStyle->SetLegendBorderSize(1);  // impostazioni che userò da qui in poi per tutte le simulazioni
    gStyle->SetLegendFillColor(0);
    gStyle->SetLegendFont(42);
    gStyle->SetOptStat(00000000);   // di default non mi stampa nessuna informazione

    gRandom->SetSeed(42);
}


/// destructor
Analyzer::~Analyzer() {
    fVettInfos.clear();
}


/// to run all the simulations configurated how is written in VettoInfos
void Analyzer::RunAnalyzer(){
    int current = 0;
    auto max = fVettInfos.size();
    for(const auto &config : fVettInfos){
        auto sim = Simulator::Instance(config);
        printf("\n**************\nSimulation %d/%lu", ++current, max);
        sim->RunSimulation()->GeneratePlots();
    }
}


/// to take main results of simulations executed and show them on multigraphs
void Analyzer::JoinResults(TCanvas *cx, uint32_t fixedN) {
    auto file = new TFile(Simulator::fFilename, "READ");

    // create multigraphs and set titles
    auto mg_NalteredVsNsent = new TMultiGraph();
    auto mg_ProbabilityVsNsent = new TMultiGraph();
    mg_NalteredVsNsent->SetTitle("Logic_vs_Physic; Nsent; Naltered");
    mg_ProbabilityVsNsent->SetTitle("Prob_to_receive_all_qbits_altered_vs_N; Nsent; Probability");

    FillMultiGraphs(file, mg_NalteredVsNsent, mg_ProbabilityVsNsent);

    cx->cd();
    cx->Clear();
    cx->SetCanvasSize(1000, 600);
    cx->SetWindowSize(1050, 630);
    cx->SetTitle("bb84");
    cx->SetName("bb84");
    cx->Divide(2,3); //separe leftRight

    AlteredVsSent(cx->cd(1), mg_NalteredVsNsent, fixedN);      // draw the multigraph mg_NalteredVsNsent
    PlotFunctionOfErrors(cx->cd(2), file, fixedN);             // draw NAlteredVsN at N = Nfixed vs sigma_noise (for each simulation)
    ProbabilityVsSent(cx->cd(3), file, mg_ProbabilityVsNsent); // draw the probability to receive all qbits altered in Nsent qbits (for each simulation)
    PlotSlopeVsNoise(cx->cd(4), file);                         // draw the slope of linear fit vs sigma_noise (for each simulation). The lines fit probability to receive all qbits altered in Nsent qbits, represented in log scale
    PlotNalteredDistributions(cx->cd(5), file);                // collect the histograms fAlteredDistrName of each simulation on the same THStack
    PlotFunctionOfAltered(cx->cd(6), file);                    // draw sigma_squared of TH1 fAlteredDistrName vs sigma_noise (for each simulation)

    file->Close();
    delete file;
}


/// cd(1)_ to draw the multigraph mg_NalteredVsNsent, with the legend and with an horizontal line at y=0.25
void Analyzer::AlteredVsSent(TVirtualPad *cx, TMultiGraph *mg_NalteredVsNsent, uint32_t fixed) const {
    auto pad = cx->cd();
    mg_NalteredVsNsent->Draw("APL");
    auto leg = pad->BuildLegend();
    TLine line25;
    Simulator::SetStylesAndDraw(&line25, "", "", kRedBlue, 6, 0);
    line25.DrawLine(0,0.25,100,0.25);
    leg->SetEntrySeparation(0);  // set vertical separation between lines in the legend
    leg->SetTextSize(0.05);

    TLine line;
    Simulator::SetStylesAndDraw(&line, "", "", kBlue, 4);
    line.DrawLine(fixed, 0.0, fixed, 0.35);
}


/// cd(3)_ to draw (NAltered/Nsent)^Nsent = probability to receive all qbits altered in Nsent qbits, with legend.
void Analyzer::ProbabilityVsSent(TVirtualPad *cx, TFile *file, TMultiGraph *mg_ProbabilityVsNsent) const {
    auto pad = cx->cd();
    pad->SetLogy();
    mg_ProbabilityVsNsent->Draw("APL");
    Simulator::SetStylesAndDraw(dynamic_cast<TF1 *> (file->Get(Simulator::fAlteredToNTeoPlotName)), "", "", kRed, 4, 0);
    auto leg = pad->BuildLegend();
    leg->SetEntrySeparation(0);
    leg->SetTextSize(0.05);
}


/// to fill multigraph with graphs taken from single simulation.
/// It sets different colors and different markers depending on the use of logic qubits and the presence of Eve
void Analyzer::FillMultiGraphs(TFile *file, TMultiGraph *mg_NalteredVsNsent, TMultiGraph *mg_ProbabilityVsNsent) const {
    TObject* g_tmpptr;  //to read file

    for(const auto &config : fVettInfos) {
        Color_t color;
        if(config.fEveIsPresent) color = kGreen; //Green(407) and violet(880) are distant RTypes.h to avoid overlap
        else color = kViolet;
        color += static_cast<Color_t>(10 * config.fSigma); //color changes due to error factor

        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(TString::Format("%s_%s", Simulator::fAlteredVsSentName, config.fInfos.c_str())));   // nel file cerco un oggetto che abbia il nome "nome grafico + info", Get restituisce il puntatore a questo oggetto, lo trasformo in un puntatore ad un TGraphError, assegno questo puntatore a g_tmpptr
        if (g_tmpptr) {
            if (config.fUseErrorCorrection) Simulator::SetStylesAndDraw(g_tmpptr, "", "", color, 2, 23);
            else Simulator::SetStylesAndDraw(g_tmpptr, "", "", color, 2, 20);   // different markers on the graph
            mg_NalteredVsNsent->Add(dynamic_cast<TGraphErrors*>(g_tmpptr));
        }else{
            std::cerr << std::endl << g_tmpptr << "nullptr reading fAlteredVsSentName" << std::endl;
        }

        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(
                TString::Format("%s_%s", Simulator::fAlteredToNPlotName, config.fInfos.c_str())));
        if(g_tmpptr){
            if (config.fUseErrorCorrection) Simulator::SetStylesAndDraw(g_tmpptr, "", "", color, 2, 23);
            else Simulator::SetStylesAndDraw(g_tmpptr, "", "", color, 2, 20);
            mg_ProbabilityVsNsent->Add(dynamic_cast<TGraphErrors*>(g_tmpptr));
        }else{
            std::cerr << std::endl << g_tmpptr << "nullptr reading fAlteredToNPlotName" << std::endl;
        }

    }
}


/// cd(2)_ It initializes 4 TGraphErrors which will collect results of simulation:
/// 1. P qbits_No Eve, 2. L qbits_No Eve, 3. P qbits_Eve, 4. L qbits_Eve.
/// Then sets points and errors getting values of NAlteredVsN at N = Nfixed from the graph fAlteredVsSentName of each
/// simulation, sets the style of these TGraphErrors. It creates a new multigraph, adds the 4 TGraphErrors and draws it.
void Analyzer::PlotFunctionOfErrors(TVirtualPad *cx, TFile *file, int Nfixed) const {

    auto pad = cx->cd();
    TGraphErrors* NalteredVsError[4]; // [0] P, [1] L, [2] P_Eve, [3] L_Eve
    InitMultiGraph(NalteredVsError, "alteredError");
    int pointCntr[4] = {0};           // counters of points we put in the 4 TGraphErrors
    TGraph* g_tmpptr = nullptr;
    double x , y;

    for(const auto &config : fVettInfos) {
        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(TString::Format("%s_%s", Simulator::fAlteredVsSentName, config.fInfos.c_str())));
        if (g_tmpptr) {
            g_tmpptr->GetPoint(Nfixed - 1, x, y); //Get value of NAlteredVsN at N = Nfixed and save coordinates in x and y
            int iGraph = config.fUseErrorCorrection?1:0; //chose the right graph
            if(config.fEveIsPresent) iGraph+=2;
            NalteredVsError[iGraph]->SetPoint(pointCntr[iGraph], config.fSigma, y);
            NalteredVsError[iGraph]->SetPointError(pointCntr[iGraph]++, 0, dynamic_cast<TGraphErrors *>(g_tmpptr)->GetEY()[Nfixed-1]);  // GetEY() returns the array of Y errors
        }
    }


    SetStyleMultiGraph(NalteredVsError);

    auto mg_NAlteredVsNoise = new TMultiGraph();
    mg_NAlteredVsNoise->SetTitle("Altered_vs_Noise; #sigma noise; Naltered");
    for (int i = 0; i < 4 ; i++) {
        NalteredVsError[i]->Set(pointCntr[i]); //set point to delete underflows
        mg_NAlteredVsNoise->Add(NalteredVsError[i]);
    }

    mg_NAlteredVsNoise->Draw("APL");
    auto leg = pad->BuildLegend();
    leg->SetEntrySeparation(0);
    leg->SetTextSize(0.05);
}


/// cd(6)_ It initializes 4 TGraphErrors wich will collect results of simulation:
/// 1. P qbits_No Eve, 2. L qbits_No Eve, 3. P qbits_Eve, 4. L qbits_Eve.
/// Then sets points and errors getting values of sigma_squared and error_sigma_squared for each TH1 fAlteredDistrName
/// of each simulation, sets the style of these TGraphErrors. It creates a new multigraph, adds the 4 TGraphErrors
/// and draws it.
void Analyzer::PlotFunctionOfAltered(TVirtualPad *cx, TFile *file) const{
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
            double devstNsameBase = dynamic_cast<TH1 *>(g_tmpptr)->GetStdDev();
            double errorSigmaSquare = 2 * devstNsameBase * dynamic_cast<TH1 *>(g_tmpptr)->GetStdDevError();
            Simulator::SetStylesAndDraw(g_tmpptr, "", "", color);
            devstVsNoise[iGraph]->SetPoint(devstVsNoiseCntr[iGraph], config.fSigma, devstNsameBase * devstNsameBase);
            devstVsNoise[iGraph]->SetPointError(devstVsNoiseCntr[iGraph]++, 0, errorSigmaSquare);
        }else{
            std::cerr << std::endl << "nullptr reading fAlteredDistrName" << std::endl;
        }
    }


    SetStyleMultiGraph(devstVsNoise);

    auto mg_Devst_sigma = new TMultiGraph();
    mg_Devst_sigma->SetTitle("#sigmaAltered_vs_Noise; #sigma noise; #devSt^2 altered");
    for (int i = 0; i < 4 ; i++) {
        devstVsNoise[i]->Set(devstVsNoiseCntr[i]);
        mg_Devst_sigma->Add(devstVsNoise[i]);
    }

    mg_Devst_sigma->Draw("APL");
    auto leg = pad->BuildLegend();
    leg->SetEntrySeparation(0);
    leg->SetTextSize(0.05);
}


/// cd(5)_ to collect the histograms fAlteredDistrName of each simulation on the same THStack
void Analyzer::PlotNalteredDistributions(TVirtualPad *cx, TFile *file) const {
    TObject* g_tmpptr = nullptr;

    auto st_Nusefuldistr = new THStack();

    for(const auto &config : fVettInfos) {
        g_tmpptr = dynamic_cast<TH1D *> (file->Get(TString::Format("%s_%s", Simulator::fAlteredDistrName, config.fInfos.c_str())));
        if (g_tmpptr) {
            dynamic_cast<TH1D*>(g_tmpptr)->SetDirectory(nullptr);
            st_Nusefuldistr->Add(dynamic_cast<TH1D *>(g_tmpptr));
        }else{
            std::cerr << std::endl << "nullptr reading fAlteredDistrName" << std::endl;
        }
    }

    cx->cd();
    st_Nusefuldistr->SetNameTitle("Distrs of altered","Distrs of altered");
    st_Nusefuldistr->Draw("nostack");
}


/// cd(4)_ It initializes 4 TGraphErrors wich will collect results of simulation:
/// 1. P qbits_No Eve, 2. L qbits_No Eve, 3. P qbits_Eve, 4. L qbits_Eve.
/// Then sets points and errors getting values of linear fit on TGraph fAlteredToNPlotName of each simulation,
/// sets the style of these TGraphErrors. So these TGraphErrors represent slope of the lines vs sigmas.
/// It creates a new multigraph, adds the 4 TGraphErrors and draws it.
void Analyzer::PlotSlopeVsNoise(TVirtualPad *cx, TFile *file) const {
    auto pad = cx->cd();

    TObject* g_tmpptr = nullptr;

    TGraphErrors* slopeVsNoise[4]; //[0] P [1] L [2] P_Eve [3]L_Eve
    InitMultiGraph(slopeVsNoise, "slope_noise");
    int slopeVsNoiseCntr[4] = {0};

    for(const auto &config : fVettInfos) {
        g_tmpptr = dynamic_cast<TGraphErrors *> (file->Get(TString::Format("%s_%s", Simulator::fAlteredToNPlotName, config.fInfos.c_str())));
        if (g_tmpptr) {
            int iGraph = config.fUseErrorCorrection?1:0;
            if(config.fEveIsPresent) iGraph+=2;
            auto fit = new TF1("lin", "TMath::Power([0],x)", 1, config.fNQbits);
            dynamic_cast<TGraphErrors*>(g_tmpptr)->Fit(fit,"Q0M");  // Q = minimum printing, 0 = do not plot the result of the fit, M = attempts to improve the found local minimum by searching for a better one
            slopeVsNoise[iGraph]->SetPoint(slopeVsNoiseCntr[iGraph], config.fSigma, fit->GetParameter(0));
            slopeVsNoise[iGraph]->SetPointError(slopeVsNoiseCntr[iGraph]++, 0, fit->GetParError(0));
            delete fit;
        }else{
            std::cerr << std::endl << "nullptr reading fAlteredToNPlotName" << std::endl;
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


/// to initialize 4 TGraphErrors to plot results of simultaions (1. using physical qbits, without Eve,
/// 2. using logical qubits, without Eve, 3. using physcal qbits with Eve and 4. using logical qbits, with Eve) vs Noise.
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


/// to set colors and markers of multigraphs
void Analyzer::SetStyleMultiGraph(TGraphErrors *const *mg) const {
    Simulator::SetStylesAndDraw(mg[0], "", "", kCyan - 2, 1, 24);
    Simulator::SetStylesAndDraw(mg[1], "", "", kViolet, 1, 32);
    Simulator::SetStylesAndDraw(mg[2], "", "", kCyan - 9, 1, 24);
    Simulator::SetStylesAndDraw(mg[3], "", "", kViolet + 4, 1, 32);
}
