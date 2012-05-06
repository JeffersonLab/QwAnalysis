#include <stdlib.h>
#include <QwAutoGUI.h>
#include <string>		
#include <unistd.h>
#include <sys/stat.h>

QwAutoGUI::QwAutoGUI(const TGWindow *p, ClineArgs args,
		     UInt_t w, UInt_t h)
  : TGMainFrame(p, w, h)
{
  dClArgs = args;

  dCurrentRun = -1;
  dCurrentSegment = -1;
  dPID = 0;
  DoWarn();
}

bool pid_from_file_is_running_process(const std::string filename, 
				      const std::string process) {
  // If stat(2) fails, the file either doesn't exist or something
  // weird has happened.  This check shouldn't be necessary for this
  // but it would be easy to forget to add later.
  struct stat buf; 
  if (stat(filename.c_str(), &buf)) return false; 

  if (0 == system( ("ps $(cat " + filename + ") | grep '" + process + "'").c_str() ))
    return true; 
  return false;
}

void QwAutoGUI::DoWarn()
{
  int tmp, res = 1;
  pid_t pid = 0;
  FILE *fp = NULL;
  Int_t retval;
  TString NewRun;
  Int_t run = -1;
  Int_t seg = -1;
  time_t start, now;

  if(dClArgs.remoterestart == 0){
  
    fp = fopen(Form("%s/Extensions/GUI/QwGUID_PID.DAT",gSystem->Getenv("QWANALYSIS")),"r");


    if(fp // && pid_from_file_is_running_process("QwGUID_PID.DAT","QwGUI -u true")
       ) {
      printf("%s:%d\n", __FILE__,__LINE__);
      strcpy(dMiscbuffer,"There is already a auto update version of the\n");
      strcat(dMiscbuffer,"QwGUI program running.\n");
      strcat(dMiscbuffer,"To maintain efficient operation between\n");
      strcat(dMiscbuffer,"the analyzer and the GUI, only one\n");
      strcat(dMiscbuffer,"such instance is allowed.\n\n");
      strcat(dMiscbuffer,"Do you want to close the running process\n");
      strcat(dMiscbuffer,"and restart a new one?\n");
      new TGMsgBox(fClient->GetRoot(),this,
		   "Running Process",dMiscbuffer,
		   kMBIconExclamation, kMBOk | kMBCancel, &retval);
      if(retval == kMBCancel) 
	CloseWindow();  
      else{
	fscanf(fp,"%d",&tmp);
	pid = (pid_t)tmp;
	fclose(fp);
	res = kill(pid,SIGUSR2);
	if(res) {
	  strcpy(dMiscbuffer,"Can't establish contact with QwAGUI program.\n\n");
	  sprintf(dMiscbuffer2,"This must be an orphaned process (PID = %d).\n",tmp);
	  strcat(dMiscbuffer,dMiscbuffer2);
	  strcat(dMiscbuffer,"Please remove the process manually.\n");
	  strcat(dMiscbuffer,"The program will now be restarted.\n");
	  new TGMsgBox(fClient->GetRoot(),this,
		       "Running Process",dMiscbuffer,
		       kMBIconExclamation, kMBOk, &retval);
	}
	system(Form("rm %s/Extensions/GUI/QwGUID_PID.DAT",gSystem->Getenv("QWANALYSIS")));
	
	pid_t child = fork();
	if (-1 == child) perror("couldn't fork to show change history");
	else if (0 == child) {
	  
	  execl("/bin/sh", "/bin/sh", "-c",
		Form("%s/Extensions/GUI/QwGUI -u true",gSystem->Getenv("QWANALYSIS")),
		(char*)0);
	  // execl(Form("%s/Extensions/GUI",gSystem->Getenv("QWANALYSIS")), "QwGUI", "-u true",(char*)0);
	  perror("couldn't exec QwGUI");
	  exit(1);
	  
	  // system(Form("gnome-terminal -e '%s/Extensions/GUI/QwGUI -u true &'",gSystem->Getenv("QWANALYSIS")));
	  // 	CloseWindow();  
	}
      }
    }
    else{

      pid_t child = fork();
      if (-1 == child) perror("couldn't fork to show change history");
      else if (0 == child) {
	
	execl("/bin/sh", "/bin/sh", "-c",
	      Form("%s/Extensions/GUI/QwGUI -u true",gSystem->Getenv("QWANALYSIS")),
	      (char*)0);
	// execl(Form("%s/Extensions/GUI",gSystem->Getenv("QWANALYSIS")), "QwGUI", "-u true",(char*)0);
	perror("couldn't exec QwGUI");
	exit(1);
      }
    
      // printf("%s:%d\n", __FILE__,__LINE__);
      // system(Form("gnome-terminal -e '%s/Extensions/GUI/QwGUI -u true &'",gSystem->Getenv("QWANALYSIS")));
    }
  }
  else{

    system(Form("rm %s/Extensions/GUI/QwGUID_PID.DAT",gSystem->Getenv("QWANALYSIS")));
    
    pid_t child = fork();
    if (-1 == child) perror("couldn't fork to show change history");
    else if (0 == child) {
      
      execl("/bin/sh", "/bin/sh", "-c",
	    Form("%s/Extensions/GUI/QwGUI -u true",gSystem->Getenv("QWANALYSIS")),
	    (char*)0);
      // execl(Form("%s/Extensions/GUI",gSystem->Getenv("QWANALYSIS")), "QwGUI", "-u true",(char*)0);
      perror("couldn't exec QwGUI");
      exit(1);
      
    }
    
  }

  // have to wait a while for the GUI to come up and do its stuff 
  gSystem->Sleep(4000); //4 seconds  

  fp = 0;

  fp = fopen(Form("%s/Extensions/GUI/QwGUID_PID.DAT",gSystem->Getenv("QWANALYSIS")),"r");
  if(fp){
    // printf("%s:%d\n", __FILE__,__LINE__);
    fscanf(fp,"%d",&tmp);
    dPID = (pid_t)tmp;
    fclose(fp);
  }  

  if(dPID){
  
    while(1){
      ///  TODO:  figure out a more clever way of getting only file
      ///         names like first100k_#####.root other than this
      ///         annoyingly nested filename.
      ///  Use reverse time ordering of the directory.
      NewRun = gSystem->GetFromPipe(Form("ls %s/first100k_[0-9][0-9][0-9][0-9][0-9].root -1rt | tail -1",gSystem->Getenv("QW_ROOTFILES")));

      if(NewRun.Length()>0 && !NewRun.Contains("jlab.org")){
	
	GetFileInfo(NewRun.Data(),run,seg);
	
	if(dCurrentRun < run){
	  
	  fp=fopen("QwAGUIRun.DAT","w");
	  if(fp){
	    sprintf(dMiscbuffer2,"%s\n",NewRun.Data());
	    fwrite(dMiscbuffer2,1,strlen(dMiscbuffer2),fp);
	    fclose(fp);
	    res = kill(dPID,SIGUSR1);
	    dCurrentRun = run;
	  }
	}
      }
      gSystem->Sleep(10000);
    }
  }
}

