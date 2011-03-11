//**********************************************************************************************//
// Author : B. Waidyawansa
// Date   : May 3rd, 2010
//**********************************************************************************************//
// This macro contains functions to perform linear least squares fit on an array of data

#include<math.h>

void  DoLinearLeastSquareFit( Int_t points, double X[], double Y[], Double_t W[],Double_t fitresults[] )
{
  // Y = aX + b
  Double_t ldebug = kFALSE;
  double A = 0.0, B = 0.0, C = 0.0, D = 0.0, E = 0.0, F = 0.0;
  for(Int_t k=0;k<4;k++)
    fitresults[k]=0.0;

  for(Int_t i = 0; i< points; i++)
    {
      A += X[i]*W[i];
      B += W[i];
      C += Y[i]*W[i];
      D += X[i]*X[i]*W[i];
      E += Y[i]*X[i]*W[i];
      F += Y[i]*Y[i]*W[i];
    } 
  if(ldebug)
  std::cout<<" A = "<<A
	   <<" B = "<<B
	   <<" C = "<<C
	   <<" D = "<<D
	   <<" E = "<<E
	   <<" F = "<<F<<std::endl;

  fitresults[0] = (E*B - C*A)/(D*B - A*A); // a (slope)
  fitresults[1] = (D*C - E*A)/(D*B - A*A); // b (offset)
  fitresults[2] = B/(D*B - A*A);           // erra
  fitresults[3] = D/(D*B - A*A);           // errb
  
  if(ldebug)
    {
      std::cout<<"slope = "<<fitresults[0]<<std::endl;
      std::cout<<"offset = "<<fitresults[1]<<std::endl;
      std::cout<<"slope error = "<<fitresults[2]<<std::endl;
      std::cout<<"offset error = "<<fitresults[3]<<std::endl;
    } 
}

