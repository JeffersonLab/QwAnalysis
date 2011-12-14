#include "QwEPICSControl.h"
#include "GreenMonster.h"
#include <csignal>

// see /home/cdaq/qweak/QwAnalysis/new/Analysis/include/QwColor.h

#define BLACK       "\033[30m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define BROWN       "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define BOLD        "\033[1m"
#define NORMAL      "\033[0m"

Bool_t globalEXIT;

Int_t rhwpscan(QwEPICSControl fEPICSCtrl,
			   int &kNumberOfPoints,
			   int &kSecondsPerPoint,
			   int &kStartAngleSet,    
			   int &kEndAngleSet);
Int_t pitascan_DAC(QwEPICSControl fEPICSCtrl,
				   int &kNumberOfPoints,
				   int &kSecondsPerPoint,
				   int &kActualVoltageRange);
 //Int_t rhwpscan(QwEPICSControl, int, int, int, int);
//Int_t pitascan_DAC(QwEPICSControl, int, int, int);
Int_t generalDACscan(QwEPICSControl);
Int_t IACscan(QwEPICSControl, int, int, int);
Int_t pedestalscan(QwEPICSControl);
Int_t PCtransScanAuto(QwEPICSControl);
Int_t PCtransScan(QwEPICSControl);

void sigint_handler(int sig)
{
	std::cout << "handling signal no. " << sig << "\n";
	globalEXIT=1;
}

Int_t main(Int_t argc, Char_t* argv[])
{
	globalEXIT=0;
	cout << "\nStarting fast epics communication... please be patient.\n";
	QwEPICSControl fEPICSCtrlVar;  

	//variable declarations
	char menuChoice;
	int newValue;
	// initial parameters
	int kRHWPNumberOfPoints=79;
	int kRHWPSecondsPerPoint=15;
	int kRHWPStartAngleSet=100;
	int kRHWPEndAngleSet=7900;
	int kPITANumberOfPoints=5;
	int kPITASecondsPerPoint=10;
	int kPITATotalVoltageRange=120;
	//while loop to display menu until user exits
	while(1)
	{
		printf("%s=====================================\n", GREEN);
		printf("||  please choose                  ||\n");
		printf("||---------------------------------||\n");
		printf("||  (1) Pedestal scan              ||\n");
		printf("||  (2) Do a %sP%sITA scan             ||\n", NORMAL,GREEN);
		printf("||  (3) Do a %sR%sHWP scan             ||\n", NORMAL,GREEN);
		printf("||  (4) general DAC scan (IA scan) ||\n");
		printf("||  (7) Automatic PC trans scan X  ||\n");
		printf("||  (8) PC translation scan helper ||\n");
		printf("||  (9) Print injector properties  ||\n");
		printf("||  (0) %sQ%suit                       ||\n", NORMAL,GREEN);
		printf("=====================================%s\n", NORMAL);
		if (fEPICSCtrlVar.Get_Pockels_Cell_status()!=1) {
		printf("||  WARNING!!!  PC is OFF!         ||\n");
		printf("=====================================\n");
		}
		cout << "Your Choice: ";
		cin >> menuChoice;
      
		switch(menuChoice)
		{
		case '1':
			pedestalscan(fEPICSCtrlVar);
			system("clear");
			break;
		case '2':
			pitascan_DAC(fEPICSCtrlVar, kPITANumberOfPoints, 
						 kPITASecondsPerPoint, kPITATotalVoltageRange);
			system("clear");
			break;
		case 'p':
			pitascan_DAC(fEPICSCtrlVar, kPITANumberOfPoints, 
						 kPITASecondsPerPoint, kPITATotalVoltageRange);
			system("clear");
			break;
		case 'P':
			pitascan_DAC(fEPICSCtrlVar, kPITANumberOfPoints, 
						 kPITASecondsPerPoint, kPITATotalVoltageRange);
			system("clear");
			break;
		case '3':
			rhwpscan(fEPICSCtrlVar, kRHWPNumberOfPoints, kRHWPSecondsPerPoint, 
					 kRHWPStartAngleSet, kRHWPEndAngleSet);
			system("clear");
			break;
		case 'r':
			rhwpscan(fEPICSCtrlVar, kRHWPNumberOfPoints, kRHWPSecondsPerPoint, 
					 kRHWPStartAngleSet, kRHWPEndAngleSet);
			system("clear");
			break;
		case 'R':
			rhwpscan(fEPICSCtrlVar, kRHWPNumberOfPoints, kRHWPSecondsPerPoint, 
					 kRHWPStartAngleSet, kRHWPEndAngleSet);
			system("clear");
			break;
		case '4':
			generalDACscan(fEPICSCtrlVar);
			system("clear");
			break;
		case '7':
			PCtransScanAuto(fEPICSCtrlVar);
			system("clear");
			break;
		case '8':
			PCtransScan(fEPICSCtrlVar);
			system("clear");
			break;
		case '9':
			printf("\n");
			fEPICSCtrlVar.Print_Qasym_Ctrls();
			break;
		case '0':
			return 1;
		case 'Q':
			return 1;
		case 'q':
			return 1;
		default:
			system("clear");
			break;

		}//ends switch
     
    }//ends while
         
	return 0;
}//ends main


