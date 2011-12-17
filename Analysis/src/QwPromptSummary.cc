
#include "QwPromptSummary.h"

#include "QwColor.h"
#include "QwLog.h"

#include "TROOT.h"

/**
 *  \file   QwPromptSummary.cc
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Friday, December 16 10:55:31 EST 2011
 */

//
//
//
//  PromptSummaryElement
//
//
//
//

ClassImp(PromptSummaryElement)
ClassImp(QwPromptSummary)

PromptSummaryElement::PromptSummaryElement()
{
  fElementName                    = "";
  
  fMean                           = 0.0;
  fMeanError                      = 0.0;
  fMeanUnit                       = "";

  fAsymmetryDifference            = 0.0;
  fAsymmetryDifferenceError       = 0.0;;
  fAsymmetryDifferenceUnit        = "";

  fAsymmetryDifferenceWidth       = 0.0;
  fAsymmetryDifferenceWidthError  = 0.0;
  fAsymmetryDifferenceWidthUnit   = "";

  

};



PromptSummaryElement::PromptSummaryElement(TString name)
{
  fElementName                    = name;
  
  fMean                           = 0.0;
  fMeanError                      = 0.0;
  fMeanUnit                       = "";

  fAsymmetryDifference            = 0.0;
  fAsymmetryDifferenceError       = 0.0;;
  fAsymmetryDifferenceUnit        = "";

  fAsymmetryDifferenceWidth       = 0.0;
  fAsymmetryDifferenceWidthError  = 0.0;
  fAsymmetryDifferenceWidthUnit   = "";

  

};

PromptSummaryElement::~PromptSummaryElement()
{

};

void
PromptSummaryElement::FillData(
				 Double_t mean, 
				 Double_t mean_err, 
				 TString  mean_unit,
				 Double_t asym_diff, 
				 Double_t asym_diff_err, 
				 TString  asym_diff_unit,
				 Double_t asym_diff_width, 
				 Double_t asym_diff_width_err, 
				 TString  asym_diff_width_unit
				 )
{
  fMean                           = mean;
  fMeanError                      = mean_err;
  fMeanUnit                       = mean_unit;

  fAsymmetryDifference            = asym_diff;
  fAsymmetryDifferenceError       = asym_diff_err;
  fAsymmetryDifferenceUnit        = asym_diff_unit;

  fAsymmetryDifferenceWidth       = asym_diff_width;
  fAsymmetryDifferenceWidthError  = asym_diff_width_err;
  fAsymmetryDifferenceWidthUnit   = asym_diff_width_unit;

};


TString
PromptSummaryElement::GetTextSummary()
{
  return Form("%14s | %12.2lf +/- %8.2lf | %+12.2lf +/- %8.2lf | %+12.2lf +/- %8.2lf\n", fElementName.Data(), fMean, fMeanError, fAsymmetryDifference, fAsymmetryDifferenceError, fAsymmetryDifferenceWidth, fAsymmetryDifferenceWidthError);
};

TString
PromptSummaryElement::GetCSVSummary()
{
  return Form("%s,%lf,%lf,%lf,%lf,%lf,%lf", fElementName.Data(), fMean, fMeanError, fAsymmetryDifference, fAsymmetryDifferenceError, fAsymmetryDifferenceWidth, fAsymmetryDifferenceWidthError);
};




QwPromptSummary::QwPromptSummary()
{
  fRunNumber    = 0;
  fRunletNumber = 0;
  fElementList = new TObjArray();
 
  fElementList -> Clear();
  fElementList -> SetOwner(kTRUE);

  fNElements = 0;

  fLocalDebug = kTRUE;
  
  this->SetupElementList();

};


QwPromptSummary::QwPromptSummary(Int_t run_number, Int_t runlet_number)
{
  fRunNumber    = run_number;
  fRunletNumber = runlet_number;

  fElementList = new TObjArray();
 
  fElementList -> Clear();
  fElementList -> SetOwner(kTRUE);

  fNElements = 0;

  fLocalDebug = kFALSE;
  
  this->SetupElementList();

};


QwPromptSummary::~QwPromptSummary()
{
  if(fElementList) delete fElementList; fElementList = NULL;
};



