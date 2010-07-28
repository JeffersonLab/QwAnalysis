#include <TList.h>
#include <TEllipse.h>
#include <TLine.h>

#include "QwVQWK_Channel.h" 
#include "QwHistogramHelper.h"
#include "QwCorrelationMonitor.h"

//==========================================================
//==========================================================
QwCorrelationMonitor::QwCorrelationMonitor() {
  fGoodEventNumber=0;
  fVariables.clear();
  hC=0;
  fAllNames="";
  par_highCorr=0.4; // for tagging 
  fCore="";
  par_nSkipEveHist=200;
} 
   
//==========================================================
//==========================================================
void QwCorrelationMonitor::AddVariableList(TString mapName){
  
  QwMessage << Form("QwCorrelMon_%s::AddVariableList:   ",fCore.Data(),mapName.Data())<<std::endl;
 
  QwParameterFile mapstr(mapName.Data());  //Open the file
  
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('#');   // Remove everything after a '#' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    TString line = mapstr.GetLine();
    AddVariables(line);
    // QwMessage<<line<<QwLog::endl;
  }  
}



//==========================================================
//==========================================================
QwCorrelationMonitor::~QwCorrelationMonitor() {
}

 

//==========================================================
//==========================================================
void QwCorrelationMonitor::AccessChannels( QwSubsystemArrayParity &detectors){
  fAllNames.ToLower();
  TObjArray* sa	=fAllNames.Tokenize(" "); sa->Sort();
  QwMessage << Form("QwCorrelMon_%s::AccessChannels:   ",fCore.Data())<<std::endl;
  TString goodNames="";
  // sa->Print();
  TIter it(sa);TObject *ob, *ob1=0;
  while ((ob = it.Next())) {
    if(ob1 && strcmp(ob->GetName(),ob1->GetName())==0) continue; // skip duplicates
    ob1=ob;		  
    TString name=(ob->GetName());
    //name=name.Strip((TString::EStripType)3,' ');    
    //  printf("s=%s=\n",ob->GetName());
    const QwVQWK_Channel* value = dynamic_cast<const QwVQWK_Channel*>(detectors.ReturnInternalValue(name));
    if(value==0) {
      QwWarning << Form("corr: skip variable '%s', null pointer from  ReturnInternalValue() ",name.Data()) << QwLog::endl;
      continue;
    } 
    QwCorrelationMonitorVariable x(name);
    x.channel=value;
    fVariables.push_back(x);    
    QwMessage << "  corrMap:  found variable "<<x.name<<QwLog::endl;    
  } // loop over names
  
  size_t dim=fVariables.size();   
  // create uBlas matrices
  fM1.resize(dim,false);
  fC2.resize(dim,dim,false);
  // initialization performed in accumulator()

}



//==========================================================
//==========================================================
void QwCorrelationMonitor::AccessChannels( VQwSubsystemParity *detector){
  fAllNames.ToLower();
  TObjArray* sa	=fAllNames.Tokenize(" "); sa->Sort();
  QwMessage << Form("QwCorrelMon_%s::AccessChannels:   ",fCore.Data())<<std::endl;
  TString goodNames="";
  // sa->Print();
  TIter it(sa);TObject *ob, *ob1=0;
  while ((ob = it.Next())) {
    if(ob1 && strcmp(ob->GetName(),ob1->GetName())==0) continue; // skip duplicates
    ob1=ob;		  
    TString name=(ob->GetName());
    //name=name.Strip((TString::EStripType)3,' ');    
    //  printf("s=%s=\n",ob->GetName());
    const QwVQWK_Channel* value = dynamic_cast<const QwVQWK_Channel*>(detector->ReturnInternalValue(name));
    if(value==0) {
      QwWarning << Form("corr: skip variable '%s', null pointer from  ReturnInternalValue() ",name.Data()) << QwLog::endl;
      continue;
    } 
    QwCorrelationMonitorVariable x(name);
    x.channel=value;
    fVariables.push_back(x);    
    QwMessage << "  corrMap:  found variable "<<x.name<<QwLog::endl;    
  } // loop over names
  
  size_t dim=fVariables.size();   
  // create uBlas matrices
  fM1.resize(dim,false);
  fC2.resize(dim,dim,false);
  // initialization performed in accumulator()

}



