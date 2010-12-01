// Author : Jeong Han Lee
// Date   : Monday, November 15 13:55:56 EST 2010
// 
// 
//          0.0.1 : Monday, November 15 13:55:56 EST 2010
//
//          0.0.2 : Friday, November 26 16:19:26 EST 2010
//                  Modified by: David Zou; dzou@jlab.org
//                 - added Beam positions on the target
//                   cc of the EPICS beam positions GUI
//          0.0.3 : Saturday, November 27 03:34:37 EST 2010
//                  jhlee@jlab.org
//                 - replaced histogram with axis and circle
//                   on pad in order to plot multiple points
//                   of BPMs and Target...
//                 - rearranged TPad
//          0.0.4 : Monday, November 29 19:22:44 EST 2010
//                  jhlee
//                 - do not draw when there are nothing
//                   in a root file
// 
//          0.0.5 : Monday, November 29 23:06:56 EST 2010, jhlee
//                 - added the golden BPM measurement if there is no
//                   way to run "caget" for test purpose
//                   https://hallcweb.jlab.org/hclog/1011_archive/101129222940.html
//
//          0.0.6 : Tuesday, November 30 22:52:39 EST 2010, jhlee
//                 - added the time stamp when it is created.
//
// TO LIST
//   * BPM offsets ?
//   * BPMs X/Y along z

// For example,
// 1) run root
// 2) .x raster.C(a root file name)
// 
// root [0] .x raster.C("Qweak_6949.000.root")
//


#include "TSystem.h"
#include "TString.h"
#include <ctime>

