{
// A. Vacheret 21/03/2003.
// to run this macro you need a runlist.dat file made with a UNIX script which
// go through the halog entries to get start of run infos.
// you can use a rejectrun.dat file to remove by hand .res files from the analysis.
// This macro generate a root file with a "run" tree.  
//#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
Int_t n = 1000;
TFile *resout;
 resout = new TFile("result_feedback.root","RECREATE","Massive run analysis TTree");
//  cout<<" All the runs MUST be of the same type of analysis, if you want to reject files,\n"<<
//        "If no reject file created all the runs are processed."<<endl;

// will add other EPICS variables in the future.
Int_t fdbkHAon,pfdbkon;
Int_t bhwp,rhwp,iaslope;
Int_t rnum1,rnum2;
Int_t runnum,runidx;
UInt_t  asybattNev[2],asybcmNev[4],diffbpm12Nev[2],diffbpm10Nev[2];
Int_t diffbattNev[2],diffbpm8Nev[2],diffbpm4ANev[2],diffbpm4BNev[2],asyNlumiNev[3];
Double_t pitadac,iadac,pztAxdac,pztAydac,pztxdac,pztydac,bcm1amp,npair;
Double_t bcmcurrent[2],bcmcurrenter[2];
Double_t bcmcurrentRMS[2],bcmcurrentRMSer[2];
Double_t asybattmean[2],asybattmeaner[2];
Double_t asybattRMS[2],asybattRMSer[2];
Double_t asybcmmean[4],asybcmmeaner[4];
Double_t asybcmRMS[4],asybcmRMSer[4];
Double_t diffbattmean[2],diffbattmeaner[2];
Double_t diffbattRMS[2],diffbattRMSer[2];
Double_t diffbpm12mean[2],diffbpm12meaner[2],diffbpm12RMS[2],diffbpm12RMSer[2];
Double_t diffbpm10mean[2],diffbpm10meaner[2],diffbpm10RMS[2],diffbpm10RMSer[2];
Double_t diffbpm8mean[2],diffbpm8meaner[2],diffbpm8RMS[2],diffbpm8RMSer[2];
Double_t diffbpm4Amean[2],diffbpm4Ameaner[2],diffbpm4ARMS[2],diffbpm4ARMSer[2];
Double_t diffbpm4Bmean[2],diffbpm4Bmeaner[2],diffbpm4BRMS[2],diffbpm4BRMSer[2];
Double_t asyNlumimean[3],asyNlumimeaner[3];
Double_t asyNlumiRMS[3],asyNlumiRMSer[3];

Double_t difftheta[2],thetat[2],bcmresol;

// RMS weighted sums for accummulation plots  
Double_t asybcmmeansum[4],asybcmmeansuma[4],asybcmmeansume[4],diffbpm12meansum[2],diffbpm12meansuma[2];
Double_t diffbpm10meansum[2],diffbpm10meansuma[2],diffbpm10meansume[2],diffbpm8meansum[2],diffbpm8meansuma[2];
Double_t diffbpm4Ameansum[2],diffbpm4Ameansuma[2],diffbpm4Ameansume[2],diffbpm4Bmeansum[2],diffbpm4Bmeansuma[2],diffbpm4Bmeansume[2];


//Int_t runlist[n],bhwplist[n],rhwplist[n],IAslopelist[n];
Int_t arrayidx = 0;
//Bool_t fdbkHAonlist[n],pfdbkonlist[n];
//Double_t pitadaclist[n],iadaclist[n],pztAxdaclist[n],pztAydaclist[n],pztxdaclist[n],pztydaclist[n];

 TTree *ia;
 ia = new TTree("IA","TREE Runs ");

 ia->Branch ("run", &runnum, "run/I", 5000); 
 ia->Branch ("bhwp", &bhwp, "bhwp/I", 5000); 
 ia->Branch ("rhwp", &rhwp, "rwhp/I", 5000); 
 ia->Branch ("pitadac", &pitadac, "pitadac/I", 5000); 
 ia->Branch ("iadac", &iadac, "iadac/D", 5000); 
 ia->Branch ("pztAxdac", &pztAxdac, "pztAxdac/D", 5000); 
 ia->Branch ("pztAydac", &pztAydac, "pztAydac/D", 5000); 
 ia->Branch ("pztxdac", &pztxdac, "pztxdac/D", 5000); 
 ia->Branch ("pztydac", &pztydac, "pztydac/D", 5000); 
 ia->Branch ("iaslope", &iaslope, "iaslope/I", 5000);
// ia->Branch ("fdbkHAon", &fdbkHAon, "fdbkHAon/i", 5000);
 ia->Branch ("pfdbkon", &pfdbkon, "pfdbkon/i", 5000);
 ia->Branch ("bcm1amp", &bcm1amp, "bcm1amp/D", 5000);
 
 ia->Branch ("bcm1cur", &bcmcurrent[0], "bcm1cur/D", 5000); 
 ia->Branch ("bcm2cur", &bcmcurrent[1], "bcm2cur/D", 5000); 
 ia->Branch ("bcm1curer", &bcmcurrenter[0], "bcm1curer/D", 5000); 
 ia->Branch ("bcm2curer", &bcmcurrenter[1], "bcm2curer/D", 5000); 
 ia->Branch ("bcm1curRMS", &bcmcurrentRMS[0], "bcm1curRMS/D", 5000); 
 ia->Branch ("bcm2curRMS", &bcmcurrentRMS[1], "bcm2curRMS/D", 5000); 
 ia->Branch ("bcm1curRMSer", &bcmcurrentRMSer[0], "bcm1curRMSer/D", 5000); 
 ia->Branch ("bcm2curRMSer", &bcmcurrentRMSer[1], "bcm2curRMSer/D", 5000); 
 ia->Branch ("bcmresol", &bcmresol, "bcmresol/D", 5000); 

 ia->Branch ("diffbatt1mean", &diffbattmean[0], "diffbatt1mean/D", 5000); 
 ia->Branch ("diffbatt2mean", &diffbattmean[1], "diffbatt2mean/D", 5000); 
 ia->Branch ("diffbatt1meaner", &diffbattmeaner[0], "diffbatt1meaner/D", 5000); 
 ia->Branch ("diffbatt2meaner", &diffbattmeaner[1], "diffbatt2meaner/D", 5000); 
 ia->Branch ("diffbatt1RMS", &diffbattRMS[0], "diffbatt1RMS/D", 5000); 
 ia->Branch ("diffbatt2RMS", &diffbattRMS[1], "diffbatt2RMS/D", 5000); 
 ia->Branch ("diffbatt1RMSer", &diffbattRMSer[0], "diffbatt1RMSer/D", 5000); 
 ia->Branch ("diffbatt2RMSer", &diffbattRMSer[1], "diffbatt2RMSer/D", 5000); 
 ia->Branch ("diffbatt1Nev", &diffbattNev[0], "diffbatt1Nev/I", 5000); 
 ia->Branch ("diffbatt2Nev", &diffbattNev[1], "diffbatt2Nev/I", 5000); 
 ia->Branch ("diffbpm12xmean", &diffbpm12mean[0], "diffbpm12xmean/D", 5000); 
 ia->Branch ("diffbpm12xmeaner", &diffbpm12meaner[0], "diffbpm12xmeaner/D", 5000); 
 ia->Branch ("diffbpm12xRMS", &diffbpm12RMS[0], "diffbpm12xRMS/D", 5000); 
 ia->Branch ("diffbpm12xRMSer", &diffbpm12RMSer[0], "diffbpm12xRMSer/D", 5000); 
 ia->Branch ("diffbpm12xNev", &diffbpm12Nev[0], "diffbpm12xNev/I", 5000); 
 ia->Branch ("diffbpm12ymean", &diffbpm12mean[1], "diffbpm12ymean/D", 5000); 
 ia->Branch ("diffbpm12ymeaner", &diffbpm12meaner[1], "diffbpm12ymeaner/D", 5000); 
 ia->Branch ("diffbpm12yRMS", &diffbpm12RMS[1], "diffbpm12yRMS/D", 5000); 
 ia->Branch ("diffbpm12yRMSer", &diffbpm12RMSer[1], "diffbpm12yRMSer/D", 5000); 
 ia->Branch ("diffbpm12yNev", &diffbpm12Nev[1], "diffbpm12yNev/I", 5000); 
 ia->Branch ("diffbpm10xmean", &diffbpm10mean[0], "diffbpm10xmean/D", 5000); 
 ia->Branch ("diffbpm10xmeaner", &diffbpm10meaner[0], "diffbpm10xmeaner/D", 5000); 
 ia->Branch ("diffbpm10xRMS", &diffbpm10RMS[0], "diffbpm10xRMS/D", 5000); 
 ia->Branch ("diffbpm10xRMSer", &diffbpm10RMSer[0], "diffbpm10xRMSer/D", 5000); 
 ia->Branch ("diffbpm10xNev", &diffbpm10Nev[0], "diffbpm10xNev/I", 5000); 
 ia->Branch ("diffbpm10ymean", &diffbpm10mean[1], "diffbpm10ymean/D", 5000); 
 ia->Branch ("diffbpm10ymeaner", &diffbpm10meaner[1], "diffbpm10ymeaner/D", 5000); 
 ia->Branch ("diffbpm10yRMS", &diffbpm10RMS[1], "diffbpm10yRMS/D", 5000); 
 ia->Branch ("diffbpm10yRMSer", &diffbpm10RMSer[1], "diffbpm10yRMSer/D", 5000); 
 ia->Branch ("diffbpm10yNev", &diffbpm10Nev[1], "diffbpm10yNev/I", 5000); 
 ia->Branch ("diffbpm8xmean", &diffbpm8mean[0], "diffbpm8xmean/D", 5000); 
 ia->Branch ("diffbpm8xmeaner", &diffbpm8meaner[0], "diffbpm8xmeaner/D", 5000); 
 ia->Branch ("diffbpm8xRMS", &diffbpm8RMS[0], "diffbpm8xRMS/D", 5000); 
 ia->Branch ("diffbpm8xRMSer", &diffbpm8RMSer[0], "diffbpm8xRMSer/D", 5000); 
 ia->Branch ("diffbpm8xNev", &diffbpm8Nev[0], "diffbpm8xNev/I", 5000); 
 ia->Branch ("diffbpm8ymean", &diffbpm8mean[1], "diffbpm8ymean/D", 5000); 
 ia->Branch ("diffbpm8ymeaner", &diffbpm8meaner[1], "diffbpm8ymeaner/D", 5000); 
 ia->Branch ("diffbpm8yRMS", &diffbpm8RMS[1], "diffbpm8yRMS/D", 5000); 
 ia->Branch ("diffbpm8yRMSer", &diffbpm8RMSer[1], "diffbpm8yRMSer/D", 5000); 
 ia->Branch ("diffbpm8yNev", &diffbpm8Nev[1], "diffbpm8yNev/I", 5000); 
 ia->Branch ("diffbpm4Axmean", &diffbpm4Amean[0], "diffbpm4Axmean/D", 5000); 
 ia->Branch ("diffbpm4Axmeaner", &diffbpm4Ameaner[0], "diffbpm4Axmeaner/D", 5000); 
 ia->Branch ("diffbpm4AxRMS", &diffbpm4ARMS[0], "diffbpm4AxRMS/D", 5000); 
 ia->Branch ("diffbpm4AxRMSer", &diffbpm4ARMSer[0], "diffbpm4AxRMSer/D", 5000); 
 ia->Branch ("diffbpm4AxNev", &diffbpm4ANev[0], "diffbpm4AxNev/I", 5000); 
 ia->Branch ("diffbpm4Aymean", &diffbpm4Amean[1], "diffbpm4Aymean/D", 5000); 
 ia->Branch ("diffbpm4Aymeaner", &diffbpm4Ameaner[1], "diffbpm4Aymeaner/D", 5000); 
 ia->Branch ("diffbpm4AyRMS", &diffbpm4ARMS[1], "diffbpm4AyRMS/D", 5000); 
 ia->Branch ("diffbpm4AyRMSer", &diffbpm4ARMSer[1], "diffbpm4AyRMSer/D", 5000); 
 ia->Branch ("diffbpm4AyNev", &diffbpm4ANev[1], "diffbpm4AyNev/I", 5000); 
 ia->Branch ("diffbpm4Bxmean", &diffbpm4Bmean[0], "diffbpm4Bxmean/D", 5000); 
 ia->Branch ("diffbpm4Bxmeaner", &diffbpm4Bmeaner[0], "diffbpm4Bxmeaner/D", 5000); 
 ia->Branch ("diffbpm4BxRMS", &diffbpm4BRMS[0], "diffbpm4BxRMS/D", 5000); 
 ia->Branch ("diffbpm4BxRMSer", &diffbpm4BRMSer[0], "diffbpm4BxRMSer/D", 5000); 
 ia->Branch ("diffbpm4BxNev", &diffbpm4BNev[0], "diffbpm4BxNev/I", 5000); 
 ia->Branch ("diffbpm4Bymean", &diffbpm4Bmean[1], "diffbpm4Bymean/D", 5000); 
 ia->Branch ("diffbpm4Bymeaner", &diffbpm4Bmeaner[1], "diffbpm4Bymeaner/D", 5000); 
 ia->Branch ("diffbpm4ByRMS", &diffbpm4BRMS[1], "diffbpm4ByRMS/D", 5000); 
 ia->Branch ("diffbpm4ByRMSer", &diffbpm4BRMSer[1], "diffbpm4ByRMSer/D", 5000); 
 ia->Branch ("diffbpm4ByNev", &diffbpm4BNev[1], "diffbpm4ByNev/I", 5000); 
 ia->Branch ("diffthetax", &difftheta[0], "diffthetax/D", 5000);
 ia->Branch ("diffthetay", &difftheta[1], "diffthetay/D", 5000);
 
 ia->Branch ("asyNlumi1mean", &asyNlumimean[0], "asyNlumi1mean/D", 5000); 
 ia->Branch ("asyNlumi2mean", &asyNlumimean[1], "asyNlumi2mean/D", 5000); 
 ia->Branch ("asyNlumi3mean", &asyNlumimean[2], "asyNlumi3mean/D", 5000); 
 ia->Branch ("asyNlumi1meaner", &asyNlumimeaner[0], "asyNlumi1meaner/D", 5000); 
 ia->Branch ("asyNlumi2meaner", &asyNlumimeaner[1], "asyNlumi2meaner/D", 5000); 
 ia->Branch ("asyNlumi3meaner", &asyNlumimeaner[2], "asyNlumi3meaner/D", 5000); 
 ia->Branch ("asyNlumi1RMS", &asyNlumiRMS[0], "asyNlumi1RMS/D", 5000); 
 ia->Branch ("asyNlumi2RMS", &asyNlumiRMS[1], "asyNlumi2RMS/D", 5000); 
 ia->Branch ("asyNlumi3RMS", &asyNlumiRMS[2], "asyNlumi3RMS/D", 5000); 
 ia->Branch ("asyNlumi1RMSer", &asyNlumiRMSer[0], "asyNlumi1RMSer/D", 5000); 
 ia->Branch ("asyNlumi2RMSer", &asyNlumiRMSer[1], "asyNlumi2RMSer/D", 5000); 
 ia->Branch ("asyNlumi3RMSer", &asyNlumiRMSer[2], "asyNlumi3RMSer/D", 5000); 
 ia->Branch ("asyNlumi1Nev", &asyNlumiNev[0], "asyNlumi1Nev/I", 5000); 
 ia->Branch ("asyNlumi2Nev", &asyNlumiNev[1], "asyNlumi2Nev/I", 5000); 
 ia->Branch ("asyNlumi3Nev", &asyNlumiNev[2], "asyNlumi3Nev/I", 5000); 
 ia->Branch ("asybatt1mean", &asybattmean[0], "asybatt1mean/D", 5000); 
 ia->Branch ("asybatt2mean", &asybattmean[1], "asybatt2mean/D", 5000); 
 ia->Branch ("asybatt1meaner", &asybattmeaner[0], "asybatt1meaner/D", 5000); 
 ia->Branch ("asybatt2meaner", &asybattmeaner[1], "asybatt2meaner/D", 5000); 
 ia->Branch ("asybatt1RMS", &asybattRMS[0], "asybatt1RMS/D", 5000); 
 ia->Branch ("asybatt2RMS", &asybattRMS[1], "asybatt2RMS/D", 5000); 
 ia->Branch ("asybatt1RMSer", &asybattRMSer[0], "asybatt1RMSer/D", 5000); 
 ia->Branch ("asybatt2RMSer", &asybattRMSer[1], "asybatt2RMSer/D", 5000); 
 ia->Branch ("asybatt1Nev", &asybattNev[0], "asybatt1Nev/I", 5000); 
 ia->Branch ("asybatt2Nev", &asybattNev[1], "asybatt2Nev/I", 5000); 

 ia->Branch ("asybcm1mean", &asybcmmean[0], "asybcm1mean/D", 5000); 
 ia->Branch ("asybcm2mean", &asybcmmean[1], "asybcm2mean/D", 5000); 
 ia->Branch ("asybcm3mean", &asybcmmean[2], "asybcm3mean/D", 5000); 
 ia->Branch ("asybcm4mean", &asybcmmean[3], "asybcm4mean/D", 5000); 
 ia->Branch ("asybcm1meaner", &asybcmmeaner[0], "asybcm1meaner/D", 5000); 
 ia->Branch ("asybcm2meaner", &asybcmmeaner[1], "asybcm2meaner/D", 5000); 
 ia->Branch ("asybcm3meaner", &asybcmmeaner[2], "asybcm3meaner/D", 5000); 
 ia->Branch ("asybcm4meaner", &asybcmmeaner[3], "asybcm4meaner/D", 5000); 
 ia->Branch ("asybcm1RMS", &asybcmRMS[0], "asybcm1RMS/D", 5000); 
 ia->Branch ("asybcm2RMS", &asybcmRMS[1], "asybcm2RMS/D", 5000); 
 ia->Branch ("asybcm3RMS", &asybcmRMS[2], "asybcm3RMS/D", 5000); 
 ia->Branch ("asybcm4RMS", &asybcmRMS[3], "asybcm4RMS/D", 5000); 
 ia->Branch ("asybcm1RMSer", &asybcmRMSer[0], "asybcm1RMSer/D", 5000); 
 ia->Branch ("asybcm2RMSer", &asybcmRMSer[1], "asybcm2RMSer/D", 5000); 
 ia->Branch ("asybcm3RMSer", &asybcmRMSer[2], "asybcm3RMSer/D", 5000); 
 ia->Branch ("asybcm4RMSer", &asybcmRMSer[3], "asybcm4RMSer/D", 5000); 
 ia->Branch ("asybcm1Nev", &asybcmNev[0], "asybcm1Nev/I", 5000); 
 ia->Branch ("asybcm2Nev", &asybcmNev[1], "asybcm2Nev/I", 5000); 
 ia->Branch ("asybcm3Nev", &asybcmNev[2], "asybcm3Nev/I", 5000); 
 ia->Branch ("asybcm4Nev", &asybcmNev[3], "asybcm4Nev/I", 5000); 

 ia->Branch ("npair", &npair,"npair/I", 5000); 
 ia->Branch ("asybcm1sum", &asybcmmeansum[0], "asybcm1sum/D", 5000); 
 ia->Branch ("asybcm2sum", &asybcmmeansum[1], "asybcm2sum/D", 5000); 
 ia->Branch ("asybcm3sum", &asybcmmeansum[2], "asybcm3sum/D", 5000); 
 ia->Branch ("asybcm4sum", &asybcmmeansum[3], "asybcm4sum/D", 5000);
 ia->Branch ("asybcm1suma", &asybcmmeansuma[0], "asybcm1suma/D", 5000); 
 ia->Branch ("asybcm2suma", &asybcmmeansuma[1], "asybcm2suma/D", 5000); 
 ia->Branch ("asybcm3suma", &asybcmmeansuma[2], "asybcm3suma/D", 5000); 
 ia->Branch ("asybcm4suma", &asybcmmeansuma[3], "asybcm4suma/D", 5000);
 ia->Branch ("asybcm1sume", &asybcmmeansume[0], "asybcm1sume/D", 5000); 
 ia->Branch ("asybcm2sume", &asybcmmeansume[1], "asybcm2sume/D", 5000); 
 ia->Branch ("asybcm3sume", &asybcmmeansume[2], "asybcm3sume/D", 5000); 
 ia->Branch ("asybcm4sume", &asybcmmeansume[3], "asybcm4sume/D", 5000);
 
 ia->Branch ("diffbpm12xmeansum", &diffbpm12meansum[0], "diffbpm12xmeansum/D", 5000); 
 ia->Branch ("diffbpm12xmeansuma", &diffbpm12meansuma[0], "diffbpm12xmeansuma/D", 5000); 
 ia->Branch ("diffbpm12ymeansum", &diffbpm12meansum[1], "diffbpm12ymeansum/D", 5000); 
 ia->Branch ("diffbpm12ymeansuma", &diffbpm12meansuma[1], "diffbpm12ymeansuma/D", 5000); 

 ia->Branch ("diffbpm10xmeansum", &diffbpm10meansum[0], "diffbpm10xmeansum/D", 5000); 
 ia->Branch ("diffbpm10xmeansuma", &diffbpm10meansuma[0], "diffbpm10xmeansuma/D", 5000); 
 ia->Branch ("diffbpm10xmeansume", &diffbpm10meansume[0], "diffbpm10xmeansume/D", 5000); 
 ia->Branch ("diffbpm10ymeansum", &diffbpm10meansum[1], "diffbpm10ymeansum/D", 5000); 
 ia->Branch ("diffbpm10ymeansuma", &diffbpm10meansuma[1], "diffbpm10ymeansuma/D", 5000); 
 ia->Branch ("diffbpm10ymeansume", &diffbpm10meansume[1], "diffbpm10ymeansume/D", 5000); 

 ia->Branch ("diffbpm8xmeansum", &diffbpm8meansum[0], "diffbpm8xmeansum/D", 5000); 
 ia->Branch ("diffbpm8xmeansuma", &diffbpm8meansuma[0], "diffbpm8xmeansuma/D", 5000); 
 ia->Branch ("diffbpm8ymeansum", &diffbpm8meansum[1], "diffbpm8ymeansum/D", 5000); 
 ia->Branch ("diffbpm8ymeansuma", &diffbpm8meansuma[1], "diffbpm8ymeansuma/D", 5000); 

 ia->Branch ("diffbpm4Axmeansum", &diffbpm4Ameansum[0], "diffbpm4Axmeansum/D", 5000); 
 ia->Branch ("diffbpm4Axmeansuma", &diffbpm4Ameansuma[0], "diffbpm4Axmeansuma/D", 5000); 
 ia->Branch ("diffbpm4Axmeansume", &diffbpm4Ameansume[0], "diffbpm4AxmeansumeD", 5000); 
 ia->Branch ("diffbpm4Aymeansum", &diffbpm4Ameansum[1], "diffbpm4Aymeansum/D", 5000); 
 ia->Branch ("diffbpm4Aymeansuma", &diffbpm4Ameansuma[1], "diffbpm4Aymeansuma/D", 5000); 
 ia->Branch ("diffbpm4Aymeansume", &diffbpm4Ameansume[1], "diffbpm4AymeansumeD", 5000); 

 ia->Branch ("diffbpm4Bxmeansum", &diffbpm4Bmeansum[0], "diffbpm4Bxmeansum/D", 5000); 
 ia->Branch ("diffbpm4Bxmeansuma", &diffbpm4Bmeansuma[0], "diffbpm4Bxmeansuma/D", 5000); 
 ia->Branch ("diffbpm4Bxmeansume", &diffbpm4Bmeansume[0], "diffbpm4BxmeansumeD", 5000); 
 ia->Branch ("diffbpm4Bymeansum", &diffbpm4Bmeansum[1], "diffbpm4Bymeansum/D", 5000); 
 ia->Branch ("diffbpm4Bymeansuma", &diffbpm4Bmeansuma[1], "diffbpm4Bymeansuma/D", 5000); 
 ia->Branch ("diffbpm4Bymeansume", &diffbpm4Bmeansume[1], "diffbpm4Bymeansume/D", 5000); 

 

Int_t massanatype = 0;
TString analysis;
TString base = "./parity03";
//TString base = "/home/tonin/work/ia/pan/results/parity03";
TString suffix = ".res";
TString runname;
Bool_t isreject = kFALSE;
Bool_t isrun = kFALSE;
Bool_t doreject = kFALSE;
TString rejectfilenumber;
char bhwpstate[10];

FILE *freject;
 freject = fopen("rejectrun.dat","r");
FILE *iarun; 
 iarun = fopen("runlist.dat","r");
//  cout<<" Give first run of the list "<<endl;
//  cin>>rnum1;
//  cout<<" Give last run of the list "<<endl;
//  cin>>rnum2;
//  cout<<" Type of analysis "<<endl;
//  cin>>analysis;
 rnum1=1277;
 rnum2=2010;
 analysis="beam";
//  cout<<" what kind of rejection process do you want (0 = standard analysis ; 1 = feedback analysis)"<<endl;
//  cin>>massanatype;

//------------- first pass to sort .res files and reject bad ones ---------------------------------//
Int_t rfn = 0;
Char_t  buf[100];
Char_t  buffer[100];
Char_t iacontrol[10];
npair =0;
 for (Int_t i =0; i<2 ;i++)
   {
    diffbpm12meansuma[i] = 0;  
    diffbpm10meansuma[i] = 0;  diffbpm8meansuma[i]  = 0;
    diffbpm4Ameansuma[i] = 0;  diffbpm4Bmeansuma[i] = 0;
    diffbpm12meansum[i] = 0;
    diffbpm10meansum[i] = 0;  diffbpm8meansum[i]  = 0;
    diffbpm4Ameansum[i] = 0;  diffbpm4Bmeansum[i] = 0;
   }
 for (Int_t i =0;i<4;i++)
   {
    asybcmmeansuma[i] =0;
    asybcmmeansum[i]  =0;
   } 
runnum=0;
for (Int_t ridx = rnum1; ridx <= rnum2 ; ridx++)
    {  
     isrun=kFALSE;
     doreject = kFALSE;  
     runname = base + "_" + ridx +"_"+ analysis + suffix;
     FILE *therun;
     therun = fopen(runname.Data(),"r");
     //cout<<"processing run: "<<runname<<endl;
     if (therun != NULL)
       { 
	// see if run number is in reject.dat file and reject it if it is the case
        // if not in reject file but will be rejected after sorting, the file will appear
        // in reject.dat  
        if (freject != NULL) 
	 { 
	  while (!feof(freject) && rfn < ridx) 
	    { 
             if (fgets(buf,100,freject) != NULL )
	      {
               sscanf(buf,"%i",&rfn);
	       //cout<<buf<<" rfn="<<rfn<<"ridx="<<ridx<<endl;
	      }
	    }
          //cout<<" 2rnf="<<rfn<<"ridx="<<ridx<<endl;	  
          if (rfn == ridx ) 
            {
             doreject = kTRUE; 
             cout<<"Rejecting file number :"<<rfn<<endl;
	    }
	 }
        // now if run is not in reject list, get data and test rejection criteria for analysis
        if (!doreject)
	  { 
	    //cout<<"Getting EPICS data"<<flush<<endl; 
           if (iarun != NULL)
	     {
	       //cout<<"enter iarun"<<endl; 
	      while (!feof(iarun) && ridx > runnum) 
	        { 
                 if (fgets(buffer,100,iarun) != NULL )
		   { 
		     //cout<<"ridx="<<ridx<<" run="<<runnum<<flush<<endl;
                 if ( ridx<=1521 ) { 
	           sscanf(buffer,"%i %s %i %i %i %lf ",&runnum,&bhwpstate,&rhwp,&pfdbkon,&iaslope,&bcm1amp);  
                   //cout<<runnum<<bhwpstate<<rhwp<<iadac<<iaslope<<bcm1amp<<endl; 
		  }
	         else 
	           sscanf(buffer,"%i %s %i %lf 5 5 %lf %lf %i %i %lf",&runnum,&bhwpstate,&rhwp,&iadac,&pztxdac,&pztydac,&pfdbkon,&iaslope,&bcm1amp);  
		 //cout<<runnum<<" "<<bhwpstate<<" "<<rhwp<<" "<<iadac<<" "<<pztAxdac<<" "<<pztAydac<<" "<<pfdbkon<<" "<<iaslope<<bcm1amp;
		   }
		}
              //cout<<"2ridx="<<ridx<<" run="<<runnum<<flush<<endl;
              if (ridx < runnum ) ridx = runnum;
              if ( ridx == runnum )
		 {
                  if ( !strcmp(bhwpstate,"IN")) bhwp = 1;
                  else bhwp = 0; 
		  //cout<<"found in iarun, run="<<runnum<<" IHWP="<<bhwpstate<<" bhwp="<<bhwp
                  //    <<" bcm1(uA)="<<bcm1amp<<endl;
                  TaIResultsFile* fi = new TaIResultsFile(ridx,analysis);
                  while (fi.ReadNextLine())
	            {
                     if (fi->GetPtagTS() == "pan")
	               {
                        if (fi->GetRtagTS() == "bcm1_mean") 
                         {   
			   cout<<"current"<<fi->GetRes()<<endl;
		          if ( (Int_t) fi->GetRes() < 10000 ) 
                           {
		            doreject = kTRUE;
                            cout<<"Rejecting file :"<<ridx<<" BEAM under 5muA "<<endl;
                            ridx++; 
		           } 
	                 }
	               }
		    }
                  delete fi; fi =NULL;	
	          // if beam current != 0 but parity feedback off or pzt not set to no deviation
                  // reject file ( should reject uncontrolled Aq runs and PZT tests  
		    if (pfdbkon == 0) {
                        doreject =kTRUE;
                        cout<<"Rejecting file :"<<ridx<<" IA FEEDBACK OFF "<<endl;ridx++;  
		    }
                  if ((pztAxdac != 0 && pztAydac != 0 && pztxdac != 0 && pztydac != 0) &&
                      (pztAxdac != 5 && pztAydac != 5 && pztxdac != 5 && pztydac != 5) )
		    { 
                      doreject =kTRUE;
                      cout<<"Rejecting file :"<<ridx<<" PZT MIRRORS MOVED "<<endl;ridx++; 
		    }
	         if (!doreject)
		   {
		    // if no reason to reject, fill tree
                     TaIResultsFile* fi = new TaIResultsFile(ridx,analysis);
		     //cout<<"filling the tree"<<endl;
                    while (fi.ReadNextLine())
	              {
                       if (fi->GetPtagTS() == "pan")
	                 {
                          
			  if (fi->GetRtagTS() == "bcm1_mean") {bcmcurrent[0]= fi->GetRes(); bcmcurrenter[0] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "bcm1_mean_RMS") {bcmcurrentRMS[0]= fi->GetRes(); bcmcurrentRMSer[0] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "bcm2_mean") {bcmcurrent[1]= fi->GetRes(); bcmcurrenter[1] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "bcm2_mean_RMS") {bcmcurrentRMS[1]= fi->GetRes(); bcmcurrentRMSer[1] = fi->GetErr();}

			  else if (fi->GetRtagTS() == "Diff_batt1_mean") {diffbattmean[0]= fi->GetRes(); diffbattmeaner[0] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_batt1_RMS")  {diffbattRMS[0]= fi->GetRes(); diffbattRMSer[0] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_batt1_Neff") {diffbattNev[0]= fi->GetRes();}
			  else if (fi->GetRtagTS() == "Diff_batt2_mean") {diffbattmean[1]= fi->GetRes(); diffbattmeaner[1] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_batt2_RMS")  {diffbattRMS[1]= fi->GetRes(); diffbattRMSer[1] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_batt2_Neff") {diffbattNev[1]= fi->GetRes();}

			  else if (fi->GetRtagTS() == "Diff_bpm10x_mean") {diffbpm10mean[0]= fi->GetRes(); diffbpm10meaner[0] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm10x_RMS")  {diffbpm10RMS[0] = fi->GetRes(); diffbpm10RMSer[0]  = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm10x_Neff") {diffbpm10Nev[0]= fi->GetRes();}
			  else if (fi->GetRtagTS() == "Diff_bpm10y_mean") {diffbpm10mean[1]= fi->GetRes(); diffbpm10meaner[1] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm10y_RMS")  {diffbpm10RMS[1] = fi->GetRes(); diffbpm10RMSer[1]  = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm10y_Neff") {diffbpm10Nev[1]= fi->GetRes();}

			  else if (fi->GetRtagTS() == "Diff_bpm12x_mean") {diffbpm12mean[0]= fi->GetRes(); diffbpm12meaner[0] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm12x_RMS")  {diffbpm12RMS[0] = fi->GetRes(); diffbpm12RMSer[0]  = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm12x_Neff") {diffbpm12Nev[0]= fi->GetRes();}
			  else if (fi->GetRtagTS() == "Diff_bpm12y_mean") {diffbpm12mean[1]= fi->GetRes(); diffbpm12meaner[1] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm12y_RMS")  {diffbpm12RMS[1] = fi->GetRes(); diffbpm12RMSer[1]  = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm12y_Neff") {diffbpm12Nev[1]= fi->GetRes();}

			  else if (fi->GetRtagTS() == "Diff_bpm8x_mean") {diffbpm8mean[0]= fi->GetRes(); diffbpm8meaner[0] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm8x_RMS")  {diffbpm8RMS[0] = fi->GetRes(); diffbpm8RMSer[0]  = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm8x_Neff") {diffbpm8Nev[0]= fi->GetRes();}
			  else if (fi->GetRtagTS() == "Diff_bpm8y_mean") {diffbpm8mean[1]= fi->GetRes(); diffbpm8meaner[1] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm8y_RMS")  {diffbpm8RMS[1] = fi->GetRes(); diffbpm8RMSer[1]  = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm8y_Neff") {diffbpm8Nev[1]= fi->GetRes();}

			  else if (fi->GetRtagTS() == "Diff_bpm4ax_mean") {diffbpm4Amean[0]= fi->GetRes(); diffbpm4Ameaner[0] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm4ax_RMS")  {diffbpm4ARMS[0] = fi->GetRes(); diffbpm4ARMSer[0]  = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm4ax_Neff") {diffbpm4ANev[0]= fi->GetRes();}
			  else if (fi->GetRtagTS() == "Diff_bpm4ay_mean") {diffbpm4Amean[1]= fi->GetRes(); diffbpm4Ameaner[1] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm4ay_RMS")  {diffbpm4ARMS[1] = fi->GetRes(); diffbpm4ARMSer[1]  = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm4ay_Neff") {diffbpm4ANev[1]= fi->GetRes();}

			  else if (fi->GetRtagTS() == "Diff_bpm4bx_mean") {diffbpm4Bmean[0]= fi->GetRes(); diffbpm4Bmeaner[0] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm4bx_RMS")  {diffbpm4BRMS[0] = fi->GetRes(); diffbpm4BRMSer[0]  = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm4bx_Neff") {diffbpm4BNev[0]= fi->GetRes();}
			  else if (fi->GetRtagTS() == "Diff_bpm4by_mean") {diffbpm4Bmean[1]= fi->GetRes(); diffbpm4Bmeaner[1] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm4by_RMS")  {diffbpm4BRMS[1] = fi->GetRes(); diffbpm4BRMSer[1]  = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Diff_bpm4by_Neff") {diffbpm4BNev[1]= fi->GetRes();}
			  
			  else if (fi->GetRtagTS() == "Asym_batt1_mean") {asybattmean[0]= fi->GetRes(); asybattmeaner[0] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Asym_batt1_RMS")  {asybattRMS[0]= fi->GetRes();  asybattRMSer[0] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Asym_batt1_Neff") {asybattNev[0]= fi->GetRes();}
			  else if (fi->GetRtagTS() == "Asym_batt2_mean") {asybattmean[1]= fi->GetRes(); asybattmeaner[1] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Asym_batt2_RMS")  {asybattRMS[1]= fi->GetRes();  asybattRMSer[1] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Asym_batt1_Neff") {asybattNev[1]= fi->GetRes();}
			  
			  else if (fi->GetRtagTS() == "Asym_bcm1_mean") {asybcmmean[0]= fi->GetRes(); asybcmmeaner[0] = fi->GetErr();cout<<asybcmmean[0]<<flush<<endl;}
			  else if (fi->GetRtagTS() == "Asym_bcm1_RMS")  {asybcmRMS[0]= fi->GetRes();   asybcmRMSer[0] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Asym_bcm1_Neff") {asybcmNev[0]= fi->GetRes();}
			  else if (fi->GetRtagTS() == "Asym_bcm2_mean") {asybcmmean[1]= fi->GetRes(); asybcmmeaner[1] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Asym_bcm2_RMS")  {asybcmRMS[1]= fi->GetRes();  asybcmRMSer[1] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Asym_bcm2_Neff") {asybcmNev[1]= fi->GetRes();}
			  else if (fi->GetRtagTS() == "Asym_bcm3_mean") {asybcmmean[2]= fi->GetRes(); asybcmmeaner[2] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Asym_bcm3_RMS")  {asybcmRMS[2]= fi->GetRes();  asybcmRMSer[2] = fi->GetErr();}
			  else if (fi->GetRtagTS() == "Asym_bcm3_Neff") {asybcmNev[2]= fi->GetRes();}
			}
		     }
		   bcmresol = asybcmmean[0] - asybcmmean[1];
		   npair += asybcmNev[0];
                   // correcting bpm differences depending on IHWP state
                   //cout<<bhwp<<endl;
                   if ( bhwp == 1 )
		     {
		       for (Int_t di = 0; di<2; di++)
			 {
		          diffbpm10mean[di] = -1*diffbpm10mean[di];
		          diffbpm12mean[di] = -1*diffbpm12mean[di];
		          diffbpm8mean[di] = -1*diffbpm8mean[di];
		          diffbpm4Amean[di] = -1*diffbpm4Amean[di];
		          diffbpm4Bmean[di] = -1*diffbpm4Bmean[di];
                          asybcmmean[di]    = -1*asybcmmean[di];
			 }
		     }                  
		   // calculating weighted sums
		   for (Int_t i =0; i<2 ;i++)
			{
			 if (diffbpm12meaner[i] != 0)
                             diffbpm12meansuma[i] += 1/(diffbpm12meaner[i]*diffbpm12meaner[i]);
                         if (diffbpm10meaner[i] != 0)
                            {
                             diffbpm10meansuma[i] += 1/(diffbpm10meaner[i]*diffbpm10meaner[i]); 
                             diffbpm10meansume[i] = sqrt(1/(diffbpm10meansuma[i]));
			    } 
                         if (diffbpm8meaner[i] != 0)   diffbpm8meansuma[i]  += 1/(diffbpm8meaner[i]*diffbpm8meaner[i]);
                         if (diffbpm4Ameaner[i] != 0)
                            {
                             diffbpm4Ameansuma[i] += 1/(diffbpm4Ameaner[i]*diffbpm4Ameaner[i]); 
                             diffbpm4Ameansume[i] = sqrt(1/(diffbpm4Ameansuma[i]));
			    } 
                         if (diffbpm4Bmeaner[i] != 0)
                            {
                             diffbpm4Bmeansuma[i] += 1/(diffbpm4Bmeaner[i]*diffbpm4Bmeaner[i]); 
                             diffbpm4Bmeansume[i] = sqrt(1/(diffbpm4Bmeansuma[i]));
			    } 
                         if (diffbpm12meaner[i] != 0)  diffbpm12meansum[i]  += diffbpm12mean[i]/(diffbpm12meaner[i]*diffbpm12meaner[i]);
                         if (diffbpm10meaner[i] != 0)  diffbpm10meansum[i]  += diffbpm10mean[i]/(diffbpm10meaner[i]*diffbpm10meaner[i]); 
                         if (diffbpm8meaner[i] != 0)   diffbpm8meansum[i]   += diffbpm8mean[i]/(diffbpm8meaner[i]*diffbpm8meaner[i]);
                         if (diffbpm4Ameaner[i] != 0)  diffbpm4Ameansum[i]  += diffbpm4Amean[i]/(diffbpm4Ameaner[i]*diffbpm4Ameaner[i]);
                         if (diffbpm4Bmeaner[i] != 0)  diffbpm4Bmeansum[i]  += diffbpm4Bmean[i]/(diffbpm4Bmeaner[i]*diffbpm4Bmeaner[i]);
			}
		   for (Int_t i =0;i<2;i++)
			{
			 if (asybcmmeaner[i]!=0) 
                           {
                            asybcmmeansuma[i] += 1/(asybcmmeaner[i]*asybcmmeaner[i]);
                            asybcmmeansume[i] = sqrt(1/(asybcmmeansuma[i]));
			   }
                         if (asybcmmeaner[i]!=0) asybcmmeansum[i]  += asybcmmean[i]/(asybcmmeaner[i]*asybcmmeaner[i]); 
			} 
//                   if ( !strcmp(bhwpstate,"IN")) bhwp = 1;
//                   else bhwp = 0; 
                   //cout<<bhwp<<" 2 "<<endl;
                   ia->Fill();         
                   delete fi; fi = NULL; 		  
                  }
		}
	     }
	  }
	
       }
  else cout<<" no .res file for run :"<<runname.Data()<<endl;     
  }
  //ia->Print();
  ia->Draw("asybcm1:run");  
  resout->Write();
  cout<<" Run Analysis Tree is filled ..."<<endl;
  resout->Close(); 
}




