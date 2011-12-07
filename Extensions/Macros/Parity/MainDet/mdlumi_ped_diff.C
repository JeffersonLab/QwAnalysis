void mdlumi_ped_diff(int run_num)
{

  const int mps_start = 0;
//  const int mps_stop = 1e9;
  const int mps_stop = 180e3;
  const bool get_md = kTRUE;
  const bool get_lumi = kTRUE;
  const bool save_file = kTRUE;

  float mean = 0.0;
  float scale = (20./(1<<18));
  gStyle->SetStatW(0.4); 
  gStyle->SetStatH(0.4);
  gStyle->SetOptStat("neMr");

  TChain chain("Hel_Tree");
//  chain.Add(Form("$QW_ROOTFILES/qwick_mdlumiped_%i.root",run_num));
//  chain.Add(Form("$QW_ROOTFILES/first100k_%i.root",run_num));
//  chain.Add(Form("$QW_ROOTFILES/Qweak_%i.000.trees.root",run_num));
  chain.Add(Form("$QW_ROOTFILES/diff_%i.000.trees.root",run_num));

  const char *lumi[16] = {
  "diff_qwk_dslumi1","diff_qwk_dslumi2","diff_qwk_dslumi3","diff_qwk_dslumi4",
  "diff_qwk_dslumi5","diff_qwk_dslumi6","diff_qwk_dslumi7","diff_qwk_dslumi8",
  "diff_qwk_uslumi1neg","diff_qwk_uslumi1pos","diff_qwk_uslumi3neg","diff_qwk_uslumi3pos",
  "diff_qwk_uslumi5neg","diff_qwk_uslumi5pos","diff_qwk_uslumi7neg","diff_qwk_uslumi7pos"};

  const char *md[16] = {
  "diff_qwk_md1neg","diff_qwk_md2neg","diff_qwk_md3neg","diff_qwk_md4neg",
  "diff_qwk_md5neg","diff_qwk_md6neg","diff_qwk_md7neg","diff_qwk_md8neg",
  "diff_qwk_md1pos","diff_qwk_md2pos","diff_qwk_md3pos","diff_qwk_md4pos",
  "diff_qwk_md5pos","diff_qwk_md6pos","diff_qwk_md7pos","diff_qwk_md8pos"};
  
  const char *bg[8] = {
  "diff_qwk_pmtonl","diff_qwk_pmtltg","diff_qwk_md9neg","diff_qwk_md9pos",
  "diff_qwk_pmtled","diff_qwk_isourc","diff_qwk_preamp","diff_qwk_cagesr"};
  

  char * pPath = "";
  if (save_file == kTRUE) pPath = getenv("QWSCRATCH");

  if (get_md == kTRUE)
    {      
/*      ofstream md_pedestal_file;
      md_pedestal_file.open(Form("%s/calib/qweak_maindet_pedestal.%i-.map",pPath,run_num));
      md_pedestal_file<<endl;
      md_pedestal_file<<Form("!The following pedestals were recorded from RUN %i",run_num)<<endl;
      md_pedestal_file<<"!channel name , Mps channelname.hw_sum_raw/num_samples , gain"<<endl;
*/
      
      TCanvas *c_md = new TCanvas("md","md",1500,1100);
      c_md->Divide(4,4);
      TH1F *mdhst[16];
      for (int i=0;i<16;i++)
        {
          c_md->cd(i+1);      
          mdhst[i] = new TH1F(Form("%h_%s",md[i]),"",100,0,0);
          mdhst[i]->SetDirectory(0);   
          chain.Draw( Form("%s>>h_%s",md[i],md[i]),Form("%s.Device_Error_Code==0 && mps_counter>%i && mps_counter<%i",md[i],mps_start,mps_stop),"");
          TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("h_%s",md[i]));
//          md_pedestal_file<<Form("%s  ,  ",md[i])<<htemp->GetMean()/(scale*1000)<<Form("  ,  %10.8f",scale)<<endl;  
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
          chain.Draw( Form("%s>>h_%s",bg[i],bg[i]),Form("%s.Device_Error_Code==0 && mps_counter>%i && mps_counter<%i",bg[i],mps_start,mps_stop),"");
          TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("h_%s",bg[i]));
          (if i<4){
//		md_pedestal_file<<Form("%s  ,  ",bg[i])<<htemp->GetMean()/(scale*1000)<<Form("  ,  %10.8f",scale)<<endl;
           }
          cout<<bg[i]<<"  "<<htemp->GetMean()<<endl;
          c_bg->Update();
        }
/*      md_pedestal_file<<"qwk_pmtled  ,  0  ,  0.00007629"<<endl;
      md_pedestal_file<<"qwk_isourc  ,  0  ,  0.00007629"<<endl;
      md_pedestal_file<<"qwk_preamp  ,  0  ,  0.00007629"<<endl;
      md_pedestal_file<<"qwk_cagesr  ,  0  ,  0.00007629"<<endl;
     
      md_pedestal_file.close();
*/
     }; //end get md

  if (get_lumi == kTRUE)
    { 
/*      ofstream lumi_pedestal_file;
      lumi_pedestal_file.open(Form("%s/calib/qweak_lumi_pedestal.%i-.map",pPath,run_num));
      lumi_pedestal_file<<endl;
      lumi_pedestal_file<<Form("!The following pedestals were recorded from RUN %i",run_num)<<endl;
      lumi_pedestal_file<<"!channel name , Mps channelname.hw_sum_raw/num_samples , gain"<<endl;
*/
      TCanvas *c_lumi = new TCanvas("lumi","lumi",1500,1100);
      c_lumi->Divide(4,4);
      TH1F *lumihst[16];
      for (int i=0;i<16;i++)
        {  
          c_lumi->cd(i+1);      
          lumihst[i] = new TH1F(Form("%h_%s",lumi[i]),"",100,0,0);
          lumihst[i]->SetDirectory(0);   
          chain.Draw( Form("%s>>h_%s",lumi[i],lumi[i]),Form("%s.Device_Error_Code==0 && mps_counter>%i && mps_counter<%i",lumi[i],mps_start,mps_stop),"");
          TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("h_%s",lumi[i]));
//          lumi_pedestal_file<<Form("%s  ,  ",lumi[i])<<htemp->GetMean()/(scale*1000)<<Form("  ,  %10.8f",scale)<<endl;  
          cout<<lumi[i]<<"  "<<htemp->GetMean()<<endl;
          c_lumi->Update();
        }
//     lumi_pedestal_file.close();
     }; // end get lumi  
     
/*
c_md->SaveAs(Form("%s/calib/pedPlots/%i_md.png",pPath,run_num));
c_bg->SaveAs(Form("%s/calib/pedPlots/%i_bg.png",pPath,run_num));
c_lumi->SaveAs(Form("%s/calib/pedPlots/%i_lumi.png",pPath,run_num));
*/

}





