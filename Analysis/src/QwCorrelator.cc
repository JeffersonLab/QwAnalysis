


#include "QwVQWK_Channel.h" 
#include "QwHistogramHelper.h"
#include "QwCorrelator.h"


//==========================================================
//==========================================================
QwCorrelator::QwCorrelator() {
  printf("\nJJ QwCorrelator constructor\n\n");
  fGoodEventNumber=0;
  fNumVar=0;
  fVariables.clear();
  hM=0;
} 
   

//==========================================================
//==========================================================
QwCorrelator::~QwCorrelator() {
}

//==========================================================
//==========================================================
void QwCorrelator::PrintSummary(){
  QwMessage << Form("\n\nQwCorrelator::PrintSummary() seen good eve=%lld",fGoodEventNumber)<<QwLog::endl;
  size_t dim=fVariables.size();

  if(fGoodEventNumber>2) { // print full matrix
    QwMessage << Form("        i="); 
    for (size_t i = 0; i <dim; i++) {
      if(fVariables[i].channel==0) continue;
      QwMessage << Form("  %8d ",i);
      if(i==0)  QwMessage << Form("%10s"," ");
    }
    QwMessage << Form("\n       name   "); 
    for (size_t i = 0; i <dim; i++) {
      if(fVariables[i].channel==0) continue;
      QwMessage << Form("  %8s ",fVariables[i].name.Data());
      if(i==0)  QwMessage << Form("%10s"," ");
    }
    QwMessage << Form("\n j               mean    sig     corelation-matrix ....\n"); 
    for (size_t i = 0; i <dim; i++) {
      if(fVariables[i].channel==0) continue;
      double mean,sig,cov;
      assert( GetElementMean(i,mean)==0);
      assert( GetElementSigma(i,sig)==0);

      QwMessage << Form(" %d %8s:  %10.1f  %.1f ",i,fVariables[i].name.Data(),mean,sig);
      for (size_t j = 1; j <dim; j++) {
	if(fVariables[j].channel==0) continue;
	if( j<=i) { QwMessage << Form("  %8s ",". . . ."); continue;}
	double sigJ; assert( GetElementSigma(j,sigJ)==0);
	assert( GetCovariance(i,j,cov)==0);
	QwMessage << Form("  %8.1f ",cov/sig/sigJ);
      }
      QwMessage << Form("\n");
    }
  }
  QwMessage << Form("QwCorrelator::PrintSummary() end\n\n",fGoodEventNumber)<<QwLog::endl;

#if 0  // testing
  for (size_t i = 0; i <dim; i++) {
    if(fVariables[i].channel==0) continue;
    for (size_t j = i+1; j < dim; j++) {// only upper triangle
      if(fVariables[j].channel==0) continue;
      double cov;  GetCovariance(i,j,cov);
      QwMessage << Form(" xxx %s %s cov=%f  C2=%f\n",fVariables[i].name.Data(),fVariables[j].name.Data(),cov,fC2(i,j) )<<QwLog::endl;
    }
  }
#endif
  
}

 
//==========================================================
//==========================================================
void QwCorrelator::recomputHistoAxis(){
  double par_kSig=5;

  QwMessage << Form("\n\nQwCorrelator::recomputHistoAxis() seen good eve=%lld",fGoodEventNumber)<<QwLog::endl;
  assert(fGoodEventNumber>1);

  size_t dim=fVariables.size();
  double *x1=new double[dim];
  double *x2=new double[dim];

  for (size_t i = 0; i <dim; i++) {
    if(fVariables[i].channel==0) continue;
 
    double sig;
    assert( GetElementSigma(i,sig)==0);
    x1[i]=fM1[i]-par_kSig*sig;
    x2[i]=fM1[i]+par_kSig*sig;
    
    QwMessage << Form(" i=%d name=%s  M1=%.1f sig=%.1f  axis range [%.1f , %.1f]\n",i,fVariables[i].name.Data(),fM2[i],sig,x1[i],x2[i]);
  }

  // rescale histograms
  for (size_t i = 0; i <dim; i++) {
    if(fVariables[i].channel==0) continue;
    for (size_t j = i+1; j < dim; j++) {// only upper triangle
      if(fVariables[j].channel==0) continue;
      TAxis *ax= hM[i][j]->GetXaxis();
      ax->Set(ax->GetNbins(),x1[i],x2[i]);
      ax= hM[i][j]->GetYaxis();
      ax->Set(ax->GetNbins(),x1[j],x2[j]);
      hM[i][j]->Reset(); // just in case
    }
  }

  // cleanup;
  delete [] x1;
  delete [] x2;

}

 
//==========================================================
//==========================================================
void QwCorrelator::SetInputVector( QwSubsystemArrayParity &detectors){
  const char *nameA[]={"md1neg","md2pos","md3neg","md4pos","md5neg","md5pos",
		       "md7neg","md7pos"};
  for(size_t i=0; i<sizeof(nameA)/sizeof(char*); i++) {
    QwCorrelatorVariable x(nameA[i]);
    fVariables.push_back(x);
    //if (i>=2) break;
  }


  size_t dim=fVariables.size();  
  for (size_t i = 0; i < dim; i++) {
    TString name=fVariables[i].name;
    const QwVQWK_Channel* value = dynamic_cast<const
      QwVQWK_Channel*>(detectors.ReturnInternalValue(name));
    if (value) {
      fVariables[i].channel=value;
      fNumVar++;
      QwMessage << "Correlator:: add variable "<<name<<QwLog::endl;
    }
    else {
      QwWarning << "skip due to null pointer for variable " << name << QwLog::endl;
    }
 }

  // create uBlas matrices
  fM1.resize(dim,false);
  fM2.resize(dim,false);
  fC2.resize(dim,dim,false);

  // initialization?

}




