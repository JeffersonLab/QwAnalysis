/*
 * QwTreeLib.h
 * Author: Wade S Duvall <wsduvall@jlab.org>
 *
 */

#ifndef _QWTREELIB_H_
#define _QWTREELIB_H_

/* Structure used to read data out of the rootfile. Should never be used
 * outside of this library */
struct temp_value {
    double value;
    double error;
    double rms;
    int n;
};

/* Class used to store the branch once it has been removed from the tree. Each
 * leaf gets stored into a vector. */
class tree_value {
    public:
        tree_value(void);                       // constructor
        void fill(temp_value);                  // fills the leafs from temp_value struct
        void fill(int);                         // fills the runlet
        void debug(void);                       // debug class that prints out value
        vector<double> return_value(void);      // getter for value
        vector<double> return_error(void);      // getter for error
        vector<double> return_rms(void);        // getter for rms
        vector<int> return_n(void);             // getter for n
        void del_index(int);                    // setter used for deleting elements of vector
        int size(void);                         // getter for the number of elements in vector
    private:
        vector<double> value;                   // vector for the value
        vector<double> error;                   // vector for the error
        vector<double> rms;                     // vector for the rms
        vector<int> n;                          // vector for the n
        vector<int> runlet;                     // vector for the runlet
        int size_of;                            // value that stores the size
};

void get_data_from_tree(TTree*, TString, tree_value*);

void get_single_value_from_tree(TTree*, TString, std::vector<double>*);

void get_single_value_from_tree(TTree*, TString, std::vector<float>*);

void get_single_value_from_tree(TTree*, TString, std::vector<int>*);

void get_single_value_from_tree(TTree*, TString, std::vector<TString>*);

void plot_histo_branch_value(TTree*, TString); 

void plot_histo_single_value(TTree*, TString);

TGraph* plot_vectors(std::vector<double>, std::vector<double>);

TGraph* plot_vectors(std::vector<int>, std::vector<double>, TString, TString);

TGraph* plot_vectors(std::vector<double>, std::vector<double>, TString, TString);

TGraph* plot_vectors(std::vector<int>, std::vector<float>, TString, TString);

#endif
