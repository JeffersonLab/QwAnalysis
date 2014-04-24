/*
 * QwTreeBranch.h
 * Author: Wade S Duvall <wsduvall@jlab.org>
 *
 */

#ifndef _QWTREEBRANCH_H_
#define _QWTREEBRANCH_H_

#include <TROOT.h>     
#include <Riostream.h>
#include <TTree.h>

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
class QwTreeBranch {
    public:
        QwTreeBranch(void);                       // constructor

        /* Setters and getters. */
        void fill(temp_value);                    // fills the leafs from temp_value struct
        void fill_runlet(int);                    // fills the runlet
        void fill_weight(double);                 // fills the weight
        void fill_sign(int);                      // fills the runlet
        vector<double> get_value(void);           // getter for value
        vector<double> get_error(void);           // getter for error
        vector<double> get_rms(void);             // getter for rms
        vector<int> get_n(void);                  // getter for n
        vector<int> get_runlet(void);             // getter for runlet
        vector<double> get_weight(void);          // getter for weight
        vector<int> get_sign(void);               // getter for sign
        void del_index(int);                      // setter used for deleting
                                                  // elements of vector
        int size(void);                           // getter for the number of
                                                  // elements in vector

        /* Debug methods. */
        void debug(void);                         // debug class that prints out value

        /* IO from ROOT file/tree methods. */
        void get_data_from_tree(TTree*, TString); // method to pull data out of
                                                  // the tree into the object
        void get_data_from_tree(TTree*,           // same as previous, only pass wien
                                TString,          // number and grab only that wien
                                int);
        void get_data_from_tree(TTree*,           // same as above ony this one
                                TString,          // fills the weight as well
                                TString); 

        void get_data_from_tree(TTree*,           // same as previous, only pass wien
                                TString,          // number and grab only that wien
                                TString,
                                int);
        void set_weight_value(TTree*, TString);   // method to set the weight to value
        void set_weight_error(TTree*, TString);   // method to set the weight to error
        void set_weight_rms(TTree*, TString);     // method to set the weight to rms
        void set_weight_n(TTree*, TString);       // method to set the weight to n
        //void clean_tree(TTree*, int);             // clean zeros from tree

    private:
        vector<double> value;                     // vector for the value
        vector<double> weight;                    // vector for the weight
        vector<double> error;                     // vector for the error
        vector<double> rms;                       // vector for the rms
        vector<int> n;                            // vector for the n
        vector<int> sign;                         // vector for the sign
        vector<int> runlet;                       // vector for the runlet
        int size_of;                              // value that stores the size
};

TTree* clean_tree(TTree*, int);

#endif
