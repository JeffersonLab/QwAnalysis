<<<<<<< .mine
void mdlumi_ped(string run, int run_num, int mps_start, int mps_stop, bool get_md, bool get_lumi)
=======
void mdlumi_ped(int run_num =8358,int mps_start =0, int mps_stop=20e3, bool get_md=kTRUE, bool get_lumi=kTRUE)
>>>>>>> .r2393
{
  TChain chain("Mps_Tree");
<<<<<<< .mine
  chain.Add(Form("$QW_ROOTFILES/%s",run));

=======
  chain.Add(Form("$QW_ROOTFILES/QwPass1_%i.*.root",run_num));

>>>>>>> .r2393
  const string lumi[16] = {
  "qwk_dslumi1","qwk_dslumi2","qwk_dslumi3","qwk_dslumi4",
  "qwk_dslumi5","qwk_dslumi6","qwk_dslumi7","qwk_dslumi8",
  "qwk_uslumi1neg","qwk_uslumi1pos","qwk_uslumi3neg","qwk_uslumi3pos",
  "qwk_uslumi5neg","qwk_uslumi5pos","qwk_uslumi7neg","qwk_uslumi7pos"};

  const string md[24] ={
  "qwk_md1neg","qwk_md2neg","qwk_md3neg","qwk_md4neg",
  "qwk_md5neg","qwk_md6neg","qwk_md7neg","qwk_md8neg",
  "qwk_md1pos","qwk_md2pos","qwk_md3pos","qwk_md4pos",
  "qwk_md5pos","qwk_md6pos","qwk_md7pos","qwk_md8pos",
  "qwk_pmtled","qwk_pmtonl","qwk_pmtltg","qwk_md9neg",
  "qwk_md9pos","qwk_isourc","qwk_preamp","qwk_cagesr"};
  
  const int n = 24;

  Bool_t save_file = kTRUE;
  //Int_t mps_stop = mps_start + 10000;
  char * pPath = "";
  if (save_file == kTRUE) pPath = getenv("QWANALYSIS");
  //printf ("The current save path is: %s",pPath);
   
  if (get_md == kTRUE)
    {      
      ofstream md_pedestal_file;
      md_pedestal_file.open(Form("%s/Parity/prminput/qweak_maindet_pedestal.%i-.map",pPath,run_num));
      md_pedestal_file<<endl;
      md_pedestal_file<<Form("!The following pedestals were recorded from RUN %i",run_num)<<endl;
      md_pedestal_file<<"!channel name , Mps channelname.hw_sum_raw/num_samples , gain"<<endl;
      //TCanvas *c_md = new TCanvas("md","md",1500,1100);
      //c_md->Divide(4,4);
      TH1F *mdhst[n];
      for (int i=0;i<n;i++)
        {
          //c_md->cd(i+1);      
          mdhst[i] = new TH1F(Form("%h_%s",md[i]),"",100,0,0);
          mdhst[i]->SetDirectory(0);   
          chain.Draw( Form("%s.hw_sum_raw/%s.num_samples>>h_%s",md[i],md[i],md[i]),Form("%s.num_samples!=0 && mps_counter>%i && mps_counter<%i",md[i],mps_start,mps_stop),"");
          TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("h_%s",md[i]));
          md_pedestal_file<<Form("%s  ,  ",md[i])<<htemp->GetMean()<<"  ,  0.00007692"<<endl;  
          cout<<md[i]<<"  "<<htemp->GetMean()<<endl;
          if (i==n-1) md_pedestal_file.close();
        }
     }; //end get md

  if (get_lumi == kTRUE)
    {      
      ofstream lumi_pedestal_file;
      lumi_pedestal_file.open(Form("%s/Parity/prminput/qweak_lumi_pedestal.%i-.map",pPath,run_num));
      lumi_pedestal_file<<endl;
      lumi_pedestal_file<<Form("!The following pedestals were recorded from RUN %i",run_num)<<endl;
      lumi_pedestal_file<<"!channel name , Mps channelname.hw_sum_raw/num_samples , gain"<<endl;
      //TCanvas *c_lumi = new TCanvas("lumi","lumi",1500,1100);
      //c_lumi->Divide(4,4);
      TH1F *lumihst[16];
      for (int i=0;i<16;i++)
        {
          //c_lumi->cd(i+1);      
          lumihst[i] = new TH1F(Form("%h_%s",lumi[i]),"",100,0,0);
          lumihst[i]->SetDirectory(0);   
          chain.Draw( Form("%s.hw_sum_raw/%s.num_samples>>h_%s",lumi[i],lumi[i],lumi[i]),Form("%s.num_samples!=0 && mps_counter>%i && mps_counter<%i",lumi[i],mps_start,mps_stop),"");
          TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("h_%s",lumi[i]));
          lumi_pedestal_file<<Form("%s  ,  ",lumi[i])<<htemp->GetMean()<<"  ,  0.00007692"<<endl;  
          cout<<lumi[i]<<"  "<<htemp->GetMean()<<endl;
          if (i==15) lumi_pedestal_file.close();
        }
     }; // end get lumi  

}



