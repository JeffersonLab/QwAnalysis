#ifndef __STRINGMANIP__
#define __STRINGMANIP__

using namespace std;

#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "Rtypes.h"

void get_numbers(const string&, vector<UInt_t>&);

void get_number(const string&, UInt_t&);

string get_firstwd(const string&);

string get_line_seg(const string& stmp, const string& delim1, const string& delim2);

string get_line_seg(const string& stmp, const string& delim);

string get_next_seg(const string& inputstring, unsigned int& pos);

void split_string(const string& inputstring, const string& delim, vector<string>& varlist);

Bool_t IsNumber(const string& word);

string get_lastwd(const string& tmpline, const Char_t* delim = 0);

#endif  
