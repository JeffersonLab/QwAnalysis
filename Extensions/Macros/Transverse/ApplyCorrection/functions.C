//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : Septembed 25, 2012
//*****************************************************************************************************//
/*
 List of functios used in ApplyCorrection macros
*/

#include "functions.h"
using namespace std;

//***************************************************
//***************************************************
//         make general database query              
//***************************************************
//***************************************************

TString query_regressed(TString datatable, TString detector, TString measurement, TString ihwp, Double_t slug_number){

  
  Bool_t ldebug = true;


  TString output = " sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2))))), sum("+datatable+".n) ";

  
  TString run_quality =  Form("(%s.run_quality_id = '1') ",
			   datatable.Data());

  TString regression = Form("%s.slope_calculation = '%s' and %s.slope_correction = 'off' ",
			    datatable.Data(),reg_set.Data(),datatable.Data()); 

  TString slug_cut =  Form("%s.slug = %f",datatable.Data(),slug_number);

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
    TString query =" SELECT " +output+ " FROM "+datatable+", slow_controls_settings WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".monitor = '"+detector+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = '"+measurement+"' AND target_position = '"+target+"' AND "
    +regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "+slug_cut+" AND "
    +datatable+".error != 0 and "+qtor_current+"; ";

    if(ldebug) std::cout<<"\n"<<query<<std::endl;


  return query;
}

//***************************************************
//***************************************************
//         make database query              
//***************************************************
//***************************************************
TString get_query(TString detector, TString measurement, TString ihwp, Double_t slug_number){

  
  Bool_t ldebug = false;
  TString datatable;

  datatable = "md_data_view";

  TString output = " sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2))))), sum("+datatable+".n) ";

  
  TString run_quality =  Form("(%s.run_quality_id = '1') ",
			   datatable.Data());

  TString regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = '%s' ",
			    datatable.Data(),datatable.Data(),reg_set.Data()); 

  TString slug_cut =  Form("%s.slug = %f",datatable.Data(),slug_number);

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
    TString query =" SELECT " +output+ " FROM "+datatable+", slow_controls_settings WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".detector = '"+detector+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND target_position = '"+target+"' AND "
    +regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "+slug_cut+" AND "
    +datatable+".error != 0 and "+qtor_current+"; ";

  if(ldebug) std::cout<<query<<std::endl;


  return query;
}


//***************************************************
//***************************************************
//         get number of patterns from mdallbars                 
//***************************************************
//***************************************************

Double_t get_patterns(TString detector, TString measurement, TString ihwp, Double_t slug_number){

 
  Bool_t ldebug = false;
  Bool_t status = true;

  TString datatable= "md_data_view";
  Double_t patterns=0;

  
  TString output = "sum("+datatable+".n)";  
  TString run_quality =  Form("(%s.run_quality_id = '1') ",datatable.Data());
  TString regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = '%s' ",datatable.Data(),datatable.Data(),reg_set.Data()); 
  TString slug_cut =  Form("%s.slug = %f",datatable.Data(),slug_number);
  
  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
  TString query =" SELECT " +output+ " FROM "+datatable+", slow_controls_settings WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".detector = '"+detector+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND target_position = '"+target+"' AND "
    +regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "+slug_cut+" AND "
    +datatable+".error != 0 and "+qtor_current+"; ";
  
  if(ldebug) std::cout<<query<<std::endl;
  
  TSQLStatement* stmt = db->Statement(query,100);
  if(!stmt)  {
    db->Close();
    exit(1);
  }
  // process statement
  if (stmt->Process()) {
    // store result of statement in buffer
    stmt->StoreResult();
    
    while (stmt->NextResultRow()) {
      if(stmt->IsNull(0)){
	std::cout<<"# of events are NULL skipping this slug.."<<slug_number<<std::endl;
	Myfile<<"# of events are NULL skipping this slug.."<<slug_number<<std::endl;
	status= false;
	return status;
      }else{
	patterns = (Double_t)(stmt->GetDouble(0));
	if(ldebug) printf(" Slug = %f Patterns = %f\n", slug_number,patterns);
	Myfile<<setw(10)<<"mdallabrs "<<" ihwp "<<setw(4)<<ihwp
	      <<setw(5)<<" patterns in mdallbars ="<<setw(10)<<patterns<<std::endl;
      }
    }
    delete stmt;    
  }
  return patterns;
}

