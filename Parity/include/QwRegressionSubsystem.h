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

class QwRegressionSubsystem:
    public VQwSubsystemParity, 
 //   public MQwCloneable<QwRegressionSubsystem>, // causes undefined reference error 
    public QwRegression {

  
  public:
      // Constructors
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
      QwRegressionSubsystem(const QwRegressionSubsystem &source): VQwSubsystem(source), VQwSubsystemParity(source), QwRegression(source) {};
      void Copy (const QwRegressionSubsystem &source);
      
      // Destructor 
      ~QwRegressionSubsystem();

      /// \brief Update the running sums
      void AccumulateRunningSum(QwRegressionSubsystem *value);
      /// \brief Calculate the average for all good events
      void CalculateRunningAverage();
      /// \brief Print values for all channels
      void PrintValue() const;


      /// \brief Overloaded Operators
      QwRegressionSubsystem* operator= (const QwRegressionSubsystem *value);
      QwRegressionSubsystem& operator+= (QwRegressionSubsystem *value);
      QwRegressionSubsystem& operator-= (const QwRegressionSubsystem *value);
      QwRegressionSubsystem& operator*= (const QwRegressionSubsystem *value);
      QwRegressionSubsystem& operator/= (const QwRegressionSubsystem *value);
      void Sum(QwRegressionSubsystem *value1, QwRegressionSubsystem *value2);
      void Difference(QwRegressionSubsystem *value1, QwRegressionSubsystem *value2);

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
      VQwSubsystem& operator=(VQwSubsystem* value);
      VQwSubsystem& operator+=(VQwSubsystem* value);
      VQwSubsystem& operator-=(VQwSubsystem* value);
      void Sum(VQwSubsystem*value1, VQwSubsystem* value2);
      void Difference(VQwSubsystem* value1, VQwSubsystem* value2);
      void Ratio(VQwSubsystem* value1, VQwSubsystem* value2);
      void Scale(Double_t value);
      VQwSubsystem* Copy();
      void AccumulateRunningSum(VQwSubsystem* input){};
      Bool_t ApplySingleEventCuts();
      Int_t GetEventcutErrorCounters();
      UInt_t GetEventcutErrorFlag();
      
      void Copy(VQwSubsystem* source);
      
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

