//////////////////////////////////////////////////////////
//   This file has been automatically generated 
//     (Thu Sep 19 15:06:54 2013 by ROOT version5.34/03)
//   from TTree slopes/slopes
//   found on file: /home/mjmchugh/scratch/rootfiles/slopesTree.root
//
//   To run: use root
//   root[0].x StabilityTest.C
//
//////////////////////////////////////////////////////////

{

gROOT->Reset();
#include <vector>

  // Flags that must be set for certain pieces to excecute.
  int calculateAverages = 1;
  int calculateSlugAverages = 0;
  int testStability = 1; 
  int makePlots = 0;
  int makeSpreadPlot = 1;

  //////////////////////////////////////////////////
  // NOTE: This must be changed for your machine. //
  //////////////////////////////////////////////////
  
  TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/home/mjmchugh/scratch/rootfiles/slopesTree.root");
  if (!f) {
    f = new TFile("/home/mjmchugh/scratch/rootfiles/slopesTree.root");
  }
  TTree* slopes = (TTree*) f->Get("slopes");
  
  int nEntries = slopes->GetEntries();  	// Total # of runs.
  const int nRuns = 32; 			// # of runs to average over. Currently ~2.
  const int nDetectors = 25;			// # of main and BG detectors used. Currently just MD bars
  const int nModTypes = 5;			// # of Modulation Types
  const int nCoils = 10;			// # of Coils (sine & cosine components)
  
  char* DetName[nDetectors];
  DetName[0] = "mdallbars";
  DetName[1] = "md1barsum";
  DetName[2] = "md2barsum";
  DetName[3] = "md3barsum";
  DetName[4] = "md4barsum";
  DetName[5] = "md5barsum";
  DetName[6] = "md6barsum";
  DetName[7] = "md7barsum";
  DetName[8] = "md8barsum";
  DetName[9] = "md9barsum";
  DetName[10] = "pmtltg";
  DetName[11] = "pmtonl";
  DetName[12] = "uslumi_sum";
  DetName[13] = "uslumi1_sum";
  DetName[14] = "uslumi3_sum";
  DetName[15] = "uslumi5_sum";
  DetName[16] = "uslumi7_sum";
  DetName[17] = "dslumi1";
  DetName[18] = "dslumi2";
  DetName[19] = "dslumi3";
  DetName[20] = "dslumi4";
  DetName[21] = "dslumi5";
  DetName[22] = "dslumi6";
  DetName[23] = "dslumi7";
  DetName[24] = "dslumi8";

  char* ModName[nModTypes];
  ModName[0] = "targetX";
  ModName[1] = "targetY";
  ModName[2] = "bpm3c12X";
  ModName[3] = "targetXSlope";
  ModName[4] = "targetYSlope";

  char* CoilName[nCoils];
  CoilName[0] = "Coil0";		// X sine
  CoilName[1] = "Coil1";		// Y sine
  CoilName[2] = "Coil2";		// E sine
  CoilName[3] = "Coil3";		// XP sine
  CoilName[4] = "Coil4";		// YP sine
  CoilName[5] = "Coil5";		// X cosine
  CoilName[6] = "Coil6";		// Y cosine
  CoilName[7] = "Coil7";		// E cosine
  CoilName[8] = "Coil8";		// XP cosine
  CoilName[9] = "Coil9";		// YP cosine

  // Vectors to hold slopes
  vector < vector < vector <Double_t> > > data;
  vector < vector < vector <Double_t> > > errors;
  vector < vector < vector <Double_t> > > coils;
  vector < vector < vector <Double_t> > > coil_errs;
  
  // Vectors to hold statistics
  vector < vector < vector <Double_t> > > rho;
  vector < vector < vector <Double_t> > > averages;
  vector < vector < vector <Double_t> > > avg_errs;
  
  // Make all of the vectors big enough
  // Idea is vector[Detector][modType/coil][run/averaging period]
  data.resize(nDetectors);
  errors.resize(nDetectors);
  rho.resize(nDetectors);
  averages.resize(nDetectors);
  avg_errs.resize(nDetectors);
  for(int i=0; i<nDetectors; i++) {
    data[i].resize(nModTypes);
    errors[i].resize(nModTypes);
    rho[i].resize(nModTypes);
    averages[i].resize(nModTypes);
    avg_errs[i].resize(nModTypes);
  }

  vector <Double_t> slugs;
  vector <Double_t> runs;
  
  //////////////////////////////////////
  // Fill all vectors from slopesTree //
  //////////////////////////////////////

  Double_t slug;
  Double_t run;
  Double_t value;
  Double_t error;
  for(int i=0; i<nDetectors; i++) {  
    for(int j=0; j<nModTypes; j++) { 
      slopes->SetBranchAddress("slug", &slug);
      slopes->SetBranchAddress("run", &run);      
      slopes->SetBranchAddress(Form("%s_%s",DetName[i],ModName[j]), &value);
      slopes->SetBranchAddress(Form("%s_%s_err",DetName[i],ModName[j]), &error);
                        
      for(int k=0; k<nEntries; k++) {
        slopes->GetEntry(k);
        
        // Insert desired cuts here (these are from elog 987)
        // Fill all data and coil vectors.
        // if(run<18000&&!(run>15240&&run<15690)&&!(run>14400&&run<14700)) {
          data[i][j].push_back(value);
          errors[i][j].push_back(error);
          
          // Make sure to record what slug and run each entry is after cuts
          if(i==0 && j==0) {
            slugs.push_back(slug);
            runs.push_back(run);            
          }
          
	//} // end if(cuts)

      } // end for(all entries)

      slopes->ResetBranchAddresses();

    } // end for(all mod types)
  } // end for(all detectors)

  // Determine the # of runs that passed the cuts
  const int nGood = data[0][0].size();
  
  // Determine the # of slugs and the starting entry of each.
  int nSlugs = 1;
  vector <int> nStart;
  nStart.push_back(0);
  int current_slug = slugs[0];
  int new_slug;

  for(int i=1; i<slugs.size(); i++) {
     new_slug = slugs[i];
     if(new_slug != current_slug) {
       nStart.push_back(i);
       nSlugs++;
     }
     current_slug = new_slug;
  }
    
  ////////////////////////
  // Calculate Averages //
  ////////////////////////
    
  // Now to actually calculate the averages
  // Calculates averages over nRuns
  if(calculateAverages) {
  
    for(int i=0; i<nDetectors; i++) {
      for(int j=0; j<nModTypes; j++) {
        for(int k=0; k<nGood-nRuns; k += nRuns) {

          Double_t mean = 0;
          Double_t error = 0;

          // Error Weighted Average
          for(int kk=k; kk<k+nRuns; kk++) {
            mean += data[i][j][kk]/(errors[i][j][kk]*errors[i][j][kk]);
            error += 1/(errors[i][j][kk]*errors[i][j][kk]);
          }
          mean /= error; 
          error = TMath::Sqrt(1/error);

          Double_t sum = 0;
          
          for(int kk=k; kk<k+nRuns; kk++) {
            Double_t val = data[i][j][kk] - mean;
            val = val*val;
            sum += val;
          }
          sum = TMath::Sqrt(sum);
          sum = sum/error;

          rho[i][j].push_back(sum);
          averages[i][j].push_back(mean);
          avg_errs[i][j].push_back(error);
        }
      }
    }
    
    cout << endl << "Averages Calculated" << endl << endl; 
  }
  
  if(calculateSlugAverages) {

    cout << endl << "Calculating Slug Averages" << endl << endl;
    for(int i=0; i<nDetectors; i++) {
      for(int j=0; j<nModTypes; j++) {
        for(int k=0; k<nSlugs-1; k++) {

          Double_t mean = 0;
          Double_t error = 0;
          Double_t sum = 0;
          
          // 1/sigma^2 = Sum_i[1/sigma_i^2]
          for(int kk=nStart[k]; kk<nStart[k+1]; kk++) {
            error += 1/(errors[i][j][kk]*errors[i][j][kk]);
            mean += data[i][j][kk]/(errors[i][j][kk]*errors[i][j][kk]);
          }
          mean /= error; 
          error = TMath::Sqrt(1/error);

         for(int kk=nStart[k]; kk<nStart[k+1]; kk++) {
            Double_t val = data[i][j][kk] - mean;
            val = val*val;
            sum += val;
          }
          sum = TMath::Sqrt(sum);
          sum = sum/error;
  
          rho[i][j].push_back(sum);
          averages[i][j].push_back(mean);
          avg_errs[i][j].push_back(error);
          
          //cout << i << "\t" << j << "\t" << k << endl;
        }
      }
    }
    cout << endl << "Slug Averages Calculated" << endl << endl;    
  }
  
  /////////////////////////////////////
  // Determine stability of the data //
  /////////////////////////////////////

  if(testStability) {

    vector <Double_t> spreads;
    int nGroups = averages[0][0].size();
    for(int k=0; k<nGroups; k++) {
      Double_t sum = 0;
      for(int i=0; i<nDetectors; i++) {
        for(int j=0; j<nModTypes; j++) {
          sum += rho[i][j][k]/(nDetectors*nModTypes);          
        }
      }
      spreads.push_back(sum);
    }

    // Now determine which group has the smallest RMS
    
    cout << "  Slug  | Spread" << endl;
    Double_t Lowest = spreads[0];
    int nLowest = 0;
    for(int i=1; i<nGroups; i++) {
      if(spreads[i] <= Lowest) {
        Lowest = spreads[i];
        nLowest = i;
      }
      cout << setw(8) << slugs[i*nRuns] << " | " << spreads[i]  << endl;
    }
    
    cout << endl << endl;
    
    cout << "  Detector   |   Spread" << endl;
    
    vector <Double_t> spreads1;
    for(int i=0; i<nDetectors; i++) {
      Double_t sum = 0;
      for(int j=0; j<nModTypes; j++) {
        for(int k=0; k<nGroups; k++) {
          sum += rho[i][j][k]/(nModTypes*nGroups);
        }
      }
      cout << setw(15) << DetName[i] << " | " << sum << endl;
      spreads1.push_back(sum);
    }

    cout << endl << endl;

    cout << " Modulation |   Spread" <<endl;
    vector <Double_t> spreads2;
    for(int j=0; j<nModTypes; j++) {
      Double_t sum = 0;
      for(int i=0; i<nDetectors; i++) {
        for(int k=0; k<nGroups; k++) {
          sum += rho[i][j][k]/(nDetectors*nGroups);
        }
      }
      cout << setw(12) << ModName[j] << " | " << sum << endl; 
      spreads2.push_back(sum);
    }

    cout << endl << "Stability Tested" << endl << endl;
  
  }  // end if(testStability)

  //////////////
  // PLOTTING //
  //////////////
  
  if(makeSpreadPlot) {
    
    Double_t x[nGroups];
    Double_t y[nGroups];
    for(int i=0; i<nGroups; i++) {
      if(calculateSlugAverages)
        x[i] = slugs[nStart[i]];
      else if(calculateAverages)
        x[i] = slugs[i*nRuns];
      y[i] = spreads[i];
      //cout << i << "\t" << slug[i*nRuns] << "\t" << spreads[i] << endl;
    }  
    
    TCanvas* c1 = new TCanvas("c1","canvas", 1500, 500);
    c1->Divide(3,1);
    
    c1->cd(1);
    TGraph* graph = new TGraph(nGroups, x, y);
    graph->SetTitle("Average Spread vs. Slugs");
    graph->GetXaxis()->SetTitle("Starting Slug");
    graph->GetYaxis()->SetTitleOffset(1.4);
    graph->GetYaxis()->SetTitle("Average spread");
    graph->GetHistogram()->SetMinimum(0.0);
    graph->GetHistogram()->SetMaximum(50.0);
    graph->Draw("AB");
    
    Double_t x1[nDetectors];
    Double_t y1[nDetectors];
    for(int i=0; i<nDetectors; i++) {
      x1[i] = i;
      y1[i] = spreads1[i];
    }

    c1->cd(2);
    TGraph* graph1 = new TGraph(nDetectors, x1, y1);
    graph1->SetTitle("Average Spread vs. Detector");
    graph1->GetXaxis()->SetTitle("Detector");
    graph1->GetYaxis()->SetTitleOffset(1.4);
    graph1->GetYaxis()->SetTitle("Average spread");
    graph1->Draw("AB");

    Double_t x2[nDetectors];
    Double_t y2[nDetectors];
    for(int i=0; i<nModTypes; i++) {
      x2[i] = i;
      y2[i] = spreads2[i];
    }

    c1->cd(3);
    TGraph* graph2 = new TGraph(nModTypes, x2, y2);
    graph2->SetTitle("Average Spread vs. ModType");
    graph2->GetXaxis()->SetTitle("ModType");
    graph2->GetYaxis()->SetTitleOffset(1.6);
    graph2->GetYaxis()->SetTitle("Average spread");
    graph2->Draw("AB");
  }
  
  if(makePlots) {
   
    Double_t x[nRuns];
    for(int i=nLowest*nRuns; i<nRuns*(nLowest+1); i++)
      x[i-nLowest*nRuns] = runs[i];
  
    TCanvas* c[nDetectors];    
    for(int i=0; i<nDetectors; i++) {
      c[i] = new TCanvas(Form("c%i", i), "canvas", 1500, 1000);
      c[i]->Divide(3,2);
      TGraphErrors* gr[nModTypes];

      for(int j=0; j<nModTypes; j++) {
        //Make array for data & errors
        Double_t graph_data[nRuns];
        Double_t graph_err[nRuns];
        Double_t coilX[nRuns];
        Double_t coilY[nRuns];

        for(int k=nLowest*nRuns; k<nRuns*(nLowest+1); k++) {
          graph_data[k-nLowest*nRuns] = data[i][j][k];
          graph_err[k-nLowest*nRuns] = errors[i][j][k];
        }

        //cout << i << "\t" << j << "\t" << data[i][j][0] << "\t" << graph_data[0] << endl;
        gr[j] = new TGraphErrors(nRuns, x, graph_data, 0, graph_err);
        c[i]->cd(j+1);
        gr[j]->SetTitle(Form("%s_%s",DetName[i],ModName[j]));
        gr[j]->GetXaxis()->SetTitle("Run");
        gr[j]->GetYaxis()->SetTitle("Slope");
        gr[j]->Draw("AP");
        c[i]->Update();

      }

      c[i]->Print(Form("%s_Slopes.png",DetName[i]));
    }

  }	// end if(makePlots)
  
}   
