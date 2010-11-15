// Author : Jeong Han Lee
// Date   : Monday, November 15 13:55:56 EST 2010
// 
// For example,
// 1) run root
// 2) .x raster.C(a root file name)
// 
// root [0] .x raster.C("~/scratch/rootfiles/Qweak_6949.000.root")



void
raster(TString name="")
{
  TFile *file =  new TFile(name.Data());
  gStyle->SetPalette(1); 
  TCanvas *c1 = new TCanvas();
 
  tracking_histo->cd();
  c1->Divide(2,2);
  c1->cd(1);
  raster_position_x -> SetTitle(Form("Raster Position X in %s", name.Data()));
  raster_position_x -> Draw();

  c1->cd(2);
  raster_position_y -> SetTitle(Form("Raster Position Y in %s", name.Data()));
  raster_position_y -> Draw();
  c1->cd(3);
  raster_posx_adc -> SetTitle(Form("Raster PosX ADC in %s", name.Data()));
  raster_posx_adc -> Draw();
  c1->cd(4);
  raster_posy_adc -> SetTitle(Form("Raster PosY ADC in %s", name.Data()));
  raster_posy_adc -> Draw();
  c1->Update();
  
  TCanvas *c2 = new TCanvas(name.Data(), name.Data(), 600, 600);
  raster_rate_map->SetTitle(Form("Raster Rate Map in %s", name.Data()));
  raster_rate_map -> Draw();
  c2->Update();
  TString image_name;
  image_name = name + ".png";
  c2 -> SaveAs(image_name);
 
}
