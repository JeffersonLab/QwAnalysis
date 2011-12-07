/*------------------------------------------------------------------------*//*!

 \defgroup QwGUIHelpBrowser QwGUIHelpBrowser

 \section helpbrowser


*//*-------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*//*!

 \file QwGUIHelpBrowser.cc
 \author Michael Gericke

 \brief Help Browser Frame class for the QwGUI executable


*//*-------------------------------------------------------------------------*/

#include <QwGUIHelpBrowser.h>

ClassImp(QwGUIHelpBrowser);


QwGUIHelpBrowser::QwGUIHelpBrowser(const TGWindow *p, const TGWindow *main, const char *objName, 
				   const char *mainname, const char* file, UInt_t w, UInt_t h) 
  : TGHtmlBrowser(file,main,w,h) 
{
  strcpy(dMainName, mainname);
  strcpy(dObjName, objName);

  Connect("IsClosing(const char*)",dMainName,(void*)main,"OnObjClose(const char*)");    
};

QwGUIHelpBrowser::~QwGUIHelpBrowser()
{
  IsClosing(dObjName);
};

void QwGUIHelpBrowser::IsClosing(const char *objname)
{
  Emit("IsClosing(const char*)",(long)objname);
}


// QwGUIHelpBrowser *gHelpBrowser;

// int main(int argc, char **argv)
// {

//   if(argv[1]){
// //     for( int i=1; i < argc; i++){
// //       if(strcmp(argv[i],"-r")==0){
// // 	dClArgs.realtime = kTrue;
// //       }
      
// //     }
//   }
//   else
//     printf("\nRun ""QwGUIHelpBrowser -help"" for command line help\n\n");

// //   if(help){
// //     strcpy(expl,"\n\nThis program takes the following commandline arguments:\n\n");
// //     strcat(expl,"1) -b        Read binary format file.\n\n");
// //     strcat(expl,"2) -t        Read ascii text file in row and column format.\n\n");
// //     strcat(expl,"3) -f        Starting filename:\n\n");
// //     strcat(expl,"             Here one of two file types must be used, based on\n");
// //     strcat(expl,"             the -b or -t parameters passed .\n");
// //     strcat(expl,"             For case -b, the program expects a\n");
// //     strcat(expl,"             a binary file with format to be specified\n");
// //     strcat(expl,"             For case -t, it expects an ascii file \n");
// //     strcat(expl,"             arranged in rows and columns of data.\n");
// //     strcat(expl,"             For the second case, the columns of interest must\n");
// //     strcat(expl,"             be specified with the -c switch (see below).\n");
// //     strcat(expl,"             Always use the full path for the input files.\n\n");
// //     strcat(expl,"4) -c        Columns. Ex: (-c 23) selects columns 2 and 3.\n\n");
// //     strcat(expl,"9) -help     Prints this help \n\n");

// //     printf("%s",expl);
// //   }
// //   else{

//     TApplication theApp("QwGUIHelpBrowser", &argc, argv);

//     gROOT->SetStyle("Plain");

//     if (gROOT->IsBatch()) {
//       fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
//       return 1;
//     }

//     QwGUIHelpBrowser mainWindow(0,gClient->GetRoot(), 800,600);

//     gHelpBrowser = &mainWindow;

//     theApp.Run();
// //   }

//   return 0;
// }
