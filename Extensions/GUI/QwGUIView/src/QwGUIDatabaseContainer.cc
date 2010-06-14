#include <QwGUIDatabaseContainer.h>

ClassImp(QwGUIDatabaseContainer);

QwGUIDatabaseContainer::QwGUIDatabaseContainer(const TGWindow *p, const TGWindow *main, 
					       const char *objName, const char *ownername, const char *dataname, 
					       ERFileModes mode, ERFileTypes type)
  : RDataContainer(p, main, objName, ownername, dataname, mode,type)
{ 
  dParent = p;
  dMain   = main;

  memset(dMiscbuffer, '\0', sizeof(dMiscbuffer));
  memset(dMiscbuffer2, '\0', sizeof(dMiscbuffer2));

  dDatabase = NULL;
}

QwGUIDatabaseContainer::~QwGUIDatabaseContainer()
{  

  //RDataContainer::~RDataContainer() //Not sure if I need this
}

int QwGUIDatabaseContainer::OpenDatabase()
{

  DatabaseOptions opts;
  opts.changeFlag = kFalse;
  opts.cancelFlag = kFalse;
  opts.dbserver = "bla";
  opts.dbname   = "";
  opts.uname    = "";
  opts.psswd    = "";
  opts.dbport   = 0;
  
 printf("QWANALYSIS = %s\n", gSystem->Getenv("QWANALYSIS"));

 gQwOptions.Usage();

 printf("HasValue(QwDatabase.dbserver) = %i\n", gQwOptions.HasValue("QwDatabase.dbserver"));

  if (gQwOptions.HasValue("QwDatabase.dbserver")) {opts.dbserver = gQwOptions.GetValue<string>("QwDatabase.dbserver");}
  if (gQwOptions.HasValue("QwDatabase.dbname")) {opts.dbname = gQwOptions.GetValue<string>("QwDatabase.dbname");}
  if (gQwOptions.HasValue("QwDatabase.dbusername")) {opts.uname = gQwOptions.GetValue<string>("QwDatabase.dbusername");}
  if (gQwOptions.HasValue("QwDatabase.dbpassword")) {opts.psswd = gQwOptions.GetValue<string>("QwDatabase.dbpassword");}
  if (gQwOptions.HasValue("QwDatabase.dbport")) {opts.dbport = gQwOptions.GetValue<int>("QwDatabase.dbport");}

  //QwGUIDatabaseSelectionDialog *sdlg =
  new QwGUIDatabaseSelectionDialog(dParent, dMain, "sdlg","QwGUIMain",&opts);

  if(opts.cancelFlag) return PROCESS_FAILED;
  if(opts.changeFlag){

     printf("server = %s\n",opts.dbserver.Data());
//     printf("name = %s\n",opts.dbname.Data());
//     printf("uname = %s\n",opts.uname.Data());
//     printf("psswd = %s\n",opts.psswd.Data());
//     printf("port = %d\n",opts.dbport);

    RDataContainer *cont =  new RDataContainer(dParent, dMain,"cont","QwGUIMain","",FM_WRITE, FT_TEXT);
    if(cont){
      if(cont->OpenFile(Form("%s/Extensions/GUI/temp_mysql.conf",gSystem->Getenv("QWANALYSIS"))) == FILE_PROCESS_OK){
	sprintf(dMiscbuffer,"[QwDatabase]\ndbserver = %s\ndbname = %s\ndbusername = %s\ndbpassword = %s\ndbport = %d\n",
		opts.dbserver.Data(),opts.dbname.Data(),opts.uname.Data(),opts.psswd.Data(),opts.dbport);
	if(cont->WriteData(dMiscbuffer,strlen(dMiscbuffer)) == FILE_PROCESS_OK){
	  gQwOptions.SetConfigFile(Form("%s/Extensions/GUI/temp_mysql.conf",gSystem->Getenv("QWANALYSIS")));
	}
	cont->Close();
      }
    }
  }

  dDatabase = new QwDatabase(gQwOptions);

  if(!dDatabase) return PROCESS_FAILED;


  return FILE_PROCESS_OK;

}

void QwGUIDatabaseContainer::CloseDatabase()
{
  if(dDatabase) delete dDatabase;
  dDatabase = NULL;

  RDataContainer::Close();
}
