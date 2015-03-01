/*
 * This shortp rogram will open a text file, and then sort the information
 * by "slugs" and "runs", and then find the average and reduced chi square
 * of the run distributions.
 * Definitions:
 *    runs: individual data point
 *    slug: combinations of data points with similar conditions
 * For instance, several measurements at one magnet setting. Each measurement
 * would be a "run", but the average of the measurements is a "slug".
 *
 * J. Magee
 * magee@jlab.org
 * July 8, 2014
 * calculate_average.cpp
 */

#include <iostream>
#include <iomanip>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "SlugCalc.h"
#include "SlugCalcVec.h"

using namespace std;

int main(void) {

  //create my vector of SlugCalc objects
  SlugCalcVec slug_vector;

  //define necessary variables
  int slugnumber;
  int runnumber;
  float value;
  float error;

  string filename = "foo";
  const char* filename_str  = filename.c_str();
  ifstream file;
  file.open(filename_str,ios::in);

  //check to see if the input file is actually open
  //exit if not
  if ( !file.is_open() ) {
    std::cout <<"Your file isn't open or doesn't exist.\n"
      <<"Exiting program." <<std::endl;
    exit(EXIT_FAILURE);
  }

  /* For now, I am assuming that the input file has the following format:
   *
   * SlugNumber RunNumber Value Error
   */

  while( file.good() ) {
    //read in inputs
    file >> slugnumber;
    file >> runnumber;
    file >> value;
    file >> error;
    if ( file.eof() ) break;

    slug_vector.push_back(slugnumber,runnumber,value,error);

  }

  //print output
  std::cout <<"Slug \tAvg \tErr \t\tChi^2/dof\n";
  for(int j=0; j<slug_vector.Size(); j++) {
    printf("%.0f  \t%.3f \t%.3f \t\t%.4f\n",
        slug_vector[j].GetSlug(),
        slug_vector[j].GetAvg(),
        slug_vector[j].GetError(),
        slug_vector[j].GetReducedChi()
    );
  }

  return 0;
} //end test function


