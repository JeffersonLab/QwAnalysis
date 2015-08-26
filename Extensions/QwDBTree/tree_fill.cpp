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
#include "QwMainDet.h"
#include "QwMainDetSlope.h"
#include "QwLumiDet.h"
#include "QwLumiDetSlope.h"
#include "QwBeamDet.h"
#include <time.h>
#include "parse.h"
#include "runlet.h"

/*
 * The tree_fill() is the main worker function. First in creates the rootfile,
 * then reads in the detector mapfiles md, lumi, and beam; reg map file is read
 * in by main(). 
 */
int tree_fill(TString reg_type, TSQLServer *db, QwRunlet &runlets, TString mapdir, TString outdir, TString target, Bool_t runavg, Bool_t slugavg, Bool_t wienavg, TString slopes) {
    /* Select name for if averaging. */
    TString avg_type = "";
    if(runavg) avg_type = "runavg";
    if(slugavg) avg_type = "slugavg";
    if(wienavg) avg_type = "wienavg";
    /* Open the output file and create the tree for outputting. */
    TFile *f;
    f = new TFile(Form("%s%s%s%s_%s_tree.root",outdir.Data(),avg_type.Data(),slopes.Data(),target.Data(),reg_type.Data()),"RECREATE");
    TTree *tree = new TTree("tree","treelibrated tree");

    /*
     * Read in all of the detector mapfiles, starting with main detector. Grab
     * the number of detectors in the mapfile for looping later. If needed,
     * also read in the run list.
     */
    QwParse mds(Form("%smd.map",mapdir.Data()));
    mds.parse();
    const Int_t num_mds = mds.num_detectors();

    /*
     * Read in lumi mapfile, and save the
     */
    QwParse lumis(Form("%slumi.map",mapdir.Data()));
    lumis.parse();
    const Int_t num_lumis = lumis.num_detectors();

    /*
     * Read in beam mapfile.
     */
    QwParse beams(Form("%sbeam.map",mapdir.Data()));
    beams.parse();
    const Int_t num_beams = beams.num_detectors();

    /*
     * Get a list of good runlets from the runlet object. Before pass5, it
     * would use the offoff runlet list, now they should all be equivalent (it
     * still uses offoff for legacy purposes).
     *
     * If this run averaging is enabled, get a list of good runs instead.
     * FIXME: change good_runlets to good_events.
     */
    vector<Int_t> good_runlets;
    /*
     * runs may or may not be needed, it is used to get the number of runs in
     * run average mode
     */
    good_runlets = runlets.get_runlets();
    if(runavg) {
        good_runlets = runlets.get_runs();
    }
    if(slugavg) {
        good_runlets = runlets.get_slugs();
    }
    if(wienavg) {
        good_runlets = runlets.get_wiens();
    }

    /*
     * Create a temporary detector object and push it onto a vector of detector
     * objects. This will be the basis for filling the tree. Start with main
     * detectors. 
     */
    
    vector<QwMainDetSlope> main_detector_slopes;
    vector<QwLumiDetSlope> lumi_detector_slopes;
    vector<QwMainDet> main_detectors;
    vector<QwLumiDet> lumi_detectors;
    vector<QwBeamDet> beam_detectors;

    if(slopes != "") {
        /* Create a vector of slopes for wrt_qwk_charge only for each MD. Only do
         * this if reg_type = "on_5+1". 
         *
         * Fill main detector slope object vector
         */
        if(reg_type == "on_5+1" || reg_type == "on" || reg_type == "on_set10" || reg_type == "on_set11" || reg_type == "on_set13") {
            for(Int_t i = 0; i < num_mds; i++) {
                QwMainDetSlope temp_detector(mds.detector(i), mds.id_type(i), reg_type, slopes, good_runlets, db, runavg, slugavg, wienavg);
                temp_detector.fill();
                main_detector_slopes.push_back(temp_detector);
            }
        }

        if(reg_type == "on_5+1" || reg_type == "on" || reg_type == "on_set10" || reg_type == "on_set11" || reg_type == "on_set13") {
            for(Int_t i = 0; i < num_lumis; i++ || reg_type == "on") {
                QwLumiDetSlope temp_detector(lumis.detector(i), lumis.id_type(i), reg_type, slopes, good_runlets, db, runavg, slugavg, wienavg);
                temp_detector.fill();
                lumi_detector_slopes.push_back(temp_detector);
            }
        }
    }
    else {
        /*
         * Fill main detector object vector.
         */
        for(Int_t i = 0; i < num_mds; i++) {
            QwMainDet temp_detector(mds.detector(i), mds.id_type(i), reg_type, good_runlets, db, runavg, slugavg, wienavg);
            temp_detector.fill();
            main_detectors.push_back(temp_detector);
        }

        /*
         * Fill lumi object vector.
         */
        for(Int_t i = 0; i < num_lumis; i++) {
            QwLumiDet temp_detector(lumis.detector(i), lumis.id_type(i), reg_type, good_runlets, db, runavg, slugavg, wienavg);
            temp_detector.fill();
            lumi_detectors.push_back(temp_detector);
        }

        /*
         * Fill beam object vector.
         */
        for(Int_t i = 0; i < num_beams; i++) {
            QwBeamDet temp_detector(beams.detector(i), beams.id_type(i), reg_type, good_runlets, db, runavg, slugavg, wienavg);
            temp_detector.fill();
            beam_detectors.push_back(temp_detector);
        }
    }

    /*
     * Create objects that will hold the data and be used to fill the tree.
     * Tree filling requires that each data point have a vector of what will go
     * into it. See QwDetector::branch() for details.
     *
     * Reserve is used here so that the memory is address when branch is
     * called. When tree->Fill() is run, it will look in the address it was
     * assigned when it is branched.
     *
     * Vectors are created for all detectors and runlet data; basically
     * anything that will end up in the tree.
     */
    QwValues runlet_data;
    runlet_data.runlet_properties.reserve(9);

    /*
     * Vectors to store normal data.
     */
    vector<QwData> main_data;
    main_data.reserve(num_mds);
    vector<QwData> lumi_data;
    lumi_data.reserve(num_lumis);
    vector<QwData> beam_data;
    beam_data.reserve(num_beams);

    /*
     * Vectors to store slope data.
     */
    vector<QwData> main_slope_data;
    main_slope_data.reserve(num_mds);
    vector<QwData> lumi_slope_data;
    lumi_slope_data.reserve(num_mds);

    /* Branch the tree. See QwDetector::branch() for details. */
    runlets.branch(tree, runlet_data);

    if(slopes != "") {
        for(Int_t i = 0; i < num_mds; i++) {
            // pass in the vector like main_data[i]
            main_detector_slopes[i].branch(tree, main_slope_data, i);
        }
        for(Int_t i = 0; i < num_lumis; i++) {
            lumi_detector_slopes[i].branch(tree, lumi_slope_data, i);
        }
    }
    else {
        for(Int_t i = 0; i < num_mds; i++) {
            main_detectors[i].branch(tree, main_data, i);
        }
        for(Int_t i = 0; i < num_lumis; i++) {
            lumi_detectors[i].branch(tree, lumi_data, i);
        }
        for(Int_t i = 0; i < num_beams; i++) {
            beam_detectors[i].branch(tree, beam_data, i);
        }
    }

    /* Fill the QwData vectors from QwDetector objects. */
    for(UInt_t j = 0; j < good_runlets.size(); j++) {
        /* Fill runlet data. */
        runlets.get_data_for_runlet(j, runlet_data);
        /* Fill main detectors, lumis, and beam detectors. */
        if(slopes != "") {
            for(Int_t i = 0; i < num_mds; i++) {
                main_detector_slopes[i].get_data_for_runlet(good_runlets[j], main_slope_data[i]);
            }
            for(Int_t i = 0; i < num_lumis; i++) {
                lumi_detector_slopes[i].get_data_for_runlet(good_runlets[j], lumi_slope_data[i]);
            }
        }
        else {
            for(Int_t i = 0; i < num_mds; i++) {
                main_detectors[i].get_data_for_runlet(good_runlets[j], main_data[i]);
            }
            for(Int_t i = 0; i < num_lumis; i++) {
                lumi_detectors[i].get_data_for_runlet(good_runlets[j], lumi_data[i]);
            }
            for(Int_t i = 0; i < num_beams; i++) {
                beam_detectors[i].get_data_for_runlet(good_runlets[j], beam_data[i]);
            }
        }
        /* Fill the tree. It will use the address given during branching. */
        tree->Fill();
    }

    /* Create new tree to store basic info. */
    TTree *tree_info = new TTree("tree_info","treelibrated tree");

    /* Set SVN revision and date. */
    int svn_revision = atoi(SVN_REV);

    /* Branch SVN revision and date before branching the remainder of the tree. */
    tree_info->Branch("svn_revision", &(svn_revision));

    /* Fill SVN revision and date. */
    tree_info->Fill();

    /* Write the tree to the previously opened rootfile. */
    f->Write();
    f->Close();

    return 0;
}
