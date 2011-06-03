
// author: Siyuan Yang
//       : College of William & Mary
//       : sxyang@email.wm.edu
// calculate the residual from combinations of hits
 
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>

// this work is to repeat Mark's calculation in VT but instead of cosmic ray data, using the real data
// here, we only apply the calculation on one chamber


using namespace std;
const int layers=12;
const int ndirs=3;
const double wirespace=1.1684;
const double offset=-18.1102;
const double location[layers]={-338.10,-336.122,-334.144,-332.166,-330.188,-328.120,-295.400,-293.422,-291.444,-289.466,,-287.488,-285.510};
const double centery[layers]={51.013,50.711,50.955,51.013,50.711,50.955,55.013,54.711,54.955,55.013,54.711,54.955};



void res_r2(Int_t event_start=0,Int_t event_end=-1,Int_t run_start=8658,Int_t run_end=8659){
    Int_t start=0,end=0;
    TChain* chain=new TChain("event_tree");
    for(int run=run_start;run<run_end;run++){
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    chain->Add(file_name.c_str());
    }

   
    QwEvent* fEvent=0;
    QwHit* hit=0;
    //QwTrack* track=0;
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
    for(int j=0;j<6;j++){
      vector<double> copy;
      banks.push_back(copy);
    }
    
    if(event_end==-1)
      event_end=nevents;

    int ngood=0,nbad=0;
    TH1F* signed_planes[6];
    for(int j=0;j<6;j++)
     signed_planes[j]=new TH1F(Form("signed_residual for plane%d",j),Form("signed_residual for plane%d",(j+1)),200,-1,1);
     TH2F* projection=new TH2F("projection","projection",80,-20,20,100,-10,40);
    int counts=0;
    //starting event loop
    for(int i=event_start;i<event_end;i++){

       chain->GetEntry(i);
       for(int j=0;j<layers;j++)
	  banks[j].clear();

       if(i%100==0){
        cout << "events process so far: " << i << endl; 
       }
	int nhits=fEvent->GetNumberOfHits();
	
	if(nhits<6 || nhits >24) continue;
        
	int pkg=0;
	bool ignore=false;

	//first filter:select the out the correct event
	bool cleanevent=true;
        int a[6]={0};
        vector<double> ddistance;
	for(int j=0;j<nhits;j++){
	  hit=fEvent->GetHit(j);
	  if(hit->GetPlane()<7 && hit->GetHitNumber()==0){
	    // cout << "wire: " << hit->GetElement() << "in event: " << i << endl;
	    a[hit->GetPlane()-1]++;
	    if(hit->GetPackage()==pkg || pkg==0){
	         pkg=hit->GetPackage();
		  double dd1,dd2,base;

		  double rCos=0,rSin=0;
		  if(hit->GetDirection()==1){
		    rCos=1;rSin=0;
		  }
		  else if(hit->GetDirection()==3){
		    rCos=0.6;rSin=-0.8;
		  }
		  else{
		    rCos=0.6;rSin=0.8;
		  }

		  // centerx is always equal to zero
		  base=centery[hit->GetPlane()-1]*rCos+(hit->GetElement()-1)*wirespace+offset;
		  // dd1 and dd2 will store the drift position
		  //cout << "distance: " << hit->GetDriftDistance() << endl;
		  dd1=base+hit->GetDriftDistance();
		  dd2=base-hit->GetDriftDistance();
		  ddistance.push_back(hit->GetDriftDistance());
		  banks[hit->GetPlane()-1].push_back(dd1);
		  banks[hit->GetPlane()-1].push_back(dd2);
		  }
		  else{
		    //    cerr<< "double packages being found!" << endl;
		    ignore=true;
		    break;
		  }
	  }
	}// end of hits loop

	 

	
	for(int j=0;j<6;j++){
	  if(a[j]>1 || a[j]==0){
	    cleanevent=false;
	    break;
	  }
	}

        vector<double>::iterator it;
	if(cleanevent==true){
	it=find(ddistance.begin(),ddistance.end(),-5);
	if(it!=ddistance.end()){
	  cleanevent=false;
	}
	} // end of if statement

      	 if(cleanevent==false) continue;
	if(ignore==false)
	  ngood++;
	else if(ignore==true){
	  nbad++;
	  continue;
	}
    
	//cout << "event: " << i << endl;
	//cout << "pass event:" << i << endl;
	// count how many missed planes in this event
	//int npts=fEvent->GetNumberOfPartialTracks();
	//for(int k=0;k<npts;k++){
	// pt=fEvent->GetPartialTrack(k);
	//  projection->Fill(pt->fOffsetX-568.17*pt->fSlopeX,pt->fOffsetY-568.17*pt->fSlopeY);
	//   }
	  
	int permutations=64;
	counts++;
	
	//	cout << "permutations: " << permutations << endl;
	
	double chi=0.0,minchi=10000;
	double signed_residual[6]={0};
	double bestsigned[6]={0};
	double fit[4]={0};
	double bestfit[4]={0};
	
	 for(int j=0;j<permutations;j++){
	   chi=0.0;
	   //mul_do will loop through banks to fill in usedhits,z,dirs
	   mul_do(j,permutations,6,banks,usedhits,z,dirs);
	   trackfit(usedhits,z,dirs,6,chi,signed_residual,fit);
	   if(chi<minchi){
	     minchi=chi;
	     for(int k=0;k<6;k++)
	       bestsigned[k]=signed_residual[k];
	     for(int k=0;k<4;k++)
	       bestfit[k]=fit[k];
	   }
	   z.clear();
	   usedhits.clear();
	   dirs.clear();
	 } // end of for permutation loop
	
	 if(minchi!=10000){
	   histo->Fill(minchi);
	   projection->Fill(bestfit[0]-568.17*bestfit[1],bestfit[2]-568.17*bestfit[3]);
	   slopey->Fill(bestfit[3]);
	   slopex->Fill(bestfit[1]);
	   for(int k=0;k<6;k++){
	     //cout << "bestsigned[" << k << "]=" <<  bestsigned[k] << endl;
	     signed_planes[k]->Fill(bestsigned[k]);
	   }
	 }    // end of if statment
	
    }    // ends of the loop for event
    // cout << "good events: " << ngood << " bad events: " << nbad << endl;
    //histo->Draw();
    
    cout << "total number of hits: " << counts << endl;
    TCanvas* c=new TCanvas("c","c",800,600);
    
    c->Divide(3,3);
    for(int k=0;k<6;k++){
      c->cd(k+1);
      signed_planes[k]->Draw();
    }
    c->cd(7);
    slopey->Draw();
    c->cd(8);
    slopex->Draw();
    //dt->Draw();
    c->cd(9);
    
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
    
    return;
}

 

