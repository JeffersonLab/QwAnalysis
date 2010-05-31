#ifndef __STRINGMANIP__
#define __STRINGMANIP__


#include <vector>
#include <string>
#include <iostream>
#include "Rtypes.h"



void get_numbers(const std::string& stmp, std::vector<UInt_t>& v);

void get_number(const std::string& stmp, UInt_t& n);

std::string get_firstwd(const std::string& tmpline);

std::string get_line_seg(const std::string& stmp, const std::string& delim1, const std::string& delim2);

std::string get_line_seg(const std::string& stmp, const std::string& delim);

std::string get_next_seg(const std::string& inputstring, unsigned int& pos);

void split_string(const std::string& inputstring, const std::string& delim, std::vector<std::string>& varlist);

Bool_t IsNumber(const std::string& word);

std::string get_lastwd(const std::string& tmpline, const Char_t* delim = 0);

#endif  
