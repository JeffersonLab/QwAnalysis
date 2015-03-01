//========================================================================
//  repairtree.C
//  Author:        James Dowd (jdowd@jlab.org)
//  Last Modified: 08-26-2014
//  Purpose:       This script was written to correct the CodaEventNumber
//                 offset that cropped up in 3-pass event mode data.  It
//                 creates a copy of the original tree, with the offset 
//                 repaired.  All variable names remain the same, but the 
//                 the new tree name has '_repaired' appended to it.
//  Usage:         The script has two input parameters, the rootfile to be 
//                 modified and the offset to be corrected.
//                 >> root -l
//                 root [0] .L repairtree.C++g
//                 root [1] repairtree("Qweak_17998.root", 6)
//========================================================================


#include <iostream>
#include <vector>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"

#include "TObjArray.h"

void repairtree(TString in, Int_t offset)
{
  // debug flag
  Bool_t debug = kFALSE;
  //Bool_t debug = kTRUE;

  // vector of TS leafs to shift
  std::vector<std::string> ts_leafs_adc;
  ts_leafs_adc.push_back("ts2m_adc");
  ts_leafs_adc.push_back("ts1m_adc");
  ts_leafs_adc.push_back("ts2p_adc");
  ts_leafs_adc.push_back("ts1p_adc");
  std::vector<std::string> ts_leafs_tdc;
  ts_leafs_tdc.push_back("ts2m_f1");
  ts_leafs_tdc.push_back("ts1m_f1");
  ts_leafs_tdc.push_back("ts2p_f1");
  ts_leafs_tdc.push_back("ts1p_f1");

  // vector of MD leafs to shift
  std::vector<std::string> md_leafs_adc;
  md_leafs_adc.push_back("md1m_adc");
  md_leafs_adc.push_back("md2m_adc");
  md_leafs_adc.push_back("md3m_adc");
  md_leafs_adc.push_back("md4m_adc");
  md_leafs_adc.push_back("md5m_adc");
  md_leafs_adc.push_back("md6m_adc");
  md_leafs_adc.push_back("md7m_adc");
  md_leafs_adc.push_back("md8m_adc");
  md_leafs_adc.push_back("md1p_adc");
  md_leafs_adc.push_back("md2p_adc");
  md_leafs_adc.push_back("md3p_adc");
  md_leafs_adc.push_back("md4p_adc");
  md_leafs_adc.push_back("md5p_adc");
  md_leafs_adc.push_back("md6p_adc");
  md_leafs_adc.push_back("md7p_adc");
  md_leafs_adc.push_back("md8p_adc");
  md_leafs_adc.push_back("wilbur_adc");
  md_leafs_adc.push_back("templeton_adc");
  md_leafs_adc.push_back("fern_adc");
  md_leafs_adc.push_back("avery_adc");
  md_leafs_adc.push_back("zuckerman_adc");
  std::vector<std::string> md_leafs_tdc;
  md_leafs_tdc.push_back("md1m_f1");
  md_leafs_tdc.push_back("md2m_f1");
  md_leafs_tdc.push_back("md3m_f1");
  md_leafs_tdc.push_back("md4m_f1");
  md_leafs_tdc.push_back("md5m_f1");
  md_leafs_tdc.push_back("md6m_f1");
  md_leafs_tdc.push_back("md7m_f1");
  md_leafs_tdc.push_back("md8m_f1");
  md_leafs_tdc.push_back("md1p_f1");
  md_leafs_tdc.push_back("md2p_f1");
  md_leafs_tdc.push_back("md3p_f1");
  md_leafs_tdc.push_back("md4p_f1");
  md_leafs_tdc.push_back("md5p_f1");
  md_leafs_tdc.push_back("md6p_f1");
  md_leafs_tdc.push_back("md7p_f1");
  md_leafs_tdc.push_back("md8p_f1");
  md_leafs_tdc.push_back("wilbur_f1");
  md_leafs_tdc.push_back("templeton_f1");
  md_leafs_tdc.push_back("fern_f1");
  md_leafs_tdc.push_back("avery_f1");
  md_leafs_tdc.push_back("zuckerman_f1");


  // open input file and get tree
  TFile* f = new TFile(in);
  if (f->IsZombie()) {
    cout << "Rootfile \'" << in << "\' not found." << endl;
    cout << "Exiting script" << endl;
    f->Close();
    return;
  }
  f->Close();
  TFile* file_in = new TFile(in,"update");
  cout << "Test" << endl;
  TTree* tree_in = (TTree*) file_in->Get("event_tree");
  tree_in->SetBranchStatus("*",0);
  tree_in->SetBranchStatus("trigscint",1);
  tree_in->SetBranchStatus("maindet",1);
  TBranch* trigscint_in = tree_in->GetBranch("trigscint");
  TBranch* maindet_in = tree_in->GetBranch("maindet");
  if (debug) std::cout << "Debug 1" << std::endl;

  // Link TS input leafs to output leafs
  std::vector<Double_t> ts_leafs_in(ts_leafs_adc.size()+ts_leafs_tdc.size());
  size_t leaf;
  for (leaf = 0; leaf < ts_leafs_adc.size(); leaf++)
    trigscint_in->GetLeaf(ts_leafs_adc[leaf].c_str())->SetAddress(&ts_leafs_in[leaf]);
  for (size_t i = 0; leaf < ts_leafs_adc.size() + ts_leafs_tdc.size(); leaf++, i++)
    trigscint_in->GetLeaf(ts_leafs_tdc[i].c_str())->SetAddress(&ts_leafs_in[leaf]);
  if (debug) std::cout << "Debug 2" << std::endl;

  // Link MD input leafs to output leafs
  std::vector<Double_t> md_leafs_in(md_leafs_adc.size()+md_leafs_tdc.size());
  for (leaf = 0; leaf < md_leafs_adc.size(); leaf++)
    maindet_in->GetLeaf(md_leafs_adc[leaf].c_str())->SetAddress(&md_leafs_in[leaf]);
  for (size_t i = 0; leaf < md_leafs_adc.size() + md_leafs_tdc.size(); leaf++, i++)
    maindet_in->GetLeaf(md_leafs_tdc[i].c_str())->SetAddress(&md_leafs_in[leaf]);
  if (debug) std::cout << "Debug 2" << std::endl;


  // create new Tree
  TTree* tree_new = new TTree("event_tree_repaired", "Fixed event_tree");

  // create new TS branch
  std::vector<Double_t> ts_leafs_out(ts_leafs_adc.size()+ts_leafs_tdc.size());
  string ts_leaf_list;
  for (size_t i = 0; i < ts_leafs_adc.size(); i++) {
    if (i != 0) ts_leaf_list += ":";
    ts_leaf_list += ts_leafs_adc[i];
    ts_leaf_list += "/D";
  } 
  for (size_t i = 0; i < ts_leafs_tdc.size(); i++) {
    ts_leaf_list += ":";
    ts_leaf_list += ts_leafs_tdc[i];
    ts_leaf_list += "/D";
  }
  tree_new->Branch("trigscint",&ts_leafs_out[0],ts_leaf_list.c_str());

  // create new MD branch
  std::vector<Double_t> md_leafs_out(md_leafs_adc.size()+md_leafs_tdc.size());
  string md_leaf_list;
  for (size_t i = 0; i < md_leafs_adc.size(); i++) {
    if (i != 0) md_leaf_list += ":";
    md_leaf_list += md_leafs_adc[i];
    md_leaf_list += "/D";
  } 
  for (size_t i = 0; i < md_leafs_tdc.size(); i++) {
    md_leaf_list += ":";
    md_leaf_list += md_leafs_tdc[i];
    md_leaf_list += "/D";
  }
  tree_new->Branch("maindet",&md_leafs_out[0],md_leaf_list.c_str());

  if (debug) std::cout << "Debug 3" << std::endl;

  // Step through all events
  for (int entry = 0; entry < tree_in->GetEntries() - offset; entry++) {
    if (entry % (tree_in->GetEntries()/10) == 0) std::cout << "(" << (100*entry)/tree_in->GetEntries() << "%)" << std::endl;

    // Fill ADC variables
    tree_in->GetEntry(entry + offset);
    for (size_t i = 0; i < ts_leafs_adc.size(); i++)
      ts_leafs_out[i] = ts_leafs_in[i];
    for (size_t i = 0; i < md_leafs_adc.size(); i++)
      md_leafs_out[i] = md_leafs_in[i];

    // Fill TDC variables
    tree_in->GetEntry(entry);
    for (size_t i = ts_leafs_adc.size(); i < ts_leafs_out.size(); i++)
      ts_leafs_out[i] = ts_leafs_in[i];
    for (size_t i = md_leafs_adc.size(); i < md_leafs_out.size(); i++)
      md_leafs_out[i] = md_leafs_in[i];

    if (debug) std::cout << "Debug 4: entry = " << entry << std::endl;
    tree_new->Fill();
  }
  if (debug) std::cout << "Debug 5" << std::endl;
  tree_new->Write();
  if (debug) std::cout << "Debug 6" << std::endl;

  file_in->Close();
  return;
}

