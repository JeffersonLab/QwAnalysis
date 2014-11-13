////////////////////////////////////////////////////////////////////
///This macro reads in a file having all information for Run2 from only one
//version of the analysis and plots the quantities over the whole of run2.
//This is not meant for comparison between different analysis versions. The
//good runs list or any other cuts on the data should be made while creating
//that mega file which this macro will analyze. 
////////////////////////////////////////////////////////////////////
#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
//#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/comptonRunConstants.h"
#include "cluster.C"
#include "simpleAvg.C"
#include "slug.C"

//const Int_t runBegin = 23220, runEnd = 23530;///runlet comparison range
const Int_t runBegin = 22659, runEnd = 25546;///whole run2 range
//const Int_t runBegin = 24248, runEnd = 24269;//range of stable scaler rates
Int_t wholeRun2(Int_t group=30, Int_t run1=runBegin, Int_t run2=runEnd)
{
    gROOT->SetStyle("publication");
    //Bool_t debug=1;//,debug1=0;
    Bool_t bCEGroup =0;
    Bool_t bCE =1;
    Bool_t bEnergy = 0;
    Bool_t bPol = 1;
    TString file;
    ifstream fIn;
    ofstream fCheck;
    //file = "polAcVer001_22Oct14.info";
    const TString destDir="/w/hallc/compton/users/narayan/svn/Compton/compareResults/";
    std::vector<Double_t> dummy;
    //TString s1, s2, s3, s4, s5, s6, s7, s8, s9, s10;
    TF1 *lFit = new TF1("lFit", "pol0", runBegin, runEnd);
    lFit->SetLineColor(kBlack);

    Int_t runRange = run2-run1+1;
    //TH1D *hPolEr = new TH1D("polEr", "pol error", runRange,0,0.1);
    TH1D *hCEEr = new TH1D("CEEr", "error in CE", runRange, 0, 0.1);
    //TH1D *hChiSq = new TH1D("chiSqr ", "chi sqr ", runRange, 0, 0.1);
    hCEEr->SetBit(TH1::kCanRebin);

    ///read in the good runs' list
    Double_t tRun,tPol,tPolEr,tChiSq,tCE,tCEEr;
    Double_t d1, d2, d3, d4, d5, d6, d7, d8, d9, d10;
    Int_t pl,goodCyc, fitStatus;
    Int_t tNDF;
    std::vector<Double_t> goodRuns;
    fIn.open("/w/hallc/compton/users/narayan/my_scratch/data/goodList_longPol.txt");
    if(fIn.is_open()) {
        while(fIn>>tRun && fIn.good()) {
            goodRuns.push_back(tRun);
        }
        fIn.close();
    } else cout<<red<<"couldn't open the list of good runs"<<normal<<endl;
    cout<<blue<<goodRuns.size()<<" runs found in the goodList"<<endl;

    fCheck.open("wholeRun2_excludedRuns.txt");
    //file = "polAcVer002_29Oct14.info";
    file = "polAcVer005_12Nov14.info";
    fIn.open(file);
    ///read in the polarization values from the version 1 file
    std::vector<Double_t> rListV1,polV1,polErV1,edge,chiSqV1,edgeEr,goodCycV1;
    std::vector<Double_t> roundedCE, diffCE;
    std::vector<Int_t> fitStat;
    if (fIn.is_open()) { 
        while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>pl >>fitStatus>>goodCyc && fIn.good()) {
            if(std::find(goodRuns.begin(), goodRuns.end(), tRun) == goodRuns.end()) continue;
            if(tRun<run1 || tRun>run2) continue; ///the list needs only results between these runs
            if(tPolEr>1.5 || tPolEr<0.05 || fabs(tPol)<80.0 || tCEEr<0.05) {
                //cout<<tRun<<"   "<<tPol<<"   "<<tPolEr<<endl;
                fCheck<<tRun<<"\t"<<tPol<<"\t"<<tPolEr<<endl;
                continue;
            }
            //cout<<tRun<<"   "<<tPol<<"   "<<tPolEr<<endl;
            rListV1.push_back((Double_t)tRun);
            //polV1.push_back(fabs(tPol));
            polV1.push_back(tPol);
            polErV1.push_back(tPolEr);
            edge.push_back(tCE);
            edgeEr.push_back(tCEEr);
            chiSqV1.push_back(tChiSq/tNDF);
            goodCycV1.push_back(goodCyc);
            fitStat.push_back(fitStatus);

            hCEEr->Fill(tCEEr);
            roundedCE.push_back(round(tCE));
            diffCE.push_back(fabs(tCE - round(tCE)));
            //} //else cout<<"for runum:"<<tRun<<", tPol:"<<tPol<<" +/- "<<tPolEr<<", CE:"<<comptEdge<<endl;
    }
    fIn.close();
} else {
    cout<<red<<file<<" failed to open"<<normal<<endl;
    return -1;
}
cout<<blue<<rListV1.size()<<" runs found in "<<file<<normal<<endl;