Int_t pedestalscan(QwEPICSControl fEPICSCtrl)
{ 
	char dummychar;
	Int_t kSecondsPerPoint=1;
	printf("--------------------\n");
	printf("Pedestal scan helper\n");
	printf("--------------------\n");
	printf("\nMake sure that the BPMs are in %sgains off%s mode!\n\n",RED,NORMAL);
	printf("This routine will write the value of the epics channel \"FCupsCORRECTED.VAL\" \n");
	printf("(the current on the first Faraday cup i the beam) to the DAQ quantity \"scandata1\".\n");
	printf("This is used to have a more accurate value of the current for plotting the BPM wire.\n");
	printf("signals against for the purposes of fitting.\n\n");
	printf("You can ctrl-C out at any point.\n");

	printf("Press %sA%s for auto scan, all else for helper scan:\n",BOLD,NORMAL);
	cin >> dummychar;
	cout << "Starting scan control.\n";
	GreenMonster  fScanCtrl;
	//  Let's clear the values for the scan data & set "not clean".
	fScanCtrl.Open();
	fScanCtrl.SCNSetStatus(SCN_INT_NOT);
	fScanCtrl.SCNSetValue(1,0);
	fScanCtrl.SCNSetValue(2,0);
	fScanCtrl.CheckScan();
	fScanCtrl.PrintScanInfo();

	switch(dummychar)
	{
	case 'A':
		printf("New pedestal scan not implemented\n");
		return 0;
		break;
	case 'Q':
		return 0;
		break;
	default:
		while (1) {
			double fc2current = fEPICSCtrl.Get_FC2_Current();
			double claseratt  = fEPICSCtrl.Get_CLaser_Att();
			fScanCtrl.SCNSetValue(1, fc2current*1000);
			fScanCtrl.SCNSetValue(2, claseratt);
			//    fScanCtrl.SCNSetStatus(SCN_INT_CLN);
			fScanCtrl.CheckScan();
			fScanCtrl.PrintScanInfo();
			signal(SIGINT, sigint_handler);
			if (globalEXIT) { 		// Allow ctrl-C escape 
				globalEXIT=0;
				cout << "Exiting pedestal scan.\n";
				// Clearing the scandata.
				fScanCtrl.SCNSetStatus(SCN_INT_NOT);
				fScanCtrl.SCNSetValue(1,0);
				fScanCtrl.SCNSetValue(2,0);
				fScanCtrl.CheckScan();
				fScanCtrl.PrintScanInfo();
				fScanCtrl.Close();
				return 0;
			}
		}
	}
	return 0;
}

