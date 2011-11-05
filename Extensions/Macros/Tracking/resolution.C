
// author: Siyuan Yang
//       : College of William & Mary
//       : sxyang@email.wm.edu
// calculate the residual from combinations of hits
 
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <ctime>

// this work is to repeat Mark's calculation in VT but instead of cosmic ray data, using the real data
// here, we only apply the calculation on one chamber


using namespace std;
const int layers=12;
const int ndirs=3;
const double wirespace=1.1684;
const double offset=-18.1102;
const double locations[layers]={-338.50,-336.522,-334.544,-332.566,-330.588,-328.610,-295.800,-293.822,-291.844,-289.866,,-287.888,-285.910};
const double centery[layers]={51.013,50.911,51.155,51.013,50.911,51.155,54.903,54.601,54.845,54.903,54.601,54.845};
const double centerx[layers]={-0.276,-0.275,-0.276,-0.276,-0.275,-0.276,-0.141,-0.140,-0.141,-0.141,-0.140,-0.141};
//const double locations[layers]={-337.9,-335.922,-333.944,-331.966,-329.988,-328.01,-295.05,-293.072,-291.094,-289.166,-287.138,-285.160};
//const double centery[layers]={51.013,50.911,51.155,51.013,50.911,51.155,55.013,54.911,55.155,55.013,54.911,55.155};
//const double centerx[layers]={0};
const pnumber=12;

