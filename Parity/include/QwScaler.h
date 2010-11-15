
#ifndef __QWSCALER__
#define __QWSCALER__


#include <vector>

#include "VQwSubsystem.h"
#include "VQwSubsystemParity.h"
#include "QwScaler_Channel.h"


class QwScaler: public VQwSubsystemParity
{

	public:
		QwScaler ( TString region_tmp ) :VQwSubsystem ( region_tmp ),VQwSubsystemParity ( region_tmp )
		{};

		~QwScaler(){
// 			DeleteHistograms();
		};

//derived from VQwSubsystem
		void ProcessOptions ( QwOptions &options ); //Handle command line options
		Int_t LoadChannelMap ( TString mapfile );
		Int_t LoadInputParameters ( TString pedestalfile );
		Int_t LoadEventCuts ( TString & filename );
		Bool_t SingleEventCuts();
		Int_t ProcessConfigurationBuffer ( const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words );
		Int_t ProcessConfigurationBuffer ( UInt_t ev_type, const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words );
		Int_t ProcessEvBuffer ( UInt_t roc_id, UInt_t bank_id, UInt_t *buffer, UInt_t num_words );
		Int_t ProcessEvBuffer ( UInt_t ev_type, UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words );
		void  PrintDetectorID();
		void  ClearEventData();
		void  ProcessEvent();
		void  ConstructHistograms ( TDirectory *folder, TString &prefix );
		void  FillHistograms();
		void  DeleteHistograms();


		void ConstructBranchAndVector ( TTree *tree, TString &prefix, std::vector<Double_t> &values );
		void ConstructBranch(TTree *tree, TString& prefix);
  		void ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& trim_file);
		void FillTreeVector ( std::vector<Double_t> &values ) const;
		VQwSubsystem& operator= ( VQwSubsystem *value );
		VQwSubsystem& operator+= ( VQwSubsystem *value );
		VQwSubsystem& operator-= ( VQwSubsystem *value );
		VQwSubsystem* Copy();
		void  Copy ( VQwSubsystem *source );
		void  Sum ( VQwSubsystem  *value1, VQwSubsystem  *value2 );
		void  Difference ( VQwSubsystem  *value1, VQwSubsystem  *value2 );
		void  Ratio ( VQwSubsystem  *value1, VQwSubsystem  *value2 );
		void  Scale ( Double_t );
		void AccumulateRunningSum ( VQwSubsystem* value );
		void CalculateRunningAverage();
		Int_t LoadEventCuts ( TString filename );
		Bool_t ApplySingleEventCuts();

		Int_t GetEventcutErrorCounters();
		UInt_t GetEventcutErrorFlag();
		Bool_t Compare ( VQwSubsystem *source );
		void print();
		void PrintValue() const;
		float* GetRawChannelArray();

		Int_t GetChannelIndex ( TString channelName, UInt_t module_number );
		float GetDataForChannelInModule ( Int_t module_number, Int_t channel_index )
		{
			return fSCAs.at(module_number).at(channel_index).GetValue();
		}

		float GetDataForChannelInModule ( Int_t module_number, TString channel_name )
		{
			return -1;
// 			return GetDataForChannelInModule ( module_number, GetChannelIndex ( channel_name,module_number ) );
		}


	protected:

                Int_t fTreeArrayIndex;

		std::vector<std::vector< QwSIS3801D24_Channel> >  fSCAs;
};

#endif