//***************************************************
//***************************************************
//         get data for each octant/detector                 
//***************************************************
//***************************************************

Bool_t get_octant_data(TString devicelist[],TString ihwp, Double_t slug_number, 
		     Double_t value[], Double_t error[])
{
  Bool_t ldebug = false;
  Bool_t status = true;

  for(Int_t i=0 ; i<8 ;i++){
    if(ldebug) {
      printf("Getting data for %20s ihwp %5s ", devicelist[i].Data(), ihwp.Data());
    }
    TString query = get_query(Form("%s",devicelist[i].Data()),"a",ihwp, slug_number);
    TSQLStatement* stmt = db->Statement(query,100);
    if(!stmt)  {
      db->Close();
      exit(1);
    }
       // process statement
      if (stmt->Process()) {
	// store result of statement in buffer
	stmt->StoreResult();

	while (stmt->NextResultRow()) {
	  if(stmt->IsNull(0)){
	    std::cout<<"Value is NULL skipping this slug.."<<slug_number<<std::endl;
	    Myfile<<"Value is NULL skipping this slug.."<<slug_number<<std::endl;
	    status= false;
	    return status;
	  }else{
	    value[i] = (Double_t)(stmt->GetDouble(0))*1e6; // convert to  ppm
	    error[i] = (Double_t)(stmt->GetDouble(1))*1e6; // ppm
	    if(value[i]==NULL){
	      std::cout<<"Value is NULL and error is NULL; Probably wrong IHWP status. exiting.."<<std::endl;
	      exit(1);
	    }
	    if(ldebug) printf(" value = %16.8lf +- %10.8lf [ppm]\n", value[i], error[i]);
	    Myfile<<setw(10)<<devicelist[i]<<" ihwp "<<setw(4)<<ihwp
		  <<setw(5)<<" value ="<<setw(10)<<value[i]
		<<" +- "<<setw(10)<<error[i]<<std::endl;
	  }
	}
      }
      delete stmt;    
  }
  return status;
}

//***************************************************
//***************************************************
//         get data for parameter                 
//***************************************************
//***************************************************


Bool_t get_data(TString devicelist[],TString measurements[], TString ihwp, Double_t slug_number, 
		     Double_t value[], Double_t error[])
{
  Bool_t ldebug = false;
  Bool_t status = false;

  Int_t k = 6;
  for(Int_t i=1 ; i<3 ;i++){
    if(ldebug) {
      printf("Getting data for %20s ihwp %5s ", devicelist[i].Data(), ihwp.Data());
    }
    TString query = query_regressed("beam_view",devicelist[i],measurements[i],ihwp, slug_number);
    TSQLStatement* stmt = db->Statement(query,100);
    if(!stmt)  {
      db->Close();
      exit(1);
    }
       // process statement
      if (stmt->Process()) {
	// store result of statement in buffer
	stmt->StoreResult();

	while (stmt->NextResultRow()) {
	  if(stmt->IsNull(0)){
	    std::cout<<"Value is NULL skipping this slug.."<<slug_number<<std::endl;
	    Myfile<<"Value is NULL skipping this slug.."<<slug_number<<std::endl;
	    status= false;
	    return status;
	  }else{
	    value[i] = (Double_t)(stmt->GetDouble(0))*1e6; // convert to  ppm/nm
	    error[i] = (Double_t)(stmt->GetDouble(1))*1e6; // ppm/nm
	    if(value[i]==NULL){
	      std::cout<<"Value is NULL and error is NULL; Probably wrong IHWP status. exiting.."<<std::endl;
	      exit(1);
	    }
	    if(ldebug) printf(" value = %16.8lf +- %10.8lf [ppm]\n", value[i], error[i]);
	  }
	}
      }
      delete stmt;    
  }
  return status;
}


