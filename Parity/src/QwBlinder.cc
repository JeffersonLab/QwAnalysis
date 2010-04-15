#include "QwBlinder.h"
#include <string.h>

//  Backup type definition for ULong64_t; needed with some older ROOT versions.
#if !defined(ULong64_t)
#if defined(R__WIN32) && !defined(__CINT__)
typedef unsigned __int64   ULong64_t; //Portable unsigned long integer 8 bytes
#else
typedef unsigned long long ULong64_t; //Portable unsigned long integer 8 bytes
#endif
#endif


QwBlinder::QwBlinder(QwDatabase* sql, UInt_t seed_id, Bool_t enable_blinding):
  fBlindFactor(-1.0),fBlindingEnabled(enable_blinding),fSeedID(seed_id),
  fMaxTests(10)
{
  //  Set a default seed phrase.
  TString defaultseed  = "\"There is a condition worse than blindness, and that is, seeing something that isn't there.\" --- Thomas Hardy.";

  fSQL = sql;
  this->ReadSeed();
  if (fSeedID == 0){
    std::cout << "QwBlinder::QwBlinder():  "
	      << "Set fSeed to internal default value." << std::endl;
    fSeed = fSeed + " " + defaultseed;
  }

  InitBlinders();
  SetTestValues(defaultseed);
};


void  QwBlinder::WriteFinalValuesToDB()
{
  fSQL->Connect();
  this->WriteChecksum();
  if (!this->CheckTestValues()){
    std::cerr << "QwBlinder::WriteFinalValuesToDB():  "
	      << "Blinded test values have changed; may be a problem in the analysis!!!"
	      << std::endl;
  }
  this->WriteTestValues();
  fSQL->Disconnect();
};




void QwBlinder::InitBlinders()
{
  Int_t    finalseed;
  Double_t newtempout;

  if (fBlindingEnabled) {
    finalseed = UsePseudorandom(fSeed);

    if ((finalseed&0x80000000) == 0x80000000){
      newtempout = -1.0 * (finalseed&0x7FFFFFFF);
    } else {
      newtempout =  1.0 * (finalseed&0x7FFFFFFF);
    }

    //  Generate a number from 0.875 through 1.125, then square it
    //  to get a number from 0.766 through 1.266.
    Double_t tmp1;
    tmp1         = 1.0 + 0.125 * newtempout/Int_t(0x7FFFFFFF);
    fBlindFactor = tmp1 * tmp1;

    //  Do another little calulation to round off the blinding factor
    Double_t tmp2;
    tmp1 = fBlindFactor*4;       // Exactly shifts by two binary places
    tmp2 = tmp1 + fBlindFactor;  // Rounds 5*fBlindFactor
    fBlindFactor = tmp2 - tmp1;  // fBlindFactor has been rounded.

    std::cout << "QwBlinder::InitBlinders():  Blinding factor has been calculated."<< std::endl;
  } else {
    fSeed   = "";
    fSeedID = 0;
    fBlindFactor = 1.0;
    std::cout << "QwBlinder::InitBlinders():  Blinding factor has been forced to 1.000."<< std::endl;
  }

  TString hex_string = Form("%.16llx",(*(ULong64_t*)(&fBlindFactor)));
  fDigest = GenerateDigest(hex_string.Data());
  fBFChecksum = "";
  for (size_t i=0; i<fDigest.size(); i++){
    fBFChecksum += string(Form("%.2x",fDigest[i]));
  }
};

