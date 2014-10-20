#ifndef __BGDCORRECT_H
#define __BGDCORRECT_H

#include "rootClass.h"
#include "comptonRunConstants.h"

///////////////////////////////////////////////////////////////////////////
Int_t bgdCorrect(Double_t rate1[], Double_t rate2[]) {
  cout<<blue<<"\nInvoking bgd correction\n"<<normal<<endl;
  ////Double_t d1, d2, d3;
  //TString file;
  //ifstream fIn;
  ////std::vector<Double_t> str, corrH1, corrH1Er;
  //Double_t corrH1[nStrips], corrH1Er[nStrips];
  ////file = Form("ratio_AcLasOffBkgdP1_24761_24762_1261.txt");
  ////file = Form("ratio_AcLasOffBkgdP1_24761_24762_2000.txt");
  //file = Form("diffAcqNormCntsB1H1L0P1_%d_%d.txt",24761,24762);
  //read3CfileArray(file, stripArray, corrH1, corrH1Er); 

  ////std::vector<Double_t> dum, corrH0, corrH0Er;
  //Double_t corrH0[nStrips], corrH0Er[nStrips];
  //file = Form("diffAcqNormCntsB1H0L0P1_%d_%d.txt",24761,24762);
  //read3CfileArray(file, stripArray, corrH0, corrH0Er);

  //Int_t trSt=0;
  //printf("%d\t%f\t%f\t%f\t%f\n",41, rate1[40], rate2[40], corrH1[40], corrH0[40]);
  cout<<"using strip "<<Cedge<<" as Compton edge to limit the bgd correction application"<<endl;
  for (Int_t s =startStrip; s <=(Int_t)Cedge+1; s++) {	  
    if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
    //rate1[s] = rate1[s]/ (corrH1[s] );
    //rate2[s] = rate2[s]/ (corrH1[s] );
    //rate1[s] = rate1[s] - (corrH0[s] );//opposite helicity
    //rate2[s] = rate2[s] - (corrH1[s] );//opposite helicity
    //rate1[s] = rate1[s] - (corrH1[s]);//same helicity
    //rate2[s] = rate2[s] - (corrH0[s]);//same helicity
    //rate1[s] = rate1[s]/ (corrH0.at(trSt) );//opposite helicity
    //rate2[s] = rate2[s]/ (corrH1.at(trSt) );//opposite helicity
    //trSt ++;
    ///opposite helicity
    //rate1[s] = rate1[s]*(1.0 - 0.0479);//p0 correction to each rate
    //rate2[s] = rate2[s]*(1.0 - 0.0445);
    //rate1[s] = rate1[s]*(1.0 - 0.025 - (s+1)*0.00064);//oppposite helicity case
    //rate2[s] = rate2[s]*(1.0 - 0.027 - (s+1)*0.00048);

    ///same helicity
    rate1[s] = rate1[s]*(1.0 - 0.0445);//p0 correction to each rate
    rate2[s] = rate2[s]*(1.0 - 0.0479);
    //rate1[s] = rate1[s]*(1.0 - 0.0269 - (s+1)*0.0005);//p1 correction to each rate
    //rate2[s] = rate2[s]*(1.0 - 0.0251 - (s+1)*0.0006);
    //rate1[s] = rate1[s]*(1.0 - 0.027 - (s+1)*0.00048);//same helicity case
    //rate2[s] = rate2[s]*(1.0 - 0.025 - (s+1)*0.00064);

  }
  //printf("%d\t%f\t%f\t%f\t%f\n",41, rate1[40], rate2[40], corrH1[40], corrH0[40]);

  return 1;
}

#endif