void QwAutoGUI::GetFileInfo(const char *filename, int &run, int &segment)
{
  run = -1;
  segment = -1;
  TString tmpfile = filename;

  TObjArray *substrings = tmpfile.Tokenize("_.");
  TIter next(substrings->MakeIterator());
  TObject *obj = next();
  Int_t flag = 0;
  while(obj){
    TObjString *substr = (TObjString*)obj;
    if(!flag && (substr->GetString()).IsDigit()){
      run = (substr->GetString()).Atoi();
      flag = 1;
    }
    else if(flag && (substr->GetString()).IsDigit()){
      segment = (substr->GetString()).Atoi();	
    }
    obj = next();
    delete substr;
  }
}


QwAutoGUI::~QwAutoGUI()
{

}

void QwAutoGUI::CloseWindow()
{
  if(dPID)
    kill(dPID,SIGUSR2);

  gApplication->Terminate(0);
}

// void QwAutoGUI::OnCloseSignal(int sig)
// {
//   gApplication->Terminate(0);
// }

// QwAutoGUI *gMain;

// void CloseSignal(int sig)
// {
//   gMain->OnCloseSignal(sig);
// }

int main(int argc, char **argv)
{

  TApplication theApp("QwAutoGUI", &argc, argv);
  
  if (gROOT->IsBatch()) {
    fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
    return 1;
  }
  
  ClineArgs dClArgs;
  dClArgs.remoterestart = 0;

  if(argv[1]){

    for(int i = 1; i < argc; i++){
      if(strcmp(argv[i],"-r")==0)
	dClArgs.remoterestart = 1;
    }

  }

  QwAutoGUI mainWindow(gClient->GetRoot(), dClArgs,
		       800, 500);

  // gMain = &mainWindow;

  // (void) signal(SIGUSR1,CloseSignal);
  
  theApp.Run();
  
  return 0;
}
