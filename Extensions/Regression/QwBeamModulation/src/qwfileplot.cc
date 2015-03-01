#include "../include/headers.h"
#include "../include/QwDataFile.hh"
#include "TGraphErrors.h"
#include "TLegend.h"

Int_t main(Int_t argc, Char_t *argv[])
{

  //
  // For some reason TApplication fucks up the input argv's
  // don't need it anyway outside of testing though...
  //
  //   TApplication theApp("App", &argc, argv);

  TString filename = TString(argv[1]);
  TString stub = "";
  TString units = "mm";
  TString yaxis = Form("TargetX Position Difference (%s)", units.Data());
  TString xaxis = "Wien";
  TString title = "Run 1 TargetX Position Difference (with sign correction)";


  QwDataFile *file = new QwDataFile(filename, fstream::in );
//   QwDataFile *file = new QwDataFile("residual_out_e.dat");

//
// In case we would like to read in axis titles from command line, loop through argv
// and check from options.
//
  Int_t i = 0;

  while(argv[i]){

    TString::TString option(argv[i]);
    
    if(option.CompareTo("--xaxis", TString::kExact) == 0){
      xaxis = TString::TString(argv[i+1]);
    }
    if(option.CompareTo("--yaxis", TString::kExact) == 0){
      yaxis = TString::TString(argv[i+1]);
    }
    if(option.CompareTo("--file-stub", TString::kExact) == 0){
      stub = TString::TString(argv[i+1]);
    }
    i++;
  }

  file->ReadFile();

  std::cout << "Finished reading" << std::endl;

  std::vector <Double_t> zero_vector(file->GetArraySize());
  TF1 *line = new TF1("line", "[0]", file->fLowerX, file->fUpperX);

  TCanvas *canvas = new TCanvas("canvas", "canvas", 1000, 700);
  canvas->cd();
  gStyle->SetOptStat(1110);
  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *tgraph = new TGraphErrors(file->GetArraySize(), file->GetX().data(), file->GetY().data(), zero_vector.data(), file->GetYError().data());
  tgraph->SetMarkerColor(4);
  tgraph->SetMarkerStyle(24);
  line->SetLineColor(6);
  // line->SetMarkerSize(0.4);
  tgraph->GetXaxis()->SetTitle(xaxis);
  tgraph->GetYaxis()->SetTitle(yaxis);
  tgraph->GetXaxis()->SetLabelSize(0.02);
  tgraph->GetYaxis()->SetLabelSize(0.02);
  tgraph->GetXaxis()->SetTitleSize(0.02);
  tgraph->GetYaxis()->SetTitleSize(0.02);
  tgraph->GetYaxis()->SetTitleOffset(1.25);
  tgraph->Fit("line", "");
  tgraph->Draw("AP");
  // tgraph->SetTitle(Form("Fit:%4.4e +- %4.4e    Chi2: %4.4f NDF: %d", 
  // 			line->GetParameter(0), 
  // 			line->GetParError(0),
  // 			line->GetChisquare(),
  // 			line->GetNDF()));

  TLegend *legend = new TLegend(0.1, 0.8, 0.45, 0.92);

  legend->AddEntry(line, Form("Average Positon Difference: %3.3e +- %3.3e %s", line->GetParameter(0), line->GetParError(0), units.Data()), "l");
  
//   canvas->SaveAs("output_plot.C");
  canvas->SaveAs(Form("output_plot_%s.C", stub.Data()));
//   theApp.Run();

  std::cout << "Finished." << std::endl;

  return 0;
}
