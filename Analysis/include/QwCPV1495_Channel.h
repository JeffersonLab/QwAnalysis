/**
 * \class	QwCPV1495_Channel	QwCPV1495_Channel.h
 *
 * \brief	Class for the decoding of the Compton V1495 FPGA data
 *
 * \author	D. Dutta
 * \date	2010-03-31 18:26:23
 * \ingroup	QwCompton
 *
 * The QwCPV1495_Channel class is defined to read the integrated and sampled
 * data from the Compton electron detector.  Because the scope of this module is
 * similar the the MQwSIS3320_Channel (integration and asymmetries), parts of
 * this class are very similar to MQwSIS3320.
 *
 * The main data members of a QwCPV1495_Channel are the vector fSamples of
 * MQwV1495_Samples where each entry stores a sample event, and the vector of
 * fStrips of MQwV1495_Accumulator where each entry stores an accumulator
 * block.  Both of these data members are derived from their --Raw counterparts
 * 
 *
 */

#ifndef __QwCPV1495_Channel__
#define __QwCPV1495_Channel__

// System headers
#include <iostream>
#include <vector>
#include <numeric>

// ROOT headers
#include <TTree.h>

// Boost math library for random number generation
#include <boost/random.hpp>
#include <boost/multi_array.hpp>

// Qweak headers
#include "VQwDataElement.h"

class QwCPV1495_Channel: public VQwDataElement {

  public:

  QwCPV1495_Channel(UInt_t channel = 0, TString name = "auto", UInt_t plane = 0, UInt_t stripnum = 0) {
    SetElementName(name);
    InitializeChannel(channel, name, plane, stripnum);
    };
    ~QwCPV1495_Channel() {
      DeleteHistograms();
    };

    void  InitializeChannel(UInt_t channel, TString name, UInt_t plane, UInt_t stripnum);

    void  ClearEventData();

    void  EncodeEventData(std::vector<UInt_t> &buffer);

    Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index = 0);
    void  ProcessEvent();

    const QwCPV1495_Channel operator+ (const Double_t &value) const;
    const QwCPV1495_Channel operator- (const Double_t &value) const;
    const QwCPV1495_Channel operator+ (const QwCPV1495_Channel &value) const;
    const QwCPV1495_Channel operator- (const QwCPV1495_Channel &value) const;
    QwCPV1495_Channel& operator=  (const QwCPV1495_Channel &value);
    QwCPV1495_Channel& operator+= (const Double_t &value);
    QwCPV1495_Channel& operator-= (const Double_t &value);
    QwCPV1495_Channel& operator+= (const QwCPV1495_Channel &value);
    QwCPV1495_Channel& operator-= (const QwCPV1495_Channel &value);
    void Sum(QwCPV1495_Channel &value1, QwCPV1495_Channel &value2);
    void Difference(QwCPV1495_Channel &value1, QwCPV1495_Channel &value2);
    void Ratio(QwCPV1495_Channel &numer, QwCPV1495_Channel &denom) { };
    void Offset(Double_t Offset);
    void Scale(Double_t Offset);


    void  ConstructHistograms(TDirectory *folder, TString &prefix);
    void  FillHistograms();
    void  DeleteHistograms();

    void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
    void  FillTreeVector(std::vector<Double_t> &values);




    void SetCalibrationFactor(const Double_t factor) { fCalibrationFactor = factor; };
    const Double_t GetCalibrationFactor() const { return fCalibrationFactor; };


    void Copy(VQwDataElement *source) { };

    void Print() const;

 protected:
    static const Int_t NPlanes = 4;
    static const Int_t StripsPerPlane = 96;

 private:

    static const Bool_t kDEBUG;

    // Identification information
    Int_t fChannel;
    Int_t fPlane;
    Int_t fNStrip;
    Bool_t fHasStripData;

    // Randomness generator
    static boost::mt19937 fRandomnessGenerator;
    static boost::normal_distribution<double> fNormalDistribution;
    static boost::variate_generator
      < boost::mt19937, boost::normal_distribution<double> >fNormalRandomVariable;

    /* FPGA Calibration */
    static const Double_t MeVperStrip;
    Double_t fCalibrationFactor;

    /* In sampling mode we have multiple events in a single data block */
    Int_t fCurrentEvent; //! Current triggered event (allow for negative sentinel)
    UInt_t fNumberOfEvents; //! Number of triggered events

    /* FPGA strip accum data */
    Int_t fNumberOfStrips;
    std::vector< std::vector <QwCPV1495_Channel> > fStrips;
    std::vector< std::vector <QwCPV1495_Channel> > fStripsRaw;
    std::vector <QwCPV1495_Channel> fFPGAChannel_ac;
    std::vector <Double_t> fPlaneVector;
    //    boost::multi_array<QwCPV1495_Channel, 2> array_type;
    //array_type fStrips(boost::extents[NPlanes][StripsPerPlane]);

 /*=====
   *  Histograms should be listed below here.
   *  They should be pointers to histograms which will be created
   *  inside the ConstructHistograms() 
   */


  TH1D *ePlane[NPlanes];
  TH1D *ePlaneRaw[NPlanes];
};

#endif // __QwCPV1495_Channel__
