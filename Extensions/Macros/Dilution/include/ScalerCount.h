/* * ScalerCount.h
 * This class quickly runs through a given branch and grabs
 * the scaler data for a given detector. At the moment
 * it really only supports Qweak Main Detector scalers, because
 * the array sizes are set by default to 8. This will be a quick
 * change to make to expand it to trigger scintillators.
 *
 * Written by Josh Magee
 * magee@jlab.org
 * May 20, 2014
 */

#ifndef _SCALER_COUNT_H_
#define _SCALER_COUNT_H_

#include <TBranch.h>
#include <TLeaf.h>
#include <TH1F.h>
#include <TCanvas.h>

using std::vector;

class ScalerCount {
  public:
    ScalerCount(); //default constructor
    ScalerCount(TBranch *); //constructor if given TBranch
    ScalerCount(TBranch *, long, long); //constructor if given TBranch
                                        //and event range
    ScalerCount(TBranch *, long, long, std::string); //also given name
    ~ScalerCount(); //default destructor

    //methods to set high/low event numbers to process
    void setEventLow(long);
    void setEventHigh(long);
    void setEventRange(long, long);

    void setName(std::string);

    void setBranch(TBranch *); //method to set TBranch
    void reset(void); //resets only distribution means/values
    void resetAll(void); //resets ALL data members
    void printRawRates(void); //print raw rates
    void printCorrections(void); //prints information on corrections
                          //like accidentals/electronic livetime
    void printCorrectedRates(void); //print corrected rates

    void processEvents(void); //processes all events within range

    //get vectors of data for a specified detector/octant number
    std::vector<double>* get_all_values(int);
    std::vector<double>* get_pos_values(int);
    std::vector<double>* get_neg_values(int);

    void setAverages(void);
    double calculateAverage(std::vector<double> *);
    double calculateRMS(std::vector<double> *);
    void calculateCorrections(void);

    void createCanvas(void); //create canvas to plot on
//    void resetHistograms(void); //reset histograms
    void createHistograms(void); //create and fill histograms
    void fillHistograms(void); //create and fill histograms
    void plotHistograms(void); //method to plot all histograms
 
    //really, ROOT histograms are ugly w/o effort
    void makeHistoPretty(TH1F*,TCanvas*,int, std::string,std::string,std::string);

  private:
    TBranch *branch;
    const static int num = 8; //number of detectors
    const static double disc = .000000040; //MD discriminator width in secs
    const static double coin = .000000079; //MD coincidence width in secs
    static const int plot_sequence[num];

    long eventLow;    //event number minimum (ie, process only events> eventLow)
    long eventHigh;   //event number maximum (ie, process only events< eventHigh)

    bool haveBranch;
    bool haveAverages;
    bool haveCorrections;

    std::string myName;

    /* The following arrays of vectorswill hold information on the
     * allbars values and uncertainties, as well as positive
     * and negative tube values and uncertainties
     */
    std::vector<double> all_values[num];
    std::vector<double> pos_values[num];
    std::vector<double> neg_values[num];

    std::vector<double> all_adc[num];
    std::vector<double> pos_adc[num];
    std::vector<double> neg_adc[num];

    TLeaf *lAll_values[num];
    TLeaf *lPos_values[num];
    TLeaf *lNeg_values[num];

    TLeaf *lPos_adc[num];
    TLeaf *lNeg_adc[num];

    double all_means[num];
    double all_error[num];
    double pos_means[num];
    double pos_error[num];
    double neg_means[num];
    double neg_error[num];

    double pos_uncorrelated_values[num];
    double pos_uncorrelated_errors[num];
    double neg_uncorrelated_values[num];
    double neg_uncorrelated_errors[num];

    double livetime[num];
    double livetime_error[num];
    double accidentals[num];
    double accidentals_error[num];

    TCanvas *canvas_adc[3];
    TH1F *h_adcpos[num];
    TH1F *h_adcneg[num];
    TH1F *h_adcall[num];
};


#endif


