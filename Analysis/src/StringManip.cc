// just a bunch of string manipulation functions that needed to decode 
// the input ascii files. jianglai 6/21/2002
#include "StringManip.h"

#include <cstdlib>
using std::string;

string get_line_seg(const string& stmp, const string& delim1, const string& delim2){
  unsigned int pos_begin_seg=0;
  unsigned int pos_end_seg=0;
  string stmp_seg;
  unsigned int total_len = stmp.size();
  pos_begin_seg = stmp.find(delim1);
  //std::cerr<<pos_begin_seg<<std::endl;
  if(pos_begin_seg!=(string::npos)){//found the 1st delimiter  
    if(delim2!="\n") pos_end_seg = stmp.find(delim2,pos_begin_seg+delim1.size());
    else pos_end_seg = total_len;
    //std::cerr<<pos_end_seg<<std::endl;
    if(pos_end_seg==(string::npos)){//2nd delimiter not found
      std::cerr<<"Could not find the 2nd delimiter "<<delim2<<std::endl;
      std::cerr<<"Take the end of the string as the 2nd delimiter"<<std::endl;
      pos_end_seg = total_len;//the default the total length
    }
    stmp_seg = stmp.substr(pos_begin_seg+delim1.size(),
			   pos_end_seg-(pos_begin_seg+delim1.size()));  
    return stmp_seg;
  }
  else{//1st delimiter was not found. If this is the case, exit.
    std::cerr<<"Could not find the 1st delimiter "<<delim1<<std::endl;
    exit(1);
  }
};

//an overloaded function with only one delimiter
//this is to separate the 1st segment of the string
string get_line_seg(const string& stmp, const string& delim)
{
  unsigned int pos_begin_seg=0;
  string stmp_seg;
  pos_begin_seg = stmp.find(delim);
  if(pos_begin_seg!=(string::npos)){//found the delimiter  
    stmp_seg = stmp.substr(0,pos_begin_seg);
    return stmp_seg;
  }
  else{
    std::cerr<<"Could not find the delimiter "<<delim<<std::endl;
    //return the whole string
    return stmp;
  }
};

//take the next segment of a line separated by a spaces, tabs or \n from 
//position pos. NOTE, pos is passed by reference and it will change!
string get_next_seg(const string& inputstring, unsigned int& pos)
{ 
  string seg;
  unsigned int first_pos = 0;
  unsigned int next_pos = 0;
  first_pos = inputstring.find_first_not_of(" \t\n",pos);
  if(first_pos==string::npos){//not found a real character, end of line already
    std::cerr<<"end of line has reached!"<<std::endl;
    exit(1);
  }
  else{//found the beginning of a valid string, try to find the end of it
    next_pos = inputstring.find_first_of(" \t\n",first_pos);
    if(next_pos==string::npos){//not found the next separator, just take the 
      //string starting from first_pos
      seg = inputstring.substr(first_pos, inputstring.size());
      pos = inputstring.size();
    }
    else{//found the next separator, take the segment, move pos to the position
      //of the next separator
      seg = inputstring.substr(first_pos, next_pos-first_pos);
      pos = next_pos;
    }
    //    std::cerr << seg << std::endl;
    return seg;
  }
};

//function to split the string by the delimiter and put the segment into 
//a vector. Before passing the vector into this function, do clear first!
void split_string(const string& inputstring, const string& delim, std::vector<string>& varlist)
{
  string seg;
  unsigned int first_pos = 0;
  unsigned int next_pos = 0;
  while(first_pos<inputstring.size()){//keep searching until the end of the string
    next_pos = inputstring.find(delim,first_pos);
    if(next_pos==string::npos){//no found the next delimiter
      //take the next_pos as the end of the string
      next_pos = inputstring.size();
      seg = inputstring.substr(first_pos,next_pos-first_pos);
      varlist.push_back(seg);
      //re-assign the location of the first_pos
      first_pos=next_pos;
    }
    else{//found the next delimiter
      seg = inputstring.substr(first_pos,next_pos-first_pos);
      varlist.push_back(seg);
      //re-assign the location of the first_pos
      first_pos=next_pos+delim.size();//skip the delimiter
    }
  }
};