void
raster(TString name="")
{ 
  TString file_dir;
  TString output_dir;
  file_dir = gSystem->Getenv("QWSCRATCH");
  output_dir = file_dir;
  file_dir += "/rootfiles/";
  output_dir += "/plots/";

  if(name.IsNull()) {
    printf("There is no root file\n");
    exit(-1);
  }
  else {
    TFile *file =  new TFile(Form("%s%s", file_dir.Data(), name.Data()));
  }

  gStyle->SetPalette(1); 
  TCanvas *c1 = new TCanvas(Form("1DRaster"), Form("RasterMap1D in %s", name.Data()), 600,400);
 
  tracking_histo->cd();

  Bool_t file_output_flag = true;

  c1->Divide(2,2);
  c1->cd(1);
  if(raster_position_x-> GetEntries() != 0.0) {
    raster_position_x -> SetTitle(Form("Raster Position X in %s", name.Data()));
    raster_position_x -> Draw();
    file_output_flag &= true;
  }
  else {
    file_output_flag &= false;
  }

  c1->cd(2);
  if(raster_position_y-> GetEntries() != 0.0) {
    raster_position_y -> SetTitle(Form("Raster Position Y in %s", name.Data()));
    raster_position_y -> Draw();
    file_output_flag &= true;
  }
  else {
    file_output_flag &= false;
  }
  c1->cd(3);
  if(raster_posx_adc-> GetEntries() != 0.0) {
    raster_posx_adc -> SetTitle(Form("Raster PosX ADC in %s", name.Data()));
    raster_posx_adc -> Draw();
    file_output_flag &= true;
  }
  else {
    file_output_flag &= false;
  }
  c1->cd(4);
  if(raster_posy_adc-> GetEntries() != 0.0) {
    raster_posy_adc -> SetTitle(Form("Raster PosY ADC in %s", name.Data()));
    raster_posy_adc -> Draw();
    file_output_flag &= true;
  }
  else {
    file_output_flag &= false;
  }
  c1->Update();

  TString image_name;
  if(file_output_flag) {
    image_name = name + "1D.png";
    c1 -> SaveAs(output_dir + image_name);
  }


  TCanvas *c2 = new TCanvas(name.Data(), name.Data(), 600, 800);
  raster_rate_map->SetTitle(Form("Raster Rate Map in %s", name.Data()));
  c2->Divide(1,2,0.0001,0.0001);
  TPad *pad1 = (TPad*) c2->GetPrimitive(Form("%s_1", name.Data()));
  TPad *pad2 = (TPad*) c2->GetPrimitive(Form("%s_2", name.Data()));
 
  pad2->Divide(2,1, 0.000, 0.000);
  TPad *pad21 = (TPad*) pad2->GetPrimitive(Form("%s_2_1", name.Data()));
  TPad *pad22 = (TPad*) pad2->GetPrimitive(Form("%s_2_2", name.Data()));
  
  pad1 -> cd();
  pad1 -> SetBorderSize(0);
  //  pad1 -> SetFixedAspectRatio();
  pad1 -> SetPad(0.1,0.4,0.9,1.0);
  if(raster_rate_map-> GetEntries() != 0.0) {
    raster_rate_map -> Draw();
  }
  gPad->Update();
  
  pad2 -> cd();
  pad2 -> SetBorderSize(0);
  pad2 -> SetPad(0.0,0.0,1.0,0.4);
  pad2 -> Update();

  pad21 -> cd();
  Double_t p_range = 3.6;
  pad21 -> SetBorderSize(0);
  pad21 -> SetFrameLineColor(0);
  
  pad21 -> Range(-p_range,-p_range,+p_range,+p_range);
  pad21 -> SetFixedAspectRatio();
  TLatex *bpm_pos = new TLatex(-2.2,3.3,"Beam Positions on BPMs and Target");
  bpm_pos->SetTextFont(22);
  bpm_pos->SetTextSize(0.05);

  // H07C and H09B are the most interesting BPMs.
  //Get BCM, target, W plug position values and save as strings
  //                                                                         xoffset, yoffset, zoffset 
  TString H07AX     = gSystem->GetFromPipe("caget -t -f 3 IPM3H07A.XPOS");  // 0.4,    0.2,   138406.0
  TString H07BX     = gSystem->GetFromPipe("caget -t -f 3 IPM3H07B.XPOS");  // 0.2,   -0.2,   139363.0
  TString H07CX     = gSystem->GetFromPipe("caget -t -f 3 IPM3H07C.XPOS");  // 0.6,    0.0,   140329.0
  TString H08X      = gSystem->GetFromPipe("caget -t -f 3 IPM3H08.XPOS");   // 0.6,    1.6,   143576.0
  TString H09X      = gSystem->GetFromPipe("caget -t -f 3 IPM3H09.XPOS");   //-0.1,    0.6,   144803.0
  TString H09BX     = gSystem->GetFromPipe("caget -t -f 3 IPM3H09B.XPOS");  //-0.6,    0.3,   147351.0
  TString targetX   = gSystem->GetFromPipe("caget -t -f 3 qw:targetX");     // 14.1,    0.1,   148739.0
  TString tungstenX = gSystem->GetFromPipe("caget -t -f 3 qw:tungstenX");   // 14.1,    0.1,   149397.0

  TString H07AY     = gSystem->GetFromPipe("caget -t -f 3 IPM3H07A.YPOS");
  TString H07BY     = gSystem->GetFromPipe("caget -t -f 3 IPM3H07B.YPOS");
  TString H07CY     = gSystem->GetFromPipe("caget -t -f 3 IPM3H07C.YPOS");
  TString H08Y      = gSystem->GetFromPipe("caget -t -f 3 IPM3H08.YPOS");
  TString H09Y      = gSystem->GetFromPipe("caget -t -f 3 IPM3H09.YPOS");
  TString H09BY     = gSystem->GetFromPipe("caget -t -f 3 IPM3H09B.YPOS");
  TString targetY   = gSystem->GetFromPipe("caget -t -f 3 qw:targetY");
  TString tungstenY = gSystem->GetFromPipe("caget -t -f 3 qw:tungstenY");

  
  //
  // Just in case, when 'caget' doesn't work well or when we
  // want to test this script not on cdaqlx machine.
  // I took all values from qwick_19500-215000_7679.root
  // see https://hallcweb.jlab.org/hclog/1011_archive/101129222940.html
  //

  if(H07AX.IsNull())     H07AX     = "2.553";
  if(H07AY.IsNull())     H07AY     = "2.692";
  if(H07BX.IsNull())     H07BX     = "1.965";
  if(H07BY.IsNull())     H07BY     = "2.191";
  if(H07CX.IsNull())     H07CX     = "1.663";
  if(H07CY.IsNull())     H07CY     = "2.072";
  if(H08X.IsNull())      H08X      = "1.314";
  if(H08Y.IsNull())      H08Y      = "1.055";
  if(H09X.IsNull())      H09X      = "0.630";
  if(H09Y.IsNull())      H09Y      = "0.624";
  if(H09BX.IsNull())     H09BX     = "0.188";
  if(H09BY.IsNull())     H09BY     = "0.020";
  if(targetX.IsNull())   targetX   = "-0.176";
  if(targetY.IsNull())   targetY   = "-0.457";
  if(tungstenX.IsNull()) tungstenX = "-0.336";
  if(tungstenY.IsNull()) tungstenY = "-0.651";

  //Convert target position strings into doubles for plotting

  // TObjArray *  Array_targetX     = targetX.Tokenize(" ");
  // TObjString * Value_targetX     = (TObjString*) Array_targetX->At(0);
  // Double_t     Double_targetX    = Value_targetX->GetString().Atof();
  // TObjArray *  Array_targetY     = targetY.Tokenize(" ");
  // TObjString * Value_targetY     = (TObjString*) Array_targetY->At(0);
  // Double_t     Double_targetY    = Value_targetY->GetString().Atof();

  // we already use "caget -t option", it returns only "number",
  // thus, we don't need to tokenize them to convert "double".
  Double_t Double_targetX = targetX.Atof();
  Double_t Double_targetY = targetY.Atof();
  Double_t Double_H07CX   = H07CX.Atof();
  Double_t Double_H07CY   = H07CY.Atof();
  Double_t Double_H09BX   = H09BX.Atof();
  Double_t Double_H09BY   = H09BY.Atof();

  Double_t BPMs_PosX[3] = {0.0};
  Double_t BPMs_PosY[3] = {0.0};
  BPMs_PosX[0] = H07CX.Atof();
  BPMs_PosX[1] = H09BX.Atof();
  BPMs_PosX[2] = targetX.Atof();
  BPMs_PosY[0] = H07CY.Atof();
  BPMs_PosY[1] = H09BY.Atof();
  BPMs_PosY[2] = targetY.Atof();


  //Create a plot for beam position
  Int_t max_plot_range = 3;

  TGaxis *xaxis = new TGaxis(-max_plot_range,0,max_plot_range,0,-max_plot_range,max_plot_range,50210,"+-");
  xaxis -> SetName("xaxis");
  xaxis -> SetLineColor(50);
  xaxis -> SetTickSize(0.01);
  TGaxis *yaxis = new TGaxis(0,-max_plot_range,0,max_plot_range,-max_plot_range,max_plot_range,50210,"+-");
  yaxis -> SetName("yaxis");
  yaxis -> SetLineColor(50);
  yaxis -> SetTickSize(0.01);
  
  TEllipse *BPMs[3] = {NULL};
  for(Int_t i=0;i<3;i++) 
    {
      BPMs[i] = new TEllipse(BPMs_PosX[i], BPMs_PosY[i], 0.14);
      BPMs[i] -> SetLineWidth(1);
      BPMs[i] -> SetFillStyle(0);
    }
  BPMs[0] -> SetLineColor(kBlue);
  BPMs[1] -> SetLineColor(kGreen);
  BPMs[2] -> SetLineColor(kRed);

  bpm_pos -> Draw();
  xaxis   -> Draw();
  yaxis   -> Draw();
  
  for(Int_t i=0;i<3;i++) 
    {
      BPMs[i] -> Draw();
    }


  // TH2F *target_Histo=NULL;
  // if(target_Histo!=NULL) delete target_Histo;
  // target_Histo= new TH2F("h2", "Beam Position on Target", 200,-2,2,200,-2,2);
  // target_Histo->SetStats(0);   
  // target_Histo->SetMarkerStyle(3); 
  // target_Histo->SetMarkerColor(4); 
  // target_Histo->Fill(Double_targetX, Double_targetY); 
  // target_Histo->Draw();
  // target_Histo->SetMarkerStyle(4); 
  // target_Histo->SetMarkerColor(4); 
  // target_Histo->Fill(Double_H07CX, Double_H07CY); 
  // target_Histo->Draw("same");

  
  pad22 ->cd();
  pad22 -> SetBorderSize(0);
  TLatex l;
  l.SetTextSize(0.05);
  // Draw the columns titles
  l.SetTextAlign(22);
  l.DrawLatex(0.165, 0.95, "Name");
  l.DrawLatex(0.465, 0.95, "XPOS");
  l.DrawLatex(0.795, 0.95, "YPOS");
  l.SetTextAlign(12);
  float y, x1, x2;
  double d = 0.09;
  // Names
  y = 0.85; x1 = 0.07; 
  l.DrawLatex(x1, y, "IPM3H07A")  ; 
  y -= d ; l.DrawLatex(x1, y, "IPM3H07B")  ;
  y -= d ; l.DrawLatex(x1, y, "#color[4]{IPM3H07C}")  ;
  y -= d ; l.DrawLatex(x1, y, "IPM3H08")  ;
  y -= d ; l.DrawLatex(x1, y, "IPM3H09")  ;
  y -= d ; l.DrawLatex(x1, y, "#color[3]{IPM3H09B}")  ;
  y -= d ; l.DrawLatex(x1, y, "#color[2]{Target}")  ;
  y -= d ; l.DrawLatex(x1, y, "W plug")  ;
  // XPOS
  y = 0.85; x1 = 0.40; 
  l.DrawLatex(x1, y, H07AX.Data())   ; 
  y -= d ; l.DrawLatex(x1, y, H07BX.Data())  ;
  y -= d ; l.DrawLatex(x1, y, H07CX.Data())  ;
  y -= d ; l.DrawLatex(x1, y, H08X.Data())  ;
  y -= d ; l.DrawLatex(x1, y, H09X.Data())  ;
  y -= d ; l.DrawLatex(x1, y, H09BX.Data())  ;
  y -= d ; l.DrawLatex(x1, y, targetX.Data())  ;
  y -= d ; l.DrawLatex(x1, y, tungstenX.Data())  ;
  // YPOS
  y = 0.85; x1 = 0.73;
  l.DrawLatex(x1, y, H07AY.Data())   ; 
  y -= d ; l.DrawLatex(x1, y, H07BY.Data())  ;
  y -= d ; l.DrawLatex(x1, y, H07CY.Data())  ;
  y -= d ; l.DrawLatex(x1, y, H08Y.Data())  ;
  y -= d ; l.DrawLatex(x1, y, H09Y.Data())  ;
  y -= d ; l.DrawLatex(x1, y, H09BY.Data())  ;
  y -= d ; l.DrawLatex(x1, y, targetY.Data())  ;
  y -= d ; l.DrawLatex(x1, y, tungstenY.Data())  ;


  
  time_t rawtime;
  time ( &rawtime );
  //  printf ( "The current local time is: %s", ctime (&rawtime) );
  l.SetTextFont(12);
  l.SetTextSize(0.08);
  l.DrawLatex(0.2,0.08, Form("%s",ctime (&rawtime)));
  
  c2->Update();
  image_name = name +".png";
  c2 -> SaveAs(output_dir+image_name);
 
}