Int_t PCtransScan(QwEPICSControl fEPICSCtrl)
{ 
	char dummychar;
	Int_t kSecondsPerPoint=1;
	printf("------------------------------------\n");
	printf("PC translation scan helper\n");
	printf("------------------------------------\n");
	printf("\nThis routine will write the value of the epics channels \"cdipc_xpos\" and\n");
	printf("\"cdipc_ypos\" (the PC X and Y positions) to the DAQ quantities \"scandata1\"\n");
	printf("and \"scandata2\" respectively.  This is used to have a more accurate value \n");
	printf("of the position for plotting and to complete the scan quicker.\n");
	printf("\nYou can ctrl-C out at any point.\n");
	printf("\nUnits are 1000 mils.\n");
// 	printf("Enter any value to continue.\n");
// 	cin >> dummychar;
// 	cout << "Starting scan control.\n";
	GreenMonster  fScanCtrl;
	//  Let's clear the values for the scan data & set "not clean".
	fScanCtrl.Open();
	fScanCtrl.SCNSetStatus(SCN_INT_NOT);
	fScanCtrl.SCNSetValue(1,0);
	fScanCtrl.SCNSetValue(2,0);
	fScanCtrl.CheckScan();
	fScanCtrl.PrintScanInfo();

	//Double_t PCXPos, PCYPos;
	Int_t secondsclean=0;
	while (1) {
		Double_t PCXPos = fEPICSCtrl.Get_PCXPos();
		Double_t PCYPos = fEPICSCtrl.Get_PCYPos();
		fScanCtrl.SCNSetValue(1, PCXPos*1000000);
		fScanCtrl.SCNSetValue(2, PCYPos*1000000);
		//    fScanCtrl.SCNSetStatus(SCN_INT_CLN);
		fScanCtrl.CheckScan();
		fScanCtrl.PrintScanInfo();
		// Get scan status and publish length of clean
		if (fScanCtrl.SCNStatusIsClean()) {
			secondsclean++;
			printf("seconds clean: %i\n",secondsclean);
		} else {
			secondsclean=0;
		}

		// Allow ctrl-C escape while sleeping
		for (Int_t i=0; i<=kSecondsPerPoint; i++) {			
			usleep(100000);
			signal(SIGINT, sigint_handler);
			if (globalEXIT) {
				globalEXIT=0;
				return 0;
			}
		}
	}

	// Return to initial position and clear the scandata.
	printf("\nScan complete\n");
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





Int_t PCtransScanAuto(QwEPICSControl fEPICSCtrl)
{ 
	char dummychar;
	Int_t kSecondsPerPoint=10;
	int kNumberOfPoints=25;
	int kXMinimum=-60;
	int kXMaximum=120;
	Int_t kXRange = kXMaximum-kXMinimum;
	Int_t kXStepSize = kXRange/(kNumberOfPoints-1.);
// 	int kYMinimum=-120;
// 	int kYMaximum=120;
	Int_t IHWPinPosVoltage=10000;
	Int_t IHWPinNegVoltage=20000;
	Int_t IHWPoutPosVoltage=30000;
	Int_t IHWPoutNegVoltage=40000;
	Double_t stepspermil = 2000;
	Double_t tolerance = 0.5;
	Double_t stepratio = 0.8;
	Bool_t atposition = 0;


	printf("---------------------------------------\n");
	printf("Automatic PC translation scan\n");
	printf("---------------------------------------\n");
	printf("\nThis routine will write the value of the epics channels \"cdipc_xpos\" and\n");
	printf("\"cdipc_ypos\" (the PC X and Y positions) to the DAQ quantities \"scandata1\"\n");
	printf("and \"scandata2\" respectively. \n");
	printf("\nYou can ctrl-C out at any point.\n");
	printf("\nUnits are 1000 mils.\n");
// 	printf("Enter any value to continue.\n");
// 	cin >> dummychar;
// 	cout << "Starting scan control.\n";
	GreenMonster  fScanCtrl;
	//  Let's clear the values for the scan data & set "not clean".
	fScanCtrl.Open();
	fScanCtrl.SCNSetStatus(SCN_INT_NOT);
	fScanCtrl.SCNSetValue(1,0);
	fScanCtrl.SCNSetValue(2,0);
	fScanCtrl.CheckScan();
	fScanCtrl.PrintScanInfo();

	Double_t XPosSet, PCXPos, XPosMove;

	// loop over the steps
	for (int count=0; count<kNumberOfPoints; count++) {
		// loop over the attempts to make the step
		while (!atposition) {
			XPosSet = kXMinimum + count * kXStepSize;
			PCXPos = 1000 * fEPICSCtrl.Get_PCXPos();
			XPosMove = XPosSet-PCXPos;
			if (fabs(XPosMove)<tolerance) atposition=1;
			fScanCtrl.SCNSetValue(1, PCXPos*1000);
			Double_t NeededSteps = stepratio*(XPosMove*stepspermil);
			if (NeededSteps>25000) NeededSteps=25000;
			if (NeededSteps<-25000) NeededSteps=-25000;
			printf("XPosSet  %f, PCXPos %f, XPosMove %f, NeededSteps %f, atposition %i\n",
				   XPosSet,PCXPos,XPosMove,NeededSteps,atposition);
			fEPICSCtrl.Set_Pockels_X_Steps(fabs(NeededSteps));
			if(NeededSteps>0) 
				fEPICSCtrl.Set_Pockels_Move_Right();
			else 
				fEPICSCtrl.Set_Pockels_Move_Left();
			Int_t busy = 1;
			while (busy) {
				sleep(1);
				signal(SIGINT, sigint_handler);
				if (globalEXIT) {
					globalEXIT=0;
					return 0;
				}
				busy = fEPICSCtrl.Get_PCMoving();
			}
		}


		// Allow ctrl-C escape while sleeping
		for (Int_t i=0; i<=kSecondsPerPoint; i++) {			
			sleep(1);
			signal(SIGINT, sigint_handler);
			if (globalEXIT) {
				globalEXIT=0;
				return 0;
			}
		}
		// Get data for IHWP out
		
		// Get data for IHWP in

	}

	// Return to initial position and clear the scandata.
	printf("\nScan complete\n");
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





Int_t rhwpscan(QwEPICSControl fEPICSCtrl,
			   int &kNumberOfPoints,
			   int &kSecondsPerPoint,
			   int &kStartAngleSet,    
			   int &kEndAngleSet) 
{
	int kAngleSetChangePerStep;
	char inchar[2];
	printf("\n--------------------\n");
	printf("Automated RHWP scan.\n");
	printf("--------------------\n");
	Bool_t goodvals=0;
	Int_t checkcurrent=0;
	Float_t current=0;
	while (!goodvals) {
		kAngleSetChangePerStep=(kEndAngleSet-kStartAngleSet)/(kNumberOfPoints-1);
		printf("start angle:\t\t%i\n",kStartAngleSet);
		printf("end angle:\t\t%i\n",kEndAngleSet);
		printf("number of points:\t%i\n",kNumberOfPoints);
		printf("change per step:\t%i\n",kAngleSetChangePerStep);
		printf("seconds per point:\t%i\n",kSecondsPerPoint);
		printf("Minimum current 0L02:\t%i\n",checkcurrent);
		printf("%i points of %i seconds each => %i hours %i minutes %i seconds.\n",
			   kNumberOfPoints,kSecondsPerPoint,
			   (kNumberOfPoints*kSecondsPerPoint)/3600,
			   ((kNumberOfPoints*kSecondsPerPoint)%3660)/60,
			   (kNumberOfPoints*kSecondsPerPoint)%60);
		printf("\n'q' to quit, 'y' or '1' to accept, all else to change parameters:");
		cin >> inchar;
		if ((strcmp(inchar,"Q") == 0) || (strcmp(inchar,"q") == 0)) {
			return 0;
		}
		if ((strcmp(inchar,"Y") == 0) || (strcmp(inchar,"y") == 0) || (strcmp(inchar,"1") == 0)) {
			goodvals=1;
		}
		if (!goodvals) {
			int newValue;
			printf("New RHWP scan parameters\nEnter the number of points: ");
			cin >> newValue;
			kNumberOfPoints=newValue;
			printf("Enter the number of seconds per point: ");
			cin >> newValue;
			kSecondsPerPoint=newValue;
			printf("Enter the starting angle: ");
			cin >> newValue;
			kStartAngleSet=newValue;
			printf("Enter the ending angle: ");
			cin >> newValue;
			kEndAngleSet=newValue;
			printf("Enter minimum current on 0L02: ");
			cin >> newValue;
			checkcurrent=newValue;
		}
		if ((kEndAngleSet-kStartAngleSet)%(kNumberOfPoints-1)!=0) {
			printf("Error: Angle range not integer divisible by number of steps.\n\n");
			goodvals=0;
		}
	}
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
		if (RHWPangleSet<100) RHWPangleSet=100;
		printf("\n");
		fEPICSCtrl.Set_RHWP_angle(RHWPangleSet);
		int failcount=0;
		printf("Waiting for angle to be set.\n");
		usleep(1000000);
		// Now lets check that the angle took
		double RHWPangleGet = fEPICSCtrl.Get_RHWP_angle();
		while (abs(RHWPangleGet-RHWPangleSet)>0.1) {
			failcount++;
			printf("%3i  %.1f != %.1f\n", failcount, RHWPangleGet, RHWPangleSet);
			usleep(1000000);
			RHWPangleGet = fEPICSCtrl.Get_RHWP_angle();
		}
		if (checkcurrent>0) {
			current = fEPICSCtrl.Get_0L02_Current();
			if (current<checkcurrent) {
				printf("Waiting for beam: %f < %i\n",current,checkcurrent);
				while (current<checkcurrent) {
					current = fEPICSCtrl.Get_0L02_Current();
					sleep(1);
					signal(SIGINT, sigint_handler);
					if (globalEXIT) {
						globalEXIT=0;
						printf("\nCtrl-C detected\n");
						fEPICSCtrl.Set_RHWP_angle(RHWPangleInit);
						fScanCtrl.SCNSetStatus(SCN_INT_NOT);
						fScanCtrl.SCNSetValue(1,0);
						fScanCtrl.SCNSetValue(2,0);
						fScanCtrl.CheckScan();
						fScanCtrl.PrintScanInfo();
						fScanCtrl.Close();
						return 0;
					}
				}
			}
		}
		usleep(100000);
		Int_t numberofsecondsleft = (kNumberOfPoints-count)*kSecondsPerPoint;
		printf("%i hours %i minutes %i seconds remaining.\n",
			   numberofsecondsleft/3600,
			   (numberofsecondsleft%3600)/60,
			   numberofsecondsleft%60);
		// Set the scan data points
		fScanCtrl.SCNSetValue(1, round(RHWPangleGet));
		fScanCtrl.SCNSetValue(2, count+1);
		fScanCtrl.SCNSetStatus(SCN_INT_CLN);
		fScanCtrl.CheckScan();
		fScanCtrl.PrintScanInfo();
		// Allow ctrl-C escape while sleeping
		for (Int_t i=0; i<=kSecondsPerPoint; i++) {
			sleep(1);
			signal(SIGINT, sigint_handler);
			if (globalEXIT) {
				globalEXIT=0;
				printf("\nCtrl-C detected\n");
				fEPICSCtrl.Set_RHWP_angle(RHWPangleInit);
				fScanCtrl.SCNSetStatus(SCN_INT_NOT);
				fScanCtrl.SCNSetValue(1,0);
				fScanCtrl.SCNSetValue(2,0);
				fScanCtrl.CheckScan();
				fScanCtrl.PrintScanInfo();
				fScanCtrl.Close();
				usleep(1000000);
				printf("Wait a moment for the RHWP to return to its initial point.\n");
				return 0;
			}
		}
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

Int_t pitascan_DAC(QwEPICSControl fEPICSCtrl,
				   int &kNumberOfPoints,
				   int &kSecondsPerPoint,
				   int &kActualVoltageRange)
{  
	Bool_t localdebug=1;
	double kDACTicksPerVolt=65536/4000.;
	int kActualVoltagePerStep=(kActualVoltageRange)/(kNumberOfPoints-1.);
	int kDACTicksPerStep=kDACTicksPerVolt*kActualVoltagePerStep;

	char inchar[2];
	printf("\n--------------------\n");
	printf("Automated PITA scan.\n");
	printf("--------------------\n");
	Bool_t goodvals=0;
	while (!goodvals) {
		printf("number of points:\t%i\n",kNumberOfPoints);
		printf("seconds per point:\t%i\n",kSecondsPerPoint);
		printf("PITA change %i Volts per step, %i ticks on the DAC.\n",kActualVoltagePerStep,kDACTicksPerStep);
		printf("%i points of %i seconds each => %i minute %i seconds.\n",
			   kNumberOfPoints,kSecondsPerPoint,
			   (kNumberOfPoints*kSecondsPerPoint)/60,
			   (kNumberOfPoints*kSecondsPerPoint)%60);

		printf("\nEnter 'Y','y' or '1' to contuinue, all else to change parameters:");
		cin >> inchar;
		if ((strcmp(inchar,"Q") == 0) || (strcmp(inchar,"q") == 0)) {
			return 1;;
		}
		if ((strcmp(inchar,"Y") == 0) || (strcmp(inchar,"y") == 0) || (strcmp(inchar,"1") == 0)) {
			goodvals=1;
		}
		if (!goodvals) {
			int newValue;
			printf("New PITA scan parameters\nEnter the number of points: ");
			cin >> newValue;
			kNumberOfPoints=newValue;
			printf("Enter the number of seconds per point: ");
			cin >> newValue;
			kSecondsPerPoint=newValue;
			printf("Enter the total voltage range: ");
			cin >> newValue;
			kActualVoltageRange=newValue;
		}
		kActualVoltagePerStep=(kActualVoltageRange)/(kNumberOfPoints-1.);
		kDACTicksPerStep=kDACTicksPerVolt*kActualVoltagePerStep;
		if (kActualVoltageRange%(kNumberOfPoints-1)!=0) {
			printf("Error: Voltage range not integer divisible by number of steps.\n\n");
			return -1;
		}
	} 

	double PCplusDACInit  = fEPICSCtrl.Get_Pockels_Cell_plus_DAC();
	double PCminusDAQInit = fEPICSCtrl.Get_Pockels_Cell_minus_DAC();
	printf("Initial PC DAC values (%.0f,%.0f)\n",PCplusDACInit,PCminusDAQInit);
	double PCplusDAQMin  = PCplusDACInit  - kActualVoltageRange/2.*kDACTicksPerVolt;
	double PCminusDAQMax = PCminusDAQInit + kActualVoltageRange/2.*kDACTicksPerVolt;
	printf("First Point (%.0f,%.0f)\n",PCplusDAQMin,PCminusDAQMax);
	int PCTotalPITAMin = -kActualVoltageRange/2;
	printf("Scan will take points at: \n");
	for (int count=0; count<kNumberOfPoints; count++) {
		printf("%2i %5i   (%.0f,%.0f)\n", count,
			   PCTotalPITAMin + (count*kActualVoltagePerStep),
			   PCplusDAQMin   + (count*kDACTicksPerStep),
			   PCminusDAQMax  - (count*kDACTicksPerStep));
	}
	printf("\n");

	cout << "Starting scan control.\n";
	GreenMonster  fScanCtrl;
	//  Let's clear the values for the scan data & set "not clean".
	fScanCtrl.Open();
	fScanCtrl.SCNSetStatus(SCN_INT_NOT);
	fScanCtrl.SCNSetValue(1,0);
	fScanCtrl.SCNSetValue(2,0);
	fScanCtrl.CheckScan();
	fScanCtrl.PrintScanInfo();

	if (localdebug) {printf("localdebug: kActualVoltageRange %i   PCTotalPITAMin %i\n",
							kActualVoltageRange, PCTotalPITAMin);}

	for (int count=0; count<kNumberOfPoints; count++) {
		// Not clean while setting the voltages
		fScanCtrl.SCNSetStatus(SCN_INT_NOT);
		usleep(100);
		double PCplusDACSet  = PCplusDAQMin  + (count*kDACTicksPerStep);
		double PCminusDACSet = PCminusDAQMax - (count*kDACTicksPerStep);
		printf("\n");
		fEPICSCtrl.Set_Pockels_Cell_plus_DAC(PCplusDACSet);
		fEPICSCtrl.Set_Pockels_Cell_minus_DAC(PCminusDACSet);
		
		usleep(200);
		// Now lets check that the voltages took
		double PCplusDACGet  = fEPICSCtrl.Get_Pockels_Cell_plus_DAC();
		double PCminusDACGet  = fEPICSCtrl.Get_Pockels_Cell_minus_DAC();
		int counter1=0, counter2=0;
		while (PCplusDACGet!=PCplusDACSet || PCminusDACGet!=PCminusDACSet) {
			//printf("%.3f != %.3f or %.3f != %.3f\n",PCplusGet,PCplusSet,PCminusGet,PCminusSet);
			usleep(200);
			counter1++;
			PCplusDACGet  = fEPICSCtrl.Get_Pockels_Cell_plus_DAC();
			PCminusDACGet  = fEPICSCtrl.Get_Pockels_Cell_minus_DAC();
			if (counter1==10) {
				counter1=0;
				counter2++;
				if (counter2==10) {
					printf("Voltages won't take, bailing out.\n\n");
					return -1;
				}
				fEPICSCtrl.Set_Pockels_Cell_plus_DAC(PCplusDACSet);
				fEPICSCtrl.Set_Pockels_Cell_minus_DAC(PCminusDACSet);
			}
		}
		printf("Waiting for voltages to stabilize.\n");
		usleep(1000000);
		// Set the scan data points
		fScanCtrl.SCNSetValue(1, PCTotalPITAMin + (count*kActualVoltagePerStep));
		fScanCtrl.SCNSetValue(2, count+1);
		fScanCtrl.SCNSetStatus(SCN_INT_CLN);
		fScanCtrl.CheckScan();
		fScanCtrl.PrintScanInfo();
		// Allow ctrl-C escape while sleeping
		for (Int_t i=0; i<=kSecondsPerPoint; i++) {
			sleep(1);
			signal(SIGINT, sigint_handler);
			if (globalEXIT) {
				globalEXIT=0;
				printf("\nDoing a graceful exit.\n");
				fEPICSCtrl.Set_Pockels_Cell_plus_DAC(PCplusDACInit);
				fEPICSCtrl.Set_Pockels_Cell_minus_DAC(PCminusDAQInit);
				fScanCtrl.SCNSetStatus(SCN_INT_NOT);
				fScanCtrl.SCNSetValue(1,0);
				fScanCtrl.SCNSetValue(2,0);
				fScanCtrl.CheckScan();
				fScanCtrl.PrintScanInfo();
				fScanCtrl.Close();
				return 0;
			}
		}
	}

	// Return voltages to initial numbers and clearing the scandata.
	printf("\nScan complete\n");
	fEPICSCtrl.Set_Pockels_Cell_plus_DAC(PCplusDACInit);
	fEPICSCtrl.Set_Pockels_Cell_minus_DAC(PCminusDAQInit);
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

Int_t generalDACscan(QwEPICSControl fEPICSCtrl)
{	
	//variable declarations
	char menuChoice;
	int newValue;
	int kDACnumber, kNumberOfPoints, kSecondsPerPoint, kDACRange;
	int kDACTicksPerStep;
	printf("----------------------\n");
	printf("Automated IA DAC scan.\n");
	printf("----------------------\n");
	Bool_t doscan=0, goodIA=0;
	while (!doscan) {
		printf("New IA DAC scan parameters\n");
		printf("Enter the DAC number:                : ");
		cin >> newValue;
		kDACnumber = newValue;
		if (kDACnumber>=3 && kDACnumber<=14) {
			goodIA=1;
			if (kDACnumber>=3 && kDACnumber<=6) {
				printf("This is a HALL A IA!\n");
			}
			if (kDACnumber>=7 && kDACnumber<=10) {
				printf("This is a HALL B IA!\n");
			}
		}
		kNumberOfPoints=newValue;
		printf("Enter the number of points:          : ");
		cin >> newValue;
		kNumberOfPoints=newValue;
		printf("Enter the number of seconds per point: ");
		cin >> newValue;
		kSecondsPerPoint=newValue;
		printf("Enter the total DAC range:           : ");
		cin >> newValue;
		kDACRange=newValue;
		kDACTicksPerStep=kDACRange/(kNumberOfPoints-1.0);

		printf("\nDAC %i\n",kDACnumber);
		printf("%i minutes %i seconds\n",
			   (kNumberOfPoints*kSecondsPerPoint)/60,(kNumberOfPoints*kSecondsPerPoint)%60);
		printf("%i points of %i seconds each.\n",kNumberOfPoints,kSecondsPerPoint);
		printf("%i DAC ticks per step\n\n",kDACTicksPerStep);
		printf("Continue with scan (Y or 1), abort (A or 0), all else redo parameters: ");
		cin >> menuChoice;
      
		switch(menuChoice)
		{
		case '1':
			doscan=1;
			break;
		case '0':
			return -1;
			break;
		default:
			system("clear");
			break;
		}//ends switch
    }//ends while

	cout << "Starting scan control.\n";
	GreenMonster  fScanCtrl;
	//  Let's clear the values for the scan data & set "not clean".
	fScanCtrl.Open();
	fScanCtrl.SCNSetStatus(SCN_INT_NOT);
	fScanCtrl.SCNSetValue(1,0);
	fScanCtrl.SCNSetValue(2,0);
	fScanCtrl.CheckScan();
	fScanCtrl.PrintScanInfo();

	double IACInit  = fEPICSCtrl.Get_IADAC(kDACnumber);
	printf("Initial DAC %.0f\n",IACInit);
	double IACMin  = IACInit - kDACRange/2.;
	printf("First Point %.2f Volts\n",IACMin);
//   int IAMinVolt = -kDACRange/2;

	printf("Scan will take points at: \n");
	for (int count=0; count<kNumberOfPoints; count++) {
		printf("%2i   %f\n", count,
			   IACMin + (count*kDACTicksPerStep));
	}
	printf("\n");

	for (int count=0; count<kNumberOfPoints; count++) {
		// Not clean while setting the voltages
		fScanCtrl.SCNSetStatus(SCN_INT_NOT);
		usleep(100);
		double IACSet  = IACMin  + (count*kDACTicksPerStep);
		printf("\n");
		fEPICSCtrl.Set_IADAC(IACSet,kDACnumber);

		usleep(200);
		// Now lets check that the voltages took
		double IACGet  = fEPICSCtrl.Get_IADAC(kDACnumber);
		while (IACGet!=IACSet) {
			//printf("%.3f != %.3f or %.3f != %.3f\n",IACGet,IACSet);
			usleep(200);
			IACGet  = fEPICSCtrl.Get_IADAC(kDACnumber);
		}
		printf("Waiting 3 seconds for voltage to stabilize...\n");
		usleep(3000000);
		// Set the scan data points
		fScanCtrl.SCNSetValue(1, IACMin + (count*kDACTicksPerStep));
		fScanCtrl.SCNSetValue(2, count+1);
		fScanCtrl.SCNSetStatus(SCN_INT_CLN);
		fScanCtrl.CheckScan();
		fScanCtrl.PrintScanInfo();
		// Allow ctrl-C escape while sleeping
		for (Int_t i=0; i<=kSecondsPerPoint; i++) {
			sleep(1);
			signal(SIGINT, sigint_handler);
			if (globalEXIT) {
				globalEXIT=0;
				return 0;
			}
		}
	}

	// Return voltages to initial numbers and clearing the scandata.
	printf("\nScan complete\n");
	fEPICSCtrl.Set_IADAC(IACInit,kDACnumber);
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




/* emacs
 * Local Variables:
 * mode:C++
 * mode:font-lock
 * c-file-style: "stroustrup"
 * tab-width: 4
 * End:
 */
