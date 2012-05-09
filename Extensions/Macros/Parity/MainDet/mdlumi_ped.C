void mdlumi_ped(int run_num)
{

  const int mps_start = 0;
  const int mps_stop = 1e9;
//  const int mps_stop = 180e3;
  const bool get_md = kTRUE;
  const bool get_lumi = kTRUE;
  const bool save_file = kTRUE;

  float mean = 0.0;
  float scale = (20./(1<<18));
  gStyle->SetStatW(0.4); 
  gStyle->SetStatH(0.4);
  gStyle->SetOptStat("neMr");

  TChain chain("Mps_Tree");
  chain.Add(Form("$QW_ROOTFILES/qwick_mdlumiped_%i.trees.root",run_num));
//  chain.Add(Form("$QW_ROOTFILES/first100k_%i.root",run_num));
//  chain.Add(Form("$QW_ROOTFILES/Qweak_%i.000.trees.root",run_num));

  const TString lumi[16] = {
  "qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4",
  "qwk_dslumi5","qwk_dslumi6","qwk_dslumi7","qwk_dslumi8",
  "qwk_uslumi1neg","qwk_uslumi1pos","qwk_uslumi3neg","qwk_uslumi3pos",
  "qwk_uslumi5neg","qwk_uslumi5pos","qwk_uslumi7neg","qwk_uslumi7pos"};

  const TString md[16] = {
  "qwk_md1neg","qwk_md2neg","qwk_md3neg","qwk_md4neg",
  "qwk_md5neg","qwk_md6neg","qwk_md7neg","qwk_md8neg",
  "qwk_md1pos","qwk_md2pos","qwk_md3pos","qwk_md4pos",
  "qwk_md5pos","qwk_md6pos","qwk_md7pos","qwk_md8pos"};
  
  const TString bg[8] = {
  "qwk_pmtonl","qwk_pmtltg","qwk_md9neg","qwk_md9pos",
  "qwk_pmtled","qwk_isourc","qwk_preamp","qwk_cagesr"};
  

  char * pPath = "";
  if (save_file == kTRUE) pPath = getenv("QWSCRATCH");

  if (get_md == kTRUE)
    {      
      ofstream md_pedestal_file;
      md_pedestal_file.open(Form("%s/calib/qweak_maindet_pedestal.%i-.map",pPath,run_num));
      md_pedestal_file<<endl;
      md_pedestal_file<<Form("!The following pedestals were recorded from RUN %i",run_num)<<endl;
      md_pedestal_file<<"!channel name , Mps channelname.hw_sum_raw/num_samples , gain"<<endl;

      
      TCanvas *c_md = new TCanvas("md","md",1500,1100);
      c_md->Divide(4,4);
      TH1F *mdhst[16];
      for (int i=0;i<16;i++)
        {
          c_md->cd(i+1);      
          mdhst[i] = new TH1F(Form("%h_%s",md[i].Data()),"",100,0,0);
          mdhst[i]->SetDirectory(0);   
          chain.Draw( Form("%s.hw_sum_raw/%s.num_samples*%f*1000>>h_%s",md[i].Data(),md[i].Data(),scale,md[i].Data()),Form("qwk_charge<1 && %s.Device_Error_Code==0 && mps_counter>%i && mps_counter<%i",md[i].Data(),mps_start,mps_stop),"");
          TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("h_%s",md[i].Data()));
          md_pedestal_file<<Form("%s  ,  ",md[i].Data())<<htemp->GetMean()/(scale*1000)<<Form("  ,  %10.8f",scale)<<endl;  
          cout<<md[i].Data()<<"  "<<htemp->GetMean()<<endl;
          c_md->Update();
        }
      TCanvas *c_bg = new TCanvas("bg","bg",1500,1100);
      c_bg->Divide(3,3);
      TH1F *bghst[8];
      for (int i=0;i<8;i++)
        {
          c_bg->cd(i+1);      
          bghst[i] = new TH1F(Form("%h_%s",bg[i].Data()),"",100,0,0);
          bghst[i]->SetDirectory(0);   
          chain.Draw( Form("%s.hw_sum_raw/%s.num_samples*%f*1000>>h_%s",bg[i].Data(),bg[i].Data(),scale,bg[i].Data()),Form("qwk_charge<1 && %s.Device_Error_Code==0 && mps_counter>%i && mps_counter<%i",bg[i].Data(),mps_start,mps_stop),"");
          TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("h_%s",bg[i].Data()));
          if (i<4) md_pedestal_file<<Form("%s  ,  ",bg[i].Data())<<htemp->GetMean()/(scale*1000)<<Form("  ,  %10.8f",scale)<<endl;
          cout<<bg[i].Data()<<"  "<<htemp->GetMean()<<endl;
          c_bg->Update();
        }
      md_pedestal_file<<"qwk_pmtled  ,  0  ,  0.00007629"<<endl;
      md_pedestal_file<<"qwk_isourc  ,  0  ,  0.00007629"<<endl;
      md_pedestal_file<<"qwk_preamp  ,  0  ,  0.00007629"<<endl;
      md_pedestal_file<<"qwk_cagesr  ,  0  ,  0.00007629"<<endl;
     
      md_pedestal_file.close();
     }; //end get md

  if (get_lumi == kTRUE)
    { 
      ofstream lumi_pedestal_file;
      lumi_pedestal_file.open(Form("%s/calib/qweak_lumi_pedestal.%i-.map",pPath,run_num));
      lumi_pedestal_file<<endl;
      lumi_pedestal_file<<Form("!The following pedestals were recorded from RUN %i",run_num)<<endl;
      lumi_pedestal_file<<"!channel name , Mps channelname.hw_sum_raw/num_samples , gain"<<endl;
      TCanvas *c_lumi = new TCanvas("lumi","lumi",1500,1100);
      c_lumi->Divide(4,4);
      TH1F *lumihst[16];
      for (int i=0;i<16;i++)
        {  
	  //  Try to get the branch.
	  //  If this returns zero, the name isn't recognized in the tree,
	  //  so skip that detector.
	  if (chain.GetBranch(lumi[i])==0) continue;
          c_lumi->cd(i+1);      
          lumihst[i] = new TH1F(Form("%h_%s",lumi[i].Data()),"",100,0,0);
          lumihst[i]->SetDirectory(0);   
	  chain.Draw( Form("%s.hw_sum_raw/%s.num_samples*%f*1000>>h_%s",lumi[i].Data(),lumi[i].Data(),scale,lumi[i].Data()),Form("qwk_charge<1 && %s.Device_Error_Code==0 && mps_counter>%i && mps_counter<%i",lumi[i].Data(),mps_start,mps_stop),"");
          TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("h_%s",lumi[i].Data()));
          lumi_pedestal_file<<Form("%s  ,  ",lumi[i].Data())<<htemp->GetMean()/(scale*1000)<<Form("  ,  %10.8f",scale)<<endl;  
          cout<<lumi[i].Data()<<"  "<<htemp->GetMean()<<endl;
          c_lumi->Update();
        }
     lumi_pedestal_file.close();
     }; // end get lumi  
     

c_md->SaveAs(Form("%s/calib/pedPlots/%i_md.png",pPath,run_num));
c_bg->SaveAs(Form("%s/calib/pedPlots/%i_bg.png",pPath,run_num));
c_lumi->SaveAs(Form("%s/calib/pedPlots/%i_lumi.png",pPath,run_num));


}