// this function is only responsible for matrix calculation
int trackfit(vector<double> distance,
	      vector<double> location,
	      vector<int> dirs,
	      double& num,
	      double& chi,
	     double* signed_residual,
	     double* fit)
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
	for(int i=0;i<4;i++)
	  fit[i]=0;

  	for ( int i = 0; i < num; i++ )
	  {
		double norm = 1.0 / ( resolution * resolution );
		double rSin=0,rCos=0;
		if(dirs.at(i)==0){
		  rSin=0;rCos=1;
	        }
		else if(dirs.at(i)==1){
		  rSin=-0.8;rCos=0.6;
		}
		else{
		  rSin=0.8;rCos=0.6;
		}
	
	        r[0] = rSin;
		r[1] = rSin * location.at(i);
		r[2] = rCos;
		r[3] = rCos * location.at(i);
		for ( int k = 0; k < 4; k++ )
		{
		  //	B[k] += norm * r[k] * ( hits[i]->GetDriftDistance() - x0[dir] );
		  B[k]+=norm*r[k]*distance.at(i);
			for ( int j = 0; j < 4; j++ )
				A[k][j] += norm * r[k] * r[j];
		}
	}
	
	M_Invert ( Ap, cov, 4 );
	M_A_times_b ( fit, cov, 4, 4, B );

	chi = 0.0;
	for ( int i = 0; i < num; i++ )
	{		
	        double z=location.at(i);
		double x=fit[0]+fit[1]*z;
		double y=fit[2]+fit[3]*z;
   
		double rSin=0,rCos=0;
		if(dirs.at(i)==0){
		  rSin=0;rCos=1;
	        }
		else if(dirs.at(i)==1){
		  rSin=-0.8;rCos=0.6;
		}
		else{
		  rSin=0.8;rCos=0.6;
		}
     
		
		double residual=y*rCos+x*rSin-distance.at(i);
		signed_residual[i]=residual;
		chi+=fabs(residual*residual);
	
	}
	chi/=(num-4);
	chi=sqrt(chi);
	return 0;

}

