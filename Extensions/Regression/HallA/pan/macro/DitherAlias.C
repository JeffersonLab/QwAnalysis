//////////////////////////////////////////////////////////////////////
//
// DitherAlias.macro
//   Bryan Moffit - August 2005
//
//  A class to create aliases to a provided tree containing
//   corrections to detectors using dithering slopes
//

#define DitherAlias_cxx

#include "DitherAlias.h"

DitherAlias::DitherAlias (TTree *tree, UInt_t run) :
    fTree(tree), 
    fRun(run),
    fDitResFile("")
  {
    Load();
    MakeAliases();
  }

DitherAlias::DitherAlias (TTree *tree, const char *filename) : 
    fTree(tree),
    fRun(0),
    fDitResFile(filename)
  {
    Load();
    MakeAliases();
  }

DitherAlias::DitherAlias (const char *filename) : 
    fRun(0),
    fDitResFile(filename)
  {
    Load();
  }

DitherAlias::~DitherAlias() 
{
}

void 
DitherAlias::Load() 
{
  // Detectors for which to look for coefficients
  fDetName.clear();
  fDetName.push_back ("det1");
  fDetName.push_back ("det2");
  fDetName.push_back ("det3");
  fDetName.push_back ("det4");
  fDetName.push_back ("det_l");
  fDetName.push_back ("det_r");
  fDetName.push_back ("det_lo");
  fDetName.push_back ("det_hi");
  fDetName.push_back ("det_all");
  fDetName.push_back ("flumi1");
  fDetName.push_back ("flumi2");
  fDetName.push_back ("blumi1");
  fDetName.push_back ("blumi2");
  fDetName.push_back ("blumi3");
  fDetName.push_back ("blumi4");
  fDetName.push_back ("blumi5");
  fDetName.push_back ("blumi6");
  fDetName.push_back ("blumi7");
  fDetName.push_back ("blumi8");
  
  RedCoefs* rcm = new RedCoefs(fRun,"dither",fDitResFile);
  fMonName = rcm->Monitors();

  vector <Double_t> v;
  for (vector<string>::const_iterator idt = fDetName.begin();
       idt != fDetName.end();
       ++idt)
    {
      v.clear();
      for (vector<string>::const_iterator imt = fMonName.begin();
	   imt != fMonName.end();
	   ++imt)
	{
	  rcm->Compute (*idt, *imt);
	  if (rcm->GetMeanErr() > 0)
	    v.push_back (rcm->GetMean());
	}
      fSlope[*idt] = v;
    }
  delete rcm;

}

void
DitherAlias::MakeAliases() 
{

  TString aliasname, aliasformula;
  for (UInt_t iDet=0; iDet<fDetName.size(); iDet++) 
    {
      vector<Double_t> corrcoef = fSlope[fDetName[iDet]];
      if (corrcoef.size())
	{
	  aliasformula = "asym_n_"+fDetName[iDet];
	  aliasname = "dit_"+aliasformula;
	  for (UInt_t i = 0; i < fMonName.size(); ++i)
	    {
	      aliasformula += Form("-(%.3f)*",corrcoef[i]);
	      aliasformula += "diff_"+fMonName[i];
	    }
	  cout << "Made Alias Called \"" << aliasname << "\"" << endl;
	  fTree->SetAlias(aliasname,aliasformula);
	} 
      else 
	{
	  aliasname = "dit_asym_n_"+fDetName[iDet];
	  cout << "Made Empty Alias Called \"" << aliasname << "\"" << endl;
	  fTree->SetAlias(aliasname,"-1e6");
	}
      
    }
}

void
DitherAlias::MakeAliases_12xKludge(TTree *tree) 
{

  fTree = tree;
  TString aliasname, aliasformula;
  for (UInt_t iDet=0; iDet<fDetName.size(); iDet++) 
    {
      vector<Double_t> corrcoef = fSlope[fDetName[iDet]];
      if (corrcoef.size())
	{
	  aliasformula = "asym_n_"+fDetName[iDet];
	  aliasname = "dit_"+aliasformula;
	  for (UInt_t i = 0; i < fMonName.size(); ++i)
	    {
	      aliasformula += Form("-(%.3f)*",corrcoef[i]);
	      if (fMonName[i]=="bpm12x") {
		aliasformula += "diff_bpm10x";
	      } else {
		aliasformula += "diff_"+fMonName[i];
	      }
	    }
	  cout << "Made Alias Called \"" << aliasname << "\"" << endl;
	  fTree->SetAlias(aliasname,aliasformula);
	} 
      else 
	{
	  aliasname = "dit_asym_n_"+fDetName[iDet];
	  cout << "Made Empty Alias Called \"" << aliasname << "\"" << endl;
	  fTree->SetAlias(aliasname,"-1e6");
	}
      
    }
}

vector <string>
DitherAlias::GetMonNames() 
{
  // Assumes all dither-corrected objects use the same set
  // of monitors, with the same ordering
  vector<string> retMonName;
  
  for (UInt_t i = 0; i < fMonName.size(); ++i)
      retMonName.push_back(fMonName[i]);
    
  return retMonName;
}

vector<Double_t>
DitherAlias::GetCorSlopes(TString inDetName) 
{
  // Assumes all dither-corrected objects use the same set
  // of monitors, with the same ordering
  vector<Double_t> corrcoef = fSlope[inDetName.Data()];
  vector<Double_t> retcoef;

  if (corrcoef.size())
    {
      for (UInt_t i = 0; i < fMonName.size(); ++i)
	retcoef.push_back(corrcoef[i]);
    } 
  return retcoef;
}

void
DitherAlias::MakeIndividualAliases(TTree *tree) 
{

  fTree = tree;
  TString aliasname, aliasformula;
  for (UInt_t iDet=0; iDet<fDetName.size(); iDet++) 
    {
      vector<Double_t> corrcoef = fSlope[fDetName[iDet]];
      if (corrcoef.size())
	{
	  for (UInt_t i = 0; i < fMonName.size(); ++i)
	    {
	      aliasname = fDetName[iDet];
	      aliasname += "_" + fMonName[i];
	      aliasformula = Form("(%.3f)*diff_",corrcoef[i]);
	      aliasformula += fMonName[i];
	      cout << "Made Alias Called \"" << aliasname << "\"" << endl;
	      fTree->SetAlias(aliasname,aliasformula);
	    }
	}       
    }
}
