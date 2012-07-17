/*****************************************************************************
 * tree_fill.c++
 *
 * Author: Wade S Duvall
 * Email: wsduvall@jlab.org
 *
 * A program to query the qweakdb server and populate a tree with specific
 * values from the database, and fill them into a ROOT tree. The tree is then
 * saved to a ROOT file for access later.
 *
 * TODO:
 * - Populate the yield vectors
 * - read in list of variables from map file (in progress)
 *
 * This project is currently under heavy development. If you wish to make
 * any changes or request any features, please contect author first.
 ***************************************************************************/
#include <TFile.h>
#include <TTree.h>
#include <Riostream.h>
#include <TSQLStatement.h>
#include <TSQLServer.h>
#include <TMath.h>
#include <string>
#include "detector.h"
#include "parse.h"

int tree_fill(TString reg_type, TSQLServer *db, QwRunlet &runlets, TString mapdir, TString outdir)
{
    // open file and database connections
    // create the root tree
    TFile *f = new TFile(Form("%s%s_tree.root",outdir.Data(),reg_type.Data()),"RECREATE");

    // create tree
    TTree *tree = new TTree("tree","treelibrated tree");

    // read in mapfile in preperation for querying db
    // read in the md map file
    cout << Form("%smd.map",mapdir.Data()) << endl;
    QwParse mds(Form("%smd.map",mapdir.Data()));
    mds.parse();
    const Int_t num_mds = mds.num_detectors();

    // read in the lumi map file
    QwParse lumis(Form("%slumi.map",mapdir.Data()));
    lumis.parse();
    const Int_t num_lumis = lumis.num_detectors();

    // read in the beam map file
    QwParse beams(Form("%sbeam.map",mapdir.Data()));
    beams.parse();
    const Int_t num_beams = beams.num_detectors();

    //runlets.branch(tree);
    vector<Int_t> good_runlets;
    // get the set of good runlets
    good_runlets = runlets.get_runlets();

    // main detector
    // fill a temp vector and push to a vector
    vector<QwMainDet> main_detectors; // mdallbars("qwk_mdallbars", db);
    for(Int_t i = 0; i < num_mds; i++)
    {
        QwMainDet temp_detector(mds.detector(i), mds.id_type(i), reg_type, good_runlets, db);
        temp_detector.fill();
        main_detectors.push_back(temp_detector);
    }

    // lumi detector
    // fill a temp vector and push to a vector
    vector<QwLumiDet> lumi_detectors;
    for(Int_t i = 0; i < num_lumis; i++)
    {
        QwLumiDet temp_detector(lumis.detector(i), lumis.id_type(i), reg_type, good_runlets, db);
        temp_detector.fill();
        lumi_detectors.push_back(temp_detector);
    }

    // beam moniters
    // fill a temp vector and push to a vector
    vector<QwBeamDet> beam_detectors; // mdallbars("qwk_mdallbars", db);
    for(Int_t i = 0; i < num_beams; i++)
    {
        QwBeamDet temp_detector(beams.detector(i), beams.id_type(i), reg_type, good_runlets, db);
        temp_detector.fill();
        beam_detectors.push_back(temp_detector);
    }

    vector<Int_t> runlet_data;
    runlet_data.reserve(6);
    vector<QwData> main_data;
    main_data.reserve(num_mds);
    vector<QwData> lumi_data;
    lumi_data.reserve(num_lumis);
    vector<QwData> beam_data;
    beam_data.reserve(num_beams);

    runlets.branch(tree, runlet_data);
    for(Int_t i = 0; i < num_mds; i++)
    {
        // pass in the vector like main_data[i]
        main_detectors[i].branch(tree, main_data, i);
    }
    for(Int_t i = 0; i < num_lumis; i++)
    {
        lumi_detectors[i].branch(tree, lumi_data, i);
    }
    for(Int_t i = 0; i < num_beams; i++)
    {
        beam_detectors[i].branch(tree, beam_data, i);
    }

    // fill the tree
    for(Int_t j = 0; j < good_runlets.size(); j++)
    {
        runlets.get_data_for_runlet(j, runlet_data);
        for(Int_t i = 0; i < num_mds; i++)
        {
            main_detectors[i].get_data_for_runlet(good_runlets[j], main_data[i]);
        }
        for(Int_t i = 0; i < num_lumis; i++)
        {
            lumi_detectors[i].get_data_for_runlet(good_runlets[j], lumi_data[i]);
        }
        for(Int_t i = 0; i < num_beams; i++)
        {
            beam_detectors[i].get_data_for_runlet(good_runlets[j], beam_data[i]);
        }
        tree->Fill();
    }

    // open rootfile
    f->Write();
    f->Close();
    return 0;
}

int main(Int_t argc, Char_t* argv[])
{
    // defaults
    TString host = "127.0.0.1";
    TString mapdir = "";
    TString outdir = "";
    TString db_name = "qw_run1_pass4b";
    // parse command line options
    for(Int_t i = 1; i < argc; i++)
    {
        if(0 == strcmp("--host", argv[i])) host = argv[i+1];
        if(0 == strcmp("--db", argv[i])) db_name = argv[i+1];
        if(0 == strcmp("--mapdir", argv[i])) mapdir = argv[i+1];
        if(0 == strcmp("--outdir", argv[i])) outdir = argv[i+1];
    }
    // open connection to qweakdb.jlab.org
    TSQLServer *db;
    db = TSQLServer::Connect(Form("mysql://%s/%s",host.Data(),db_name.Data()),"qweak", "QweakQweak");

    // read in the md map file
    QwParse reg_types("reg.map");
    reg_types.parse();
    
    // get the filled runlet object (which will also generate temp tables for ALL trees to use)
    QwRunlet runlets(db);
    runlets.fill(reg_types);

    const Int_t num_regs = reg_types.num_detectors();
    for(Int_t i = 0; i < num_regs; i++)
    {
        tree_fill(reg_types.detector(i), db, runlets, mapdir, outdir);
    }

    db->Close();
}
