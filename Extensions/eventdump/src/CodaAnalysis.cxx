#include "CodaAnalysis.hh"
#include "TextColor.hh"

CodaAnalysis::CodaAnalysis(const char *s)
{

  coda_file_status           = -1;
  coda_read_status           = -1;
  physics_exist_status       = false;
  sync_exist_status          = false;
  prestart_exist_status      = false;
  go_exist_status            = false;
  end_exist_status           = false;

  total_event_number         = 0;
  total_physics_event_number = 0;


  int string_length = strlen(s);          // set size
  filename = new char[string_length + 1]; // allot storage
  strcpy(filename,s);                     // initialize pointer; 
  coda_file = NULL;


};


CodaAnalysis::~CodaAnalysis()
{
  reset_color();
  delete coda_file;
  delete [] filename;
};


const char *const CodaAnalysis::char_event_type[21] = 
  {
    "", "Physics","",
    "","","","","","",
    "","","","","","","",
    "Sync", "PreStart", "Go", "Pause", "End"
  };



void
CodaAnalysis::OpenCodaFile()
{
  coda_file        = new THaCodaFile();
  coda_file_status = coda_file -> codaOpen( filename );
};

void
CodaAnalysis::CloseCodaFile()
{
  coda_file->codaClose();
  delete coda_file; coda_file = NULL;

};


void
CodaAnalysis::SkipCodaBuffer()
{

  coda_read_status = coda_file->codaRead();  
  
  if( coda_read_status!=S_SUCCESS )
    {
      if ( coda_read_status==EOF ) 
	{
	  //printf("Normal end of file.\n");
	  // 	  exit(EXIT_SUCCESS);
	}
      else
	{
	  printf("ERROR: codaRread status = 0x%x\n", coda_read_status);
	}
    }
};



void
CodaAnalysis::CheckCodaBuffer(int physics_event_number)
{
  if( coda_read_status!=S_SUCCESS )
    {
      if ( coda_read_status==EOF ) 
	{
	  //	  printf("Normal end of file.\n");
	  // 	  exit(EXIT_SUCCESS);
	}
      else
	{
	  printf("ERROR: coda read status = 0x%x\n", coda_read_status);
	}
    }
  int *buffer = ReadCodaBuffer();
  int evtype = buffer[1]>>16;
  int evnum = 0;
  
  if (evtype ==  PhysicsEvent) 
    {
      evnum = buffer[4];
      if(evnum == physics_event_number) PrintBuffer(buffer);
    }
  
};


int 
*CodaAnalysis::ReadCodaBuffer()
{

  coda_read_status = coda_file->codaRead();  
  
  if( coda_read_status!=S_SUCCESS )
    {
      if ( coda_read_status==EOF ) 
	{
	  // 	  printf("Normal end of file.\n");
	  // 	  exit(EXIT_SUCCESS);
	}
      else
	{
	  printf("ERROR: codaRread status = 0x%x\n", coda_read_status);
	}
    }
  
  return coda_file->getEvBuffer();
  
};



void
CodaAnalysis::Init()
{
 OpenCodaFile();
};


void
CodaAnalysis::End()
{
  CloseCodaFile();
};