void res_r2(Int_t event_start=0,Int_t event_end=-1,int pkg=1,Int_t run_start=8658,Int_t run_end=8659){
  time_t start,end;
  time(&start);
    TChain* chain=new TChain("event_tree");
    for(int run=run_start;run<run_end;run++){
    string file_name= Form ( "%s/Qweak_%d_check.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    chain->Add(file_name.c_str());
    }

   
    QwEvent* fEvent=0;
    QwHit* hit=0;
    QwPartialTrack* pt=0;
    TH1F* histo=new TH1F("chi","chi",100,0,500);
    TH1F* slopey=new TH1F("slope in y","slope in y",200,-1,1);
    TH1F* slopex=new TH1F("slope in x","slope in x",200,-1,1);
    TH1F* dt=new TH1F("drift time","drift time",400,0,400);
    chain->SetBranchAddress ( "events",&fEvent );
    Int_t nevents=chain->GetEntries();
    
    cout << "total events:  " << nevents << endl;
    vector< vector<double> > banks;
    vector< double > usedhits;
    vector< double > z;
    vector< int > dirs;
   
    for(int j=0;j<pnumber;++j){
      vector<double> copy;
      banks.push_back(copy);
    }
    

    int ngood=0,nbad=0;
    TH1F* signed_planes[pnumber];
    for(int j=0;j<pnumber;++j)
     signed_planes[j]=new TH1F(Form("signed_residual for plane%d",j),Form("signed_residual for plane%d",(j+1)),200,-1,1);
    TH2F* projection=new TH2F("projection","projection",80,-20,20,100,-10,40);
    int counts=0;
    if(event_end==-1)
      event_end=nevents;

    //starting event loop
    for(int i=event_start;i<event_end;++i){
       chain->GetEntry(i);

       for(int j=0;j<pnumber;++j)
	  banks[j].clear();

       if(i%1000==0){
        cout << "events process so far: " << i << endl; 
       }
	int nhits=fEvent->GetNumberOfHits();
	     
	if(nhits<9) continue;
        
	//first filter:select out the correct event
        int a[pnumber]={0};
        vector<double> ddistance;
		
	for(int j=0;j<nhits;++j){
	  hit=fEvent->GetHit(j);
	  if(hit->GetPlane()<pnumber+1 && hit->GetRegion()==2 && hit->GetDriftDistance()>=0 && hit->GetPackage()==pkg && hit->GetHitNumber()==0){
	    a[hit->GetPlane()-1]++;
	   
		  double dd1,dd2,base;
                  
		  double rCos=0,rSin=0;
		  //double angle=hit->GetDetectorInfo()->GetElementAngle();
		  //cout << "angle: " << angle << endl;
		  if(hit->GetDirection()==1){
		    rCos=1;rSin=-0.004;
		  }
		  else if(hit->GetDirection()==3){
		    rCos=0.597;rSin=-0.802;
		  }
		  else{
		    rCos=0.603;rSin=0.798;
		  }

		  base=centery[hit->GetPlane()-1]*rCos+centerx[hit->GetPlane()-1]*rSin+(hit->GetElement()-1)*wirespace+offset;
		  // dd1 and dd2 will store the drift position
		  dd1=base+hit->GetDriftDistance();
		  dd2=base-hit->GetDriftDistance();
		  ddistance.push_back(hit->GetDriftDistance());
		  banks[hit->GetPlane()-1].push_back(dd1);
		  banks[hit->GetPlane()-1].push_back(dd2);
		  //cout << "plane: " << hit->GetPlane() << ", distance=" << hit->GetElement() << "+" << dd1 << endl;
		  //cout << "plane: " << hit->GetPlane() << ", distance=" << hit->GetElement() << "-" << dd2 << endl;
	  }
	}// end of hits loop

	
	int planes_with_hits=0;
	bool discard=false;
	
        for(int j=0;j<pnumber;++j){
	  if(a[j]!=0)
	    ++planes_with_hits;
      
	  if(a[j]>2){
	    discard=true;
	  }
      	}

	if(discard==true) continue;

	int permutations=1;
	if(planes_with_hits>(pnumber-3)){
	  //a[3]=0;
	  //banks[3].clear();
	  for(int j=0;j<pnumber;++j){
	    if(a[j]!=0)
	    permutations*=(2*a[j]);
	  }
	}
	
      
	if(permutations==1 ||  permutations>5000) continue;
	//cout << "permutations: "<< permutations << " in event " << i << endl;
	
	  
	//int permutations=1 << pnumber;
	
	++counts;
	
 
	
	double chi=0.0,minchi=10000;
	double signed_residual[pnumber]={0};
	double bestsigned[pnumber]={0};
	for(int k=0;k<pnumber;++k){
	  bestsigned[k]=signed_residual[k]=-10;
	}
	double fit[4]={0};
	double bestfit[4]={0};
	int best_per=-1;

	 for(int j=0;j<permutations;++j){
	   //mul_do will loop through banks to fill in usedhits,z,dirs
	
	   mul_do(j,permutations,pnumber,banks,usedhits,z,dirs);
	   trackfit(usedhits,z,dirs,usedhits.size(),chi,signed_residual,fit);
	   /*
	   if(j==486){
	     for(int k=0;k<usedhits.size();++k)
	     cout << usedhits.at(k) << " at plane" << z.at(k) << endl;
	     cout << "fit: " << fit[0] << " " << fit[1] << " " << fit[2] << " " << fit[3] << endl;
	     cout << "pos: " << fit[0]-568.17*fit[1] << " " << fit[2]-568.17*fit[3] << " " << chi << endl;
	   }
	   */
	    
	   if(chi<minchi){
	     minchi=chi;
	     best_per=j;
	     for(int k=0;k<pnumber;++k){
	       bestsigned[k]=signed_residual[k];
	     }
	     for(int k=0;k<4;++k)
	       bestfit[k]=fit[k];
	   }
	   chi=0;
	   for(int k=0;k<pnumber;++k)
	     signed_residual[k]=-10;
	   z.clear();
	   usedhits.clear();
	   dirs.clear();
	 } // end of for permutation loop
	 /*
	  if(best_per!=-1){
	 mul_do(best_per,permutations,pnumber,banks,usedhits,z,dirs);
	 for(int k=0;k<usedhits.size();++k)
	   cout << usedhits.at(k) << " at plane" << z.at(k) << endl;
	 }
	 */

	 if(minchi!=10000){
	   histo->Fill(minchi);
	   //cout << "x,y(z=0) = (" << bestfit[0] << " cm, " << bestfit[2] << " cm), d(x,y)/dz = (" << bestfit[1] << ", " << bestfit[3] << "), chi=" << 100*minchi << endl;
	   projection->Fill(bestfit[0]-568.17*bestfit[1],bestfit[2]-568.17*bestfit[3]);
	   slopey->Fill(bestfit[3]);
	   slopex->Fill(bestfit[1]);
	   for(int k=0;k<pnumber;++k){
	     if(bestsigned[k]!=-10)
	     signed_planes[k]->Fill(bestsigned[k]);
	   }
	 }    // end of if statment
	
    }    // ends of the loop for event
    // cout << "good events: " << ngood << " bad events: " << nbad << endl;
    //histo->Draw();
    
    cout << "total number of qualified events: " << counts << endl;
    TCanvas* c=new TCanvas("c","c",800,600);
    
    c->Divide(3,5);
    for(int k=0;k<pnumber;++k){
      c->cd(k+1);
      signed_planes[k]->Draw();
    }
    c->cd(pnumber+1);
    slopey->Draw();
    c->cd(pnumber+2);
    slopex->Draw();
    c->cd(pnumber+3);
    
    gStyle->SetPalette(1);
    projection->SetTitle("projection to collmator 1: z=-568.17");
    projection->GetXaxis()->SetTitle("x axis:cm");
    projection->GetYaxis()->SetTitle("y axis:cm");
    projection->Draw("colz");
    //projection_profile->SetMaximum(30);
    //projection_profile->SetMinimum(6);
    //projection_profile->Draw("colz");
    
    
    // ds coll1
    double highy=18.46,middley=12.63,lowy=7.03;
    double highx=3.65,middlex=3.65,lowx=1.91;
    //double highy=53.7,middley=37.1,lowy=30.57;
    //double highx=9.2,middlex=9.2,lowx=6.5;
    //double highy=77.5,middley=51,lowy=39.95;
    //double highx=14.5,middlex=14.5,lowx=10.1;
    TLine* t1=new TLine(-highx,highy,highx,highy);
    TLine* t2=new TLine(-lowx,lowy,lowx,lowy);
    TLine* t3=new TLine(-highx,highy,-middlex,middley);
    TLine* t4=new TLine(highx,highy,middlex,middley);
    TLine* t5=new TLine(-middlex,middley,-lowx,lowy);
    TLine* t6=new TLine(middlex,middley,lowx,lowy);
    t1->Draw("same");
    t2->Draw("same");
    t3->Draw("same");
    t4->Draw("same");
    t5->Draw("same");
    t6->Draw("same");
    time(&end);
    double dif=difftime(end,start);
    cout << "time consuming " << dif << " seconds" << endl;
    return;
}

 

// this function is only responsible for matrix calculation
int trackfit(vector<double>& distance,
	      vector<double>& location,
	      vector<int>& dirs,
	      double& num,
	      double& chi,
	     double* signed_residual,
	     double* fit
	     )
{
  //initialize
        double r[4]={0.0};
        double A[4][4]={{0},{0},{0},{0}};
	double *Ap = &A[0][0];
	double covp[4][4]={{0},{0},{0},{0}};
	double *cov = &covp[0][0];
	double B[4]={0};
        double resolution=0.01;
	// make sure fit is equal to zero
	for(int i=0;i<4;++i)
	  fit[i]=0;
	
  	for ( int i = 0; i < num; ++i )
	  {
		double norm = 1.0 / ( resolution * resolution );
		double rSin=0,rCos=0;
		if(dirs.at(i)==0){
		  rSin=-0.004;rCos=1;
	        }
		else if(dirs.at(i)==1){
		  rSin=-0.802;rCos=0.597;
		}
		else{
		  rSin=0.798;rCos=0.603;
		}
	
	        r[0] = rSin;
		r[1] = rSin * location.at(i);
		r[2] = rCos;
		r[3] = rCos * location.at(i);
		for ( int k = 0; k < 4; ++k )
		{
		  //	B[k] += norm * r[k] * ( hits[i]->GetDriftDistance() - x0[dir] );
		  B[k]+=norm*r[k]*distance.at(i);
			for ( int j = 0; j < 4; ++j )
				A[k][j] += norm * r[k] * r[j];
		}
	}
	

	M_Invert ( Ap, cov, 4 );
	M_A_times_b ( fit, cov, 4, 4, B );

	chi = 0.0;
	for ( int i = 0; i < num; ++i )
	{		
	        double z=location.at(i);
		double x=fit[0]+fit[1]*z;
		double y=fit[2]+fit[3]*z;
   
		double rSin=0,rCos=0;
		if(dirs.at(i)==0){
		  rSin=-0.004;rCos=1;
	        }
		else if(dirs.at(i)==1){
		  rSin=-0.802;rCos=0.597;
		}
		else{
		  rSin=0.798;rCos=0.603;
		}
	
     
		
		double residual=y*rCos+x*rSin-distance.at(i);
		int plane=find_plane(location.at(i));
		signed_residual[plane]=residual;
		chi+=fabs(residual*residual);
	
	}
	chi/=(num-4);
	chi=sqrt(chi);
	return 0;

}

double *M_Unit (double *A, const int n)
{
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      A[i*n+j] = 0;
    }
    A[i*n+i] = 1;
  }

  return A;
}