double *M_Unit (double *A, const int n)
{
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      A[i*n+j] = 0;
    }
    A[i*n+i] = 1;
  }

  return A;
}


void RowMult (const double a, double *A, const double b, const double *B, const int n)
{
  for (int i = 0; i < n; i++)
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
		for ( int j = 0; j < l; j++ )
		{              
		  //	ha[j] = hx[j][0];
		  //if(r[j].size()==0) continue;
		  used.push_back(r[j].at(0));
		  z.push_back(location[j]);
		  dirs.push_back(j%3);
		}
	}
	else
	{
	  int index=0;
		for ( int j = 0; j < l; j++ )
		{
		  //if(r[j].size()==0) continue;
		  if ( r[j].size() == 1 )
				s = 0;
			else
			{
			  s = i % (r[j].size());
			  i /= (r[j].size());
			}
		  //	ha[j] = hx[j][s];
		  used.push_back(r[j].at(s));
		  z.push_back(location[j]);
		  dirs.push_back(j%3);
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
  for(int i=0;i<n;i++){
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


void build(){
  double zx=-338.10,zu=-336.122,zv=-334.144;
  double slopex=0.11283,offsetx=49.9989;
  double slopeu=0.0262245,offsetu=16.6384;
  double slopev=0.0935466,offsetv=53.3007;

  double centerx=51.013,centeru=50.711,centerv=50.955;
    Int_t start=0,end=0;
    TChain* chain=new TChain("event_tree");
    for(int run=run_start;run<run_end;run++){
    string file_name= Form ( "%s/Qweak_%d_base.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    chain->Add(file_name.c_str());
    }

   
    QwEvent* fEvent=0;
    QwHit* hit=0;

   
    chain->SetBranchAddress ( "events",&fEvent );
    Int_t nevents=chain->GetEntries();

  double wirespace=1.1684,first=-18.1102;
  double cosu=0.6,sinu=-0.8;
  double cosv=0.6,sinv=0.8;

  double x1=offsetx+slopex*zx;
  double u1=offsetu+slopeu*zx-(centeru-centerx)*0.6;
  double v1=offsetv+slopev*zx-(centerv-centerx)*0.6;

  double ux=u1*cosu,uy=u1*sinu;
  double vx=v1*cosv,vy=v1*sinv;

  x1-=0.5*wirespace;
  x1+=first;

  ux-=0.5*wirespace;
  ux+=first;

  vx-=0.5*wirespace;
  vx+=first;
  //three planes
  // first x
  double para[3][4];
  para[0][0]=1;
  para[0][1]=0;
  para[0][2]=-slopex;
  para[0][3]=-(x1+para[0][2]*zx);

  // first uvoid 
  para[1][0]=-0.75;
  para[1][1]=1;
  para[1][2]=fabs(slopeu*para[1][0]);
  para[1][3]=-(para[1][0]*ux+para[1][1]*uy+para[1][2]*zx);

 // first v
  para[2][0]=0.75;
  para[2][1]=1;
  para[2][2]=-fabs(slopev*para[1][0]);
  para[2][3]=-(para[2][0]*vx+para[2][1]*vy+para[2][2]*zx);
 


  // set two points
  double z1=0,z2=-439;
  // question is: how do we want to combine, three optionsvoid 


  y1=((para[0][0]*para[2][2]-para[0][2]*para[2][0])*z1+(para[0][0]*para[2][3]-para[0][3]*para[2][0]))/(para[0][1]*para[2][0]-para[0][0]*para[2][1]);
  x1=-(para[0][1]*y1+para[0][2]*z1+para[0][3])/para[0][0];

  y2=((para[0][0]*para[2][2]-para[0][2]*para[2][0])*z2+(para[0][0]*para[2][3]-para[0][3]*para[2][0]))/(para[0][1]*para[2][0]-para[0][0]*para[2][1]);
  x2=-(para[0][1]*y1+para[0][2]*z2+para[0][3])/para[0][0];

  double fit[4]={0};
  fit[3]=(x1-x2)/(z1-z2);
  fit[2]=x1-fit[1]*z1;
 

  fit[1]=(y1-y2)/(z1-z2);
  fit[0]=y1-fit[3]*z1;

  fit[2]+=centerx;
  
  cout << "x,y(z=0)=(" << fit[0] << "," << fit[2] << ") d(x,y)/dz=(" << fit[1] << "," << fit[3] << ")" << endl;  
  return;
}


  void filter_ar(Int_t event_start=0,Int_t event_end=-1,Int_t run_start=8599,Int_t run_end=8600){
    
    TChain* chain=new TChain("event_tree");
    for(int run=run_start;run<run_end;run++){
    string file_name= Form ( "%s/Qweak_%d_base.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    chain->Add(file_name.c_str());
    }


    TH1F* ar_x=new TH1F("treeline: x ar","treeline: x ar",100,0,0.3);
    TH1F* ar_u=new TH1F("treeline: u ar","treeline: u ar",100,0,0.3);
    TH1F* ar_v=new TH1F("treeline: v ar","treeline: v ar",100,0,0.3);
   
    QwEvent* fEvent=0;
    QwPartialTrack* pt=0;
    QwTrackingTreeLine* tl=0;
    QwHit* hit=0;
    chain->SetBranchAddress ( "events",&fEvent);
 

    Int_t nevents=chain->GetEntries();
     
    if(event_end==-1)
      event_end=nevents;

    for(int i=event_start;i<event_end;i++){
     
       if(i%100==0)
	cout << "events processed so far: " << i << endl;
      chain->GetEntry(i);

      	bool cleanevent=true;
        int a[6]={0};
	int nhits=fEvent->GetNumberOfHits();
	for(int j=0;j<nhits;j++){
	  hit=fEvent->GetHit(j);
	  if(hit->GetPlane()<7 && hit->GetHitNumber()==0)
	    a[hit->GetPlane()-1]++;
	}

	for(int j=0;j<6;j++){
	  if(a[j]>1 || a[j]==0){
	    cleanevent=false;
	    break;
	  }
	}
	
      int npts=fEvent->GetNumberOfPartialTracks();
      if(npts!=0 && cleanevent==true){
	for(int j=0;j<fEvent->GetNumberOfTreeLines();j++){
	  tl=fEvent->GetTreeLine(j);
	  double ar=tl->GetAverageResidual();
	  if(tl->GetDirection()==1)
	    ar_x->Fill(ar);
	  else if(tl->GetDirection()==3)
	    ar_u->Fill(ar);
	  else
	    ar_v->Fill(ar);
	}   // end of treeline loop
      }    // end of event loop
    }
    TCanvas* c=new TCanvas("c","c",800,600);
    c->Divide(1,3);
    c->cd(1);
    ar_x->Draw();
    c->cd(2);
    ar_u->Draw();
    c->cd(3);
    ar_v->Draw();

      return;

}

// compare the scattering angle in y from pt(uv combined) with the y directly from treeline x

  void comparey(Int_t event_start=0,Int_t event_end=-1,Int_t direction=1,Int_t run_start=8658,Int_t run_end=8659){
    
    TChain* chain=new TChain("event_tree");
    for(int run=run_start;run<run_end;run++){
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    chain->Add(file_name.c_str());
    }


    TH1F* uv=new TH1F("slope diff","slope diff",100,-0.3,0.3);
    TH1F* vertex=new TH1F("vertex","vertex",300,-1000,-400);
    TH1F* vertex_tl=new TH1F("vertex from treeline","vertex",300,-1000,-400);
    QwEvent* fEvent=0;
    QwPartialTrack* pt=0;

   
    chain->SetBranchAddress ( "events",&fEvent);
 

    Int_t nevents=chain->GetEntries();
     
   


    if(event_end==-1)
      event_end=nevents;

    for(int i=event_start;i<event_end;i++){

      if(i%10000==0)
	cout << "events processed so far: " << i << endl;
      
      chain->GetEntry(i);

      double npts=fEvent->GetNumberOfPartialTracks();
      bool has_pt=false;
      double slopey=0,slopex=0;
     
      for(int j=0;j<npts;j++){
	pt=fEvent->GetPartialTrack(j);
	if(pt->GetRegion()!=2) continue;
	
	has_pt=true;
	slopey=pt->fSlopeY;
        
	slopex=pt->TSlope[1];
	uv->Fill(slopey-slopex);

	vertex->Fill(-pt->fOffsetY/pt->fSlopeY);
	vertex_tl->Fill(-(pt->TOffset[1]+32.4086)/pt->TSlope[1]);
        //tl=pt->fTreeLine[1];

	//if(tl)
        //cout << "direction: " << slopex << " event " << i << endl;
	

      }

      /* if(has_pt==false) continue;
      double ntls=fEvent->GetNumberOfTreeLines();
      for(int j=0;j<ntls;j++){
	tl=fEvent->GetTreeLine(j);
	if(tl->GetRegion()==2 && tl->GetDirection()==1){
	  double diff=slopey-tl->fSlope;
	  y->Fill(diff);
	}
      }
      */
    }
 

    TCanvas* c=new TCanvas("c","c",800,600);
    
    //y->SetLineColor(kBlue);
    //y->Draw();

    vertex->SetLineColor(kRed);
    vertex->Draw();
    vertex_tl->SetLineColor(kBlue);
    vertex_tl->Draw("same");

      return;

}


 void reconstruction(Int_t event_start=0,Int_t event_end=-1,Int_t run_start=8658,Int_t run_end=8659){
    
    TChain* chain=new TChain("event_tree");
    for(int run=run_start;run<run_end;run++){
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    chain->Add(file_name.c_str());
    }


   
    TH1F* vertex=new TH1F("vertex in Z","vertex in Z",300,-1000,-400);
    TH1F* vertexr=new TH1F("vertex in R","vertex in R",100,0,10);
    TH1F* test=new TH1F("test","test",100,-1,1);
    TH2F* projection=new TH2F("projection","projection",80,-20,20,100,-10,40);
    TProfile2D* projection_profile=new TProfile2D("projection related with number of hits","projection related with hits",80,-20,20,100,-10,40);
    //TH1F* slopex=new TH1F("slope in x","slope in x",100,-0.2,0.2);
    
    // only calcualte from those events which has tracks
    QwEvent* fEvent=0;
    QwTrack* track=0;
    QwPartialTrack* pt=0;
    QwHit* hit=0;
    double meanresidual[24]={0};
    double sigmaresidual[24]={0};
    double hitsnumber[24]={0};
    double planenumber[24]={0};
    TH1F* signedresidual[24];
    for(int i=0;i<24;i++){
      signedresidual[i]=new TH1F(Form("projection in plane %d",i+1),Form("projection in plane %d",i+1),100,-1,1);
    }
    chain->SetBranchAddress ( "events",&fEvent);
    Int_t nevents=chain->GetEntries();

    if(event_end==-1)
      event_end=nevents;

    for(int i=event_start;i<event_end;i++){

      if(i%10000==0)
	cout << "events processed so far: " << i << endl;
      
      chain->GetEntry(i);

      double ntracks=0;
      ntracks=fEvent->GetNumberOfTracks();

      //if(ntracks!=0) cout << "event " << i << " has a track!" << endl;
      //fit will store the final results
      double fit[4]={0};
      
      //get rid of those events which has two partial tracks
      double npts=0;
      npts=fEvent->GetNumberOfPartialTracks();

      nhits=fEvent->GetNumberOfHits();
      int hit_in_r2=0;
      for(int j=0;j<nhits;j++){
	hit=fEvent->GetHit(j);
	if(hit->GetRegion()==2 && hit->GetHitNumber()==0)
	  hit_in_r2++;
      }
      // call build function
      if(ntracks!=0){
	//	cout << "npts: " << npts << endl;
      for(int j=0;j<npts;j++){
	pt=fEvent->GetPartialTrack(j);
	if(pt->GetRegion()!=2) continue;
	
		double vertexz=-(pt->fSlopeX*pt->fOffsetX + pt->fSlopeY*pt->fOffsetY)/(pt->fSlopeX*pt->fSlopeX+pt->fSlopeY*pt->fSlopeY);
		double vertex_r=sqrt((pt->fSlopeX*vertexz+pt->fOffsetX)*(pt->fSlopeX*vertexz+pt->fOffsetX)+(pt->fSlopeY*vertexz+pt->fOffsetY)*(pt->fSlopeY*vertexz+pt->fOffsetY));
		//filled in signed residual
		/*
		for(int k=0;k<12;k++){
		  if(pt->fSignedResidual[k]!=0){
		    //hitsnumber[(pt->GetPackage()-1)*12+k]++;
		    //meanresidual[(pt->GetPackage()-1)*12+k]+=pt->fSignedResidual[k];}
		    signedresidual[(pt->GetPackage()-1)*12+k]->Fill(pt->fSignedResidual[k]);
		}
		}   // end of for loop
		*/
		//	if(pt->GetPackage()==1 && pt->fSignedResidual[0]!=0){
		// test->Fill(pt->fSignedResidual[0]);
		//}
		  
		/*		
       if(pt->fChi<1000){
	 if(vertexz>-500) cout << "event: " << i << endl;
	vertex->Fill(vertexz);
	vertexr->Fill(vertex_r);
		}
		*/
	projection->Fill(pt->fOffsetX-568.17*pt->fSlopeX,pt->fOffsetY-568.17*pt->fSlopeY);
	// projection_profile->Fill(pt->fOffsetX-568.17*pt->fSlopeX,pt->fOffsetY-568.17*pt->fSlopeY,pt->fChi,1);
	 projection_profile->Fill(pt->fOffsetX-568.17*pt->fSlopeX,pt->fOffsetY-568.17*pt->fSlopeY,hit_in_r2,1);
         }
       } 
     }
    /*
    double errorx[24]={0};
    for(int i=0;i<24;i++){
      meanresidual[i]=signedresidual[i]->GetMean();
      sigmaresidual[i]=signedresidual[i]->GetRMS();
      planenumber[i]=i+1;
    }
    */
   
    TCanvas* c=new TCanvas("c","c",800,600);
    vertex->Draw();
    //signedresidual[0]->Draw();
    //test->Draw();
     //test->SetTitle("residual distribution from plane 1 in package 1 ");
     //test->GetXaxis()->SetTitle("cm");
    //TGraphErrors* tg=new TGraphErrors(24,planenumber,meanresidual,errorx,sigmaresidual);
    /*
    TGraph* tg=new TGraph(24,planenumber,meanresidual);
    tg->Draw("ap");
    tg->SetMarkerSize(0.7);
    tg->SetMarkerStyle(20);
    tg->SetMarkerColor(kRed);
    tg->GetXaxis()->SetTitle("plane number");
    tg->GetYaxis()->SetTitle("average residual");
    */
         
    gStyle->SetPalette(1);
    projection->SetTitle("projection to collmator 2: z=-568.17");
    projection_profile->GetXaxis()->SetTitle("x axis:cm");
    projection_profile->GetYaxis()->SetTitle("y axis:cm");
    //projection->Draw("colz");
    projection_profile->SetMaximum(30);
    projection_profile->SetMinimum(6);
    projection_profile->Draw("colz");
    
    
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
    
    /* c->Divide(1,2);
    c->cd(1);
    vertex->SetLineColor(kRed);
    vertex->GetXaxis()->SetTitle("z axis: cm");
    vertex->Draw();
    vertex_comp->SetLineColor(kBlue);
    vertex_comp->Draw("same");
    c->cd(2);
    
    vertexr_comp->SetLineColor(kBlue);
    vertexr->GetXaxis()->SetTitle("r: cm");
    vertexr_comp->Draw();
    
    vertexr->SetLineColor(kRed);
   
    vertexr->Draw("same");
    */
    //slopex->Draw();
    return;

 }

void buildtrack(double slope1,double offset1,double slope2,double offset2,double slope3,double offset3,double* fit)
{

  // cout << "slope1: " << slope1 << " " << offset1 << " " << slope2 << " " << offset2 << endl;
  double zx=-338.10,zu=-336.122,zv=-334.144;
  double centerx=51.013,centeru=50.711,centerv=50.955;
  double wirespace=1.1684,first=-18.1102;
  double slopex=slope1,offsetx=offset1;
  double slopeu=slope2,offsetu=offset2;
  double slopev=slope3,offsetv=offset3;

  double cosu=0.6,sinu=-0.8;
  double cosv=0.6,sinv=0.8;

  double x=offsetx+slopex*zx;
  double u=offsetu+slopeu*zx-(centeru-centerx)*cosu;
  double v=offsetv+slopev*zx-(centerv-centerx)*cosv;

  double ux=u*cosu,uy=u*sinu;
  double vx=v*cosv,vy=v*sinv;

  x-=0.5*wirespace;
  x+=first;

  ux-=0.5*wirespace;
  ux+=first;

  vx-=0.5*wirespace;
  vx+=first;
  //three planes
  // first x
  double para[3][4];
  para[0][0]=1;
  para[0][1]=0;
  para[0][2]=-slopex;
  para[0][3]=-(x+para[0][2]*zx);

  // first u
  para[1][0]=-0.75;
  para[1][1]=1;
  para[1][2]=fabs(slopeu*para[1][0]);
  para[1][3]=-(para[1][0]*ux+para[1][1]*uy+para[1][2]*zx);

 // first v
  para[2][0]=0.75;
  para[2][1]=1;
  para[2][2]=-fabs(slopev*para[2][0]);
  para[2][3]=-(para[2][0]*vx+para[2][1]*vy+para[2][2]*zx);
 


  // set two points
  double z1=-63,z2=-439;
  // question is: how do we want to combine, three optionsvoid 
  double x1=0,x2=0,y1=0,y2=0;
  double i=1,j=2; // means u,v
  
  y1=((para[i][0]*para[j][2]-para[i][2]*para[j][0])*z1+(para[i][0]*para[j][3]-para[i][3]*para[j][0]))/(para[i][1]*para[j][0]-para[i][0]*para[j][1]);
  x1=-(para[i][1]*y1+para[i][2]*z1+para[i][3])/para[i][0];

  y2=((para[i][0]*para[j][2]-para[i][2]*para[j][0])*z2+(para[i][0]*para[j][3]-para[i][3]*para[j][0]))/(para[i][1]*para[j][0]-para[i][0]*para[j][1]);
  x2=-(para[i][1]*y2+para[i][2]*z2+para[i][3])/para[i][0];

  //fit[3]=(x1-x2)/(z1-z2);
  //fit[2]=x1-fit[1]*z1;
 

  fit[1]=(y1-y2)/(z1-z2);
  fit[0]=y1-fit[1]*z1;

  fit[3]=slopex;
  fit[2]=offsetx+centerx+first-0.5*wirespace;
  //fit[2]+=centerx;
  
  // cout << "x,y(z=0)=(" << fit[0] << "," << fit[2] << ") d(x,y)/dz=(" << fit[1] << "," << fit[3] << ")" << endl;  
  return;
}

/*

 void signed_check(Int_t event_start=0,Int_t event_end=-1,Int_t run_start=8658,Int_t run_end=8659){
    
    TChain* chain=new TChain("event_tree");
    for(int run=run_start;run<run_end;run++){
    string file_name= Form ( "%s/Qweak_%d_base.root",gSystem->Getenv ( "QW_ROOTFILES" ),run );
    chain->Add(file_name.c_str());
    }


   
    double signed[24]={0};
 

    
    QwEvent* fEvent=0;
    QwTrack* track=0;
    QwPartialTrack* pt=0;

  
    chain->SetBranchAddress ( "events",&fEvent);
    Int_t nevents=chain->GetEntries();

    
    if(event_end==-1)
      event_end=nevents;

    for(int i=event_start;i<event_end;i++){

      if(i%10000==0)
	cout << "events processed so far: " << i << endl;
      
      chain->GetEntry(i);

      double ntracks=0;
      ntracks=fEvent->GetNumberOfTracks();

      if(ntracks!=0){

	double npts=fEvent->GetNumberOfPartialTracks();
      }

*/
