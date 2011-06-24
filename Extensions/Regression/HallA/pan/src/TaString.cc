//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       TaString.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaString.cc,v 1.9 2008/01/07 21:07:10 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
// Derives from STL string; provides additional methods.  No
// additional storage is defined, so strings and TaStrings can be
// converted back and forth as needed; e.g. to convert a string to
// lowercase you can do something like
//
//      string mixedstring ("MixedCaseString");
//      TaString temp = mixedstring;
//      string lowerstring = temp.ToLower();
//
////////////////////////////////////////////////////////////////////////

#include "TaString.hh"
#include <sstream>

#ifndef NODICT
ClassImp(TaString)
#endif

// Constructors/destructors/operators



// Major functions

int 
TaString::CmpNoCase (const string& s) const
{
  // Case insensitive compare.  Returns -1 if "less", 0 if equal, 1 if
  // "greater".

  string::const_iterator p = this->begin();
  string::const_iterator p2 = s.begin();

  while (p != this->end() && p2 != s.end())
    {
      if (toupper(*p) != toupper(*p2))
	return (toupper(*p) < toupper(*p2)) ? -1 : 1;
      ++p;
      ++p2;
    }

  return (s.size() == this->size()) ? 0 : (this->size() < s.size()) ? -1 : 1;
}

vector<string> 
TaString::Split (size_t n) const
{
  // Split on whitespace (to a maximum of n strings if n>0)
  vector<string> v;

  size_t i(0);
  size_t j(0);
  i = this->find_first_not_of (" \n\t\v\b\r\f", j);
  while ((n == 0 || v.size() < n-1) && i < this->size())
    {
      j = this->find_first_of (" \n\t\v\b\r\f", i+1);
      if (i < this->size())
	{
	  if (j < this->size())
	    {
	      v.push_back (this->substr (i, j-i));
	      i = this->find_first_not_of (" \n\t\v\b\r\f", j);
	    }
	  else
	    {
	      v.push_back (this->substr (i));
	      i = this->size();
	    }
	}
    }

  if (j < this->size())
    {
      i = this->find_first_not_of (" \n\t\v\b\r\f", j);
      if (i < this->size())
	v.push_back (this->substr (i));
    }

  return v;
}


UInt_t 
TaString::Hex() const
{
  // Conversion to to unsigned interpreting as hex.
  istringstream ist(this->c_str());
  UInt_t in;
  ist >> hex >> in;
  return in;
}

TaString 
TaString::ToLower () const
{
  // Conversion to lower case.
  TaString::const_iterator p = this->begin();
  TaString result("");
  while (p != this->end()) 
    {
      result += tolower(*p);
      ++p;
    }
  return result;
}

TaString 
TaString::ToUpper () const
{
 // Conversion to upper case.
  TaString::const_iterator p = this->begin();
  TaString result("");
  while (p != this->end()) 
    {
      result += toupper(*p);
      ++p;
    }
  return result;
}

TaString
TaString::RemoveChar(const char* toremove)
{
// Remove "toremove" 
  TaString::const_iterator p = this->begin();
  TaString result("");
  while (p != this->end()) 
    {
      if (*p != *toremove) result += *p;
      ++p;
    }
  return result;
}



// Private member functions



// Non member functions

