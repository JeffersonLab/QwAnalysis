/* Combine APV +- stat +- syst for set0,set1,set2....
   with proper accounting of any CORRELATIONS betn set0,set1,set2,...
  
  Based on Jens Erler's paper: http://arxiv.org/abs/1507.08210 

  Fully functional -- 3 Mar, 2015 -- rupesh
*/

#include "ErlerErrProp.h"

// APVAll contains sets of asym as asym_0,stat_0,syst_0, asym_1,stat_1,syst_1,...
ErlerErrProp::ErlerErrProp(std::vector<Double_t> APVAll){

  dataSetN = 0;
  totErr.clear();

  asymData.clear();
  for(std::vector<Double_t>::iterator it = APVAll.begin(); it != APVAll.end(); ++it){
    asymData.push_back(*it);
  }

  finalAsym = 0;
  cumErr = 0;
  cumStatErr = 0;
  cumSystErr = 0;
  
  Asym.clear(); // this is the final results
}


std::vector<Double_t> ErlerErrProp::EvalCumulativeAsym(){

  dataSetN = asymData.size()/3;

  //  printf("dataSetN:: %d\n", dataSetN);
  
  // AsymMatD is a Nx3 matrix
  TMatrixD AsymMatD = TMatrixD(dataSetN,3,&asymData[0]); //(Nrows,3cols,data)
  printf("The asymmetries (APV +- stat +- syst):");
  AsymMatD.Print();
    
  // Evaluate total error -- 1D List
  for (Int_t i=0;i<dataSetN;i++){
    totErr.push_back(TMath::Sqrt(TMath::Power(AsymMatD[i][1],2) + TMath::Power(AsymMatD[i][2],2)));
    //    printf("%d\t%.2f\n",i,totErr[i]);
  }

  // Pearson's correlation coefficient, rho
  TMatrixD RhoMat = TMatrixD(dataSetN,dataSetN); // all 0's
  // Now update individual elements of matrix
  for(Int_t i=0;i<dataSetN;i++){
    RhoMat[i][i]=1;
    for(Int_t j=0;j<i;j++){
      RhoMat[i][j]=0.01;
      RhoMat[j][i]=-0.01;
    }
  }
  // Manually fill in the Pearson's correlation coefficient
  // RhoMat[1][2] = 0.02;   RhoMat[2][1] = RhoMat[1][2];
  printf("Pearson's correlation coefficient betn data sets:");
  RhoMat.Print();

  // Error Matrix Without Correlation
  TMatrixD ErrMatNoCorr = TMatrixD(dataSetN,dataSetN); // all 0's
  for(Int_t i=0;i<dataSetN;i++){
    for(Int_t j=0;j<dataSetN;j++){
      ErrMatNoCorr[i][j]=totErr[i]*totErr[j];
    }
  }
  printf("Covariance Matrix WITHOUT correlations:");
  ErrMatNoCorr.Print();

  // Error Matrix WITH Correlation -- equiv to Erler Eq.52
  TMatrixD ErrMatWithCorr = TMatrixD(dataSetN,dataSetN); // all 0's
  for(Int_t i=0;i<dataSetN;i++){
    for(Int_t j=0;j<dataSetN;j++){
      ErrMatWithCorr[i][j]=ErrMatNoCorr[i][j]*RhoMat[i][j];
    }
  }
  printf("Covariance Matrix WITH correlations:");
  ErrMatWithCorr.Print();

  /// Now evaluate wts
  const Int_t nCols=ErrMatWithCorr.GetNcols();
  const Int_t nRows=ErrMatWithCorr.GetNrows();
  Double_t vecWt[nCols];

  cout << "Weights of each data set:" << endl;
  for(Int_t i=0;i<nCols;i++){
    vecWt[i] = EvalWts(ErrMatWithCorr,i);
    cout << vecWt[i] << endl;
  }
  cout << endl;

  //// *************
  // Evaluate final asym - Eq.45
  Double_t asymSum = 0, totWt = 0;
  for(Int_t i=0;i<nCols;i++){
    asymSum += vecWt[i] * asymData[3*i];
    totWt += vecWt[i];
  }
  finalAsym = asymSum/totWt;

  //// *************
  // Evaluate total error given as sqrt(det(cov mat)/sum(wts)) - Eq.49 
  cumErr = TMath::Sqrt(ErrMatWithCorr.Determinant()/totWt);
  // cout << "Tot Err:: " << cumErr << endl;

  //// *************
  // Evaluate stats error given as sqrt(sum(wt_i^2*stat_i^2)/sum(wts)) - Eq. 50
  Double_t statSum=0;
  for(Int_t i=0;i<nCols;i++){
    statSum += TMath::Power(vecWt[i] * asymData[3*i+1],2);
    //    cout << asymData[3*i+1] << endl;
  }
  cumStatErr = TMath::Sqrt(statSum)/totWt;
  //  cout << "Stat Err:: " << cumStatErr << endl;

  //// *************
  // Evaluate syst error given as
  // sqrt( (sum(w_i^2*syst_i^2) +sum(i!+j, w_i*w_j*ErrMat_ij))/sum(wts) ) - Eq. 50
  Double_t systSum1=0, systSum2=0;
  for(Int_t i=0;i<nCols;i++){
    systSum1 += TMath::Power(vecWt[i] * asymData[3*i+2],2);
  }
  for(Int_t i=0;i<nCols;i++){
    for(Int_t j=0;j<nRows;j++){
      if(i!=j)
	systSum2 += vecWt[i]*vecWt[j]*ErrMatWithCorr[i][j];
    }
  }  
  cumSystErr = TMath::Sqrt(systSum1+systSum2)/totWt;
  //cout << "Syst Err:: " << cumSystErr << endl;

  //// *************
  Asym.push_back(finalAsym);
  Asym.push_back(cumStatErr);
  Asym.push_back(cumSystErr);
  Asym.push_back(cumErr);
  //  printf("Asym Final: %.4f +- %.4f +- %.4f \n",finalAsym,cumStatErr,cumSystErr);

  return Asym;
}