void 
QwPromptSummary::SetupElementList()
{
  this->AddElement(new PromptSummaryElement("charge"));
  this->AddElement(new PromptSummaryElement("target_x"));
  this->AddElement(new PromptSummaryElement("target_y"));
  this->AddElement(new PromptSummaryElement("angle_x"));
  this->AddElement(new PromptSummaryElement("angle_y"));
  this->AddElement(new PromptSummaryElement("energy"));
  
  this->AddElement(new PromptSummaryElement("bcm1"));
  this->AddElement(new PromptSummaryElement("bcm2"));
  this->AddElement(new PromptSummaryElement("bcm5"));
  this->AddElement(new PromptSummaryElement("bcm6"));
  this->AddElement(new PromptSummaryElement("bcm7"));
  this->AddElement(new PromptSummaryElement("bcm8"));
  this->AddElement(new PromptSummaryElement("bpm3h04_effectivecharge"));
  this->AddElement(new PromptSummaryElement("bpm3h09_effectivecharge"));
  this->AddElement(new PromptSummaryElement("3c12x"));
  this->AddElement(new PromptSummaryElement("3c12y"));
  this->AddElement(new PromptSummaryElement("3h04x"));
  this->AddElement(new PromptSummaryElement("3h04y"));
  this->AddElement(new PromptSummaryElement("3h07cx"));
  this->AddElement(new PromptSummaryElement("3h07cy"));
  this->AddElement(new PromptSummaryElement("3h09x"));
  this->AddElement(new PromptSummaryElement("3h09y"));
  this->AddElement(new PromptSummaryElement("3h09bx"));
  this->AddElement(new PromptSummaryElement("3h09by"));

  this->AddElement(new PromptSummaryElement("bcm1-bcm2"));
  this->AddElement(new PromptSummaryElement("bcm1-bcm5"));
  this->AddElement(new PromptSummaryElement("bcm1-bcm6"));
  this->AddElement(new PromptSummaryElement("bcm2-bcm5"));
  this->AddElement(new PromptSummaryElement("bcm2-bcm6"));
  this->AddElement(new PromptSummaryElement("bcm5-bcm6"));

  this->AddElement(new PromptSummaryElement("bcm1-bcm7"));
  this->AddElement(new PromptSummaryElement("bcm1-bcm8"));

  // this->AddElement(new PromptSummaryElement("bcm2-bcm7"));
  // this->AddElement(new PromptSummaryElement("bcm2-bcm8"));

  // this->AddElement(new PromptSummaryElement("bcm5-bcm7"));
  // this->AddElement(new PromptSummaryElement("bcm5-bcm8"));
  // this->AddElement(new PromptSummaryElement("bcm6-bcm7"));
  // this->AddElement(new PromptSummaryElement("bcm6-bcm8"));

  this->AddElement(new PromptSummaryElement("bcm5-bcm7"));
  this->AddElement(new PromptSummaryElement("bcm7-bcm8"));
  



  this->AddElement(new PromptSummaryElement("MD1"));
  this->AddElement(new PromptSummaryElement("MD2"));
  this->AddElement(new PromptSummaryElement("MD3"));
  this->AddElement(new PromptSummaryElement("MD4"));
  this->AddElement(new PromptSummaryElement("MD5"));
  this->AddElement(new PromptSummaryElement("MD6"));
  this->AddElement(new PromptSummaryElement("MD7"));
  this->AddElement(new PromptSummaryElement("MD8"));

  this->AddElement(new PromptSummaryElement("MD1-MD5"));
  this->AddElement(new PromptSummaryElement("MD2-MD6"));
  this->AddElement(new PromptSummaryElement("MD3-MD7"));
  this->AddElement(new PromptSummaryElement("MD4-MD8"));

  this->AddElement(new PromptSummaryElement("MD_AllBars"));
  this->AddElement(new PromptSummaryElement("MD_EvenBars"));
  this->AddElement(new PromptSummaryElement("MD_OddBars"));

  this->AddElement(new PromptSummaryElement("MD_Even-Odd"));
  this->AddElement(new PromptSummaryElement("MD1/5-MD3/7"));
  this->AddElement(new PromptSummaryElement("MD2/6-MD4/8"));

  this->AddElement(new PromptSummaryElement("dslumi1"));
  this->AddElement(new PromptSummaryElement("dslumi2"));
  this->AddElement(new PromptSummaryElement("dslumi3"));
  this->AddElement(new PromptSummaryElement("dslumi4"));
  this->AddElement(new PromptSummaryElement("dslumi5"));
  this->AddElement(new PromptSummaryElement("dslumi6"));
  this->AddElement(new PromptSummaryElement("dslumi7"));
  this->AddElement(new PromptSummaryElement("dslumi8"));

  this->AddElement(new PromptSummaryElement("dslumi_even"));
  this->AddElement(new PromptSummaryElement("dslumi_odd"));
  this->AddElement(new PromptSummaryElement("dslumi_sum"));
  
  this->AddElement(new PromptSummaryElement("uslumi1"));
  this->AddElement(new PromptSummaryElement("uslumi3"));
  this->AddElement(new PromptSummaryElement("uslumi5"));
  this->AddElement(new PromptSummaryElement("uslumi7"));

  this->AddElement(new PromptSummaryElement("uslumi_sum"));



};


