#include "TROOT.h"
#include <Riostream.h>
#include <TSQLServer.h>
#include "tree_fill.h"
#include "detector.h"
#include "parse.h"
#include "runlet.h"

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

    cout << "host = " << host << endl;
    cout << "mapdir = " << mapdir << endl;
    cout << "outdir = " << outdir << endl;
    cout << "db = " << db_name << endl;
    // open connection to qweakdb.jlab.org
    TSQLServer *db;
    db = TSQLServer::Connect(Form("mysql://%s/%s",host.Data(),db_name.Data()),"qweak", "QweakQweak");

    // read in the md map file
    QwParse reg_types(Form("%sreg.map",mapdir.Data()));
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
