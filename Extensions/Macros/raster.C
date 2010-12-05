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
//          0.0.7 : Sunday, December  5 03:06:26 EST 2010, jhlee
//                 - added three interesting beam position
//                   aloing z offsets (1D)
//                 - check "cdaqlx" or not (if not, don't run caget)
// 
// TO LIST
//   * BPM offsets      -> fixed by Buddhini (0.0.7) QwAnalysis Rev 2272
//   * BPMs X/Y along z -> done jhlee (0.0.7)
//   * Calculated BPMs X/Y along z according to the beam optics
//     under the midplane asymmetry. 
//   * One Frame with two buttons (Submit to HClog and Exit)
//     may be Transient Frame? need to contact Brad how to submit HClog automatically
//



// For example,
// 1) run root
// 2) .x raster.C(a root file name)
// 
// root [0] .x raster.C("qwick_145000-165000_7292.root")
//


#include "TSystem.h"
#include "TString.h"
#include "TGraph.h"
#include "TMarker.h"

#include <ctime>
#include <unistd.h>


void
raster(TString name="")
{ 
  Int_t i = 0;

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
  tracking_histo->cd();

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

  TString H07AX;
  TString H07BX;
  TString H07CX;
  TString H08X;
  TString H09X;
  TString H09BX;
  TString targetX;
  TString tungstenX;

  TString H07AY;
  TString H07BY;
  TString H07CY;
  TString H08Y;
  TString H09Y;
  TString H09BY;
  TString targetY;
  TString tungstenY;
 
  TString host_name;
  host_name = gSystem->HostName();
 
  //  printf("This machine is %s\n", host_name.Data());
  if( host_name.Contains("cdaql") ) {
    // H07C and H09B are the most interesting BPMs.
    //Get BCM, target, W plug position values and save as strings
    //                                                                         xoffset, yoffset, zoffset 
    H07AX     = gSystem->GetFromPipe("caget -t -f 3 IPM3H07A.XPOS");  // 0.4,    0.2,   138406.0
    H07BX     = gSystem->GetFromPipe("caget -t -f 3 IPM3H07B.XPOS");  // 0.2,   -0.2,   139363.0
    H07CX     = gSystem->GetFromPipe("caget -t -f 3 IPM3H07C.XPOS");  // 0.6,    0.0,   140329.0 (*)
    H08X      = gSystem->GetFromPipe("caget -t -f 3 IPM3H08.XPOS");   // 0.6,    1.6,   143576.0
    H09X      = gSystem->GetFromPipe("caget -t -f 3 IPM3H09.XPOS");   //-0.1,    0.6,   144803.0 
    H09BX     = gSystem->GetFromPipe("caget -t -f 3 IPM3H09B.XPOS");  //-0.6,    0.3,   147351.0 (*)
    targetX   = gSystem->GetFromPipe("caget -t -f 3 qw:targetX");     // 0.1,    1.2,   148750.0 (*)
    tungstenX = gSystem->GetFromPipe("caget -t -f 3 qw:tungstenX");   // 0.1,    0.1,   149408.0
  
    H07AY     = gSystem->GetFromPipe("caget -t -f 3 IPM3H07A.YPOS");
    H07BY     = gSystem->GetFromPipe("caget -t -f 3 IPM3H07B.YPOS");
    H07CY     = gSystem->GetFromPipe("caget -t -f 3 IPM3H07C.YPOS");
    H08Y      = gSystem->GetFromPipe("caget -t -f 3 IPM3H08.YPOS");
    H09Y      = gSystem->GetFromPipe("caget -t -f 3 IPM3H09.YPOS");
    H09BY     = gSystem->GetFromPipe("caget -t -f 3 IPM3H09B.YPOS");
    targetY   = gSystem->GetFromPipe("caget -t -f 3 qw:targetY");
    tungstenY = gSystem->GetFromPipe("caget -t -f 3 qw:tungstenY");


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

  }
  else {
    H07AX     = "2.553";
    H07AY     = "2.692";
    H07BX     = "1.965";
    H07BY     = "2.191";
    H07CX     = "1.663";
    H07CY     = "2.072";
    H08X      = "1.314";
    H08Y      = "1.055";
    H09X      = "0.630";
    H09Y      = "0.624";
    H09BX     = "0.188";
    H09BY     = "0.020";
    targetX   = "-0.176";
    targetY   = "-0.457";
    tungstenX = "-0.336";
    tungstenY = "-0.651";
  }

  //Convert target position strings into doubles for plotting

  Double_t Double_H07AX     = H07AX.Atof();
  Double_t Double_H07BX     = H07BX.Atof();
  Double_t Double_H07CX     = H07CX.Atof();
  Double_t Double_H09BX     = H09BX.Atof();
  Double_t Double_H08X      = H08X.Atof();
  Double_t Double_H09X      = H09X.Atof();
  Double_t Double_targetX   = targetX.Atof();
  Double_t Double_tungstenX = tungstenX.Atof();


  Double_t Double_H07AY     = H07AY.Atof();
  Double_t Double_H07BY     = H07BY.Atof();
  Double_t Double_H07CY     = H07CY.Atof();
  Double_t Double_H09BY     = H09BY.Atof();
  Double_t Double_H08Y      = H08Y.Atof();
  Double_t Double_H09Y      = H09Y.Atof();
  Double_t Double_targetY   = targetY.Atof();
  Double_t Double_tungstenY = tungstenY.Atof();

  Double_t Double_H07AZ     = 138406.0;
  Double_t Double_H07BZ     = 139363.0;
  Double_t Double_H07CZ     = 140329.0;
  Double_t Double_H08Z      = 143576.0;
  Double_t Double_H09Z      = 144803.0;
  Double_t Double_H09BZ     = 147351.0;
  Double_t Double_targetZ   = 148750.0;
  Double_t Double_tungstenZ = 149408.0;

 
  Double_t BPMs_X[3] = {0.0};
  Double_t BPMs_Y[3] = {0.0};
  BPMs_X[0] = H07CX.Atof();
  BPMs_X[1] = H09BX.Atof();
  BPMs_X[2] = targetX.Atof();
  BPMs_Y[0] = H07CY.Atof();
  BPMs_Y[1] = H09BY.Atof();
  BPMs_Y[2] = targetY.Atof();
  

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
  for(i=0;i<3;i++) 
    {
      BPMs[i] = new TEllipse(BPMs_X[i], BPMs_Y[i], 0.14);
      BPMs[i] -> SetLineWidth(1);
      BPMs[i] -> SetFillStyle(0);
    }
  BPMs[0] -> SetLineColor(kBlue);
  BPMs[1] -> SetLineColor(kGreen);
  BPMs[2] -> SetLineColor(kRed);

  bpm_pos -> Draw();
  xaxis   -> Draw();
  yaxis   -> Draw();
  
  for(i=0;i<3;i++) 
    {
      BPMs[i] -> Draw();
    }
  
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
 


  TCanvas *c1 = new TCanvas("1DRaster", Form("RasterMap1D in %s", name.Data()), 1100,640);
 
  c1->Divide(2,1,0.0001,0.0001);
  TPad *c11 = (TPad*) c1->GetPrimitive("1DRaster_1");
  TPad *c12 = (TPad*) c1->GetPrimitive("1DRaster_2");
  
  Bool_t file_output_flag = true;

  c11->Divide(2,2);
  c11->cd(1);
  if(raster_position_x-> GetEntries() != 0.0) {
    raster_position_x -> SetTitle(Form("Raster Position X in %s", name.Data()));
    raster_position_x -> Draw();
    file_output_flag &= true;
  }
  else {
    file_output_flag &= false;
  }

  c11->cd(2);
  if(raster_position_y-> GetEntries() != 0.0) {
    raster_position_y -> SetTitle(Form("Raster Position Y in %s", name.Data()));
    raster_position_y -> Draw();
    file_output_flag &= true;
  }
  else {
    file_output_flag &= false;
  }
  c11->cd(3);
  if(raster_posx_adc-> GetEntries() != 0.0) {
    raster_posx_adc -> SetTitle(Form("Raster PosX ADC in %s", name.Data()));
    raster_posx_adc -> Draw();
    file_output_flag &= true;
  }
  else {
    file_output_flag &= false;
  }
  c11->cd(4);
  if(raster_posy_adc-> GetEntries() != 0.0) {
    raster_posy_adc -> SetTitle(Form("Raster PosY ADC in %s", name.Data()));
    raster_posy_adc -> Draw();
    file_output_flag &= true;
  }
  else {
    file_output_flag &= false;
  }
  c11->Update();

  c12-> cd();
 

  TMultiGraph *hor_mgraph = new TMultiGraph();
  TMultiGraph *ver_mgraph = new TMultiGraph();

  Double_t pos[3] = {0.0};
  Double_t zz[3] = {Double_H07CZ, Double_H09BZ, Double_targetZ};
  for(i=0;i<3;i++) zz[i] *= 1e-3;
  TGraph* bpms_pos = new TGraph(3,zz,pos);
  // TGraph* horizontal = new TGraph(3, zz, BPMs_X);
  // TGraph* vertical = new TGraph(3, zz, BPMs_Y);

  TMarker *x_pos[3] = {NULL};
  TMarker *y_pos[3] = {NULL};
  for(i=0;i<3;i++) 
    {
      x_pos[i] = new TMarker(zz[i], BPMs_X[i], 4);
      y_pos[i] = new TMarker(zz[i], BPMs_Y[i], 4);
    
    }
  bpms_pos   -> SetMarkerStyle(5);
  bpms_pos   -> SetMarkerSize(2);
  // horizontal -> SetLineColor(kRed);
  // horizontal -> SetLineWidth(0);
  // vertical   -> SetLineColor(kRed);
  // vertical   -> SetLineWidth(0);

  hor_mgraph -> Add(bpms_pos, "p");
  // hor_mgraph -> Add(horizontal);
  ver_mgraph -> Add(bpms_pos, "p");
  // ver_mgraph -> Add(vertical);
  

  hor_mgraph -> SetMaximum(+max_plot_range);
  hor_mgraph -> SetMinimum(-max_plot_range);
  ver_mgraph -> SetMaximum(+max_plot_range);
  ver_mgraph -> SetMinimum(-max_plot_range);

  c12->Divide(1,2);
  c12->cd(1);
  gPad -> SetGridy();
  hor_mgraph -> Draw("apl");
  hor_mgraph -> SetTitle("Electron Beam Centroids (horizontal plane)");
  hor_mgraph -> GetXaxis() -> SetTitle("z (m)");
  hor_mgraph -> GetXaxis() -> SetTitleSize(0.05);
  hor_mgraph -> GetXaxis() -> SetTitleFont(22);
  hor_mgraph -> GetYaxis() -> SetTitle("x (mm)");
  hor_mgraph -> GetYaxis() -> CenterTitle(true);
  hor_mgraph -> GetYaxis() -> SetTitleSize(0.05);
  hor_mgraph -> GetYaxis() -> SetTitleOffset(0.5);
  hor_mgraph -> GetYaxis() -> SetTitleFont(22);
  x_pos[0] -> SetMarkerColor(kBlue);
  x_pos[1] -> SetMarkerColor(kGreen);
  x_pos[2] -> SetMarkerColor(kRed);
  x_pos[0] -> Draw();
  x_pos[1] -> Draw();
  x_pos[2] -> Draw();
  
  gPad->Update();

  c12->cd(2);
  gPad -> SetGridy();
  ver_mgraph -> Draw("apl");
  ver_mgraph -> SetTitle("Electron Beam Centroids (vertical plane)");
  ver_mgraph -> GetXaxis() -> SetTitle("z (m)");
  ver_mgraph -> GetXaxis() -> SetTitleSize(0.05);
  ver_mgraph -> GetXaxis() -> SetTitleFont(22);
  ver_mgraph -> GetYaxis() -> SetTitle("y (mm)");
  ver_mgraph -> GetYaxis() -> CenterTitle(true);
  ver_mgraph -> GetYaxis() -> SetTitleSize(0.05);
  ver_mgraph -> GetYaxis() -> SetTitleOffset(0.5);
  ver_mgraph -> GetYaxis() -> SetTitleFont(22);
  y_pos[0] -> SetMarkerColor(kBlue);
  y_pos[1] -> SetMarkerColor(kGreen);
  y_pos[2] -> SetMarkerColor(kRed);
  y_pos[0] -> Draw();
  y_pos[1] -> Draw();
  y_pos[2] -> Draw();
  gPad->Update();

  
  c12->cd(0);
  c12->Modified();
  c12->Update();

  TString image_name;
  if(file_output_flag) {
    image_name = name + "1D.png";
    c1 -> SaveAs(output_dir + image_name);
  }


}
