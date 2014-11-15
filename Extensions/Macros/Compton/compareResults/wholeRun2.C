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
const Int_t runBegin = 23463, runEnd = 23484;///test range
//const Int_t runBegin = 22659, runEnd = 25546;///whole run2 range
//const Int_t runBegin = 24248, runEnd = 24269;//range of stable scaler rates
Int_t wholeRun2(Int_t group=30, Int_t run1=runBegin, Int_t run2=runEnd) {
    gROOT->SetStyle("publication");
    //Bool_t debug=1;//,debug1=0;
    Bool_t bCEGroup =0;
    Bool_t bCE    =0;
    Bool_t bEnergy=0;
    Bool_t bPol   =0;
    Bool_t bPolEr =0;
    Bool_t bChiSq  =0;
    Bool_t bCycles=1;
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
    TH1D *hPolEr = new TH1D("polEr", "pol error", runRange,0,0.1);
    TH1D *hCEEr = new TH1D("CEEr", "error in CE", runRange, 0, 0.1);
    TH1D *hChiSq = new TH1D("chiSqr ", "chi sqr ", runRange, 0, 0.1);
    TH1D *hCycles = new TH1D("goodCycles", "number of lasCyc used", runRange, 0, 0.1);
    hPolEr->SetBit(TH1::kCanRebin);
    hCEEr->SetBit(TH1::kCanRebin);
    hChiSq->SetBit(TH1::kCanRebin);
    hCycles->SetBit(TH1::kCanRebin);

    ///read in the good runs' list
    Double_t tRun,tPol,tPolEr,tChiSq,tCE,tCEEr;
    Double_t d1, d2, d3, d4, d5, d6, d7, d8, d9, d10;
    Int_t pl,tCyc, fitStatus;
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
    std::vector<Double_t> rList,pol,polEr,edge,chiSq,edgeEr,goodCyc;
    std::vector<Double_t> roundedCE, diffCE;
    std::vector<Double_t> absPol;
    std::vector<Int_t> fitStat;
    if (fIn.is_open()) { 
        while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>pl >>fitStatus>>tCyc && fIn.good()) {
            if(std::find(goodRuns.begin(), goodRuns.end(), tRun) == goodRuns.end()) continue;
            if(tRun<run1 || tRun>run2) continue; ///the list needs only results between these runs
            if(tPolEr>1.5 || tPolEr<0.06 || fabs(tPol)<80.0 || tCEEr<0.05) {
                //cout<<tRun<<"   "<<tPol<<"   "<<tPolEr<<endl;
                fCheck<<tRun<<"\t"<<tPol<<"\t"<<tPolEr<<endl;
                continue;
            }
            if(tPolEr<0.15) cout<<tRun<<", pol:"<<tPol<<"+/-"<<tPolEr<<", chiSq:"<<tChiSq/tNDF<<", CE:"<<tCE<<"+/-"<<tCEEr<<", fit:"<<fitStatus<<endl;
            rList.push_back((Double_t)tRun);
            absPol.push_back(fabs(tPol));
            pol.push_back(tPol);
            polEr.push_back(tPolEr);
            edge.push_back(tCE);
            edgeEr.push_back(tCEEr);
            chiSq.push_back(tChiSq/tNDF);
            goodCyc.push_back(tCyc);
            fitStat.push_back(fitStatus);

            hChiSq->Fill(tChiSq/tNDF);
            hPolEr->Fill(tPolEr);
            hCEEr->Fill(tCEEr);
            hCycles->Fill(tCyc);
            roundedCE.push_back(round(tCE));
            diffCE.push_back(fabs(tCE - round(tCE)));
            //} //else cout<<"for runum:"<<tRun<<", tPol:"<<tPol<<" +/- "<<tPolEr<<", CE:"<<comptEdge<<endl;
    }
    fIn.close();
} else {
    cout<<red<<file<<" failed to open"<<normal<<endl;
    return -1;
}
cout<<blue<<rList.size()<<" runs finally processed for this analysis from "<<file<<normal<<endl;

