/**
 *  QwCommandLine.h
 *
 *    Qweak Command line handler class
 *
 */


#ifndef __QWCOMMANDLINE__
#define __QWCOMMANDLINE__

#include <getopt.h>
#include <cstdlib>
#include <string>
#include <Rtypes.h>


///
/// \ingroup QwAnalysis
class QwCommandLine{
 public:
  QwCommandLine(): fDEBUG(kTRUE),fRunRange(0,0),fEventRange(0,kMaxInt){};
  ~QwCommandLine(){ };

  Int_t Parse(Int_t argc, Char_t* argv[]);


  Int_t GetFirstRun(){return fRunRange.first;};
  Int_t GetLastRun(){return fRunRange.second;};

  Int_t GetFirstEvent(){return fEventRange.first;};
  Int_t GetLastEvent(){return fEventRange.second;};

  Bool_t DoOnlineAnalysis(){return fDoOnline==1;};

 protected:
  void Usage();

  std::pair<Int_t, Int_t> ParseRange(std::string flag, std::string range);

 protected:
  Bool_t fDEBUG;

  std::string fCommandName;

  std::pair<Int_t, Int_t> fRunRange;
  std::pair<Int_t, Int_t> fEventRange;

  Int_t fDoOnline;

};

void QwCommandLine::Usage()
{
  std::cerr << "\nUsage: " << fCommandName << " -r #[:#]  [-e #[:#]]\n"
	    << "\t -r #[:#]  Run range (required)\n"
	    << "\t -e #[:#]  Event range\n"
	    << "\t           Both the run range and the event range\n"
	    << "\t           may be expressed as a single integer\n"
	    << "\t           or as a colon separated range.\n"
	    << "\t           If the first character is a colon, the\n"
	    << "\t           starting vlue will be taken to be zero.\n"
	    << "\t           If the final character is a colon, the\n"
	    << "\t           ending value will be taken to be "
	    << kMaxInt << ".\n"
	    << std::endl;
  exit(1);
};



Int_t QwCommandLine::Parse(Int_t argc,Char_t* argv[])
{
  /** @brief Parses the options from the command line
   *
   *  Parses the options from the command line using
   *  getopt.
   *  We may want to change to use the boost::program_options
   *  library instead or in addition.
   */

  if (fDEBUG){
    //  Add an extra line in the output if we're sending debugging
    //  output to stderr.
    std::cerr << std::endl;
  }
  fCommandName = argv[0];               //  Store the command name.
  /* First set up some long options.      */
  int option_index = 0;
  static struct option long_options[] =
    {
      {"online", 0, &fDoOnline, 1},
      {"run", 1, 0, 'r'},
      {"event", 1, 0, 'e'},
      {0, 0, 0, 0}
    };
  int option_char;
  while ((option_char = getopt_long(argc, argv, "r:e:",
				    long_options, &option_index)) != EOF){
    switch (option_char)
      {
      case 0:{
	printf ("option %s", long_options[option_index].name);
	if (optarg)
	  printf (" with arg %s", optarg);
	printf ("\n");
	break;
      }
      case 'r':{
	fRunRange = ParseRange("r", optarg);
	break;
      }
      case 'e':{
	fEventRange = ParseRange("e", optarg);
	break;
      }
      default:
	Usage();
      }
  }
  if (fDoOnline == 1){
#ifdef __CODA_ET
    //  If we're doing online analysis, clobber the run and event limits.
    std::cerr << "\nWARN:  In online mode, the run and event number ranges are ignored!"
	      << std::endl;
    fRunRange.first    = 0;
    fRunRange.second   = 0;
    fEventRange.first  = 0;
    fEventRange.second = kMaxInt;
#else
    std::cerr << "\nERROR:  Online mode will not work without the CODA libraries!"
	      << std::endl;
    exit(2);
#endif
  } else if (fRunRange.first == 0){
    std::cerr << "\nERROR:  There must be at least one run specified!"
	      << std::endl;
    Usage();
  }
  if (fDEBUG){
    //  Add an extra line in the output if we're sending debugging
    //  output to stderr.
    std::cerr << std::endl;
  }
  return optind;
}


std::pair<Int_t, Int_t> QwCommandLine::ParseRange(std::string flag,
						  std::string range)
{
  /** @brief Separates a colon separated range of integers
   *         into a pair of values.
   *
   *  @param flag  The flag which has this range as the arguement;
   *               this is only used in generating error messages.
   *  @param range String containing two integers separated
   *               by a colon, or a single value.
   *               If the string begins with a colon, the
   *               first value is taken as zero.
   *               If the string ends with a colon, the second
   *               value is taken as kMaxInt.
   *
   *  @return  Pair of integers of the first and last values
   *           of the range.
   *           If the range contains a single value, the
   *           two integers will be identical.
   */
  std::pair<Int_t, Int_t> mypair;
  size_t pos   = range.find(":");
  if (pos == std::string::npos){
    //  Separator not found.
    mypair.first  = atoi(range.substr(0,range.length()).c_str());
    mypair.second = mypair.first;
  } else {
    size_t end = range.length() - pos - 1;
    if (pos == 0) {
      // Separator is the first character
      mypair.first  = 0;
      mypair.second = atoi(range.substr(pos+1, end).c_str());
    } else if (pos == range.length()-1) {
      // Separator is the last character
      mypair.first  = atoi(range.substr(0,pos).c_str());
      mypair.second = kMaxInt;
    } else {
      mypair.first  = atoi(range.substr(0,pos).c_str());
      mypair.second = atoi(range.substr(pos+1, end).c_str());
    }
  }
  //  Check the values for common errors.
  if (mypair.first < 0){
    std::cerr << "ERROR in QwCommandLine::ParseRange for flag \"-"
	      << flag << "\": "
	      << "The first value must not be negative!"
	      << std::endl;
    exit(1);
  } else if (mypair.first > mypair.second){
    std::cerr << "ERROR in QwCommandLine::ParseRange for flag \"-"
	      << flag << "\": "
	      << "The first value must not be larger than the second value"
	      << std::endl;
    exit(1);
  }
  //  Print the contents of the pair for debugging.
  if (fDEBUG){
    std::cerr << "INFO in QwCommandLine::ParseRange for flag \"-"
	      << flag << "\": "
	      << "The range goes from " << mypair.first
	      << " to " << mypair.second
	      << std::endl;
  }
  return mypair;
};



#endif