void QwBlinder::SetTestValues(TString &barestring)
{
  Int_t finalseed;
  finalseed = UsePseudorandom(barestring);
  Double_t tempval;

  Int_t counter = 0;

  if (fBlindFactor == -1.0) {
    std::cerr<<"QwBlinder::SetTestValues: The blinding factor has not been correctly calculated.  Exiting..."
             <<std::endl;
  } else {
    for (UInt_t i=0; i<fMaxTests; i++){
      for (Int_t j=0; j<16; j++){
	finalseed &= 0x7FFFFFFF;
	if ((finalseed & 0x800000) == 0x800000){
	  finalseed = ((finalseed^0x00000d)<<1) | 0x1;
	} else {
	  finalseed<<=1;
	}
      }
      if ((finalseed&0x8000)==0x8000){
	tempval = -1.0 * (finalseed&0xFFF) / (4096.0 * 1024.0);  // This produces a value from -0.24 ppm to -976.6 ppm
      } else {
	tempval =  1.0 * (finalseed&0xFFF) / (4096.0 * 1024.0);  // This produces a value from 0.24 ppm to 976.6 ppm
      }
      counter++;
      fTestNumber.push_back(counter);
      fTestValue.push_back(tempval);
      this->BlindMe(tempval);
      fBlindTestValue.push_back(tempval);
    }
    std::cout << "QwBlinder::SetTestValues():  A total of " << std::dec << counter
	 <<" test values have been calculated successfully."<< std::endl;
  }
};

Int_t QwBlinder::UseStringManip(TString &barestring)
{
  std::vector<UInt_t> choppedwords;
  UInt_t tmpword;
  Int_t finalseed = 0;

  for (Int_t i=0; i<barestring.Length(); i++){
    if (i%4 == 0)  tmpword = 0;
    tmpword |= (char(barestring[i]))<<(24-8*(i%4));
    if (i%4 == 3 || i==barestring.Length()-1) {
      choppedwords.push_back(tmpword);
      finalseed ^= (tmpword);
    }
  }
  for (Int_t i=0; i<64; i++){
    finalseed &= 0x7FFFFFFF;
    if ((finalseed & 0x800000) == 0x800000){
      finalseed = ((finalseed^0x00000d)<<1) | 0x1;
    } else {
      finalseed<<=1;
    }
  }
  if ((finalseed&0x80000000) == 0x80000000){
    finalseed = -1 * (finalseed&0x7FFFFFFF);
  } else {
    finalseed = (finalseed&0x7FFFFFFF);
  }
  return finalseed;
};

Int_t QwBlinder::UsePseudorandom(TString &barestring)
{
  ULong64_t finalseed;
  Int_t bitcount;
  Int_t tempout = 0;

  //  This is an attempt to build a portable 64-bit constant
  ULong64_t longmask = (0x7FFFFFFF);
  longmask<<=32;
  longmask|=0xFFFFFFFF;

  finalseed = 0;
  bitcount  = 0;
  for (Int_t i=0; i<barestring.Length(); i++){
    if ( ((barestring[i])&0xC0)!=0 && ((barestring[i])&0xC0)!=0xC0){
      finalseed = ((finalseed&longmask)<<1) | (((barestring[i])&0x40)>>6);
      bitcount++;
    }
    if ( ((barestring[i])&0x30)!=0 && ((barestring[i])&0x30)!=0x30){
      finalseed = ((finalseed&longmask)<<1) | (((barestring[i])&0x10)>>4);
      bitcount++;
    }
    if ( ((barestring[i])&0xC)!=0 && ((barestring[i])&0xC)!=0xC){
      finalseed = ((finalseed&longmask)<<1) | (((barestring[i])&0x4)>>2);
      bitcount++;
    }
    if ( ((barestring[i])&0x3)!=0 && ((barestring[i])&0x3)!=0x3){
      finalseed = ((finalseed&longmask)<<1) | ((barestring[i])&0x1);
      bitcount++;
    }
  }
  for (Int_t i=0; i<(192-bitcount); i++){
    if ((finalseed & 0x800000) == 0x800000){
      finalseed = ((finalseed^0x00000d)<<1) | 0x1;
    } else {
      finalseed<<=1;
    }
  }
  tempout = (finalseed&0xFFFFFFFF) ^ ((finalseed>>32)&0xFFFFFFFF);
  if ((tempout&0x80000000) == 0x80000000){
    tempout = -1 * (tempout&0x7FFFFFFF);
  } else {
    tempout =  1 * (tempout&0x7FFFFFFF);
  }
  return tempout;
};