void RowMult (const double a, double *A, const double b, const double *B, const int n)
{
  for (int i = 0; i < n; ++i)
    A[i] = a*A[i] + b*B[i];

  return;
}


double* M_Invert (double *Ap, double *Bp, const int n)
{
  double *row1, *row2;
  double a=0.0,b=0.0;

  // Initialize B to unit matrix (wdc: assumed n = 4 here)
  M_Unit (Bp, n);

  // Print matrices Ap and Bp
  //M_Print (Ap, Bp, n);

  // This will not be generalized for nxn matrices.
  // (wdc: TODO  faster algs could be useful later)
  if (n == 2) {
    double det = (Ap[0] * Ap[3]) - (Ap[1] * Ap[2]);
    Bp[0] =  Ap[0] / det;
    Bp[1] = -Ap[1] / det;
    Bp[2] = -Ap[2] / det;
    Bp[3] =  Ap[3] / det;
  }

  if (n == 4) {
    // Calculate inverse matrix using row-reduce method.
    // (No safety for singular matrices!)

    // get A10 to An0 to be zero.
    for (int i = 1; i < n; i++) {
      a = -(Ap[0]);
      b = Ap[i*n];
      row1 = &(Ap[i*n]);
      row2 = &(Ap[0]);
      RowMult(a, row1, b, row2, 4);
      row1 = &(Bp[i*n]);
      row2 = &(Bp[0]);
      RowMult(a, row1, b, row2, 4);
    }

    // get A21 and A31 to be zero
    for (int i = 2; i < n; i++) {
      a = -(Ap[5]);
      b = Ap[i*n+1];
      row1 = &(Ap[i*n]);
      row2 = &(Ap[4]);
      RowMult(a, row1, b, row2, 4);
      row1 = &(Bp[i*n]);
      row2 = &(Bp[4]);
      RowMult(a, row1, b, row2, 4);
    }

    // get A32 to be zero
    a = -(Ap[10]);
    b = Ap[14];
    row1 = &(Ap[12]);
    row2 = &(Ap[8]);
    RowMult(a, row1, b, row2, 4);
    row1 = &(Bp[12]);
    row2 = &(Bp[8]);
    RowMult(a, row1, b, row2, 4);

    // Now the matrix is upper right triangular.

    // row reduce the 4th row
    a = 1/(Ap[15]);
    row1 = &(Ap[12]);
    RowMult(a, row1, 0, row2, 4);
    row1 = &(Bp[12]);
    RowMult(a, row1, 0, row2, 4);

    // get A03 to A23 to be zero
    for (int i = 0; i < n-1; i++) {
      a = -(Ap[15]);
      b = Ap[i*n+3];
      row1 = &(Ap[i*n]);
      row2 = &(Ap[12]);
      RowMult(a, row1, b, row2, 4);
      row1 = &(Bp[i*n]);
      row2 = &(Bp[12]);
      RowMult(a, row1, b, row2, 4);
    }

    // row reduce the 3rd row
    a = 1/(Ap[10]);
    row1 = &(Ap[8]);
    RowMult(a, row1, 0, row2, 4);
    row1 = &(Bp[8]);
    RowMult(a, row1, 0, row2, 4);

    // get A02 and A12 to be zero
    for (int i = 0; i < 2; i++) {
      a = -(Ap[10]);
      b = Ap[i*n+2];
      row1 = &(Ap[i*n]);
      row2 = &(Ap[8]);
      RowMult(a, row1, b, row2, 4);
      row1 = &(Bp[i*n]);
      row2 = &(Bp[8]);
      RowMult(a, row1, b, row2, 4);
    }

    // row reduce the 2nd row
    a = 1/(Ap[5]);
    row1 = &(Ap[4]);
    RowMult(a, row1, 0, row2, n);
    row1 = &(Bp[4]);
    RowMult(a, row1, 0, row2, n);

    // get A01 to be zero
    a = -(Ap[5]);
    b = Ap[1];
    row1 = &(Ap[0]);
    row2 = &(Ap[4]);
    RowMult(a, row1, b, row2, n);
    row1 = &(Bp[0]);
    row2 = &(Bp[4]);
    RowMult(a, row1, b, row2, n);

    // row reduce 1st row
    a = 1/(Ap[0]);
    row1 = &(Ap[0]);
    RowMult(a, row1, 0, row2, n);
    row1 = &(Bp[0]);
    RowMult(a, row1, 0, row2, n);
  }

  return Bp;
}


