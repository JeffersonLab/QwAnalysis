#include <vector>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <string>



gROOT->Reset();

const Int_t kNumberOfChannels=64;
const Int_t kNumberOfSlots=4;
const Short_t LIBNUM=3;
const Char_t* LIBNAME[LIBNUM]=
{
    "QwHit","QwHitContainer","QwHitRootContainer"
};

void
check_libraries()
{
    for ( Short_t i=0;i<3;i++ )
    {
        if ( !TClassTable::GetDict ( Form ( "%s",LIBNAME[i] ) ) )
        {
            gSystem -> Exec ( Form ( "gcc -shared -Wl,-soname,./lib%s.so -o lib%s.so %s/Tracking/src/%s.o %s/Tracking/dictionary/%sDict.o",  LIBNAME[i], LIBNAME[i], gSystem->Getenv ( "QWANALYSIS" ), LIBNAME[i],  gSystem->Getenv ( "QWANALYSIS" ), LIBNAME[i] ) );
            gSystem -> Load ( Form ( "%s/Extensions/Macros/Tracking/lib%s.so",  gSystem->Getenv ( "QWANALYSIS" ), LIBNAME[i] ) );
        }

    }
}

void create_tdc_tree ( Int_t ev_start=-1,Int_t ev_end=-1, Int_t run_number=1672 )
{

    check_libraries();
    TFile* f=new TFile ( Form ( "%s/rootfiles/Qweak_%d.root",getenv ( "QWANALYSIS" ),run_number ) );
    TFile* f_tdc=new TFile ( Form ( "%s/rootfiles/Qweak_%d_TDCBased.root",getenv ( "QWANALYSIS" ),run_number ),"RECREATE" );
    TTree *R=new TTree ( "R","Hit event data tree" );
    TString prebase="";
    std::vector<Int_t> tdc_dt;





    ConstructTDCStructure ( R,prebase,tdc_dt );

    TTree* tree= ( TTree* ) f->Get ( "tree" );
    QwHitRootContainer* hitContainer=NULL;

    tree->SetBranchAddress ( "hits",&hitContainer );
    Int_t nevent=tree->GetEntries();
    if ( ev_start==-1 && ev_end==-1 )
    {
        ev_start=0;
        ev_end=nevent;
    }

    for ( Int_t ev_id=ev_start;ev_id<ev_end;ev_id++ )
    {
        tree->GetEntry ( ev_id );
        Int_t nhit=hitContainer->GetSize();
        QwHit* hit=NULL;
	for ( Int_t index=0;index<kNumberOfChannels*kNumberOfSlots;index++ )
            tdc_dt.at( index ) =1;

        for ( Int_t hit_id=0;hit_id<nhit;hit_id++ )
        {
            hit=hitContainer->GetHit ( hit_id );
            tdc_dt.at(hit->GetChannel()) = hit->GetTime();
	    //cout << hit->GetChannel() << " " << hit->GetTime() << endl;
        }
	
        R->Fill();

	
        


    }
    R->Write();
}


void ConstructTDCStructure ( TTree* tree,TString& prefix,std::vector<Int_t>& values )
{

    TString basename;
    Int_t subscript;
    values.resize(kNumberOfChannels*kNumberOfSlots);
    for ( Int_t i=0;i<kNumberOfSlots;i++ )
    {
        for ( Int_t j=0;j<kNumberOfChannels;j++ )
        {
            basename=prefix;
            basename+="Slot";
            basename+=i;
            basename+="_TDC";
            basename+=j;
            subscript=i*kNumberOfChannels+j;
	    //cout << "basename: " << basename << " subscript: " << subscript << endl;
            tree->Branch ( basename,& ( values.at(subscript) ) );
        }
    }
};