//***************************************************
//***************************************************
//    Calcualte IN-OUT, fit and return fit results                
//***************************************************
//***************************************************

void fit_and_correct(){


  Double_t x[8];
  Double_t errx[8];
  Double_t valuesin[8]={0};
  Double_t errorsin[8]={0};
  Double_t valuesout[8]={0};
  Double_t errorsout[8]={0};
  Double_t cvalue[8]={0};
  Double_t in_out[8]={0};
  Double_t in_out_err[8]={0};
  Double_t fit[5]={0};

  for(Int_t i =0;i<8;i++){
    x[i] = i+1;   
    errx[i] = 0.0;
  }

  // fit  function
  TF1 *fit_t = new TF1("fit_t","-4.80*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1)))) + [2]",1,8);
  fit_t->SetParName(0,"P_{V}");
  fit_t->SetParName(1,"P_{H}");
  fit_t->SetParName(2,"C");

  // get asymmetris from the two slugs
  get_octant_data(quartz_barsum,"in", slug,  valuesin, errorsin);
  get_octant_data(quartz_barsum,"out", slug+1,valuesout, errorsout);
 

  //Take the weighted difference in the IHWP in and out half wave plate values.
  //Here from IN+OUT ~ 0 we know that IN and OUT are a measurement of the same thing
  //so we can take the weighted error difference when we take IN-OUT.

  Myfile<<"! IN-OUT "<<std::endl;
  for(Int_t i =0;i<8;i++){
    in_out[i]=((valuesin[i]/pow(errorsin[i],2)) - (valuesout[i]/pow(errorsout[i],2))) /((1/pow(errorsin[i],2)) + (1/pow(errorsout[i],2)));
    in_out_err[i]= sqrt(1/((1/(pow(errorsin[i],2)))+(1/pow(errorsout[i],2))));
    Myfile<<"Value IN-OUT = "<<setw(10)<<in_out[i]<<" +-  "<<setw(10)<<in_out_err[i]<<std::endl;
  }

  // Create canvas
  TCanvas * c = new TCanvas("c","",0,0,1000,500);
  c->Draw();
  c->cd();

  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  TString text = Form("Corrected and un-corrected AVG(IN-OUT) of slug %i and slug %i", slug, slug+1);
  TText*t = new TText(0.06,0.3,text);
  t->SetTextSize(0.7);
  t->Draw();

  pad2->cd();
  pad2->SetFillColor(0);

  // draw uncorrected IN-OUT
  TGraphErrors* grp_in_out  = new TGraphErrors(8,x,in_out,errx,in_out_err);
  grp_in_out ->SetMarkerSize(0.8);
  grp_in_out ->SetMarkerStyle(21);
  grp_in_out ->SetMarkerColor(kRed);
  grp_in_out ->Fit("fit_t","B");
  TF1* fit_temp = grp_in_out->GetFunction("fit_t");
  fit_temp->SetLineColor(kRed);
  fit_temp->SetLineStyle(2);
  fit_temp->DrawCopy("same");

  // get fit results
  fit[0]=fit_temp->GetParameter(0);
  fit[1]=fit_temp->GetParError(0);
  fit[2]=fit_temp->GetParameter(1);
  fit[3]=fit_temp->GetParError(1);
  fit[4]=fit_temp->GetParameter(2);
  fit[5]=fit_temp->GetParError(2);
  Myfile<<"Fit results from uncorrected data: [0]cos(phi)-[1]sin(phi)+[2]"<<std::endl;
  Myfile<<"[0] = "<<setw(10)<<fit[0]<<" +- "<<setw(10)<<fit[1]<<std::endl;
  Myfile<<"[1] = "<<setw(10)<<fit[2]<<" +- "<<setw(10)<<fit[3]<<std::endl;
  Myfile<<"[2] = "<<setw(10)<<fit[3]<<" +- "<<setw(10)<<fit[5]<<std::endl;

  // Apply corrections
  Myfile<<"! Corrected values "<<std::endl;
  for(Int_t i =0;i<8;i++){
    cvalue[i]=in_out[i]-( -4.90*(fit[0]*TMath::Cos((TMath::Pi()/180)*(45*i)) - fit[2]*TMath::Sin((TMath::Pi()/180)*(45*i))));
    Myfile<<"Corrected IN-OUT value = "<<setw(10)<<cvalue[i]<<" +- "<<setw(10)<<in_out_err[i]<<std::endl;
  }
  
  // draw corrected data
  TGraphErrors* grp_corrected  = new TGraphErrors(8,x,cvalue,errx,in_out_err);
  grp_corrected ->SetMarkerSize(0.8);
  grp_corrected ->SetMarkerStyle(21);
  grp_corrected ->SetMarkerColor(kBlue);
  grp_corrected ->SetMarkerStyle(3);
  grp_corrected ->Fit("fit_t","B");
  TF1* fit_temp1 = grp_corrected->GetFunction("fit_t");
  fit_temp1->SetLineColor(kBlue);
  fit_temp1->SetLineStyle(2);
  fit_temp1->DrawCopy("same");

  // get fit results
  fit[0]=fit_temp1->GetParameter(0);
  fit[1]=fit_temp1->GetParError(0);
  fit[2]=fit_temp1->GetParameter(1);
  fit[3]=fit_temp1->GetParError(1);
  fit[4]=fit_temp1->GetParameter(2);
  fit[5]=fit_temp1->GetParError(2);
  Myfile<<"Fit results from corrected data: [0]cos(phi)-[1]sin(phi)+[2]"<<std::endl;
  Myfile<<"[0] = "<<setw(10)<<fit[0]<<" +- "<<setw(10)<<fit[1]<<std::endl;
  Myfile<<"[1] = "<<setw(10)<<fit[2]<<" +- "<<setw(10)<<fit[3]<<std::endl;
  Myfile<<"[2] = "<<setw(10)<<fit[3]<<" +- "<<setw(10)<<fit[5]<<std::endl;


  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_in_out);
  grp->Add(grp_corrected);
  grp ->Draw("AP");
  grp ->SetTitle("");
  grp ->GetXaxis()->SetTitle("Octant");
  grp ->GetXaxis()->CenterTitle();
  grp ->GetYaxis()->SetTitle("Asymmetry (ppm)");
  grp ->GetYaxis()->CenterTitle();
  grp->GetXaxis()->SetTitleOffset(1.0);
  grp->GetYaxis()->SetTitleOffset(0.8);

  TPaveStats *stats1 = (TPaveStats*)grp_in_out->GetListOfFunctions()->FindObject("stats");
  stats1->SetTextColor(kRed);
  stats1->SetFillColor(kWhite); 
  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);  

  TPaveStats *stats2 = (TPaveStats*)grp_corrected->GetListOfFunctions()->FindObject("stats");
  stats2->SetTextColor(kBlue);
  stats2->SetFillColor(kWhite); 
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); stats2->SetY1NDC(0.35);stats2->SetY2NDC(0.60);  

  TLegend *legend = new TLegend(0.1,0.75,0.25,0.85,"","brNDC");
  legend->AddEntry(fit_temp, "Un-corrected", "l");
  legend->AddEntry(fit_temp1, "Corrected", "l");
  legend->SetTextFont(132);
  legend->SetFillColor(0);
  legend->Draw("");

  
  // save canvas
  c->Modified();
  c->Update();

  c->Print(Form("slug_%i_%i_",slug,slug+1)+interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_octant_correction_plots_"+reg_set+"_"+database_stem+".C");
  c->Print(Form("slug_%i_%i_",slug,slug+1)+interaction+"_"+qtor_stem+"_"+polar+"_"+target+"_octant_correction_plots_"+reg_set+"_"+database_stem+".png");


}


