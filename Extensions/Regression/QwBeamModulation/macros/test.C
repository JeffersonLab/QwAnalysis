#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "TGraph.h"
#include "TCanvas.h"

void test(int row = 4, int col = 9)
{
  TCanvas *c = new TCanvas("c","c", 0,0,800,800);
  TGraph *gr[4];
  c->Divide(2,2);
  vector< vector<Double_t> > myVector, x;
  myVector.resize(row);
  x.resize(row);
  for(int irow=0;irow<row;++irow)
    for(int icol=0;icol<col;++icol){
      x[irow].push_back(icol*1.0);
      myVector[irow].push_back(irow*10.0+icol);
    }
  for(int irow=0;irow<row;++irow){
    c->cd(irow+1);
    Double_t *myX = x[irow].data();
    Double_t *myArray = myVector[irow].data();
    gr[irow] = new TGraph(row,(double *)x.data(), (double *)myVector.data());
    gr[irow]->Draw("ap");
    gPad->Update();
  }
  return;
}
