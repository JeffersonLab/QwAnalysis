/*
 *
 **/

// Include BOOST headers and setup boost namespace
#include <boost/program_options.hpp>
namespace po = boost::program_options;

// Standard Library includes
#include <iostream>
#include <iterator>
#include <cstdlib>
#include <fstream>
#include <dlfcn.h>

// Local includes
#include "ComptonSession.h"
#include "VComptonMacro.h"

int main(int argc, char *argv[])
{
  // Setup the command line options
  Int_t opt;
  TString config_file;
  TString db_file;
  Bool_t find_cycles = kTRUE;
  TString pass;
  std::vector<std::string> macros;
  TString macros_path;
  TString web_dir;
  TString storage_dir;

  // Generic command line options
  po::options_description generic("Generic options");
  generic.add_options()
    ("help,h","print this help message") // help
    ("run,r",po::value<int>(),"run number") // runnumber (required)
    ("config,c",po::value<std::string>(),"config file")
    ;
  // Shared with configuration file
  po::options_description config("Configuration");
  config.add_options()
    ("db-file",po::value<std::string>(),"database file name")
    ("with-db-cycles","uses the laser cycle definitions already in the database")
    ("pass,p",po::value<std::string>(),"pass prefix (i.e. Pass2b)")
    ("macros,m",po::value<std::vector<std::string> >(),"ordered list of macros to process")
    ("macros-path",po::value<std::string>(),"path to shared macros")
    ("web-dir",po::value<std::string>(),"path web directory")
    ("storage-dir",po::value<std::string>(),"path storage directory")
    ;

  // Finally, add them to boost
  po::options_description cmdline_options;
  cmdline_options.add(generic).add(config);

  po::options_description config_file_options;
  config_file_options.add(config);

  po::options_description visible("Allowed options");
  visible.add(generic).add(config);

  // Now process them generic options
  po::variables_map vm;
  po::store(po::parse_command_line(argc,argv,cmdline_options),vm);
  po::notify(vm);

  // First, check for the config file before parsing
  if(vm.count("config")) { // was specified!
    config_file = vm["config"].as<std::string>();
  } else {
    config_file = TString(std::getenv("QWSCRATCH"))+"/compton_pass_config.cfg";
  }
  // Open the file as a stream for boost
  ifstream ifs(config_file.Data());
  if(ifs) {
    // Process config
    po::store(parse_config_file(ifs,config_file_options),vm);
  } else {
    std::cout << "NOTICE: Did not find configuration file." << std::endl;
  }

  // Process help
  if(vm.count("help")) {
    std::cout << generic << "\n";
    return 1;
  }

  // Process the mandatory runnumber
  int runnumber;
  if(!vm.count("run")) {
    std::cerr << "No runnumber specified. Loudly dying!" << std::endl;
    return -1;
  } else {
    runnumber = vm["run"].as<int>();
  }

  // Process the mandatory database file
  if(!vm.count("db-file")){
    std::cerr << "Did not specify a database file!" << std::endl;
    return -1;
  } else {
    db_file = vm["db-file"].as<std::string>();
  }

  // Process mandatory pass name
  if(!vm.count("pass")) {
    std::cerr << "Did not specify a pass." << std::endl;
    return -1;
  } else {
    pass = vm["pass"].as<std::string>();
  }

  // What macros should we process?
  if(!vm.count("macros")){
    std::cerr << "No macros will be parsed" << std::endl;
  } else {
    macros = vm["macros"].as< std::vector<std::string> >();
  }

  // Should we find new cycles?
  if(vm.count("with-db-cycles")) {
    find_cycles = kFALSE;
  }

  // Process macro library dir
  if(vm.count("macros-path")) {
    macros_path = vm["macros-path"].as<std::string>();
  } else { // Default hard coded path!
    macros_path = TString(std::getenv("QWANALYSIS"))
      +"/Extensions/Compton/RunMacros/macros.d";
  }

  // Process mandatory web dir
  if(!vm.count("web-dir")) {
    std::cerr << "Must specify web directory!" << std::endl;
    return 1;
  } else {
    web_dir = vm["web-dir"].as<std::string>();
  }

  // Process mandatory storage dir
  if(!vm.count("storage-dir")) {
    std::cerr << "Must specify storage directory!" << std::endl;
    return 1;
  } else {
    storage_dir = vm["storage-dir"].as<std::string>();
  }



  // Create a session to handle the run
  ComptonSession *session = new ComptonSession(runnumber,db_file,pass);
  if(!session->SetWebDir(web_dir)) {
    std::cerr << "Web directory path not accessible!" << std::endl;
    return -1;
  }
  if(!session->SetStorageDir(storage_dir)) {
    std::cerr << "Storage directory path not accessible!" << std::endl;
    return -1;
  }
  if(find_cycles) {
    session->FindLaserCycles();
  } else {
    session->LaserCyclesFromDB();
  }

  // Setup the VComptonMacro API
  typedef VComptonMacro* create_t();
  typedef void destroy_t(VComptonMacro*);

  // Finally, process the dynamic macros
  for(std::vector<std::string>::iterator it = macros.begin();
      it != macros.end(); ++it) {
    void *handle;
    TString library=macros_path+Form("/lib%s.so",(*it).c_str());
    handle = dlopen(library.Data(),RTLD_NOW);
    if(!handle) {
      std::cout << "WARNING: Error processing library " << *it
        << " at " << library.Data()
        << "<<\t with error" << dlerror() << "\tSkipping!" << std::endl;
    } else {
      std::cout << "Processing macro: " << *it << std::endl;
      create_t* create=(create_t*)dlsym(handle,"create");
      destroy_t* destroy=(destroy_t*)dlsym(handle,"destroy");
      if(!create || !destroy) {
        std::cout << "ERROR: Cannot process " << *it
          << "\t -- " << dlerror() << std::endl;
      }

      VComptonMacro* m = create();
      m->init(session);
      m->run();
      destroy(m);
      std::cout << "Finished with macro: " << *it << std::endl;
    }
  }

  // Cleanup
  if(session)
    delete session;
  session = NULL;

  // Got this far? I guess all is well then! :D
  return 0;
}
