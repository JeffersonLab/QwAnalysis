#ifndef QWDETECTOR_H
#define QWDETECTOR_H
#include <TTree.h>
#include <Riostream.h>
#include <TSQLServer.h>
#include "QwData.h"

/* QwDetector class, from which specific detectors inherit. */
class QwDetector {
    public:
        QwDetector(TString, TString, TString, vector<Int_t>, TSQLServer*, Bool_t, Bool_t, Bool_t);
        void branch(TTree*, vector<QwData>&, Int_t);
        void fill(void);
        void get_data_for_runlet(Int_t runlet, QwData&);

    protected:
        /* Measurement for this detector. */
        TString measurement_id;
        /* Regression type for this detector. */
        TString reg_type;
        /* Name and regression type of detector in database. */
        TString detector_name;
        /* Pointer to database */
        TSQLServer *db;
        /* Bool governing run averages */
        Bool_t runavg;
        /* Bool governing slug averages */
        Bool_t slugavg;
        /* Bool governing wien averages */
        Bool_t wienavg;

        /* runlets for this detector, mapped to number of runlet in tree */
        map<Int_t, Int_t> runlet_id;
        /* number of runlets for this detector */
        vector<Int_t> good_runlets;

        /* QwData vector for this detector. This fills the tree. */
        vector<QwData> data;

        /*
         * Virtual method to query db.
         *
         * FIXME: = 0. wat r u doing. = 0. stawp.
         */
        virtual TString query(void) = 0;
};

#endif
