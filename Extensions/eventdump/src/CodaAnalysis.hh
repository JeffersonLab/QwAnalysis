#ifndef CODA_ANALYSIS_HH
#define CODA_ANALYSIS_HH


#include "TROOT.h"
#include "TFile.h"

#include "THaCodaFile.h"


#include <iostream>
#include <iomanip>
#include <string>

using namespace std;


enum EventType {
  PhysicsEvent  = 1, 
  SyncEvent     = 16, 
  PreStartEvent = 17,
  GoEvent       = 18, 
  PauseEvent    = 19, 
  EndEvent      = 20
};


class THaCodaFile;
class CodaAnalysis 
{

private :

  THaCodaFile   *coda_file;
  char          *filename;
  int           coda_file_status;
  int           coda_read_status;

  unsigned int  total_event_number;
  unsigned int  total_physics_event_number;

  bool          physics_exist_status;
  bool          sync_exist_status;
  bool          prestart_exist_status;
  bool          go_exist_status;
  bool          pause_exist_status;
  bool          end_exist_status;

  static const char *const  char_event_type[21];

public:
  CodaAnalysis(const char *s);
  CodaAnalysis();
  ~CodaAnalysis();

 
  void PrintBuffer(int *buffer);
  int *ReadCodaBuffer();
  void SkipCodaBuffer();
  void CheckCodaBuffer(int physics_event_number);
  void Init();
  void End();
  void OpenCodaFile();
  void CloseCodaFile();
  void CheckCodaFile(bool summary_status);

  unsigned int GetTotalEvtNum()     { return total_event_number; };
  unsigned int GetTotalPhysEvtNum() { return total_physics_event_number; };


};
#endif
