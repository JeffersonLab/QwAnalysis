
//#include "QwAnalysis.h"
//#include "TApplication.h"
//#include <boost/shared_ptr.hpp>

#include "QwEPICSControl.h"
#include "GreenMonster.h"

Int_t rhwpscan(QwEPICSControl, int, int, int, int);
Int_t pitascan(QwEPICSControl, int, int, int);
Int_t IACscan(QwEPICSControl, int, int, int);

Int_t main(Int_t argc, Char_t* argv[])
{
  cout << "\nStarting fast epics communication... please be patient.\n";
  QwEPICSControl fEPICSCtrlVar;  

  //variable declarations
  char menuChoice;
  int newValue;
  // initial parameters
  int kRHWPNumberOfPoints=19;
  int kRHWPSecondsPerPoint=30;
  int kRHWPStartAngleSet=0;
  int kRHWPEndAngleSet=9000;
  int kPITANumberOfPoints=3;
  int kPITASecondsPerPoint=60;
  int kPITATotalVoltageRange=240;
  int kIAscanNumberOfPoints=3;
  int kIAscanSecondsPerPoint=60;
  int kIAscanVoltageRange=2;
  //while loop to display menu until user exits
  while(1)//menuChoice != 8)
    {
      printf("================================================================================\n");
      printf("||  please choose an item on the menu below                                   ||\n");
      printf("||----------------------------------------------------------------------------||\n");
      printf("||  (1) Do a PITA scan                                                        ||\n");
      printf("||                 (%2i points at %3i seconds per point over %3i Volts range.) ||\n", 
	     kPITANumberOfPoints, kPITASecondsPerPoint, kPITATotalVoltageRange);
      printf("||  (2) Do a RHWP scan                                                        ||\n");
      printf("||                 (%2i points at %3i seconds per point from %4i to %4i.)    ||\n",
	     kRHWPNumberOfPoints, kRHWPSecondsPerPoint, kRHWPStartAngleSet, kRHWPEndAngleSet);
      printf("||  (3) Do an IA scan                                                         ||\n");
      printf("||                 (%2i points at %3i seconds per point over %2i Volts range.)  ||\n", 
	     kIAscanNumberOfPoints, kIAscanSecondsPerPoint, kIAscanVoltageRange);
      printf("||  (4) Change PITA scan parameters                                           ||\n");
      printf("||  (5) Change RHWP scan parameters                                           ||\n");
      printf("||  (6) Change IA scan parameters                                             ||\n");
      printf("||  (7) Print current injector properties                                     ||\n");
      printf("||  (0) Quit                                                                  ||\n");
      printf("================================================================================\n");
      if (fEPICSCtrlVar.Get_Pockels_Cell_status()!=1) {
	printf("||  WARNING!!!  Pockels Cell is OFF!  WARNING!!!                              ||\n");
	printf("================================================================================\n");
      }
      
      cout << "Your Choice: ";
      cin >> menuChoice;
      
      switch(menuChoice)
	{
	case '1':
	  //system("clear");
	  pitascan(fEPICSCtrlVar, kPITANumberOfPoints, kPITASecondsPerPoint, kPITATotalVoltageRange);
	  system("clear");
	  break;
	case '2':
	  //system("clear");
	  rhwpscan(fEPICSCtrlVar, kRHWPNumberOfPoints, kRHWPSecondsPerPoint, kRHWPStartAngleSet, kRHWPEndAngleSet);
	  system("clear");
	  break;
	case '3':
	  //system("clear");
	  IACscan(fEPICSCtrlVar, kIAscanNumberOfPoints, kIAscanSecondsPerPoint, kIAscanVoltageRange);
	  system("clear");
	  break;
	case '4':
	  //system("clear");
	  printf("New PITA scan parameters\nEnter the number of points: ");
	  cin >> newValue;
	  kPITANumberOfPoints=newValue;
	  printf("Enter the number of seconds per point: ");
	  cin >> newValue;
	  kPITASecondsPerPoint=newValue;
	  printf("Enter the total voltage range: ");
	  cin >> newValue;
	  kPITATotalVoltageRange=newValue;
	  system("clear");
	  break;
	case '5':
	  //system("clear");
	  printf("New RHWP scan parameters\nEnter the number of points: ");
	  cin >> newValue;
	  kRHWPNumberOfPoints=newValue;
	  printf("Enter the number of seconds per point: ");
	  cin >> newValue;
	  kRHWPSecondsPerPoint=newValue;
	  printf("Enter the starting angle: ");
	  cin >> newValue;
	  kRHWPStartAngleSet=newValue;
	  printf("Enter the ending angle: ");
	  cin >> newValue;
	  kRHWPEndAngleSet=newValue;
	  system("clear");
	  break;
	case '6':
	  //system("clear");
	  printf("New IA scan parameters\nEnter the number of points: ");
	  cin >> newValue;
	  kIAscanNumberOfPoints=newValue;
	  printf("Enter the number of seconds per point: ");
	  cin >> newValue;
	  kIAscanSecondsPerPoint=newValue;
	  printf("Enter the total voltage range: ");
	  cin >> newValue;
	  kIAscanVoltageRange=newValue;
	  system("clear");
	  break;
	case '7':
	  printf("\n");
	  fEPICSCtrlVar.Print_Qasym_Ctrls();
	  break;
	case '0':
	  return 1;
	default:
	  system("clear");
	  break;

	}//ends switch
     
    }//ends while
         
  return 0;
}//ends main

