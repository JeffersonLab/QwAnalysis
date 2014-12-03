/*
 * TauCalc.h
 * This class calculates the deadtime correction
 * for a given set of data at varying currents.
 *
 * Written by Josh Magee
 * magee@jlab.org
 * October 10, 2014
 */

#ifndef _TAUCALC_H_
#define _TAUCALC_H_

#include <sstream>
#include <vector>

#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TColor.h>

using std::vector;
using std::string;

class TauCalc {
  public:
    TauCalc(); //default constructor
    TauCalc(std::string); //constructor if given a name
    ~TauCalc(); //default destructor

    void reset(); //clears all data

    /* The object's name is important to define.
     * It is used to create the TGraphErrors and TCanvas
     * objects, and is used internally by ROOT. If using
     * multiple instances of TauCalc objects, one MUST
     * provide each an individual name, else seg faults
     * are likely to occur.
     */
    void setName(std::string); //method to set a name
    std::string getName(void); //method to get the name if supplied

    /* The new methods deal with pushing data into the class.
     * The first entry is for the current.
     * The second is for the rate
     * The third, if present, is uncertainty of the rate
     * If no uncertainty is provided, and uncertainy of 0
     * is assumed.
     */
    void push_data(double, double, double);

    //This print function is only partially implemented.
    void print_data(void); //method to print all the data

    /* The following methods provide the actual calculated
     * deadtime in ns, and the "true" yield - ie, the
     * intercept of the fits.
     */
    double getTauNs(void); //method to get Tau (ns)
    double getZeroRate(void); //method to get zero_rate_yield

    /* Processing the data is a multi-step process.
     * First one must take measured rates and calculate yields.
     * Then, the intercept of that fit gives the true yield.
     * The "true_rates" vector is rate*current.
     *
     * Then, the relative rate is determined, and the
     * deadtime (tau) is determined. The "corrected rates"
     * are then plotted to show they are consistent to
     * a 0th order fit.
     *
     * All of this is done automatically, and in the correct
     * order by calling the processDataAutomatically() method.
     */
    void processDataAutomatically(void);

    void calculateMeasuredYields(void);
    void calculateTrueYields(void);
    void calculateRelativeRates(void);
    void calculateCorrectedRates(void);

    void graphMeasuredRates(void);
    void graphMeasuredYields(void);
    void graphRelativeRates(void);
    void graphCorrectedRates(void);

    //getters to retrieve pointers to graphs
    TGraphErrors* getRateGraph(void);
    TGraphErrors* getYieldGraph(void);
    TGraphErrors* getRelativeGraph(void);
    TGraphErrors* getCorrectedGraph(void);

    //the following methods create the canvas and
    //are in charge of making it look acceptible.
    void createCanvas(void); //create canvas to plot
    void makeGraphsPretty(TGraphErrors*,std::string, std::string, std::string); //makes the TGraph objects look nice
    void setPlotColor(std::string); //sets color schema for plots
    void setPlotColor(int ); //sets color schema for plots
                            //each detector #1-8 has a different color

  private:
    static const bool debug = false; //global function debugger

    /* The following bools are internal to keep track of
     * what analysis has been previously done. For example,
     * one shouldn't be able to calculate tau without calculating
     * yields first.
     */
    bool haveData;
    bool haveYields;
    bool haveTau;
    bool correctedData;

    //the name is important, as discussed above
    std::string myName; //name of data set

    double tau; //calculated deadtime (typically in ns)
    double zero_rate_yield; //intercept of fit

    /* The following are vectors that contain the data to operate
      * on. In the future a vector of vectors might be
      * nice to implement, but this does the trick for now.
      *
      * Explanation of the following vectors:
      *
      * currents[j]: each element holds a given current
      * data[j]: measured rates (in kHz)
      * yields[j]: data[j]/current[j]
      *   from this zero_rate_yield (intercept) is found
      * true_rates[j]: zero_rate_yield*data[j]
      * relative_rate[j]: data[j]/true_rates[j]
      *   from this tau is determined
      * yields_corrected[j]: yields[j]*exp(-true_rates[j]*tau)
      *   these are the rates with deadtime correction applied
      */
    std::vector <double> currents;
    std::vector <double> data;
    std::vector <double> data_error;
    std::vector <double> yields;
    std::vector <double> yields_error;
    std::vector <double> true_rates;
    std::vector <double> relative_rate;
    std::vector <double> relative_error;
    std::vector <double> yields_corrected;
    std::vector <double> yields_corrected_error;

    //the following are pointers to TCanvas and TGraphErrors objects
    TCanvas *canvas;
    TGraphErrors *rate_graph;
    TGraphErrors *yield_graph;
    TGraphErrors *relative_graph;
    TGraphErrors *corrected_graph;

    int plot_marker; //specify plotting symbol type
    int plot_color; //specify plotting symbol color
    int fit_color;  //specify color of fitted line

};


#endif