///read in the scaler output of the current version of analyssi
file = "polScVer001_22Oct14.info";
fIn.open(file);
///read in the polarization values from the version 1 file
std::vector<Double_t> rListSc,polSc,polErSc,edgeSc,chiSqSc,edgeScEr,goodCycSc;
std::vector<Int_t> fitStatSc;
if (fIn.is_open()) { 
    while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>pl >>fitStatus>>goodCyc && fIn.good()) {
        if(std::find(goodRuns.begin(), goodRuns.end(), tRun) == goodRuns.end()) continue;
        if(tRun<run1 || tRun>run2) continue; ///the list needs only results between these runs
        if(tPolEr>1.5 || tPolEr<0.05 || fabs(tPol)<80.0 || tCEEr<0.05) {
            //cout<<tRun<<"   "<<tPol<<"   "<<tPolEr<<endl;
            fCheck<<tRun<<"\t"<<tPol<<"\t"<<tPolEr<<endl;
            continue;
        }
        //cout<<tRun<<"   "<<tPol<<"   "<<tPolEr<<endl;
        rListSc.push_back((Double_t)tRun);
        //polSc.push_back(fabs(tPol));
        polSc.push_back(tPol);
        polErSc.push_back(tPolEr);
        edgeSc.push_back(tCE);
        edgeScEr.push_back(tCEEr);
        chiSqSc.push_back(tChiSq/tNDF);
        goodCycSc.push_back(goodCyc);
        fitStatSc.push_back(fitStatus);
    }
    fIn.close();
} else {
    cout<<red<<file<<" failed to open"<<normal<<endl;
    return -1;
}
cout<<blue<<rListSc.size()<<" runs found in "<<file<<normal<<endl;
fCheck.close();

if(bPol) {
    Bool_t bMoller = 1;
    std::vector<Double_t> runIn, polIn, polInEr; 
    std::vector<Double_t> runOut, polOut, polOutEr; 
    slug(rListV1, polV1, polV1, polErV1, runIn, polIn, polInEr, runOut, polOut, polOutEr, 1);
    cout<<magenta<<runIn.size()<<" slugs with HWP IN"<<normal<<endl;
    cout<<blue<<runOut.size()<<" slugs with HWP Out"<<normal<<endl;

    TCanvas *cPol = new TCanvas("pol%","polarization by slug",100,100,1000,400);
    cPol->SetGridx();
    TGraphErrors *grM, *gr1, *gr2;
    TMultiGraph *gr = new TMultiGraph();
    if(bMoller) {
        grM = new TGraphErrors("/w/hallc/compton/users/narayan/my_scratch/data/moller_run2.txt","%lg %lg %lg");
        grM->SetMarkerColor(kGreen);
        grM->SetMarkerStyle(kFullSquare);
        gr->Add(grM);
    }
    gr1 = new TGraphErrors((Int_t)runIn.size(), runIn.data(), polIn.data(), 0, polInEr.data());
    gr1->SetMarkerColor(kRed);
    gr1->SetMarkerStyle(kOpenSquare);
    gr2 = new TGraphErrors((Int_t)runOut.size(), runOut.data(), polOut.data(), 0, polOutEr.data());
    gr2->SetMarkerColor(kBlue);
    gr2->SetMarkerStyle(kOpenSquare);

    gr->Add(gr1);
    gr->Add(gr2);
    gr->Draw("AP");
    gr->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    gr->GetXaxis()->SetTitle("Run number"); 
    gr->GetYaxis()->SetTitle("polarization (%)");
    TLegend *leg = new TLegend(0.8,0.78,0.90,0.95);
    leg->AddEntry(gr1,"HWP In","lpe");
    leg->AddEntry(gr2,"HWP Out","lpe");
    leg->AddEntry(grM,"Moller","lpe");
    leg->SetFillStyle(0);//transparent
    leg->Draw();

    cPol->SaveAs(Form("slugPol_%d_%d.png",run1,run2));
}

if(bCEGroup) {
    //if(debug) cout<<blue<<"Compton edge analysis"<<normal<<endl; 
    std::vector<Double_t> wmRuns, wmCE, wmCEEr;
    cluster(group, rListV1, edge, edgeEr, wmRuns, wmCE, wmCEEr);
    cout<<blue<<wmRuns.size()<<" runs after cluster of CE"<<normal<<endl;
    TCanvas *c2 = new TCanvas("CE","Compton edge rounded",100,100,1000,400);
    c2->SetGridx();
    TGraphErrors *gr1 = new TGraphErrors((Int_t)wmRuns.size(), wmRuns.data(), wmCE.data(), 0, wmCEEr.data());
    gr1->SetMarkerStyle(kOpenSquare);
    gr1->SetMarkerSize(0.5);
    gr1->SetMarkerColor(kRed);
    gr1->Draw("AP");
    //TGraphErrors *gr2 = new TGraphErrors((Int_t)rListV1.size(), rListV1.data(), roundedCE.data(), 0, edgeEr.data());
    //TGraphErrors *gr2 = new TGraphErrors((Int_t)rListV1.size(), rListV1.data(), diffCE.data(), 0, edgeEr.data());
    //TGraph *gr2 = new TGraph((Int_t)rListV1.size(), rListV1.data(), diffCE.data());
    //TGraph *gr2 = new TGraph((Int_t)rListV1.size(), edgeEr.data(), diffCE.data());

    //TGraph *gr2 = new TGraph((Int_t)rListV1.size(), edgeEr.data(), polErV1.data());
    //gr2->SetMarkerStyle(kFullSquare);
    //gr2->SetMarkerColor(kBlue);
    //gr2->GetXaxis()->SetTitle("edge error"); 
    ////gr2->GetYaxis()->SetTitle("Compton edge");
    ////gr2->GetYaxis()->SetTitle("abs(CE - int(CE))");
    //gr2->GetYaxis()->SetTitle("pol error");
    //gr2->Draw("AP");

    gr1->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    gr1->GetXaxis()->SetTitle("Run number"); 
    c2->SaveAs(Form("CE_%d_%d_G%d.png",run1,run2,group));
}

