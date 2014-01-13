/*
 * QwTreeBranchPlot.h
 * Author: Wade S Duvall <wsduvall@jlab.org> 
 * Adapted from code written by:
 * Josh Magee
 * Josh Hoskins
 *
 */

#ifndef _QWTREEBRANCHPLOT_H_
#define _QWTREEBRANCHPLOT_H_

#include "QwTreeBranch.h"

/*
 * Child of the QwTreeBranch class for plotting. Will inherit all the tree IO
 * stuff from QwTreeBranch.
 */
class QwTreeBranchPlot : public QwTreeBranch {
    public:
        QwTreeBranchPlot(void);                         // empty constructor
        QwTreeBranchPlot(TString, TString, TString);    // constructor w/ label names
        void ValueRunletPlot(void);                     // plot value vs runlet
        void ValueRunletPlotSignCorr(void);             // plot value vs runlet sign corrected
        void ValueRunletPlotWeight(void);               // plot value vs runlet
        void ValueRunletPlotSignCorrWeight(void);       // plot value vs runlet sign corrected
        void ValuePlot(void);                           // plot histo of value
        void ValuePlotSignCorr(void);                   // plot histo of value sign corrected
        void ValuePlotWeight(void);                     // plot histo of value weighted
        void ValuePlotSignCorrWeight(void);             // plot histo of value sign corrceted and weighted
        void ValuePullPlot(void);                       // plot pull plot of value
    private:
        TString plot_label;                             // stores plot label
        TString x_axis_label;                           // stores x axis label
        TString y_axis_label;                           // stores y axis label
};

#endif
