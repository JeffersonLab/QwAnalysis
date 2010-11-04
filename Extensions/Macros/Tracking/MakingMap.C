// Name  :   MakingMap.C
// Author:   Siyuan Yang
// email :   sxyang@email.wm.edu sxyang@jlab.org
// Date:     Mon July  5 12:46:20 EST 2010
// Version:  0.0.1
//
//  This is the script to generate the TDC Based root file from the orginal //  root file while the flag kUseTDCHits is turned on in QwTracking.cc
//  another function in this script is draw_peaks and the purpose of it is to //  draw the results of difference between two adjacent tdc channels. You 
//  could also assign the number of bins as the last parameter in the final //  histogram.
 



#include <vector>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <string>



gROOT->Reset();

const Int_t kNumberOfChannels=64;
const Int_t kNumberOfSlots=4;

void create_tdc_tree ( Int_t ev_start=-1,Int_t ev_end=-1, Int_t run_number=5148 )
{
        TFile* f=new TFile ( Form ( "%s/Qweak_%d.root",getenv ( "QWSCRATCH" ),run_number ) );
        TFile* f_tdc=new TFile ( Form ( "%s/Qweak_%d_TDCBased.root",getenv ( "QWSCRATCH" ),run_number ),"RECREATE" );
    TTree *R=new TTree ( "event_tree","Hit event data tree" );
    TString prebase="";
    std::vector<Int_t> tdc_dt;





    ConstructTDCStructure ( R,prebase,tdc_dt );

    TTree* tree= ( TTree* ) f->Get ( "event_tree" );

    QwEvent* fEvent=0;
    QwHit* hit=0;
    tree->SetBranchAddress("events",&fEvent);
    Int_t nevent=tree->GetEntries();
    if ( ev_start==-1 && ev_end==-1 )
    {
        ev_start=0;
        ev_end=nevent;
    }
    cout << "events: " << nevent << endl;

    for ( Int_t ev_id=ev_start;ev_id<ev_end;ev_id++ )
    {
        if ( ev_id % 1000 ==0 ) std::cout << "events processed so far: " << ev_id << std::endl;
        tree->GetEntry ( ev_id );
        Int_t nhit=fEvent->GetNumberOfHits();
        for ( Int_t index=0;index<2*kNumberOfChannels*kNumberOfSlots;index++ )
            tdc_dt.at ( index ) =0;

	
        for ( Int_t hit_id=nhit-1;hit_id>=0;hit_id-- )

        {
            hit=fEvent->GetHit ( hit_id );
            Int_t slot=0;
            if ( hit->GetSubbankID() ==1 )
            {
                if ( hit->GetModule() ==4 ) slot=0;
                else slot=1;
            }
            else if ( hit->GetSubbankID() ==3 )
            {
                if ( hit->GetModule() ==4 ) slot=2;
                else slot=3;
            }
            else continue;

            tdc_dt.at(kNumberOfChannels*kNumberOfSlots+slot*kNumberOfChannels+hit->GetChannel())++;

            tdc_dt.at ( slot*kNumberOfChannels+hit->GetChannel() ) = hit->GetTime();
        }

        R->Fill();
    }
    R->Write();
}


void ConstructTDCStructure ( TTree* tree,TString& prefix,std::vector<Int_t>& values )
{

    TString basename;
    Int_t subscript=0;
    values.resize ( kNumberOfChannels*kNumberOfSlots*2 );
    for ( Int_t i=0;i<kNumberOfSlots;i++ )
    {
        for ( Int_t j=0;j<kNumberOfChannels;j++ )
        {
            basename=prefix;
            basename+="slot";
            basename+=i;
            basename+="_tdc";
            basename+=j;
            subscript=i*kNumberOfChannels+j;
            //cout << "basename: " << basename << " subscript: " << subscript << endl;
            tree->Branch ( basename,& ( values.at ( subscript ) ) );
        }
    }
	
    for ( Int_t i=0;i<kNumberOfSlots;i++ )
    {
        for ( Int_t j=0;j<kNumberOfChannels;j++ )
        {
            basename=prefix;
	    basename+="hits";
            basename+="slot";
            basename+=i;
            basename+="_tdc";
            basename+=j;
            subscript=kNumberOfChannels*kNumberOfSlots+i*kNumberOfChannels+j;
            //cout << "basename: " << basename << " subscript: " << subscript << endl;
            tree->Branch ( basename,& ( values.at ( subscript ) ) );
        }
    }
};



void draw_peaks ( Int_t slot_num=0,Int_t tdc_num=0, Int_t run_number=1672,Int_t bins=600 )
{

    check_libraries();
        TFile* f=new TFile ( Form ( "%s/rootfiles/Qweak_%d_TDCBased.root",getenv ( "QWSCRATCH" ),run_number ) );

    TH1F* h1=new TH1F("h1","h1",bins,-300,300);

    TTree* tree=(TTree*)f->Get("event_tree");
    char firstchan[100]="slot";
    char secondchan[100]="slot";
    string middle="_tdc";
    char buffer[4];
    char buffer1[4];
    char buffer2[4];
    sprintf ( buffer,"%d",slot_num );
    sprintf ( buffer1,"%d",tdc_num );
    sprintf ( buffer2,"%d",++tdc_num );
    strcat ( firstchan,buffer );
    strcat ( firstchan,middle.c_str() );
    strcat ( firstchan,buffer1 );

    strcat ( secondchan,buffer );
    strcat ( secondchan,middle.c_str() );
    strcat ( secondchan,buffer2 );

    char expr[100]="(";
    strcat ( expr,firstchan );
    strcat ( expr,"-" );
    strcat ( expr,secondchan );
    strcat ( expr,")" );
    strcat ( expr,">>h1" );

    char select[200]="";
    strcat ( select,firstchan );
    strcat ( select,"<0&&" );
    strcat ( select,secondchan );
    strcat ( select,"<0&&" );
    strcat ( select,"(" );
    strcat ( select,firstchan );
    strcat ( select,"-" );
    strcat ( select,secondchan );
    strcat ( select,")>-250&&" );
    strcat ( select,"(" );
    strcat ( select,firstchan );
    strcat ( select,"-" );
    strcat ( select,secondchan );
    strcat ( select,")<250" );



    //std::cout << "expr: " << expr << std::endl;
    //std::cout << "select: " << select << std::endl;
    TCanvas* c=new TCanvas ( "c","c",800,600 );
    tree->Draw ( expr,select );
    c->SetTitle ( Form ( "slot%d_tdc%d-slot%d_tdc%d",slot_num,--tdc_num,slot_num,++tdc_num ) );
    c->Update();
}