//==========================================================
//==========================================================
void QwCorrelationMonitor::Accumulate(){
    
  fGoodEventNumber++;
  size_t dim=fVariables.size();
  if(dim<1) return; // needs at least 2  variables for correlations 

  if( fGoodEventNumber==par_nSkipEveHist) recomputHistoAxis();

  // increment  M1,M2 and monitoring histos
  for (size_t i = 0; i <dim; i++) {
    double u=fVariables[i].channel->GetHardwareSum();
    double udel=u-fM1[i];
    if(fGoodEventNumber<=1) fM1[i]=u;
    else fM1[i]+=udel/fGoodEventNumber;

    if(fGoodEventNumber<=2)  fC2(i,i)=0;
    else fC2(i,i)+=(u-fM1[i])*udel; // Note, it uses pre & post incremented mean!

    // monitoring 1D
    if( fGoodEventNumber>  par_nSkipEveHist) hM[i]->Fill(u);
    
    for (size_t j = i+1; j < dim; j++) {// only upper triangle
      double v=fVariables[j].channel->GetHardwareSum();
      double vdel=v-fM1[j];
      
      if(fGoodEventNumber<=1) fC2(i,j)=0;
      else fC2(i,j)+=udel*vdel*(fGoodEventNumber-1)/fGoodEventNumber;

      // monitoring 2D
      if( fGoodEventNumber>  par_nSkipEveHist) hC[i][j]->Fill(u,v);
      
    }
  }

}


//==========================================================
//==========================================================
Int_t  QwCorrelationMonitor::GetElementMean(const int i, Double_t &mean ){
   mean=-1e50;
   if(i<0 || i >= (int)fVariables.size() ) return -1; 
   if( fGoodEventNumber<1) return -3;
   mean=fM1[i];    return 0;
}

//==========================================================
//==========================================================
Int_t  QwCorrelationMonitor::GetElementSigma(const int i, Double_t &sigma ){
    sigma=-1e50;
    if(i<0 || i >= (int)fVariables.size() ) return -1; 
    if( fGoodEventNumber<2) return -3;
    sigma=sqrt(fC2(i,i)/(fGoodEventNumber-1.));
    return 0;
  }

//==========================================================
//==========================================================
Int_t  QwCorrelationMonitor::GetCovariance( int i, int j, Double_t &covar ){
    covar=-1e50;
    if(i==j)  return  GetElementSigma(i,covar); // diagonal 
    if( i>j) { int k=i; i=j; j=k; }//swap
    //... now we need only upper right triangle 
    if(i<0 || j >= (int)fVariables.size() ) return -11; 
    if( fGoodEventNumber<2) return -14;
    covar=fC2(i,j)/(fGoodEventNumber-1.);
    return 0;
}

//==========================================================
//==========================================================
void QwCorrelationMonitor::PrintSummary(){
  QwMessage << Form("QwCorrelMon_%s::PrintSummary() seen good eve=%lld",fCore.Data(),fGoodEventNumber)<<QwLog::endl;
 
  size_t dim=fVariables.size();

  if(fGoodEventNumber>2) { // print full matrix
    QwMessage << Form("           i=              "); 
    for (size_t i = 0; i <dim; i++) {
      QwMessage << Form("  %8d ",i);

    }
    QwMessage << Form("\n         name:             "); 
    for (size_t i = 0; i <dim; i++) {
      QwMessage << Form("  %8s ",fVariables[i].name.Data());
    }
    QwMessage << Form("\n j                   mean       sig     corelation-matrix ....\n"); 
    for (size_t i = 0; i <dim; i++) {
      double meanI,sigI;
      assert( GetElementMean(i,meanI)==0);
      assert( GetElementSigma(i,sigI)==0);

      QwMessage << Form(" %3d %8s:  %+12.4g  %8.1f ",i,fVariables[i].name.Data(),meanI,sigI);
      for (size_t j = 1; j <dim; j++) {
	if( j<=i) { QwMessage << Form("  %8s ",". . . ."); continue;}
	double sigJ,cov; assert( GetElementSigma(j,sigJ)==0);
	assert( GetCovariance(i,j,cov)==0);
	double corel=cov/sigI/sigJ;
	char tag=' '; if(fabs(corel)>par_highCorr) tag='*';
	QwMessage << Form("  %7.3f%c ",corel,tag);
      }
      QwMessage << Form("\n");
    }
  }
  QwMessage << Form("QwCorrelationMonitor::PrintSummary() end\n\n",fGoodEventNumber)<<QwLog::endl;
   
#if 0  // testing
  for (size_t i = 0; i <dim; i++) {
    for (size_t j = i+1; j < dim; j++) {// only upper triangle
      double cov;  GetCovariance(i,j,cov);
      QwMessage << Form(" xxx %s %s cov=%f  C2=%f\n",fVariables[i].name.Data(),fVariables[j].name.Data(),cov,fC2(i,j) )<<QwLog::endl;
    }
  }
#endif
  
  }