double *M_A_times_b (double *y, const double *A, const int n, const int m, const double *b)
{
  for (int i = 0; i < n; i++) {
    y[i] = 0;
    for (int j = 0; j < m; j++) {
      y[i] += A[i*n+j] * b[j];
    }
  }
  return y;
}

void mul_do (
    int i,
    int mul,
    int l,
    vector<vector<double> > & r,
    vector<double>& used,
    vector<double>& z,
    vector<int>& dirs
         )
{

	int s;

	if ( i == 0 )
	{
	  int index=0;
		for ( int j = 0; j < l; ++j )
		{              
		  //	ha[j] = hx[j][0];
		  if(r[j].size()==0) continue;
		  used.push_back(r[j].at(0));
		  z.push_back(locations[j]);
		  dirs.push_back(j%3);
		}
	}
	else
	{
	  int index=0;
		for ( int j = 0; j < l; ++j )
		{
		  if(r[j].size()==0) continue;
		  if ( r[j].size() == 1 )
				s = 0;
			else
			{
			  s = i % (r[j].size());
			  i /= (r[j].size());
			}
		  used.push_back(r[j].at(s));
		  z.push_back(locations[j]);
		  dirs.push_back(j % 3);
		}
	}
	return;
}


double weight_lsq (
    vector<double>& usedhits,	//!< list of hits in every plane
    vector<double>& z
    )		//!< number of planes with hits
{
	// Declaration of matrices
  int n=z.size();
  double sum_x=0.0,sum_y=0.0,sum_x2=0.0,sum_xy=0.0;
  double a=0,b=0;
  for(int i=0;i<n;++i){
    sum_x+=z.at(i);
    sum_y+=usedhits.at(i);
    sum_x2+=(z.at(i)*z.at(i));
    sum_xy+=(usedhits.at(i)*z.at(i));
  }
  
  
  a=(sum_y*sum_x2-sum_x*sum_xy)/(n*sum_x2-sum_x*sum_x);
  b=(n*sum_xy-sum_x*sum_y)/(n*sum_x2-sum_x*sum_x);
  
  //calculate average residual
	
        double sum_residual=0;       
	for ( int i = 0; i < n; i++ )
	{
	  double residual = usedhits.at(i)-(b*z.at(i)+a);
          sum_residual+=fabs(residual);
	}


	sum_residual/=n;
	return sum_residual;

}