//get the numbers (separated by a space) in a string and put them into a vector
void get_numbers(const string& stmp, std::vector<UInt_t>& v){
  UInt_t pos_begin_num = 0;
  UInt_t pos_end_num = 0;
  string stmp_num;
  UInt_t tmp_num;
  
  while(pos_end_num<stmp.size()){
    pos_begin_num=stmp.find_first_of("1234567890.",pos_end_num);
    pos_end_num=stmp.find_first_not_of("1234567890.",pos_begin_num);
    if(pos_begin_num!=(string::npos)){
      stmp_num=stmp.substr(pos_begin_num,(pos_end_num-pos_begin_num));
      //std::cerr<<stmp_num<<std::endl;
      tmp_num=atoi(stmp_num.c_str());
      v.push_back(tmp_num);
    }
    else{
      //std::cerr<<"Could not find any more numbers"<<std::endl;
      break;}
  }
}

//get A number in a string
void get_number(const string& stmp, UInt_t& n){
  UInt_t pos_begin_num = 0;
  UInt_t pos_end_num = 0;
  string stmp_num;
  UInt_t tmp_num;
  pos_begin_num=stmp.find_first_of("1234567890.",pos_end_num);
  if(pos_begin_num!=(string::npos)){
    if (stmp.size()>pos_begin_num+2 &&
	(stmp.substr(pos_begin_num,2) == "0x" ||
	 stmp.substr(pos_begin_num,2)=="0X" ) )   {
      //  This is a hexadecimal number!
      pos_end_num=stmp.find_first_not_of("1234567890ABCDEFabcdef.xX",pos_begin_num);
      stmp_num=stmp.substr(pos_begin_num,(pos_end_num-pos_begin_num));
      tmp_num=strtol(stmp_num.c_str(),NULL,0);
      n=tmp_num;
    } else {
      //  Treat this as a decimal number.
      pos_end_num=stmp.find_first_not_of("1234567890.",pos_begin_num);
      stmp_num=stmp.substr(pos_begin_num,(pos_end_num-pos_begin_num));
      tmp_num=atoi(stmp_num.c_str());
      n=tmp_num;
    }
  } else {
    std::cerr<<"Did not find any number in this line"<<std::endl;
  }
}

//get the first word in a line
string get_firstwd(const string& tmpline){
  UInt_t pos1,pos2;
  string stmp;
  pos1=tmpline.find_first_not_of(" \t\n");
  pos2=tmpline.find_first_of(" \t\n",pos1);
  stmp=tmpline.substr(pos1,pos2-pos1);
  return stmp;
}

//Test whether the string is a number string or not
Bool_t IsNumber(const string& word){
  UInt_t pos;
  pos = word.find_first_not_of("0123456789.+-eE");//white space not allowed
  if(pos!=string::npos) return kFALSE;
  else return kTRUE;
}

//get the last word in a string
string get_lastwd(const string& tmpline, const Char_t* delim){
  UInt_t pos1,pos2;
  string stmp;
  pos1 = tmpline.find_last_not_of(" \t\n"); //last valid character
  if(pos1==string::npos) return "\0"; //empty string
  else{ 
    stmp = tmpline.substr(0,pos1+1);//trimmed string
    std::cout<<"trimmed string \""<<stmp<<"\""<<std::endl;
    pos2 = stmp.length();//set the end iterator
    if(pos2==0) return "\0"; //empty string
    //now try to find the last white space
    if(!delim) pos1 = stmp.find_last_of(" \t\n");
    else pos1 = stmp.find_last_of(delim);
    if(pos1!=string::npos){//did find a white space
      stmp = stmp.substr(pos1+1,pos2-(pos1+1));
      std::cout<<"final string \""<<stmp<<"\""<<std::endl;
      return stmp;
    }
    else{//did not find a white space, take this whole string and return
      return stmp;
    }
  }
};
  
