void mdlumi_ped(int run_num)
{

  const int mps_start = 0;
  const int mps_stop = 10000;
  const bool get_md = kTRUE;
  const bool get_lumi = kTRUE;
  const bool save_file = kTRUE;

  float mean = 0.0;
  float scale = (20./(1<<18));
  gStyle->SetStatW(0.4); 
  gStyle->SetStatH(0.4);

  TChain chain("Mps_Tree");
  chain.Add(Form("$QW_ROOTFILES/first100k_%i.root",run_num));

  const string lumi[16] = {
  "qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4",
  "qwk_dslumi5","qwk_dslumi6","qwk_dslumi7","qwk_dslumi8",
  "qwk_uslumi1neg","qwk_uslumi1pos","qwk_uslumi3neg","qwk_uslumi3pos",
  "qwk_uslumi5neg","qwk_uslumi5pos","qwk_uslumi7neg","qwk_uslumi7pos"};

  const string md[16] = {
  "qwk_md1neg","qwk_md2neg","qwk_md3neg","qwk_md4neg",
  "qwk_md5neg","qwk_md6neg","qwk_md7neg","qwk_md8neg",
  "qwk_md1pos","qwk_md2pos","qwk_md3pos","qwk_md4pos",
  "qwk_md5pos","qwk_md6pos","qwk_md7pos","qwk_md8pos"};
  
  const string bg[8] = {
  "qwk_pmtled","qwk_pmtonl","qwk_pmtltg","qwk_md9neg",
  "qwk_md9pos","qwk_isourc","qwk_preamp","qwk_cagesr"};
  

  char * pPath = "";
  if (save_file == kTRUE) pPath = getenv("QWANALYSIS");

  if (get_md == kTRUE)
    {      
      ofstream md_pedestal_file;
      md_pedestal_file.open(Form("%s/Extensions/Macros/Parity/MainDet/qweak_maindet_pedestal.%i-.map",pPath,run_num));
      md_pedestal_file<<endl;
      md_pedestal_file<<Form("!The following pedestals were recorded from RUN %i",run_num)<<endl;
      md_pedestal_file<<"!channel name , Mps channelname.hw_sum_raw/num_samples , gain"<<endl;

      
      TCanvas *c_md = new TCanvas("md","md",1500,1100);
      c_md->Divide(4,4);
      TH1F *mdhst[16];
      for (int i=0;i<16;i++)
        {
          c_md->cd(i+1);      
          mdhst[i] = new TH1F(Form("%h_%s",md[i]),"",100,0,0);
          mdhst[i]->SetDirectory(0);   
          chain.Draw( Form("%s.hw_sum_raw/%s.num_samples*%f*1000>>h_%s",md[i],md[i],scale,md[i]),Form("%s.hw_sum_raw!=0 && mps_counter>%i && mps_counter<%i",md[i],mps_start,mps_stop),"");
          TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("h_%s",md[i]));
          md_pedestal_file<<Form("%s  ,  ",md[i])<<htemp->GetMean()/(scale*1000)<<Form("  ,  %10.8f",scale)<<endl;  
          cout<<md[i]<<"  "<<htemp->GetMean()<<endl;
          c_md->Update();
        }
      TCanvas *c_bg = new TCanvas("bg","bg",1500,1100);
      c_bg->Divide(3,3);
      TH1F *bghst[8];
      for (int i=0;i<8;i++)
        {
          c_bg->cd(i+1);      
          bghst[i] = new TH1F(Form("%h_%s",bg[i]),"",100,0,0);
          bghst[i]->SetDirectory(0);   
          chain.Draw( Form("%s.hw_sum_raw/%s.num_samples*%f*1000>>h_%s",bg[i],bg[i],scale,bg[i]),Form("%s.hw_sum_raw!=0 && mps_counter>%i && mps_counter<%i",bg[i],mps_start,mps_stop),"");
          TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("h_%s",bg[i]));
          md_pedestal_file<<Form("%s  ,  ",bg[i])<<htemp->GetMean()/(scale*1000)<<Form("  ,  %10.8f",scale)<<endl;  
          cout<<bg[i]<<"  "<<htemp->GetMean()<<endl;
          c_bg->Update();
        }  
      md_pedestal_file.close();
     }; //end get md

  if (get_lumi == kTRUE)
    { 
      ofstream lumi_pedestal_file;
      lumi_pedestal_file.open(Form("%s/Extensions/Macros/Parity/MainDet/qweak_lumi_pedestal.%i-.map",pPath,run_num));
      lumi_pedestal_file<<endl;
      lumi_pedestal_file<<Form("!The following pedestals were recorded from RUN %i",run_num)<<endl;
      lumi_pedestal_file<<"!channel name , Mps channelname.hw_sum_raw/num_samples , gain"<<endl;
      TCanvas *c_lumi = new TCanvas("lumi","lumi",1500,1100);
      c_lumi->Divide(4,4);
      TH1F *lumihst[16];
      for (int i=0;i<16;i++)
        {  
          c_lumi->cd(i+1);      
          lumihst[i] = new TH1F(Form("%h_%s",lumi[i]),"",100,0,0);
          lumihst[i]->SetDirectory(0);   
          chain.Draw( Form("%s.hw_sum_raw/%s.num_samples*%f*1000>>h_%s",lumi[i],lumi[i],scale,lumi[i]),Form("%s.hw_sum_raw!=0 && mps_counter>%i && mps_counter<%i",lumi[i],mps_start,mps_stop),"");
          TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("h_%s",lumi[i]));
          lumi_pedestal_file<<Form("%s  ,  ",lumi[i])<<htemp->GetMean()/(scale*1000)<<Form("  ,  %10.8f",scale)<<endl;  
          cout<<lumi[i]<<"  "<<htemp->GetMean()<<endl;
          c_lumi->Update();
        }
     lumi_pedestal_file.close();
     }; // end get lumi  
     
//gSystem->Exec(Form("gedit %s/Extensions/Macros/Parity/MainDet/qweak_maindet_pedestal.%i-.map",pPath,run_num));
//gSystem->Exec(Form("gedit %s/Extensions/Macros/Parity/MainDet/qweak_lumi_pedestal.%i-.map",pPath,run_num));

c_md->SaveAs(Form("%s/Extensions/Macros/Parity/MainDet/%i_md.png",pPath,run_num));
c_bg->SaveAs(Form("%s/Extensions/Macros/Parity/MainDet/%i_bg.png",pPath,run_num));
c_lumi->SaveAs(Form("%s/Extensions/Macros/Parity/MainDet/%i_lumi.png",pPath,run_num));

string save_peds;

cout<<"If the pedestals look OK, type 'y' to make them available to the analyzer"<<endl;
cin>>save_peds;


if (save_peds=="y")
{
cout<<Form("moving files to %s/Parity/prminput",pPath)<<endl;
gSystem->Exec(Form("mv %s/Extensions/Macros/Parity/MainDet/qweak_maindet_pedestal.%i-.map %s/Parity/prminput/.",pPath,run_num,pPath));
gSystem->Exec(Form("mv %s/Extensions/Macros/Parity/MainDet/qweak_lumi_pedestal.%i-.map %s/Parity/prminput/.",pPath,run_num,pPath));
}

}



