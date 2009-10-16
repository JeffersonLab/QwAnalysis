#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "TROOT.h"

#include "THaCodaFile.h"
#include "CodaAnalysis.hh"
#include "TextColor.hh"

const char* program_name;

// Some functions are from "advanced linux programming chapter 2" 

void 
print_usage (FILE* stream, int exit_code)
{
  fprintf (stream, "This program is used to look at HEX numbers in the CODA file.\n");
  fprintf (stream, "Usage: %s -i {coda file}  -r {a:b} -p {n} -s \n", program_name);
  fprintf (stream, 
	   " -i input CODA file.\n"
	   " -r event range [a,b) with a < b\n"
	   " -s print summary of CODA file\n"
	   " -p specified physics event number\n"
	   );
  exit (exit_code);
}


int 
main(int argc, char* argv[])
{ 

  bool file_flag    = false;
  bool range_flag   = false;
  bool evnum_flag   = false;
  bool summary_flag = false;
  char* filename    = NULL;

  unsigned int  evnum_range[2] = {0,0};
  unsigned int  physics_evnum  = 0;
  
  program_name = argv[0];

  int cc = 0; 

  while ( (cc= getopt(argc, argv, "i:r:p:s")) != -1)
    switch (cc)
      {
      case 'i':
	{
	  file_flag = true;
	  filename  = optarg;
	}
	break;
      case 'p':
	{
	  evnum_flag    = true;
	  physics_evnum = atoi(optarg);
	}
	break;
      case 's':
	{
	  summary_flag = true;
	}
	break;
     case 'r':
	{
	  range_flag = true;
	  char *c;
	  /*
	   * Allow the specification of alterations
	   * to the pty search range.  It is legal to
	   * specify only one, and not change the
	   * other from its default.
	   */
	  c = strchr(optarg, ':');
	  if (c) {
	    *c++ = '\0';
	    evnum_range[1] = atoi(c);
	  }
	  if (*optarg != '\0')
	    evnum_range[0] = atoi(optarg);
	  if ((evnum_range[0] > evnum_range[1]) || (evnum_range[0] < 0) ) 
	    {
	      print_usage(stdout,0);
	    }
	}
	break;
      case '?':
	{
	  if (optopt == 'r')
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  else if (optopt == 'i')
	    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  else if (isprint (optopt))
	    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	  else
	    fprintf (stderr, "Unknown option character `\\x%x'.\n",  optopt);
	  return 1;
	}
	break;
      default:
	abort () ;
      }
  
  if (file_flag)
    {
      unsigned int total_event_number         = 0;
      unsigned int total_physics_event_number = 0;
      
      CodaAnalysis *analysis = new CodaAnalysis(filename);
      
      analysis -> CheckCodaFile(summary_flag);
      
      total_event_number         = analysis->GetTotalEvtNum();
      total_physics_event_number = analysis->GetTotalPhysEvtNum();
      
      if( (evnum_range[0]==0)&&(evnum_range[1]==0) )
	{
	  evnum_range[0] = 0;
	  evnum_range[1] = total_event_number;
	}
      
      if( !summary_flag )
	{
	  printf("Total event number range ");
	  printf("[%d,%d) ", evnum_range[0], evnum_range[1]);
	  printf("Note that event number is not the physics event number\n\n");
	}
      
      if( physics_evnum > total_physics_event_number )
	{
	  text_color(RED);
	  printf("WARNING ");
	  reset_color();
	  printf(": the number you selected is out of range of total physics event number\n");
	  return 0;
	}
      
      if( evnum_range[1] > total_event_number )
	{
	  text_color(RED);
	  printf("WARNING ");
	  reset_color();
	  printf(": the number you selected is out of range of total event number\n");
	  return 0;
	}
      
      if (range_flag || evnum_flag ) 
	{
	  analysis -> Init();
	  for (unsigned int iev=0; iev<evnum_range[1]; iev++) 
	    {
	      if( range_flag )
		{
		  if(iev < evnum_range[0]) analysis -> SkipCodaBuffer();
		  else 	                   analysis -> PrintBuffer(  analysis -> ReadCodaBuffer() );
		}
	      else if ( evnum_flag )
		{
		  analysis -> CheckCodaBuffer(physics_evnum);
		}
	      else
		{
		  analysis -> PrintBuffer(  analysis -> ReadCodaBuffer() );
		}
	    }
	  analysis -> End();
	}
      return 0;
    }
  else
    {
      print_usage(stdout,0);
    }
  
}

