#include "TROOT.h"
#include <Riostream.h>
#include <TSQLServer.h>
#include "tree_fill.h"
#include "detector.h"
#include "parse.h"
#include "runlet.h"

/*
 * main.cpp
 *
 * First, defaults database variables, output folder, and mapfiles. Then,
 * connect to database (default is set to 127.0.0.1). Then, the regression
 * mapfile is read in, and QwRunlets run to fill the temporary tables for each
 * regression type and to fill the runlet object with data for each runlet.
 * Then tree_fill is run for each regression type in the mapfile.
 */

int main(Int_t argc, Char_t* argv[]) {
    /*
     * Defaults are set. No default mapfile is set, and run level averaging is
     * disabled.
     */
    TString host = "127.0.0.1";         // database server defaults to 127.0.0.1 
    TString mapdir = "";                // mapdir defaults to current directory
    TString outdir = "";                // output directory defaults to current directory
    TString db_name = "qw_run1_pass4b"; // database defaults to qw_run1_pass4b
    TString runlist = "";               // no default (uses all runlets)
    Bool_t runavg = kFALSE;             // disabled by default
    
    // Parse command line options.
    for(Int_t i = 1; i < argc; i++) {
        if(0 == strcmp("--host", argv[i])) host = argv[i+1];
        if(0 == strcmp("--db", argv[i])) db_name = argv[i+1];
        if(0 == strcmp("--mapdir", argv[i])) mapdir = argv[i+1];
        if(0 == strcmp("--outdir", argv[i])) outdir = argv[i+1];
        if(0 == strcmp("--runlist", argv[i])) runlist = argv[i+1];
        if(0 == strcmp("--runavg", argv[i])) runavg = kTRUE;
    }

    /* print all settings */
    cout << "host = " << host << endl;
    cout << "mapdir = " << mapdir << endl;
    cout << "outdir = " << outdir << endl;
    cout << "db = " << db_name << endl;
    cout << "runlist = " << runlist << endl;
    if(runavg) cout << "runavg : enabled" << endl;
    else cout << "runavg : disabled" << endl;

    /* Open connection to specified database with qweak credentials. */
    TSQLServer *db;
    db = TSQLServer::Connect(Form("mysql://%s/%s",host.Data(),db_name.Data()),"qweak", "QweakQweak");

    /* Read in the regression mapfile. */
    QwParse reg_types(Form("%sreg.map",mapdir.Data()));
    reg_types.parse();

    /* If enabled, read in run list mapfile */
    QwParse runlist_str(runlist);
    if(!(runlist == "")) {
        runlist_str.parse();
    }

    /*
     * Get the filled runlet object (which will also generate temp tables for
     * ALL trees to use).
     */
    QwRunlet runlets(db);
    runlets.fill(reg_types, runlist_str);

    /* Run tree_fill to grab the remaining data from the database. */
    const Int_t num_regs = reg_types.num_detectors();
    for(Int_t i = 0; i < num_regs; i++) {
        tree_fill(reg_types.detector(i), db, runlets, mapdir, outdir, runavg);
    }

    /* close the database and die */
    db->Close();
    return 0;
}