void check_events(Int_t event_start=0,Int_t event_end=-1,int pkg=1,Int_t run=8658){
  time_t start,end;
  time(&start);
   bool fDebug=false;
   string file_name=Form("%s/Qweak_%d_check.root",gSystem->Getenv ( "QW_ROOTFILES" ),run);
   TFile* file=new TFile(file_name.c_str());
   
    QwEvent* fEvent=0;
    QwHit* hit=0;
    QwTrack* track=0;
    QwPartialTrack* pt=0;

    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    Int_t nevents=event_tree->GetEntries();
    cout << "total events: " << nevents << endl;
    if(event_end==-1)
      event_end=nevents;

    event_tree->SetBranchStatus("events",1);
    TBranch* event_branch=event_tree->GetBranch("events");
    event_branch->SetAddress(&fEvent);
  
   
  
    for(int i=event_start;i<event_end;++i){

      if(i%10000==0)
	cout << "events processed so far: " << i << endl;
      
     
      event_branch->GetEntry(i);
      double npts=fEvent->GetNumberOfPartialTracks();

      double chi=0;
      // call build function
      for(int j=0;j<npts;++j){
	pt=fEvent->GetPartialTrack(j);
	if(pt->GetRegion()!=2 || pt->GetPackage()!=pkg) continue;
	cout << "partial track found in event: " << i+1 << endl;
	double x=pt->fOffsetX-568.17*pt->fSlopeX;
	double y=pt->fOffsetY-568.17*pt->fSlopeY;
	//cout << "coll position: "<< x << " " << y <<" in event " << i << " with chi: " << pt->fChi << endl;
      }
    }  // end of for loop over events
    
}
   
   
int find_plane(double z){
  int plane=-1;
  int first=0,last=pnumber;
  while(last>=first){
    int i=(first+last)/2;
    if(z==locations[i]){
      plane=i;
      break;
    }
    else if(z>locations[i]){
      first=i+1;
    }
    else{
      last=i-1;
    }
  }
  if(plane==-1){
    cerr << "error happens!" << endl;
  }
 
  return plane;
}


void test_linear(){

  //double Dx=2.3112;
  double space=1.1684;
  double Dx=2.1174;
  //double z[4]={-336.522,-330.588,-293.822,-287.888};
  //double z[3]={-334.544,-328.61,-285.91};
  double z[3]={-338.5,-295.8,-289.866};
  double y[3]={55.5019,62.2409,63.2114};
  double w[3]={16,17,18};
  /*
  for(int i=0;i<3;++i){
      y[i]=(w[i]-0.5)*space+y[i];
      if(i!=0)
	y[i]+=Dx;
  }
  */
  TGraph* t=new TGraph(3,z,y);
  TF1 *f=new TF1("m1","pol1");
  t->Fit("m1");
  t->SetMarkerSize(1);
  t->SetMarkerStyle(21);
  t->Draw("AP");
  double res=0;
  for(int i=0;i<3;++i)
    res+=fabs(f->Eval(z[i])-y[i]);
  cout << "res: " << res/3 << endl;
  return;

}



/*
16.5822 at plane-334.544
16.3814 at plane-328.61
18.4155 at plane-285.91



distance: 15.4602 z:-334.544
distance: 15.661 z:-328.61
distance: 18.2925 z:-285.91
*/