///read in the scaler output of the current version of analysis
file = "polScVer001_22Oct14.info";
fIn.open(file);
///read in the polarization values from the version 1 file
std::vector<Double_t> rListSc,polSc,polErSc,edgeSc,chiSqSc,edgeScEr,goodCycSc;
std::vector<Int_t> fitStatSc;
if (fIn.is_open()) { 
    while(fIn >>tRun >>tPol >>tPolEr >>tChiSq >>tNDF >>tCE >>tCEEr >>pl >>fitStatus>>tCyc && fIn.good()) {
        if(std::find(goodRuns.begin(), goodRuns.end(), tRun) == goodRuns.end()) continue;
        if(tRun<run1 || tRun>run2) continue; ///the list needs only results between these runs
        if(tPolEr>1.5 || tPolEr<0.06 || fabs(tPol)<80.0 || tCEEr<0.05) {
            fCheck<<tRun<<"\t"<<tPol<<"\t"<<tPolEr<<endl;
            continue;
        }
        rListSc.push_back((Double_t)tRun);
        polSc.push_back(tPol);
        polErSc.push_back(tPolEr);
        edgeSc.push_back(tCE);
        edgeScEr.push_back(tCEEr);
        chiSqSc.push_back(tChiSq/tNDF);
        goodCycSc.push_back(tCyc);
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
    slug(rList, pol, pol, polEr, runIn, polIn, polInEr, runOut, polOut, polOutEr, 1);
    cout<<magenta<<runIn.size()<<" slugs with HWP IN"<<normal<<endl;
    cout<<blue<<runOut.size()<<" slugs with HWP Out"<<normal<<endl;

    TCanvas *cPol = new TCanvas("cPol","polarization by slug",100,100,1000,400);
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

if(bPolEr) {
    TCanvas *cPolEr = new TCanvas("cPolEr","hist stat uncertainty (pol%)",10,10,500,500);
    hPolEr->GetXaxis()->SetTitle("stat. error pol%");
    hPolEr->Draw("H");
    cPolEr->SaveAs(Form("histPolEr_%d_%d.png",run1,run2));

    ///accross run2
    TCanvas *cPolEr2 = new TCanvas("cPolEr2","PolEr Vs run", 0,0,1000,400);
    cPolEr2->SetGridx();
    TGraph *gr1 = new TGraph((Int_t)rList.size(), rList.data(), polEr.data());
    gr1->SetMarkerStyle(kFullCircle);
    //gr1->SetMarkerSize(0.5);
    gr1->SetMarkerColor(kBlack);
    gr1->GetYaxis()->SetTitle("polEr");
    gr1->GetXaxis()->SetTitle("run number");
    gr1->Draw("AP");
    gr1->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    cPolEr2->SaveAs(Form("polEr_%d_%d.png",run1,run2));

    ///correlation plot
    TCanvas *cPolEr3 = new TCanvas("cPolEr3","statistical uncertainty (pol%)",10,10,500,500);
    cPolEr3->cd();
    cPolEr3->SetGrid();
    TGraph *gPC = new TGraph((Int_t)polEr.size(), absPol.data(), polEr.data());
    gPC->SetMarkerStyle(kOpenCircle);
    gPC->SetMarkerSize(0.5);
    gPC->SetMarkerColor(kBlue);
    gPC->SetLineColor(kBlue);
    gPC->GetXaxis()->SetTitle("polarization (%)");
    gPC->GetYaxis()->SetTitle("statistical uncertainty");
    gPC->Draw("AP");
    cPolEr3->SaveAs(Form("correlation_pol_polEr.png"));
}

if(bChiSq) {
    TCanvas *cChiSq = new TCanvas("cChiSq","fit chi sq (pol%)",10,10,500,500);
    hChiSq->GetXaxis()->SetTitle("ChiSq");
    hChiSq->Draw("H");
    cChiSq->SaveAs(Form("histChiSq_%d_%d.png",run1,run2));

    ///accross run2
    TCanvas *cChiSq2 = new TCanvas("cCqRun2","chi sq Vs run", 0,0,1000,400);
    cChiSq2->SetGridx();
    TGraph *gr1 = new TGraph((Int_t)rList.size(), rList.data(), chiSq.data());
    gr1->SetMarkerStyle(kFullCircle);
    //gr1->SetMarkerSize(0.5);
    gr1->SetMarkerColor(kMagenta);
    gr1->GetYaxis()->SetTitle("chi Sq");
    gr1->GetXaxis()->SetTitle("run number");
    gr1->Draw("AP");
    gr1->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    cChiSq2->SaveAs(Form("chiSq_%d_%d.png",run1,run2));

    ///correlation plot
    TCanvas *cChiSq3 = new TCanvas("cChiSq3","chiSq correlation",10,10,500,500);
    cChiSq3->cd();
    cChiSq3->SetGrid();
    TGraphErrors *gPC = new TGraphErrors((Int_t)absPol.size(), chiSq.data(), absPol.data(), 0, polEr.data());
    gPC->SetMarkerStyle(kOpenCircle);
    gPC->SetMarkerSize(0.5);
    gPC->SetMarkerColor(kBlue);
    gPC->SetLineColor(kBlue);
    gPC->GetXaxis()->SetTitle("chi Sq");
    gPC->GetYaxis()->SetTitle("polarization (%)");
    gPC->Draw("AP");
    cChiSq3->SaveAs(Form("correlation_chiSq_pol.png"));
}

if(bCycles) {
    TCanvas *cCyc1 = new TCanvas("cCyc1","hist Cycles",10,10,500,500);
    hCycles->GetXaxis()->SetTitle("Cycles");
    hCycles->Draw("H");
    cCyc1->SaveAs(Form("histCycles_%d_%d.png",run1,run2));

    TCanvas *cCyc = new TCanvas("cCycle2","Cycles Vs run", 0,0,1000,400);
    cCyc->SetGridx();
    TGraph *gr1 = new TGraph((Int_t)rList.size(), rList.data(), goodCyc.data());
    gr1->SetMarkerStyle(kFullCircle);
    //gr1->SetMarkerSize(0.5);
    gr1->SetMarkerColor(kGreen);
    gr1->GetXaxis()->SetTitle("Run number");
    gr1->GetYaxis()->SetTitle("used laser cycles");
    gr1->Draw("AP");
    gr1->GetXaxis()->SetLimits(runBegin-5,runEnd+5); 
    cCyc->SaveAs(Form("cycles_%d_%d.png",run1,run2));
    ///correlation plot
    TCanvas *cCycle3 = new TCanvas("cCycle3","cCycle3 correlation",10,10,500,500);
    cCycle3->cd();
    cCycle3->SetGrid();
    //TGraphErrors *gPC = new TGraphErrors((Int_t)absPol.size(), goodCyc.data(), absPol.data(), 0, polEr.data());
    TGraph *gPC = new TGraph((Int_t)goodCyc.size(), goodCyc.data(), polEr.data());
    gPC->SetMarkerStyle(kOpenCircle);
    gPC->SetMarkerSize(0.5);
    gPC->SetMarkerColor(kBlue);
    gPC->SetLineColor(kBlue);
    gPC->GetXaxis()->SetTitle("used cycles");
    gPC->GetYaxis()->SetTitle("polarization error(%)");
    gPC->Draw("AP");
    cCycle3->SaveAs(Form("correlation_cycles_polEr.png"));
}

if(bCEGroup) {
    //if(debug) cout<<blue<<"Compton edge analysis"<<normal<<endl; 
    std::vector<Double_t> wmRuns, wmCE, wmCEEr;
    cluster(group, rList, edge, edgeEr, wmRuns, wmCE, wmCEEr);
    cout<<blue<<wmRuns.size()<<" runs after cluster of CE"<<normal<<endl;
    TCanvas *c2 = new TCanvas("CE","Compton edge rounded",100,100,1000,400);
    c2->SetGridx();
    TGraphErrors *gr1 = new TGraphErrors((Int_t)wmRuns.size(), wmRuns.data(), wmCE.data(), 0, wmCEEr.data());
    gr1->SetMarkerStyle(kOpenSquare);
    gr1->SetMarkerSize(0.5);
    gr1->SetMarkerColor(kRed);
    gr1->Draw("AP");
    //TGraphErrors *gr2 = new TGraphErrors((Int_t)rList.size(), rList.data(), roundedCE.data(), 0, edgeEr.data());
    //TGraphErrors *gr2 = new TGraphErrors((Int_t)rList.size(), rList.data(), diffCE.data(), 0, edgeEr.data());
    //TGraph *gr2 = new TGraph((Int_t)rList.size(), rList.data(), diffCE.data());
    //TGraph *gr2 = new TGraph((Int_t)rList.size(), edgeEr.data(), diffCE.data());

    //TGraph *gr2 = new TGraph((Int_t)rList.size(), edgeEr.data(), polEr.data());
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
    slug(rList, pol, edge, edgeEr, runCEIn, edgeIn, edgeInEr, runCEOut, edgeOut, edgeOutEr, 0);
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