if(bCE) {
    std::vector<Double_t> runCEIn, edgeIn, edgeInEr; 
    std::vector<Double_t> runCEOut, edgeOut, edgeOutEr; 
    slug(rListV1, polV1, edge, edgeEr, runCEIn, edgeIn, edgeInEr, runCEOut, edgeOut, edgeOutEr, 0);
    cout<<blue<<runCEIn.size()<<" runs after slugging CE"<<normal<<endl;

    TCanvas *cCE = new TCanvas("cCE","Compton edge",10,10,500,500);
    hCEEr->Draw("H");
    cCE->SaveAs(Form("histCE_%d_%d.png",run1,run2));

    TCanvas *c2 = new TCanvas("c2","Compton edge slug",100,100,1000,400);
    c2->SetGridx();
    TGraphErrors *gr1 = new TGraphErrors((Int_t)runCEIn.size(), runCEIn.data(), edgeIn.data(), 0, edgeInEr.data());
    gr1->SetMarkerStyle(kOpenSquare);
    gr1->SetMarkerColor(kRed);
    TGraphErrors *gr2 = new TGraphErrors((Int_t)runCEOut.size(), runCEOut.data(), edgeOut.data(), 0, edgeOutEr.data());
    gr2->SetMarkerStyle(kOpenSquare);
    gr2->SetMarkerColor(kBlue);
    TMultiGraph *gr = new TMultiGraph();
    gr->Add(gr1);
    gr->Add(gr2);
    gr->Draw("AP");
    gr->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    gr->GetXaxis()->SetTitle("Run number"); 
    gr->GetYaxis()->SetTitle("Compton edge");
    TLegend *leg = new TLegend(0.78,0.18,0.90,0.3);
    leg->AddEntry(gr1,"HWP In","lpe");
    leg->AddEntry(gr2,"HWP Out","lpe");
    leg->SetFillStyle(0);//transparent
    leg->Draw();


    c2->SaveAs(Form("slugCE_%d_%d.png",run1,run2));
}

if(bEnergy) {
    file = "beamProp_ver002_10Nov14.info";
    std::vector<Double_t> rListE, hWien, vWien, ihwp1set, ihwp1read, rhwp, ihwp2read, edetPos, meanE, meanE_RMS;
    fIn.open(file);
    if(fIn.is_open()) {
        //fBeamProp<<"run\tHWien\tVWien\t\tIHWP1set\tIHWP1read\tRHWP\tIHWP2read\tedetPos\tmeanE\tmeanE_RMS"<<endl;
        while(fIn>>d1 >>d2 >>d3 >>d4 >>d5 >>d6 >>d7 >>d8 >>d9 >>d10 && fIn.good()) {
            if(std::find(goodRuns.begin(), goodRuns.end(), d1) == goodRuns.end()) continue;
            if(d1<run1 || d1>run2) continue; ///the list needs only results between these runs
            //printf("%f\t%f\t%f\n",d1, d9, d10);

            rListE.push_back(d1);
            meanE.push_back(d9);
            meanE_RMS.push_back(d10);
        }
        fIn.close();
    } else cout<<red<<"failed to open "<<file<<normal<<endl;
    cout<<blue<<rListE.size()<<" runs found in "<<file<<normal<<endl;

    TCanvas *cEn = new TCanvas("Energy","beam energy",200,100,1000,400);
    cEn->SetGridx();
    std::vector<Double_t> wmRuns, wmEnergy;
    simpleAvg(group, rListE, meanE, wmRuns, wmEnergy);
    cout<<blue<<wmRuns.size()<<" runs after averaging energy"<<normal<<endl;
    TGraph *gr1 = new TGraph((Int_t)wmRuns.size(), wmRuns.data(), wmEnergy.data());
    gr1->SetMarkerStyle(kOpenSquare);
    gr1->SetMarkerSize(0.5);
    gr1->SetMarkerColor(kRed);
    gr1->Draw("AP");

    gr1->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    gr1->GetXaxis()->SetTitle("Run number"); 
    cEn->SaveAs(Form("energy_%d_%d_G%d.png",run1,run2,group));
}

return 1;
}