Int_t rhwpscan(QwEPICSControl fEPICSCtrl,
	       int kNumberOfPoints=19,
	       int kSecondsPerPoint=10,
	       int kStartAngleSet=0,    
	       int kEndAngleSet=9000) 
{
  int kAngleSetChangePerStep=(kEndAngleSet-kStartAngleSet)/(kNumberOfPoints-1);

  printf("-----------------------------\n");
  printf("Doing an automated RHWP scan.\n");
  printf("-----------------------------\n");
  printf("Taking %i points of %i seconds each.\n",kNumberOfPoints,kSecondsPerPoint);
  printf("Scan will take about %i minute %i seconds.\n",
	 (kNumberOfPoints*kSecondsPerPoint)/60,(kNumberOfPoints*kSecondsPerPoint)%60);
  if ((kEndAngleSet-kStartAngleSet)%(kNumberOfPoints-1)!=0) {
    printf("Error: Angle range not integer divisible by number of steps.\n\n");
    return -1;
  }
  printf("RHWP change %i per step\n",kAngleSetChangePerStep);
  

  cout << "Starting scan control.\n";
  GreenMonster  fScanCtrl;
  //  Let's clear the values for the scan data & set "not clean".
  fScanCtrl.Open();
  fScanCtrl.SCNSetStatus(SCN_INT_NOT);
  fScanCtrl.SCNSetValue(1,0);
  fScanCtrl.SCNSetValue(2,0);
  fScanCtrl.CheckScan();
  fScanCtrl.PrintScanInfo();

  double RHWPangleInit = fEPICSCtrl.Get_RHWP_angle();
  printf("Initial RHWP angle %.3f\n",RHWPangleInit);

  for (int count=0; count<kNumberOfPoints; count++) {
    // Not clean while setting the RHWP position
    fScanCtrl.SCNSetStatus(SCN_INT_NOT);
    usleep(100);
    double RHWPangleSet  = kStartAngleSet + (count*kAngleSetChangePerStep);
    if (RHWPangleSet<50) RHWPangleSet=50;
    printf("\n");
    fEPICSCtrl.Set_RHWP_angle(RHWPangleSet);
    int failcount=0;
    printf("Waiting for angle to be set.\n");
    usleep(1000000);
    // Now lets check that the angle took
    double RHWPangleGet = fEPICSCtrl.Get_RHWP_angle();
    while (RHWPangleGet!=RHWPangleSet) {
      failcount++;
      printf("%3i  %.0f != %.0f\n", failcount, RHWPangleGet, RHWPangleSet);
      usleep(1000000);
      RHWPangleGet = fEPICSCtrl.Get_RHWP_angle();
    }
    usleep(1000000);
    // Set the scan data points
    fScanCtrl.SCNSetValue(1, RHWPangleGet);
    fScanCtrl.SCNSetValue(2, count+1);
    fScanCtrl.SCNSetStatus(SCN_INT_CLN);
    fScanCtrl.CheckScan();
    fScanCtrl.PrintScanInfo();
    sleep(kSecondsPerPoint);
  }

  // Return voltages to initial numbers and clearing the scandata.
  printf("\nScan complete!!\n");
  fEPICSCtrl.Set_RHWP_angle(RHWPangleInit);
  fScanCtrl.SCNSetStatus(SCN_INT_NOT);
  fScanCtrl.SCNSetValue(1,0);
  fScanCtrl.SCNSetValue(2,0);
  fScanCtrl.CheckScan();
  fScanCtrl.PrintScanInfo();
  fScanCtrl.Close();
  usleep(1000000);
  printf("Wait a moment for the RHWP to return to its initial point.");
  // *** Make some noise to signal the end of the scan.
  for (int i=0; i<=10; i++) {
    printf("\a\n");
    usleep(200000);
  }
  return 0;
}


