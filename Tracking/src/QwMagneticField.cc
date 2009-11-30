//
//  date: Sun Nov 29 22:28:23 CST 2009
//
//  jpan: QTOR field map reading class. 
//        Modified from the Qweak geant4 simulation code
//

#include "QwMagneticField.h"
#include "TMath.h"

QwMagneticField::QwMagneticField() 
  :invertX(false),invertY(false),invertZ(false)
{
   std::cout << std::endl << "###### Calling QwMagneticField::QwMagneticField " << std::endl << std::endl;

   // needed later for field rotation
   BField_ANSYS = new TVector3();

   // initialize field map parameters
   // here: from the QTOR field map file

   rMinFromMap =     2.0;
   rMaxFromMap =   300.0;

   // new field map boundaries
   phiMinFromMap =    0.0;
   phiMaxFromMap =  359.0;
   
   zMinFromMap = -250.0;
   zMaxFromMap =  250.0;
   
   gridstepsize_r   = 2.0;
   gridstepsize_phi = 1.0;
   gridstepsize_z   = 2.0;

   Unit_Bfield = 0.1;    //kilogauss;    // units of new field map ???? yes, tested it (1kG = 0.1T)
                                         // used field unit tesla withs results in a too strong field

   // initialize variables
   //Initialize();

  InitializeGrid();

  std::cout << std::endl << "###### Leaving QwMagneticField::QwMagneticField " << std::endl << std::endl;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void QwMagneticField::InitializeGrid()
{
  std::cout << std::endl << "###### Calling QwMagneticField::Initialize " << std::endl << std::endl;
  
  nGridPointsInR   =  int ( (rMaxFromMap   - rMinFromMap)   /gridstepsize_r   ) + 1;
  nGridPointsInZ   =  int ( (zMaxFromMap   - zMinFromMap)   /gridstepsize_z   ) + 1;
  nGridPointsInPhi =  int ( (phiMaxFromMap - phiMinFromMap) /gridstepsize_phi ) + 1;

  BFieldGridData_X.clear();
  BFieldGridData_Y.clear();
  BFieldGridData_Z.clear();
  
  fGridSize = (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi);

  BFieldGridData_X.resize(fGridSize);
  BFieldGridData_Y.resize(fGridSize);
  BFieldGridData_Z.resize(fGridSize);

  for (int i = 0; i < (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi); i++) {

    BFieldGridData_X[i] = 0.0;
    BFieldGridData_Y[i] = 0.0;
    BFieldGridData_Z[i] = 0.0;
  } 
  
    
  std::cout << std::endl << "###### Leaving QwMagneticField::Initialize " << std::endl << std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void QwMagneticField::ReadFieldMap(std::string filename)
{
  std::cout << std::endl << "###### Calling QwMagneticField::ReadFieldMap " << std::endl << std::endl;

  int r_index = 0;
  int phi_index = 0;
  int z_index = 0;
  int oct = 0;
  int ind = 0;

  int raw_R_cm = 0, raw_Z_cm = 0, raw_Phi_deg = 0;
  double val_R = 0, val_Z = 0, val_Phi = 0, bx = 0, by = 0, bz = 0;

  int nlines = 0;

  // open the field map file
  std::ifstream inputfile;
  inputfile.open(filename.c_str(), std::ios_base::in);

  // Check for success
  if (!inputfile.good())
  {
    std::cerr << "[QwMagneticField] Error: Could not open field map file!" << std::endl;
    std::cerr << "File name: " << filename << std::endl;
  }


  std::cout << "--------------------------------------------------------------------------------" << std::endl;
  std::cout << " Magnetic field interpolation: Reading the field grid from " <<std::endl<<filename<< std::endl; 
  std::cout << "--------------------------------------------------------------------------------" << std::endl;

  unsigned int entries = 0;
  while(inputfile.good()){

    // Progress bar
    if (entries % (fGridSize / 10) == 0)
      std::cout << 100 * entries / fGridSize << "%" << std::flush;
    if (entries % (fGridSize / 10) != 0 && entries % (fGridSize / 40) == 0)
      std::cout << "." << std::flush;

    inputfile >> raw_R_cm >> raw_Z_cm >> raw_Phi_deg >> bx >> by >> bz;

    ind = (int)((raw_Phi_deg - phiMinFromMap)/gridstepsize_phi 
		  + nGridPointsInPhi*(raw_R_cm - rMinFromMap)/gridstepsize_r
		  + nGridPointsInPhi*nGridPointsInR*(raw_Z_cm - zMinFromMap)/gridstepsize_z);

    if (ind >= fGridSize) {
      unsigned int oldsize = fGridSize;
      unsigned int newsize = ind + (int)(nGridPointsInR*nGridPointsInPhi) + 1;
      //std::cout << "[QwMagneticField] Warning: Need to resize the field map"
      //                      << " from " << oldsize
      //                      << " to "   << newsize
      //                      << " at index " << ind << std::endl;
      fGridSize = newsize;
      BFieldGridData_X.resize(newsize);
      BFieldGridData_Y.resize(newsize);
      BFieldGridData_Z.resize(newsize);
    }

    BFieldGridData_X[ind] = bx*Unit_Bfield;  // unit: [T]
    BFieldGridData_Y[ind] = by*Unit_Bfield;
    BFieldGridData_Z[ind] = bz*Unit_Bfield;

    //std::cout<<"bx by bz :"<<bx<<", "<<by<<", "<<bz<<std::endl;

    entries++;
  };

  // close file
  inputfile.close();

//   fclose(fp);

  std::cout << "... done reading" << nlines << "lines" << std::endl;
  
  
  std::cout << std::endl << "###### Leaving QwMagneticField::ReadFieldMap " << std::endl << std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
QwMagneticField::~QwMagneticField()
{
  delete BField_ANSYS ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void QwMagneticField::GetFieldValueFromGridCell( const int GridPoint_R,
							const int GridPoint_Phi, 
							const int GridPoint_Z, 
							double *BFieldGridValue ) const
{
  // get the value of the cell ix, iy,iz
//   BFieldGridValue[0] = BFieldGridData_X[GridPoint_R][GridPoint_Phi][GridPoint_Z]  ;
//   BFieldGridValue[1] = BFieldGridData_Y[GridPoint_R][GridPoint_Phi][GridPoint_Z]  ;
//   BFieldGridValue[2] = BFieldGridData_Z[GridPoint_R][GridPoint_Phi][GridPoint_Z]  ;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void QwMagneticField::GetFieldValue(const double Point[3], double *Bfield, double scalefactor ) const
{

  double lpoint[3] = {Point[0],Point[1],Point[2]};

  if(lpoint[2] < zMinFromMap || lpoint[2] > zMaxFromMap){
    Bfield[0] = 0.0;
    Bfield[1] = 0.0;
    Bfield[2] = 0.0;
    return;
  }

  double xyRadius;
  double phi;
  double z;

  xyRadius = sqrt(lpoint[0]*lpoint[0] + lpoint[1]*lpoint[1]);
  if(xyRadius < rMinFromMap || xyRadius > rMaxFromMap){
    Bfield[0] = 0.0;
    Bfield[1] = 0.0;
    Bfield[2] = 0.0;
    return;
  }

//   printf("zMinFromMap = %f\n",zMinFromMap);
//   printf("zMinFromMap = %f\n",zMaxFromMap);
//   printf("xs = %f, ys = %f, zs = %f\n",lpoint[0],lpoint[1],lpoint[2]);

  z = lpoint[2];
  double trPhy = TMath::Pi()/8.0;

  double fpoint[3]={lpoint[0]*TMath::Sin(trPhy)+lpoint[1]*TMath::Cos(trPhy),
                     -lpoint[0]*TMath::Cos(trPhy)+lpoint[1]*TMath::Sin(trPhy),
                      lpoint[2]};

//   printf("xf = %f, yf = %f, zf = %f\n",fpoint[0],fpoint[1],fpoint[2]);
  
  if(fpoint[0] == 0 && fpoint[1] > 0) phi =  90.0;//*degree;
  if(fpoint[0] < 0 && fpoint[1] == 0) phi = 180.0;//*degree;
  if(fpoint[0] == 0 && fpoint[1] < 0) phi = 270.0;//*degree;
  if(fpoint[0] > 0 && fpoint[1] == 0) phi = 360.0;//*degree;

  if(fpoint[0] > 0 && fpoint[1] > 0) phi = TMath::ATan(fpoint[1]/fpoint[0])*TMath::RadToDeg();//*degree;
  if(fpoint[0] < 0 && fpoint[1] > 0) phi = (TMath::Pi()+TMath::ATan(fpoint[1]/fpoint[0]))*TMath::RadToDeg();//*degree;
  if(fpoint[0] < 0 && fpoint[1] < 0) phi = (TMath::Pi()+TMath::ATan(fpoint[1]/fpoint[0]))*TMath::RadToDeg();//*degree;
  if(fpoint[0] > 0 && fpoint[1] < 0) phi = (2.0*TMath::Pi()+TMath::ATan(fpoint[1]/fpoint[0]))*TMath::RadToDeg();//*degree;
 
//   printf("phi = %f\n",phi);

  double r_int, phi_int, z_int;
  double r_frac,phi_frac,z_frac;

  r_frac = modf(xyRadius,&r_int);
  phi_frac = modf(phi,&phi_int);
  z_frac = modf(z,&z_int);

//   printf("r_frac %f, phi_frac %f, z_frac %f\n",r_frac,phi_frac,z_frac);
//   printf("r_int %f, phi_int %f, z_int %f\n",r_int,phi_int,z_int);
 
  double r_rem   = static_cast<int>(r_int)%static_cast<int>(gridstepsize_r)+r_frac;
  double phi_rem = static_cast<int>(phi_int)%static_cast<int>(gridstepsize_phi)+phi_frac;
  double z_rem   = static_cast<int>(z)%static_cast<int>(gridstepsize_z)+z_frac;

//   printf("r_rem %f, phi_rem %f, z_rem %f\n",r_rem,phi_rem,z_rem);

  double r_local   = (r_rem)/gridstepsize_r;
  double phi_local = (phi_rem)/gridstepsize_phi;
  double z_local   = (z_rem)/gridstepsize_z;

//   printf("r_rem %f, phi_rem %f, z_rem %f\n",r_rem,phi_rem,z_rem);

  int ind1 = (int)((phi - phi_rem - phiMinFromMap)/gridstepsize_phi 
		       + nGridPointsInPhi*(xyRadius - r_rem - rMinFromMap)/gridstepsize_r
		       + nGridPointsInPhi*nGridPointsInR*(z - z_rem - zMinFromMap)/gridstepsize_z);

//   printf("ind 1 = %d, r = %f, z = %f, phi = %f\n",ind1, xyRadius - r_rem, z - z_rem, phi - phi_rem);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind1],BFieldGridData_Y[ind1],BFieldGridData_Z[ind1]);

  int ind2 = (int)((phi - phi_rem - phiMinFromMap)/gridstepsize_phi 
		       + nGridPointsInPhi*(xyRadius - r_rem - rMinFromMap)/gridstepsize_r
		       + nGridPointsInPhi*nGridPointsInR*((z - z_rem - zMinFromMap)/gridstepsize_z+1));
  
//   printf("ind 2 = %d, r = %f, z = %f, phi = %f\n",ind2, xyRadius - r_rem, z - z_rem + gridstepsize_z, phi - phi_rem);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind2],BFieldGridData_Y[ind2],BFieldGridData_Z[ind2]);

  int ind3 = (int)((phi - phi_rem - phiMinFromMap)/gridstepsize_phi + 1 
		       + nGridPointsInPhi*(xyRadius - r_rem - rMinFromMap)/gridstepsize_r
		       + nGridPointsInPhi*nGridPointsInR*((z - z_rem - zMinFromMap)/gridstepsize_z));

//   printf("ind 3 = %d, r = %f, z = %f, phi = %f\n",ind3, xyRadius - r_rem, z - z_rem, phi - phi_rem + gridstepsize_phi);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind3],BFieldGridData_Y[ind3],BFieldGridData_Z[ind3]);

  int ind4 = (int)((phi - phi_rem - phiMinFromMap)/gridstepsize_phi + 1
		       + nGridPointsInPhi*(xyRadius - r_rem - rMinFromMap)/gridstepsize_r
		       + nGridPointsInPhi*nGridPointsInR*((z - z_rem - zMinFromMap)/gridstepsize_z + 1));

//   printf("ind 4 = %d, r = %f, z = %f, phi = %f\n",ind4, xyRadius - r_rem, z - z_rem + gridstepsize_z, phi - phi_rem + gridstepsize_phi);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind4],BFieldGridData_Y[ind4],BFieldGridData_Z[ind4]);

  int ind5 = (int)((phi - phi_rem - phiMinFromMap)/gridstepsize_phi 
		       + nGridPointsInPhi*((xyRadius - r_rem - rMinFromMap)/gridstepsize_r + 1)
		       + nGridPointsInPhi*nGridPointsInR*((z - z_rem - zMinFromMap)/gridstepsize_z));

//   printf("ind 5 = %d, r = %f, z = %f, phi = %f\n",ind5, xyRadius - r_rem + gridstepsize_r, z - z_rem, phi - phi_rem);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind5],BFieldGridData_Y[ind5],BFieldGridData_Z[ind5]);

  int ind6 = (int)((phi - phi_rem - phiMinFromMap)/gridstepsize_phi 
		       + nGridPointsInPhi*((xyRadius - r_rem - rMinFromMap)/gridstepsize_r + 1)
		       + nGridPointsInPhi*nGridPointsInR*((z - z_rem - zMinFromMap)/gridstepsize_z + 1));

//   printf("ind 6 = %d, r = %f, z = %f, phi = %f\n",ind6, xyRadius - r_rem + gridstepsize_r, z - z_rem + gridstepsize_z, phi - phi_rem);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind6],BFieldGridData_Y[ind6],BFieldGridData_Z[ind6]);

  int ind7 = (int)((phi - phi_rem - phiMinFromMap)/gridstepsize_phi + 1 
		       + nGridPointsInPhi*((xyRadius - r_rem - rMinFromMap)/gridstepsize_r + 1)
		       + nGridPointsInPhi*nGridPointsInR*((z - z_rem - zMinFromMap)/gridstepsize_z));

//   printf("ind 7 = %d, r = %f, z = %f, phi = %f\n",ind7, xyRadius - r_rem + gridstepsize_r, z - z_rem, phi - phi_rem + gridstepsize_phi);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind7],BFieldGridData_Y[ind7],BFieldGridData_Z[ind7]);

  int ind8 = (int)((phi - phi_rem - phiMinFromMap)/gridstepsize_phi + 1 
		       + nGridPointsInPhi*((xyRadius - r_rem - rMinFromMap)/gridstepsize_r + 1)
		       + nGridPointsInPhi*nGridPointsInR*((z - z_rem - zMinFromMap)/gridstepsize_z + 1));

  if(ind1 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind2 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind3 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind4 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind5 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind6 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind7 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind8 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi)){

    Bfield[0] = 0.0;
    Bfield[1] = 0.0;
    Bfield[2] = 0.0;
    return;
  }

  double Bx_ANSYS, By_ANSYS, Bz_ANSYS;

//   Full 3-dimensional version: trilinear interpolation
  Bx_ANSYS =
    BFieldGridData_X[ind1] * (1-r_local) * (1-phi_local) * (1-z_local) +
    BFieldGridData_X[ind2] * (1-r_local) * (1-phi_local) *    z_local  +
    BFieldGridData_X[ind3] * (1-r_local) *    phi_local  * (1-z_local) +
    BFieldGridData_X[ind4] * (1-r_local) *    phi_local  *    z_local  +
    BFieldGridData_X[ind5] *    r_local  * (1-phi_local) * (1-z_local) +
    BFieldGridData_X[ind6] *    r_local  * (1-phi_local) *    z_local  +
    BFieldGridData_X[ind7] *    r_local  *    phi_local  * (1-z_local) +
    BFieldGridData_X[ind8] *    r_local  *    phi_local  *    z_local ;
	
	
  By_ANSYS =
    BFieldGridData_Y[ind1] * (1-r_local) * (1-phi_local) * (1-z_local) +
    BFieldGridData_Y[ind2] * (1-r_local) * (1-phi_local) *    z_local  +
    BFieldGridData_Y[ind3] * (1-r_local) *    phi_local  * (1-z_local) +
    BFieldGridData_Y[ind4] * (1-r_local) *    phi_local  *    z_local  +
    BFieldGridData_Y[ind5] *    r_local  * (1-phi_local) * (1-z_local) +
    BFieldGridData_Y[ind6] *    r_local  * (1-phi_local) *    z_local  +
    BFieldGridData_Y[ind7] *    r_local  *    phi_local  * (1-z_local) +
    BFieldGridData_Y[ind8] *    r_local  *    phi_local  *    z_local ;
  

  Bz_ANSYS =
    BFieldGridData_Z[ind1] * (1-r_local) * (1-phi_local) * (1-z_local) +
    BFieldGridData_Z[ind2] * (1-r_local) * (1-phi_local) *    z_local  +
    BFieldGridData_Z[ind3] * (1-r_local) *    phi_local  * (1-z_local) +
    BFieldGridData_Z[ind4] * (1-r_local) *    phi_local  *    z_local  +
    BFieldGridData_Z[ind5] *    r_local  * (1-phi_local) * (1-z_local) +
    BFieldGridData_Z[ind6] *    r_local  * (1-phi_local) *    z_local  +
    BFieldGridData_Z[ind7] *    r_local  *    phi_local  * (1-z_local) +
    BFieldGridData_Z[ind8] *    r_local  *    phi_local  *    z_local ;
  
  Bfield[0] = Bx_ANSYS*TMath::Sin(trPhy)-By_ANSYS*TMath::Cos(trPhy)*scalefactor;
  Bfield[1] = Bx_ANSYS*TMath::Cos(trPhy)+By_ANSYS*TMath::Sin(trPhy)*scalefactor;
  Bfield[2] = Bz_ANSYS*scalefactor;

//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind1],BFieldGridData_Y[ind1],BFieldGridData_Z[ind1]);
//   printf("Bix %f, Biy %f, Biz %f\n",Bx_ANSYS,By_ANSYS,Bz_ANSYS);
//   printf("Btx %f, Bty %f, Btz %f\n",Bfield[0],Bfield[1],Bfield[2]);
  
} //end of QwMagneticField::GetFieldValue()

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
