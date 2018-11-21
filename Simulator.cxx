//
// Created by Filippo Valle on 15/11/2018.
//

#include "Simulator.h"
#include <stdio.h>

Simulator* Simulator::fgSimulator = nullptr;

Simulator::Simulator(bool useLogicQbits) :
        fNqbits(100),
        fUseLogicQbits(useLogicQbits)
{
    fChannels = new Channel *[2];

    fChannels[0] = new Channel();
    fChannels[0]->SetNoisy(false);
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

Simulator * Simulator::Instance(bool useLogicQbits = false) {
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

    auto NVsNHist = new TH1D(fNPlotName, fNPlotName, fNqbits, 0.5, fNqbits+0.5);
    auto NVsNHist_distr = new TH1D(fNDistrName, fNPlotName, 10, 0, 1);

    auto Useful_distr = new TH1D(fUsefulPlotName, "Useful", 10, 0, 1);
    auto PdfperLenghtCom = new TH1D*[fNqbits]; //(fPdfperLenghtCom, fPdfperLenghtCom, 30, 0, 1);
    char title[50];
    for(int i=0; i<fNqbits; i++){
      sprintf(title, "PdfperLenghtCom_%d", i);
      PdfperLenghtCom[i] = new TH1D(title, title, 10, 0, 1);
    }

    auto tree = dynamic_cast<TTree*>(file.Get(fTreename));
    if(tree && !tree->IsZombie()) {
        TBranch *data = tree->GetBranch(fBranchName);
        static fStructToSave currentData;
        data->SetAddress(&currentData);

        double fractionOfIntercepted = 0.;
        double distrNormFactor = 1./ fSimulations / fNqbits;
	int j = 0;

        auto entries = tree->GetEntries();
        for (int entry = 0; entry < entries; ++entry) {
            printf("\r%u/%llu", entry+1, entries);
            tree->GetEvent(entry);

            int NSamebasis = currentData.SameBasisIntercept + currentData.SameBasisNoIntercept;
            if (NSamebasis != 0)
                fractionOfIntercepted = static_cast<double>(currentData.SameBasisIntercept) / NSamebasis;
            else fractionOfIntercepted = 0.;

            NVsNHist->Fill(currentData.Ntot, fractionOfIntercepted / fSimulations);
            NVsNHist_distr->Fill(fractionOfIntercepted, distrNormFactor);

            Useful_distr->Fill(static_cast<double>(NSamebasis)/currentData.Ntot, distrNormFactor);

	    j = entry%fNqbits;
	    PdfperLenghtCom[j] -> Fill(fractionOfIntercepted, 1./fSimulations);  

        }

	TF1 *fit_gaus = new TF1("fit_gaus", "gaus", 0., 1.);     //TF1 *fa = new TF1("fa","[0]*x*sin([1]*x)",-3,3);
	double sigma_PdfperLenght[fNqbits];
	double mean_PdfperLenght[fNqbits];
	double p_value = 0;
	for(int i=0; i<fNqbits; i++){
	  PdfperLenghtCom[i]->Fit("fit_gaus", "Q");
	  mean_PdfperLenght[i] = fit_gaus->GetParameter("Mean");
	  sigma_PdfperLenght[i] = fit_gaus->GetParameter("Sigma");
	  if(i==10){
	    cout << "mean: " << fit_gaus->GetParameter("Mean") << "\t sigma: " <<  fit_gaus->GetParameter("Sigma") << endl;
	  }	  
	  cout << "grafico PdfperLenghtCom " << i << "\t mean: " << mean_PdfperLenght[i] << endl;
	  //p_value = fit_gaus->GetProb();
	  //if(p_value < 0.05) cout <<" *** in communication with "<< i+1 << " Qbits, p_value is smaller than 0.05"<< endl; 
	}

	
        for(int nqbit = 1; nqbit <= fNqbits; nqbit++){
            double teoValue=TMath::Power(0.25, nqbit);
            probVsNHist_teo->Fill(nqbit, teoValue);

            double simulatedFrac = NVsNHist->GetBinContent(nqbit);
            probVsNHist->SetBinContent(nqbit, TMath::Power(simulatedFrac, nqbit));
            if(Qbit::DEBUG) printf("\nn:%20.15f pow:%20.15f", simulatedFrac, TMath::Power(simulatedFrac, nqbit));
        }


	

//        probVsNHist->Write();
//        probVsNHist_teo->Write();
//        NVsNHist->Write();
//        NVsNHist_distr->Write();
//        Useful_distr->Write();
        file.Write();
        file.Close();

    }else{
        std::cerr<<"Tree not found on file"<<std::endl;
    }

//    delete probVsNHist;
//    delete probVsNHist_teo;
//    delete NVsNHist;
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
        auto NVsNHist = dynamic_cast<TH1D*>(file.Get(fNPlotName));
        NVsNHist->SetDirectory(nullptr);
        auto NVsNHist_distr = dynamic_cast<TH1D*>(file.Get(fNDistrName));
        NVsNHist_distr->SetDirectory(nullptr);
        auto UsefulHist = dynamic_cast<TH1D*>(file.Get(fUsefulPlotName));
        UsefulHist->SetDirectory(nullptr);
        

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

    }else{
        std::cerr<<"TCanvas is nullptr"<<std::endl;
    }
    return this;
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

/*
Simulator* Simulator::PlotSigmas() {
  auto PdfperLenghtCom = new TH1D*[fNqbits]; //(fPdfperLenghtCom, fPdfperLenghtCom, 10, 0, 1);
  char title[50];
  for(int i=0; i<fNqbits; i++){
    sprintf(title, "fPdfperLenghtCo_%d", i);
    PdfperLenghtCom[i] = new TH1D(title, title, 10, 0, 1);
  }


  auto PdfperLenghtCom = dynamic_cast<TH1D*>(file.Get(fPdfperLenghtCom));
  PdfperLenghtCom->SetDirectory(nullptr);


  
}
*/