Int_t pitascan(QwEPICSControl fEPICSCtrl,
	       int kNumberOfPoints=3,
	       int kSecondsPerPoint=30,
	       int kTotalVoltageRange=12)
{  
  double kVernierVoltagePerStep=kTotalVoltageRange/(kNumberOfPoints-1.000)/60;
  int kTotalVoltagePerStep=kTotalVoltageRange/(kNumberOfPoints-1);

  printf("-----------------------------\n");
  printf("Doing an automated PITA scan.\n");
  printf("-----------------------------\n");
  printf("Taking %i points of %i seconds each.\n",kNumberOfPoints,kSecondsPerPoint);
  printf("Scan will take about %i minute %i seconds.\n",
	 (kNumberOfPoints*kSecondsPerPoint)/60,(kNumberOfPoints*kSecondsPerPoint)%60);
  if (kTotalVoltageRange%(kNumberOfPoints-1)!=0) {
    printf("Error: Voltage range not integer divisible by number of steps.\n\n");
    return -1;
  }
  printf("PITA change %i Volts per step, %.3f Volts on vernier\n",kTotalVoltagePerStep,kVernierVoltagePerStep);
  

  cout << "Starting scan control.\n";
  GreenMonster  fScanCtrl;
  //  Let's clear the values for the scan data & set "not clean".
  fScanCtrl.Open();
  fScanCtrl.SCNSetStatus(SCN_INT_NOT);
  fScanCtrl.SCNSetValue(1,0);
  fScanCtrl.SCNSetValue(2,0);
  fScanCtrl.CheckScan();
  fScanCtrl.PrintScanInfo();

  double PCplusInit  = fEPICSCtrl.Get_Pockels_Cell_plus();
  double PCminusInit = fEPICSCtrl.Get_Pockels_Cell_minus();
  printf("Initial voltages (%.3f,%.3f)\n",PCplusInit,PCminusInit);
  double PCplusMin  = PCplusInit  - kTotalVoltageRange/2./60.;
  double PCminusMax = PCminusInit + kTotalVoltageRange/2./60.;
  printf("First Point (%.3f,%.3f)\n",PCplusMin,PCminusMax);
  int PCTotalPITAMin = -kTotalVoltageRange/2;

  printf("Scan will take points at: ");
  for (int count=0; count<kNumberOfPoints; count++) {
    printf("(%5.3f,%5.3f) ",PCplusMin+(count*kVernierVoltagePerStep),PCminusMax-(count*kVernierVoltagePerStep));
  }
  printf("\n");

  for (int count=0; count<kNumberOfPoints; count++) {
    // Not clean while setting the voltages
    fScanCtrl.SCNSetStatus(SCN_INT_NOT);
    usleep(100);
    double PCplusSet  = PCplusMin  + (count*kVernierVoltagePerStep);
    double PCminusSet = PCminusMax - (count*kVernierVoltagePerStep);
    printf("\n");
    fEPICSCtrl.Set_Pockels_Cell_plus(PCplusSet);
    fEPICSCtrl.Set_Pockels_Cell_minus(PCminusSet);

    usleep(200);
    // Now lets check that the voltages took
    double PCplusGet  = fEPICSCtrl.Get_Pockels_Cell_plus();
    double PCminusGet  = fEPICSCtrl.Get_Pockels_Cell_minus();
    while (PCplusGet!=PCplusSet || PCminusGet!=PCminusSet) {
      //printf("%.3f != %.3f or %.3f != %.3f\n",PCplusGet,PCplusSet,PCminusGet,PCminusSet);
      usleep(200);
      PCplusGet  = fEPICSCtrl.Get_Pockels_Cell_plus();
      PCminusGet  = fEPICSCtrl.Get_Pockels_Cell_minus();
    }
    printf("Waiting for voltages to stabilize.\n");
    usleep(1000000);
    // Set the scan data points
    fScanCtrl.SCNSetValue(1, PCTotalPITAMin + (count*kTotalVoltagePerStep));
    fScanCtrl.SCNSetValue(2, count+1);
    fScanCtrl.SCNSetStatus(SCN_INT_CLN);
    fScanCtrl.CheckScan();
    fScanCtrl.PrintScanInfo();
    sleep(kSecondsPerPoint);
  }

  // Return voltages to initial numbers and clearing the scandata.
  printf("\nScan complete\n");
  fEPICSCtrl.Set_Pockels_Cell_plus(PCplusInit);
  fEPICSCtrl.Set_Pockels_Cell_minus(PCminusInit);
  fScanCtrl.SCNSetStatus(SCN_INT_NOT);
  fScanCtrl.SCNSetValue(1,0);
  fScanCtrl.SCNSetValue(2,0);
  fScanCtrl.CheckScan();
  fScanCtrl.PrintScanInfo();
  fScanCtrl.Close();
  // *** Make some noise to signal the end of the scan.
  for (int i=0; i<=5; i++) {
    printf("\a\n");
    usleep(200000);
  }
  return 0;
}