// Evaluate the wts
/* w_i: determinant of matrix constructed by subtracting ith column from each of
   the other columns in ErrMatWithCorr, and removing ith column and ith row
*/
Double_t ErlerErrProp::EvalWts(TMatrixD &Mat, Int_t idx){

  Int_t nCols = Mat.GetNcols();
  Int_t nRows = Mat.GetNrows();
  TMatrixD WtMat_idx = TMatrixD(nRows,nCols); // all 0's
  
  // first create a matrix with the column/row to be subtracted as the only non-zero
  //  element
  for(Int_t i=0;i<nCols;i++){
    for(Int_t j=0;j<nRows;j++){
      if(j==idx || i==idx)
	WtMat_idx[i][j] = Mat[i][j];
    }
  }

  // subtract the idx_th column/row in the ErrMatWithCorr
  TMatrixD WtMat = TMatrixD(nRows,nCols); // all 0's
  WtMat = Mat - WtMat_idx;
  //  WtMat.Print();

  DropColumn(WtMat,idx);
  WtMat.Transpose(WtMat);
  DropColumn(WtMat,idx);
  WtMat.Transpose(WtMat);
  //  WtMat.Print();

  return WtMat.Determinant();
}
  
void ErlerErrProp::DropColumn(TMatrixD &Mat,Int_t colNum){
  
  TMatrixD Mat2 = Mat;
  Int_t nCols = Mat.GetNcols();
  Int_t nRows = Mat.GetNrows();
  for(Int_t i=0;i<nCols-1;i++){
    if(i>=colNum)
      TMatrixDColumn(Mat2,i) = TMatrixDColumn(Mat,i+1);
  }
  Mat2.GetSub(0,nRows-1,0,nCols-2,Mat,"S");
}

