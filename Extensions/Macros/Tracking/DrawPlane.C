




#include<iostream>
#include<vector>

//author: Siyuan Yang
//email : sxyang@email.wm.edu
// this script is used to draw the hits(drift distance) along with the fitting track in plane0. Now only dedicated to R3.

const int MAXLAYERS=8;
const double firstpos=-69.5751;
const double wirespace=0.496965;
// package 1 and 2
const double delta_perp[2]={39.2835,39.3679};
const double delta_para[2]={37.471,37.4559};

void SetDrawStyle(TGraph* t,const int style=21,const int color=6,const double size=0.5){
        t->SetMarkerStyle(style);
        t->SetMarkerSize(size);
        t->SetMarkerColor(color);
}

void draw(int event_number=0,Int_t run_number=6327){
 
    string file_name= Form ( "%s/Qweak_%d.root",gSystem->Getenv ( "QWSCRATCH" ),run_number );
    TFile *file = new TFile ( file_name.c_str() );
           
    TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
    QwEvent* fEvent=0;
    QwHit* hit=0;
    QwTreeLine* tl=0;

    event_tree->SetBranchAddress ( "events",&fEvent );
    Int_t nevent=event_tree->GetEntries();

    if(event_number<0 || event_number>nevent){
        cout << "event_number is wrong, please check.." << endl;
        return;
    }
    
    TCanvas* c=new TCanvas("c","c",800,800);
    c->Divide(2,2);
        
        event_tree->GetEntry(event_number);
        int ntls=fEvent->GetNumberOfTreeLines();
        double uslope=0,uoffset=0,vslope=0,voffset=0;
        double x[4][15]={0};
        double y[4][15]={0};
        double index[4]={0};

        int package=0;
        if(ntls!=0) package=fEvent->GetTreeLine(0).GetPackage();
        for(int t=0;t<ntls;t++){
                
                tl=fEvent->GetTreeLine(t);
                
                if(tl->GetRegion()!=3)
                    continue;
                int dir=tl->GetDirection();
                if(tl->GetPlane()==0) {
                        if(dir==3){
                        uslope=tl->GetSlope();
                        uoffset=tl->GetOffset(); 
                        }
                        else{
                        vslope=tl->GetSlope();
                        voffset=tl->GetOffset();
                        }
                        continue;                       
                }
                
                hit=tl->GetHit(0);
                int plane=tl->GetPlane(),row=0;
                  if(plane<3)
                row=(dir-3)*2;
                else
                row=(dir-3)*2+1;
                
                
                int count=0,nhits=tl->GetNumberOfHits();
                
                for(int n=0;n<nhits;n++){
                    hit=tl->GetHit(n);
                    
                    if(plane<3){
                    x[row][count]=(hit->GetElement()-1)*wirespace+firstpos;
                    }
                    else if(hit->GetPackage()==1){
                    x[row][count]=(hit->GetElement()-1)*wirespace+firstpos-delta_para[0];
                    }
                    else if(hit->GetPackage()==2){
                    x[row][count]=(hit->GetElement()-1)*wirespace+firstpos+delta_para[1];
                    }
                    y[row][count]=hit->GetDriftDistance();
                    count++;
                }
                index[row]=count;
                }
        //begin to draw
        TCanvas* c=new TCanvas("c","c",600,600);
        c->Divide(2,2);
        for(int i=0;i<4;i++){
                c->cd(i+1);
                const int dim=index[i];
                double x_copy[dim]={0};               
                double y_copy[dim]={0};
                double y_mirror[dim]={0};
                double y_track[dim]={0};
                string title="";
                for(int j=0;j<dim;j++){
                    x_copy[j]=x[i][j];
                    y_copy[j]=y[i][j];
                    y_mirror[j]=-y[i][j];
                    if(i==0){
                    y_track[j]=uslope*x[i][j]+uoffset;
                    title="u front";
                    }
                    else if(i==1){
                    y_track[j]=uslope*x[i][j]-delta_perp[package-1]+uoffset;
                    title="u back";
                    }
                    else if(i==2){
                    y_track[j]=vslope*x[i][j]+voffset;
                    title="v front";
                    }
                    else{
                    y_track[j]=vslope*x[i][j]-delta_perp[package-1]+voffset;
                    title="v back";
                    }
                }
                
                TGraph* tg1=new TGraph(dim,x_copy,y_copy);
                TGraph* tg2=new TGraph(dim,x_copy,y_mirror);
                TGraph* tg3=new TGraph(dim,x_copy,y_track);
                
                SetDrawStyle(tg1);
                SetDrawStyle(tg2);
                SetDrawStyle(tg3,21,3);
                TMultiGraph* tmul=new TMultiGraph();
                tmul->Add(tg1);
                tmul->Add(tg2);
                tmul->Add(tg3);
                tmul->Draw("ap");
                tmul->SetTitle(title.c_str());
        }
    
    return;
}


