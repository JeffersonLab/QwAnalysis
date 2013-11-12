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

  TString filename0 = TString(argv[1]);
  TString filename1 = TString(argv[2]);
  
  // TString yaxis = "Detector Yield (ppm)";
  TString units = "ppm/urad";
  // TString yaxis = Form("TargetX Position Difference (%s)", units.Data());
  TString yaxis = Form("TargetYSlope Sensitivity (%s)", units.Data());
  //   TString yaxis = "Detector Response (mV/uA)";
  //   TString yaxis = "Monitor Response (mm)";
  // TString yaxis = "Correction (ppb)";
  // TString xaxis = "Octant Number";
  TString xaxis = "Wien";
  // TString xaxis = "Run Number";
  //   TString title = "Residual Yield";
  //   TString title = "Correction to Asymmetry";
  TString title = "";
  
  //   TString yaxis = "Asymmetry Width (ppm)";
  //   TString xaxis = "Run Number";
  //   TString title = "Asymmetry Width Modulation Data";
  
  QwDataFile *file0 = new QwDataFile(filename0, fstream::in);
  QwDataFile *file1 = new QwDataFile(filename1, fstream::in);
  //   QwDataFile *file = new QwDataFile("residual_out_e.dat");
  
  file0->ReadFile();
  file1->ReadFile();

  std::cout << "Finished reading" << std::endl;

  std::vector <Double_t> zero_vector0(file0->GetArraySize());
  TF1 *line0 = new TF1("line0", "[0]", file0->fLowerX, file0->fUpperX);

  TCanvas *canvas = new TCanvas("canvas", "canvas", 1000, 700);
  //  canvas->cd();
  gStyle->SetOptStat(1110);
  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *tgraph0 = new TGraphErrors(file0->GetArraySize(), file0->GetX().data(), file0->GetY().data(), zero_vector0.data(), file0->GetYError().data());
  tgraph0->SetMarkerColor(8);
  tgraph0->SetMarkerStyle(21);
  line0->SetLineColor(8);
  //   line->SetMarkerSize(0.4);
  tgraph0->GetXaxis()->SetLabelSize(0.02);
  tgraph0->GetYaxis()->SetLabelSize(0.02);
  tgraph0->GetXaxis()->SetTitleSize(0.02);
  tgraph0->GetYaxis()->SetTitleSize(0.02);
  //   tgraph0->Draw("AP");

  // TF1 *sine0 = new TF1("sine0", "[0] + [1]*sin((TMath::Pi()/4)*x + [2])", 0., 360.);
  // sine0->SetLineColor(2);
  // tgraph0->Fit("sine0", "R B+");


  tgraph0->Fit("line0", "");
  line0->SetLineColor(8);
     // tgraph0->SetTitle(Form("Fit:%e +- %e    Chi2/NDF: %e", 
     // 			line->GetParameter(0), 
     // 			line->GetParError(0),
     // 			line->GetChisquare()/line->GetNDF()));
  

  //
  // File 1
  //
  
  std::vector <Double_t> zero_vector1(file1->GetArraySize());
  TF1 *line1 = new TF1("line1", "[0]", file1->fLowerX, file1->fUpperX);
  
  //   TCanvas *canvas = new TCanvas("canvas", "canvas", 1000, 700);
  //   canvas->cd();
  gStyle->SetOptStat(1110);
  gPad->SetGridx();
  gPad->SetGridy();

  TGraphErrors *tgraph1 = new TGraphErrors(file1->GetArraySize(), file1->GetX().data(), file1->GetY().data(), zero_vector1.data(), file1->GetYError().data());
  tgraph1->SetMarkerColor(9);
  tgraph1->SetMarkerStyle(21);
  line1->SetLineColor(9);
  //   line->SetMarkerSize(0.4);
  tgraph1->GetXaxis()->SetLabelSize(0.02);
  tgraph1->GetYaxis()->SetLabelSize(0.02);
  tgraph1->GetXaxis()->SetTitleSize(0.02);
  tgraph1->GetYaxis()->SetTitleSize(0.02);
  //   tgraph1->Draw("AP");

  // TF1 *sine1 = new TF1("sine1", "[0] + [1]*sin((TMath::Pi()/4)*x + [2])", 0., 360.);
  // sine1->SetLineColor(4);
  // tgraph1->Fit("sine1", "R B+");

  tgraph1->Fit("line1", "");
  line1->SetLineColor(4);
     // tgraph1->SetTitle(Form("Fit:%e +- %e    Chi2/NDF: %e", 
     // 			 line->GetParameter(0), 
     // 			 line->GetParError(0),
     // 			 line->GetChisquare()/line->GetNDF()));
  
  canvas->cd();
  
  TMultiGraph *mgraph = new TMultiGraph();
  mgraph->Add(tgraph0);
  mgraph->Add(tgraph1);
  mgraph->Draw("AP");

  mgraph->GetXaxis()->SetLabelSize(0.02);
  mgraph->GetYaxis()->SetLabelSize(0.02);
  mgraph->GetXaxis()->SetTitleSize(0.02);
  mgraph->GetYaxis()->SetTitleSize(0.02);
  mgraph->GetYaxis()->SetTitleOffset(1.5);
  mgraph->GetXaxis()->SetTitle(xaxis);
  mgraph->GetYaxis()->SetTitle(yaxis);
  mgraph->SetTitle(title);
  
  TLegend *legend = new TLegend(0.1, 0.8, 0.45, 0.92);
    // legend->AddEntry(line0, Form("Sine: %3.3e +- %3.3e ", line0->GetParameter(0), line0->GetParError(0)), "l");
    // legend->AddEntry(line1,  Form("Cosine: %3.3e +- %3.3e ", line1->GetParameter(0), line1->GetParError(0)), "l");
    // legend->Draw();

  // TLegend *legend = new TLegend(0.1, 0.8, 0.45, 0.92);
  // legend->AddEntry(sine0, Form("Const: %3.3e +- %3.3e ",   sine0->GetParameter(0), sine0->GetParError(0)), "l");
  // legend->AddEntry(sine1, Form("Const: %3.3e +- %3.3e ",   sine1->GetParameter(0), sine1->GetParError(0)), "l");
  // legend->AddEntry(sine0, Form("Sine: %3.3e +- %3.3e ",    sine0->GetParameter(1), sine0->GetParError(1)), "l");
  // legend->AddEntry(sine1,  Form("Cosine: %3.3e +- %3.3e ", sine1->GetParameter(1), sine1->GetParError(1)), "l");
  // legend->Draw();
  