Int_t IACscan(QwEPICSControl fEPICSCtrl,
	       int kNumberOfPoints=3,
	       int kSecondsPerPoint=30,
	       int kVoltageRange=4)
{  
  //  double kVernierVoltagePerStep=kTotalVoltageRange/(kNumberOfPoints-1.000)/60;
  Float_t kVoltagePerStep=kVoltageRange/(kNumberOfPoints-1.0);

  printf("-----------------------------\n");
  printf("Doing an automated IA scan.\n");
  printf("-----------------------------\n");
  printf("Taking %i points of %i seconds each.\n",kNumberOfPoints,kSecondsPerPoint);
  printf("Scan will take about %i minute %i seconds.\n",
	 (kNumberOfPoints*kSecondsPerPoint)/60,(kNumberOfPoints*kSecondsPerPoint)%60);
//   if (kVoltageRange%(kNumberOfPoints-1)!=0) {
//     printf("Error: Voltage range not integer divisible by number of steps.\n\n");
//     return -1;
//   }
  printf("IA change %.2f Volts per step\n",kVoltagePerStep);
  

  cout << "Starting scan control.\n";
  GreenMonster  fScanCtrl;
  //  Let's clear the values for the scan data & set "not clean".
  fScanCtrl.Open();
  fScanCtrl.SCNSetStatus(SCN_INT_NOT);
  fScanCtrl.SCNSetValue(1,0);
  fScanCtrl.SCNSetValue(2,0);
  fScanCtrl.CheckScan();
  fScanCtrl.PrintScanInfo();

  double IACInit  = fEPICSCtrl.Get_HallCIA();
  printf("Initial voltage %.2f\n",IACInit);
  double IACMin  = IACInit - kVoltageRange/2.;
  printf("First Point %.2f Volts\n",IACMin);
//   int IAMinVolt = -kVoltageRange/2;

  printf("Scan will take points at: ");
  for (int count=0; count<kNumberOfPoints; count++) {
    printf("%5.3f, ",IACMin+(count*kVoltagePerStep));
  }
  printf("\n");

  for (int count=0; count<kNumberOfPoints; count++) {
    // Not clean while setting the voltages
    fScanCtrl.SCNSetStatus(SCN_INT_NOT);
    usleep(100);
    double IACSet  = IACMin  + (count*kVoltagePerStep);
    printf("\n");
    fEPICSCtrl.Set_HallCIA(IACSet);

    usleep(200);
    // Now lets check that the voltages took
    double IACGet  = fEPICSCtrl.Get_HallCIA();
    while (IACGet!=IACSet) {
      //printf("%.3f != %.3f or %.3f != %.3f\n",IACGet,IACSet);
      usleep(200);
      IACGet  = fEPICSCtrl.Get_HallCIA();
    }
    printf("Waiting for voltages to stabilize.\n");
    usleep(3000000);
    // Set the scan data points
    fScanCtrl.SCNSetValue(1, IACMin + (count*kVoltagePerStep));
    fScanCtrl.SCNSetValue(2, count+1);
    fScanCtrl.SCNSetStatus(SCN_INT_CLN);
    fScanCtrl.CheckScan();
    fScanCtrl.PrintScanInfo();
    sleep(kSecondsPerPoint);
  }

  // Return voltages to initial numbers and clearing the scandata.
  printf("\nScan complete\n");
  fEPICSCtrl.Set_HallCIA(IACInit);
  fScanCtrl.SCNSetStatus(SCN_INT_NOT);
  fScanCtrl.SCNSetValue(1,0);
  fScanCtrl.SCNSetValue(2,0);
  fScanCtrl.CheckScan();
  fScanCtrl.PrintScanInfo();
  fScanCtrl.Close();
  // *** Make some noise to signal the end of the scan.
  for (int i=0; i<=5; i++) {
    printf("\a\n");
    usleep(200000);
  }
  return 0;
}