void 
CodaAnalysis::CheckCodaFile(bool summary_status)
{
  
  OpenCodaFile();

  if(summary_status) printf("\n::: Begin : ----- Summary ----- :::\n");
  
  if(coda_file_status != S_SUCCESS)
    {
      printf("error opening %s file\n", filename);
      CloseCodaFile();
      exit(0);
    }
  else
    {
      static const int MAXEVTYPE = 40; //200; Why 200?
      int evtype_sum[MAXEVTYPE]  = {0};
      
      for (short kk=0; kk<MAXEVTYPE; kk++) evtype_sum[kk] = 0;
      
      unsigned int total_nevent = 0;
      short        event_type = 0;
      int          *dbuffer     = NULL;
      
      while ( (coda_file->codaRead()) == S_SUCCESS ) 
	{
	  total_nevent++;
	  if(summary_status) printf("Total number events count : %d\r", total_nevent);

	  dbuffer    = coda_file->getEvBuffer();
	  event_type = dbuffer[1]>>16;

	  if ((event_type>=0) && (event_type<MAXEVTYPE)) 
	    {
	      evtype_sum[event_type]++;
	    }
	}
      
      total_event_number         = total_nevent;
      total_physics_event_number = evtype_sum[PhysicsEvent];//total_event_number - 3; 

      // 3 represents "PreStart, Go, and End Events"
	  
      // The following lines are valid when the CODA file has "PreStart", "GO", and
      // "Physics". If not, the whole decoding is wrong.
      
      // prestart and end event have only one entry number
      if(evtype_sum[PreStartEvent] == 1) prestart_exist_status = true;
      else                               prestart_exist_status = false;
      if(evtype_sum[EndEvent]      == 1) end_exist_status      = true;
      else                               end_exist_status      = false;
      // go may have more ?
      if(evtype_sum[GoEvent]       == 0) go_exist_status       = false;
      else                               go_exist_status       = true;
      if(evtype_sum[PauseEvent]    == 0) pause_exist_status    = false;
      else                               pause_exist_status    = true;
      if(evtype_sum[SyncEvent]     == 0) sync_exist_status     = false;
      else                               sync_exist_status     = true;
      if(evtype_sum[PhysicsEvent]  == 0) physics_exist_status  = false;
      else                               physics_exist_status  = true;
      
      if(summary_status) 
	{
	  printf("Summary of event types in file %s\n", filename);
	  
	  for (int ety=0; ety<MAXEVTYPE; ety++) 
	    {
	      if (evtype_sum[ety] != 0) 
		{
		  printf("Event Type ");
		  if(ety == 1 ) {text_attribute(BOLD); text_color(BLUE);}
		  else          text_color(RED);
		  printf("%8s ", char_event_type[ety]);
		  reset_color();
		  printf("[%2d] : total EvNum  %d\n",  ety, evtype_sum[ety]);
		}
	    }
	  
	  printf("---------------------------------------\n");
	  if(!prestart_exist_status) 
	    printf("NO PRESTART EVENT in the CODA file\n");
	  if(!end_exist_status) 
	    printf("NO END      EVENT in the CODA file\n");
	  if(!go_exist_status) 
	    printf("NO GO       EVENT in the CODA file\n");
	  if(!pause_exist_status) 
	    printf("NO PAUSE    EVENT in the CODA file\n");
	  if(!sync_exist_status) 
	    printf("NO SYNC     EVENT in the CODA file\n");
	  if(!physics_exist_status) 
	    printf("NO PHYSICS  EVENT in the CODA file\n");
	  printf("---------------------------------------\n");
	  printf("Total Event Number is %d\n", total_event_number);
	  printf("::: End : ----- Summary ----- :::\n\n");
	}
    }
  CloseCodaFile();
}; 


void 
CodaAnalysis::PrintBuffer(int*buffer) 
{

  int len    = buffer[0] + 1;
  int evtype = buffer[1]>>16;
  int evnum  = 0;
  
  bool physics_event_status =  false;
  
  if (evtype ==  PhysicsEvent) physics_event_status = true;
  else                         physics_event_status = false;
  
  if(physics_event_status)  evnum = buffer[4];
  else                      evnum = -1;
  text_attribute(BOLD);
  if(evtype==PhysicsEvent) text_color(BLUE);
  else                     text_color(RED);
  printf("%8s ", char_event_type[evtype]);
  reset_color();
  printf("EvNum ");
  
  if(physics_event_status)   
    {
      text_attribute(BOLD);
      printf(" %d, ", evnum);
      reset_color();
    }
  
  printf("Length %d [evtype: %d] \n", len, evtype);
  
  int ipt = 0;
  
  for (int j = 0; j < (len/5); j++) 
    {
      printf("buffer[%5d] = 0x:", ipt);
      for (int k=j; k<j+5; k++) 
	{
	  printf("%12x", buffer[ipt++]);
	}
      printf("\n");
    }
  
  if (ipt < len) 
    {
      printf("buffer[%5d] = 0x:", ipt);
      for (int k=ipt; k<len; k++) 
	{
	  printf("%12x", buffer[ipt++]);
	}
      printf("\n");
    }
  printf("\n");
    

};