//   legend->AddEntry(line0, Form("Raw: %4.4f +- %4.4f ppm", line0->GetParameter(0), line0->GetParError(0)), "l");
//   legend->AddEntry(line1,  Form("Corrected: %4.4f +- %4.4f ppm", line1->GetParameter(0), line1->GetParError(0)), "l");
//   legend->Draw();

// Do a little math to get the NULL and average
//

  Double_t mean0  = line0->GetParameter(0);
  Double_t mean1  = line1->GetParameter(0);
  Double_t error0 = line0->GetParError(0);
  Double_t error1 = line1->GetParError(0);

  // Average

  Double_t average = (mean0/TMath::Power(error0,2) + mean1/TMath::Power(error1,2))/(1/TMath::Power(error0,2) + 1/TMath::Power(error1,2));
  Double_t average_error = TMath::Sqrt(1/(1/TMath::Power(error0,2) + 1/TMath::Power(error1,2)));
  Double_t null = (mean0 - mean1); 
  Double_t null_error = TMath::Sqrt(TMath::Power(error0,2) + TMath::Power(error1,2));

  // legend->AddEntry(line0, Form("SET11: %3.3e +- %3.3e %s", line0->GetParameter(0), line0->GetParError(0), units.Data()), "l");
  // legend->AddEntry(line1,  Form("BMOD: %3.3e +- %3.3e %s", line1->GetParameter(0), line1->GetParError(0), units.Data()), "l");
  legend->AddEntry(line0, Form("IN: %3.3e +- %3.3e %s", line0->GetParameter(0), line0->GetParError(0), units.Data()), "l");
  legend->AddEntry(line1,  Form("OUT: %3.3e +- %3.3e %s", line1->GetParameter(0), line1->GetParError(0), units.Data()), "l");
  // legend->AddEntry((TObject *)NULL,  Form("Weighted Average: %3.3e +- %3.3e %s", average, average_error, units.Data()), "p");
  // legend->AddEntry((TObject *)NULL,  Form("In-Out: %3.3e +- %3.3e %s", null, null_error, units.Data()), "p");
  legend->Draw();

  /*
   std::fstream sine_out_file;
   sine_out_file.open("sine_output.dat", fstream::out | fstream::app);

   std::fstream cosine_out_file;
   cosine_out_file.open("cosine_output.dat", fstream::out | fstream::app);

   sine_out_file   << line0->GetParameter(0) << " " 
   		  << line0->GetParError(0)  << std::endl;
   cosine_out_file << line1->GetParameter(0) << " " 
   		  << line1->GetParError(0)  << std::endl;

   sine_out_file.close();
   cosine_out_file.close();
  */
  canvas->Update();
  canvas->SaveAs("output_plot.C");
  //   theApp.Run();
  /*
  std::cout << "Calculate Averages : )" << std::endl;

  Double_t m1 = 0;
  Double_t m2 = 0;
  Double_t m3 = 0;
  Double_t m4 = 0;

  Double_t m1_error = 0;
  Double_t m2_error = 0;
  Double_t m3_error = 0;
  Double_t m4_error = 0;

  Double_t iter = file0->GetY().size()*0.25;

  for(Int_t i = 0; i < (Int_t)iter; i++){
    m1 += (file0->GetY()[i])/(TMath::Power(file0->GetYError()[i], 2));
    m1_error += 1/TMath::Power(file0->GetYError()[i], 2);

    m2 += (file0->GetY()[i+1])/(TMath::Power(file0->GetYError()[i+1], 2));
    m2_error += 1/TMath::Power(file0->GetYError()[i+1], 2);

    m3 += (file0->GetY()[i+2])/(TMath::Power(file0->GetYError()[i+2], 2));
    m3_error += 1/TMath::Power(file0->GetYError()[i+2], 2);

    m4 += (file0->GetY()[i+3])/(TMath::Power(file0->GetYError()[i+3], 2));
    m4_error += 1/TMath::Power(file0->GetYError()[i+3], 2);


  }
  m1 = m1/m1_error;
  m1_error = TMath::Sqrt(1/m1_error);

  m2 = m2/m2_error;
  m2_error = TMath::Sqrt(1/m2_error);

  m3 = m3/m3_error;
  m3_error = TMath::Sqrt(1/m3_error);

  m4 = m4/m4_error;
  m4_error = TMath::Sqrt(1/m4_error);

  std::cout << "Sin: \n" 
	    << "1 " << m1 << " " << m1_error << "\n"
	    << "2 " << m2 << " " << m2_error << "\n"
	    << "3 " << m3 << " " << m3_error << "\n"
	    << "4 " << m4 << " " << m4_error << "\n"
	    << std::endl;

  m1 = 0;
  m2 = 0;
  m3 = 0;
  m4 = 0;

  m1_error = 0;
  m2_error = 0;
  m3_error = 0;
  m4_error = 0;

  iter = file1->GetY().size()*0.25;

  for(Int_t i = 0; i < (Int_t)iter; i++){
    m1 += (file1->GetY()[i])/(TMath::Power(file1->GetYError()[i], 2));
    m1_error += 1/TMath::Power(file1->GetYError()[i], 2);

    m2 += (file1->GetY()[i+1])/(TMath::Power(file1->GetYError()[i+1], 2));
    m2_error += 1/TMath::Power(file1->GetYError()[i+1], 2);

    m3 += (file1->GetY()[i+2])/(TMath::Power(file1->GetYError()[i+2], 2));
    m3_error += 1/TMath::Power(file1->GetYError()[i+2], 2);

    m4 += (file1->GetY()[i+3])/(TMath::Power(file1->GetYError()[i+3], 2));
    m4_error += 1/TMath::Power(file1->GetYError()[i+3], 2);


  }
  m1 = m1/m1_error;
  m1_error = TMath::Sqrt(1/m1_error);

  m2 = m2/m2_error;
  m2_error = TMath::Sqrt(1/m2_error);

  m3 = m3/m3_error;
  m3_error = TMath::Sqrt(1/m3_error);

  m4 = m4/m4_error;
  m4_error = TMath::Sqrt(1/m4_error);

  std::cout << "Cos: \n" 
	    << "1 " << m1 << " " << m1_error << "\n"
	    << "2 " << m2 << " " << m2_error << "\n"
	    << "3 " << m3 << " " << m3_error << "\n"
	    << "4 " << m4 << " " << m4_error << "\n"
	    << std::endl;
  */
  std::cout << "Finished." << std::endl;

  return 0;
}