Int_t QwBlinder::UseMD5(TString &barestring)
{
  Int_t j;
  Int_t temp;
  Int_t tempout = 0;
  
  temp = 0;
  vector<UChar_t> digest = this->GenerateDigest(barestring.Data(),"md5");
  for (size_t i=0; i<digest.size(); i++){
    j = i%4;
    if (j == 0){
      temp = 0;
    }
    temp |= (digest[i])<<(24-(j*8));
    if ( (j==3) || (i==(digest.size()-1))){
      tempout ^= temp;
    }
  }
  if ((tempout&0x80000000) == 0x80000000){
    tempout = -1 * (tempout&0x7FFFFFFF);
  } else {
    tempout = (tempout&0x7FFFFFFF);
  }
  return tempout;
};


//------------------------------------------------------------
//------------------------------------------------------------
// Function to read the seed in from the database.
//
// Parameters:
//	seed_id = ID number of seed to use (0 = most recent seed)
//
// Return: Int_t
//
//------------------------------------------------------------
//------------------------------------------------------------

Int_t QwBlinder::ReadSeed()
{
      fSQL->Connect();
      string s_sql = "SELECT seed_id, seed FROM seeds ";

      if (fSeedID > 0) {
	// Use specified seed
	Char_t s_seed_id[10];
	sprintf(s_seed_id, "%d", fSeedID);
	
	s_sql += "WHERE seed_id = ";
	s_sql += string(s_seed_id);
	
      } else {
	// Use most recent seed
	s_sql += "ORDER BY seed_id DESC LIMIT 1";
      }

  try {
    mysqlpp::Query query = fSQL->Query(s_sql);
    std::vector<QwParityDB::analysis> res;
    query.storein(res);

    std::vector<QwParityDB::analysis>::iterator it;
    for (it = res.begin(); it != res.end(); ++it) {
      std::cout << it->seed_id << std::endl;
    }
    fSQL->Disconnect();
  }
  catch (const mysqlpp::Exception& er) {
    QwError << er.what() << QwLog::endl;
    return(-1);
  }
  return 0;
}  //end QwBlinder::ReadSeed()

//------------------------------------------------------------
//------------------------------------------------------------
// Function to write the checksum into the analysis table
//
// Parameters: void
//
// Return: void
//
// Note:  This function assumes that the analysis table has already
//        been filled for the run.
//------------------------------------------------------------
//------------------------------------------------------------
void QwBlinder::WriteChecksum() {

  fSQL->Connect();
  //---------------------------------------------------------- 
  // Construct SQL
  //---------------------------------------------------------- 
  Char_t s_number[20];
  string s_sql = "UPDATE analysis SET seed_id = ";
  sprintf(s_number, "%d", fSeedID);
  s_sql += string(s_number);
  s_sql += ", bf_checksum = ";
  s_sql += "\'" + fBFChecksum + "\'";
  s_sql += " WHERE analysis_id = ";
  sprintf(s_number, "%d", fSQL->GetAnalysisID());
  s_sql += string(s_number);

  //---------------------------------------------------------- 
  // Execute SQL
  //---------------------------------------------------------- 
  mysqlpp::Query query = fSQL->Query(s_sql);
  fSQL->Disconnect();
} //End QwBlinder::WriteChecksum