//==========================================================
//==========================================================
void QwCorrelator::SetHistos(){
  size_t dim=fVariables.size(); 
  assert(dim>0); // variable list must be initialized
  hM=new TH1**[dim];

  int k=0;
  for (size_t i = 0; i <dim; i++) {
    if(fVariables[i].channel==0) continue;
    hM[i]=new TH1*[dim];
    memset( hM[i], 0, sizeof( TH1*)*dim); 
    TString name1=fVariables[i].name;
    for (size_t j = i+1; j < dim; j++) {// only upper triangle
      if(fVariables[j].channel==0) continue;
      TString name2=fVariables[j].name;
      k++;
      TString name=name1+"_"+name2+"_hw_corr";
      std::cout<<k<<"_"<<name<<std::endl;
      TH1* h=gQwHists.Construct2DHist(name);
      h->GetXaxis()->SetTitle(name1);
      h->GetYaxis()->SetTitle(name2); 
      hM[i][j]=h;       
    }
  }
  
  QwMessage << Form(" corr:initHisto() done, added %d correlatin histos\n",k)<<QwLog::endl;
  // testing
  TH1F* h1=gQwHists.Construct1DHist((TString)"jan_h1");
  h1->Fill(5.0);

}



//==========================================================
//==========================================================
void QwCorrelator::AddEvent(){
  int par_numEve4histo=200;// initializes axis of histograms 

  fGoodEventNumber++;
  if(fNumVar<1) return; // needs at least 2  variables for correlations 
  size_t dim=fVariables.size();
  assert(dim>0); // variable list must be initialized

  if( fGoodEventNumber== par_numEve4histo) recomputHistoAxis();

  // increment  M1,M2 and monitoring histos
  for (size_t i = 0; i <dim; i++) {
    if(fVariables[i].channel==0) continue;
    double u=fVariables[i].channel->GetHardwareSum();
    //printf(" i=%d u=%f\n",i,u);
    double udel=u-fM1[i];
    if(fGoodEventNumber<=1) fM1[i]=u;
    else fM1[i]+=udel/fGoodEventNumber;

    if(fGoodEventNumber<=2) fM2[i]=0;
    else fM2[i]+=udel*udel;

   for (size_t j = i+1; j < dim; j++) {// only upper triangle
      if(fVariables[j].channel==0) continue;
      double v=fVariables[j].channel->GetHardwareSum();
      double vdel=v-fM1[j];

      if(fGoodEventNumber<=1) fC2(i,j)=0;
      else fC2(i,j)+=udel*vdel/fGoodEventNumber;

      // monitoring
      if( fGoodEventNumber> par_numEve4histo) hM[i][j]->Fill(u,v);
     
    }
  }

}