//==========================================================
//==========================================================
void QwCorrelationMonitor::DeleteHistograms(){
  size_t dim=fVariables.size(); 
  for (size_t i = 0; i <dim; i++) {
    hM[i]->Delete();
    for (size_t j = i+1; j < dim; j++) {// only upper triangle
      hC[i][j]->Delete();
    }
  }
  hA->Delete();
}

//==========================================================
//==========================================================
void QwCorrelationMonitor::ConstructHistograms(){
  size_t dim=fVariables.size(); 
  assert(dim>0); // variable list must be initialized
  hC=new TH1**[dim];
  hM=new TH1*[dim]; memset( hM, 0, sizeof( TH1*)*dim); 

  int k=0;
  TString totList="";
  for (size_t i = 0; i <dim; i++) {
    hC[i]=new TH1*[dim]; 
    memset( hC[i], 0, sizeof( TH1*)*dim); 
    TString name1=fVariables[i].name;
    totList+=" "+name1;

    // ...... 1D histos per channel ..................
    TString name=name1+"_hw_single";
    QwMessage << Form(" corrMon-%s:initHisto() i=%d name=%s  (1D) ",fCore.Data(),i,name.Data())<<QwLog::endl;
    TH1* h=gQwHists.Construct1DHist(name);
    h->GetXaxis()->SetTitle(name1);
    h->GetYaxis()->SetTitle("");
    TString tit=Form("%s %d_ ,%s",h->GetTitle(),i,fCore.Data());
    h->SetTitle(tit);
    h->SetName(fCore+h->GetName());    
    hM[i]=h;       

    // ...... 2D histos per pair of channels ..................
    for (size_t j = i+1; j < dim; j++) {// only upper triangle
      TString name2=fVariables[j].name;
      k++;
      TString name=name1+"_"+name2+"_hw_corr";
      QwMessage << Form(" corrMon-%s:initHisto() i=%d name=%s  (2D) ",fCore.Data(),i,name.Data())<<QwLog::endl;
      TH1* h=gQwHists.Construct2DHist(name);
      h->GetXaxis()->SetTitle(name1);
      h->GetYaxis()->SetTitle(name2); 
      TString tit=Form("%s %d_%d:%d",h->GetTitle(),i,j,k);
      h->SetTitle(tit);
      h->SetName(fCore+h->GetName());   
      hC[i][j]=h;       
    }
  }
  
  QwMessage << Form(" corr:initHisto() done, added %d  histos for %d variables\n",k,dim)<<QwLog::endl;
  // testing
  hA=gQwHists.Construct1DHist((TString)"jan_list");
  hA->SetTitle(totList);
  hA->SetName(fCore+hA->GetName());    
  hA->Fill(5.0); // for fun
  

}