//------------------------------------------------------------
//------------------------------------------------------------
// Function to write the test values into the database
//
// Parameters: void
//
// Return: void
//------------------------------------------------------------
//------------------------------------------------------------
void QwBlinder::WriteTestValues() {

  fSQL->Connect();
  //---------------------------------------------------------- 
  // Construct Initial SQL
  //---------------------------------------------------------- 
  Char_t s_number[20];

  string s_sql_pre = "INSERT INTO bf_tests (analysis_id, test_number, test_value) VALUES (";
  // analysis_id
  sprintf(s_number, "%d", fSQL->GetAnalysisID());
  s_sql_pre += string(s_number);
  s_sql_pre += ", ";

  //---------------------------------------------------------- 
  // Construct Individual SQL and Execute
  //---------------------------------------------------------- 
  // Loop over all test values
  for (UInt_t i = 0; i<fMaxTests; i++) {
    string s_sql = s_sql_pre;

    // test_number
    sprintf(s_number, "%d", fTestNumber[i]);
    s_sql += string(s_number);
    s_sql += ", ";

    // test_value
    sprintf(s_number, "%9g", fBlindTestValue[i]);
    s_sql += s_number;
    s_sql += ")";

    // Execute SQL
  mysqlpp::Query query = fSQL->Query(s_sql);
  fSQL->Disconnect();
  } // End loop over test values
} //End QwBlinder::WriteTestValues



Bool_t QwBlinder::CheckTestValues()
{
  //  This routines checks to see if the stored fBlindTestValue[i]
  //  match a recomputed blinded test value.  The values are cast
  //  into floats, and their difference must be less than a change
  //  of the least-significant-bit of fBlindTestValue[i].
  Double_t checkval;
  Bool_t   status = kTRUE;

  Float_t test1, test2, epsilon;

  for (UInt_t i=0; i<fMaxTests; i++){
    checkval = fTestValue[i];
    this->BlindMe(checkval);

    test1 = fBlindTestValue[i]*2.0;      //  Shift by one factor of 2.
    test2 = test1 + fBlindTestValue[i];  //  Round to 3*fBlindTestValue[i].
    epsilon = test2 - test1;             //  Rounded fBlindTestValue[i].
    epsilon -= fBlindTestValue[i];       //  (Rounded - original).
    if (epsilon<0.0){
      epsilon *= -1.0;
    }
    test1 = fBlindTestValue[i];
    test2 = checkval;
    if ( (test1-test2)<=(-1.0*epsilon) || (test1-test2)>=epsilon ) {
      std::cerr << "QwBlinder::CheckTestValues():  Blinded test value "
		<< std::dec << fTestNumber[i] 
		<< " does not agree with reblinded test value, " 
		<< "with a difference of "
		<< (fBlindTestValue[i]-checkval) << "."
		<< std::endl;
      status = kFALSE;
    }
  }
  return status;
};


vector<UChar_t> QwBlinder::GenerateDigest(const char* input, TString digest_name)
{
  EVP_MD_CTX mdctx;
  const EVP_MD *md;
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len, i;

  vector<UChar_t> output;

  OpenSSL_add_all_digests();
  md = EVP_get_digestbyname(digest_name.Data());
  if (!md) {
    TString errmessage = TString("Unknown message digest ")+digest_name;
    std::cerr<<"QwBlinder::GenerateDigest(): "<<errmessage.Data()<<std::endl;
  }
  EVP_MD_CTX_init(&mdctx);
  EVP_DigestInit_ex(&mdctx, md, NULL);
  EVP_DigestUpdate(&mdctx, input, strlen(input));
  EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
  EVP_MD_CTX_cleanup(&mdctx);
 
  for (i = 0; i < md_len; i++){
    output.push_back(md_value[i]);
  }
  return output;
};


void QwBlinder::PrintFinalValues()
{
  std::cout << "QwBlinder::PrintCheckValues():  Begin summary"  << std::endl;
  std::cout << "\tThe blinding factor checksum for seed ID "
	    << this->fSeedID << " is: \n\t\t\'" << fBFChecksum << "\'" << std::endl;
  this->CheckTestValues();
  std::cout << "\tThe blinded test values are:\n\t\tIndex\tValue" << std::endl;
  for (UInt_t i=0; i<fMaxTests; i++) {
    std::cout << "\t\t" << fTestNumber[i] << "\t" << Form("% 9g",fBlindTestValue[i]) << std::endl;
  }
  std::cout << "QwBlinder::PrintCheckValues():  End of summary"  << std::endl;
};
