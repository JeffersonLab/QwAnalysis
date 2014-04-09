/*
 * QwTreeLib.h
 * Author: Wade S Duvall <wsduvall@jlab.org>
 *
 */

#ifndef _QWTREEBRANCHPAIR_H_
#define _QWTREEBRANCHPAIR_H_

/*
 * A class that exists soley for making correlations. Currently just a basic
 * placeholder, I will talk with Manolis about getting his correlation stuff in
 * here.
 */
class QwTreeBranchPair {
    public:
        QwTreeBranchPair(QwTreeBranch, QwTreeBranch);   // constructor
        QwTreeBranch get_x(void);                    // getter for x
        QwTreeBranch get_y(void);                    // getter for y

        /* Add plot/correlation functions here. */
    private:
        QwTreeBranch x;                                 // QwTreeBranch class for x
        QwTreeBranch y;                                 // QwTreeBranch class for y
        TString x_axis_label;                           // x axis label
        TString y_axis_label;                           // y axis label
};

#endif