//==========================================================
//==========================================================
void QwCorrelationMonitor::recomputHistoAxis(){
  double par_kSig=8;
  double par_fact=2;
  QwMessage << Form("QwCorrelMon_%s::recomputHistoAxis() seen good eve=%lld",fCore.Data(),fGoodEventNumber)<<QwLog::endl;

  assert(fGoodEventNumber>1);

  size_t dim=fVariables.size();
  double *x1=new double[dim];
  double *x2=new double[dim];
  double *x3=new double[dim];

  for (size_t i = 0; i <dim; i++) { 
    double sig;
    assert( GetElementSigma(i,sig)==0);
    x1[i]=fM1[i]-par_kSig*sig;
    x2[i]=fM1[i]+par_kSig*sig;
    x3[i]=par_kSig*sig*par_fact;
    
    
    QwMessage << Form(" i=%d name=%s  M1=%.2g sig=%.1f  axis range [%.1f , %.1f]",i,fVariables[i].name.Data(),fM1[i],sig,x1[i],x2[i])<<QwLog::endl;
  }

  // rescale histograms
  for (size_t i = 0; i <dim; i++) {
    TAxis *ax= hM[i]->GetXaxis();
    ax->Set(ax->GetNbins(),x1[i]-x3[i],x2[i]+x3[i]);
    hM[i]->Reset(); // just in case

    for (size_t j = i+1; j < dim; j++) {// only upper triangle
      TAxis *ax= hC[i][j]->GetXaxis();
      ax->Set(ax->GetNbins(),x1[i],x2[i]);
      ax= hC[i][j]->GetYaxis();
      ax->Set(ax->GetNbins(),x1[j],x2[j]);
      hC[i][j]->Reset(); // just in case
    }
  }

  // cleanup;
  delete [] x1;
  delete [] x2;
  delete [] x3;

}

 
//==========================================================
//==========================================================
void QwCorrelationMonitor::NiceOutput(){
  size_t dim=fVariables.size();
  for (size_t i = 0; i <dim; i++) {
    double meanI,sigI;
    assert( GetElementMean(i,meanI)==0);
    assert( GetElementSigma(i,sigI)==0);
    TList *L= hM[i]->GetListOfFunctions(); assert(L);
    double yMx=1.5*hM[i]->GetMaximum();
    TLine *ln=new TLine(meanI,0,meanI,yMx); ln->SetLineColor(kMagenta); L->Add(ln);
    ln=new TLine(meanI-sigI,0,meanI-sigI,yMx); ln->SetLineColor(kMagenta);ln->SetLineStyle(2); L->Add(ln);
    ln=new TLine(meanI+sigI,0,meanI+sigI,yMx); ln->SetLineColor(kMagenta);ln->SetLineStyle(2); L->Add(ln);

    for (size_t j = i+1; j <dim; j++) {	// tag histos w/ large correlations
      double sigJ,cov; 
      assert( GetCovariance(i,j,cov)==0);
      assert( GetElementSigma(j,sigJ)==0);
      double corel=cov/sigI/sigJ;
      double meanJ;  assert( GetElementMean(j,meanJ)==0);
      TList *L= hC[i][j]->GetListOfFunctions(); assert(L);
      for(int k=0;k<2;k++){// y-lines
	double x1=meanI+sigI*(1-2*k), x2=meanI+sigI*(1-2*k);
	double y1=meanJ-10*sigJ,       y2=meanJ+10*sigJ;
	ln=new TLine(x1,y1,x2,y2); ln->SetLineColor(kMagenta);ln->SetLineStyle(2); L->Add(ln);      }
      for(int k=0;k<2;k++){// x-lines
	double x1=meanI-sigI*10,         x2=meanI+sigI*10;
	double y1=meanJ+sigJ*(1-2*k),    y2=meanJ+sigJ*(1-2*k);
	ln=new TLine(x1,y1,x2,y2); ln->SetLineColor(kMagenta);ln->SetLineStyle(2); L->Add(ln);      }
      
      if(fabs(corel)<par_highCorr) continue;
      // now only special cases
      double fac=5;
      TEllipse *el=new TEllipse(meanI,meanJ,fac*sigI,fac*sigJ);
      el->SetFillStyle(0);	  el->SetLineColor(kRed); el->SetLineWidth(2);
      L->Add(el);
      // printf("ellipse added to histo=%s\n", hC[i][j]->GetName());
    }// end of J
  }// end of I
}