void 
QwPromptSummary::AddElement(PromptSummaryElement *in)
{
  Int_t pos = 0;

  pos = fElementList -> AddAtFree(in);
  if(fLocalDebug) {
    printf("AddElement at pos %d\n", pos);
  }

  fNElements++;
  return;
}


PromptSummaryElement *
QwPromptSummary::GetElementByName(TString name)
{

  PromptSummaryElement* an_element = NULL;
  TObjArrayIter next(fElementList);
  TObject* obj = NULL;
  TString get_name = "";

  while ( (obj = next()) )
    {
      an_element = (PromptSummaryElement*) obj;
      get_name   = an_element->GetName();
      if( get_name.Contains(name) ) {
	if(fLocalDebug) {
	  std::cout << "System " << an_element->GetName()
		    << " QwPromptSummary::GetElementByName address at" << an_element << std::endl;
	}
	return an_element;
      }
    }
  
  return NULL;
};

TString
QwPromptSummary::PrintTextSummaryHeader()
{
  
  TString out = "";
  //  TString filename = "";

  //  filename = Form("summary_%d_%d.txt", fRunNumber, fRunletNumber);
  
  out = "======= BEGIN ======= \n";
  out += Form("        RUN = %5d RUNLET %2d \n\n\n\n\n\n", fRunNumber, fRunletNumber);
  return out;
};



TString
QwPromptSummary::PrintTextSummaryTailer()
{
  TString out = "";
  
  out =  " ======== END of SUMMARY======== \n";
  out += " ======== END ======== \n\n";
  out += " Please contact Rakitha Beminiwattha for any queries and suggestions \n";
  out += " rakithab@jlab.org \n";
  return out;
};


TString
QwPromptSummary::PrintCSVHeader()
{
  TString out = "";
  TString filename = "";

  filename = Form("summary_%d_%d.txt", fRunNumber, fRunletNumber);
  
  out = "! This csv file is desinged for making plots easily.\n";
  out += "!See ";
  out += filename;
  out += " in http://qweak.jlab.org/textsummaries/ for theirs units\n";
  out += "!Please contact Jeong Han Lee via jhlee@jlab.org if one has comments or questions.\n";

  return out;
};



void
QwPromptSummary::FillDataInElement(TString  name, 
				   Double_t mean, 
				   Double_t mean_err, 
				   TString  mean_unit, 
				   Double_t asym_diff, 
				   Double_t asym_diff_err, 
				   TString  asym_diff_unit, 
				   Double_t asym_diff_width, 
				   Double_t asym_diff_width_err, 
				   TString  asym_diff_width_unit)
{
  PromptSummaryElement* an_element = NULL;
  an_element = this->GetElementByName(name);
  if(an_element) {
    
    an_element->FillData(mean, mean_err, mean_unit, asym_diff, asym_diff_err, asym_diff_unit, asym_diff_width, asym_diff_width_err, asym_diff_width_unit);
  }
  else {
    if(fLocalDebug) {
      std::cout 
	<< "QwPromptSummary::FillDataInElement : No Element with the name " 
	<< name 
	<<  std::endl;
    }
  }
  return;
};

void
QwPromptSummary::PrintCSV()
{
  printf("-----------------------\n");
  TObjArrayIter next(fElementList);
  TObject* obj = NULL;
  while ( (obj = next()) )
    {
      PromptSummaryElement* an_element = (PromptSummaryElement*) obj;
      std::cout << an_element -> GetCSVSummary() << std::endl;
    }
  printf("-----------------------\n");
  return;
};

void
QwPromptSummary::PrintTextSummary()
{
  printf("-----------------------\n");
  TObjArrayIter next(fElementList);
  TObject* obj = NULL;
  while ( (obj = next()) )
    {
      PromptSummaryElement* an_element = (PromptSummaryElement*) obj;
      std::cout << an_element -> GetTextSummary() << std::endl;
    }
  printf("-----------------------\n");
  return;
};

