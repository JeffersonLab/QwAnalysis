/*
 * QwRegressionSubsystem.h
 *
 *  Created on: Aug 11, 2011
 *      Author: meeker
 */


#ifndef __QWREGRESSIONSUBSYSTEM__
#define __QWREGRESSIONSUBSYSTEM__

// headers
#include "QwOptions.h"
#include "VQwSubsystemParity.h"
#include "QwSubsystemArrayParity.h"
#include "QwRegression.h"


/**
 * \class QwRegressionSubsystem
 *
 * \brief
 *
 */

class QwRegressionSubsystem: public VQwSubsystemParity, 
  public MQwSubsystemCloneable<QwRegressionSubsystem>,
  public QwRegression
{
  
  
  public:
      // Constructors
      /// \brief Constructor with just name.
      /// (use gQwOptions to initialize the QwRegression baseclass)
      QwRegressionSubsystem(TString name): VQwSubsystem(name), VQwSubsystemParity(name), QwRegression(gQwOptions) {};
      /// \brief Constructor with only options
      QwRegressionSubsystem(QwOptions &options, TString name): VQwSubsystem(name), VQwSubsystemParity(name), QwRegression(options) {};
      /// \brief Constructor with single event
      QwRegressionSubsystem(QwOptions &options, QwSubsystemArrayParity& event, TString name):VQwSubsystem(name),   VQwSubsystemParity(name), QwRegression(options, event) {};     
      /// \brief Constructor with helicity pattern
      QwRegressionSubsystem(QwOptions &options, QwHelicityPattern& helicitypattern, TString name):VQwSubsystem(name), VQwSubsystemParity(name), QwRegression(options, helicitypattern) {};
      /// \brief Constructor with single event and helicity pattern
      QwRegressionSubsystem(QwOptions &options, QwSubsystemArrayParity& event, QwHelicityPattern& helicitypattern,TString name ): VQwSubsystem(name), 
	VQwSubsystemParity(name), QwRegression(options, event, helicitypattern) {};
     
      // Copy Constructor
      QwRegressionSubsystem(const QwRegressionSubsystem &source)
	: VQwSubsystem(source), VQwSubsystemParity(source),
	QwRegression(source) {
	this->Copy(&source);
      }
	
      // Destructor 
      ~QwRegressionSubsystem();

      void Copy(const VQwSubsystem* source);


      /// \brief Update the running sums
      void AccumulateRunningSum(VQwSubsystem* input);
      void DeaccumulateRunningSum(VQwSubsystem* value);
      /// \brief Calculate the average for all good events
      void CalculateRunningAverage();
      /// \brief Print values for all channels
      void PrintValue() const;



      /// \brief Overloaded Operators
      VQwSubsystem& operator=  (VQwSubsystem *value);
      VQwSubsystem& operator+= (VQwSubsystem *value);
      VQwSubsystem& operator-= (VQwSubsystem *value);
      VQwSubsystem& operator*= (VQwSubsystem *value);
      VQwSubsystem& operator/= (VQwSubsystem *value);
      void Sum(VQwSubsystem *value1, VQwSubsystem *value2);
      void Difference(VQwSubsystem *value1, VQwSubsystem *value2);
      void Ratio(VQwSubsystem* value1, VQwSubsystem* value2);
      void Scale(Double_t value);


      //update the same error flag in the classes belong to the subsystem.
      void UpdateEventcutErrorFlag(UInt_t errorflag);

      //update the error flag in the subsystem level from the top level routines related to stability checks. This will uniquely update the errorflag at each channel based on the error flag in the corresponding channel in the ev_error subsystem
      void UpdateEventcutErrorFlag(VQwSubsystem *ev_error);


      /// \brief Derived functions
	// not sure if there should be empty definition, no definition or defined 
      Int_t LoadChannelMap(TString);
      Int_t LoadInputParameters(TString);
      Int_t LoadEventCuts(TString);
      void ClearEventData(){};
      Int_t ProcessConfigurationBuffer(UInt_t, UInt_t, UInt_t*, UInt_t);
      Int_t ProcessEvBuffer(UInt_t, UInt_t, UInt_t*, UInt_t);
      void ProcessEvent(){};
      void ConstructHistograms(TDirectory*, TString&){};
      void FillHistograms(){};
      void DeleteHistograms(){};
      void ConstructBranchAndVector(TTree*, TString&, std::vector<Double_t, std::allocator<Double_t> >&){};
      void ConstructBranch(TTree*, TString&){};
      void ConstructBranch(TTree*, TString&, QwParameterFile&){};
      void FillTreeVector(std::vector<Double_t, std::allocator<Double_t> >&) const{};
      
      Bool_t ApplySingleEventCuts();
      Int_t GetEventcutErrorCounters();
      UInt_t GetEventcutErrorFlag();


  private: 
       
     /**
      * Default Constructor 
      * 
      * Error: tries to call default constructors of base class, 
      * 	QwRegression() is private
      */
   //   QwRegressionSubsystem() {};     

      
}; // class QwRegressionSubsystem


#endif // __QWREGRESSIONSUBSYSTEM__

