/*!
 * \file   QwBlinder.cc
 * \brief  A class for blinding data, adapted from G0 blinder class.
 *
 * \author Peiqing Wang
 * \date   2010-04-14
 */

#include "QwBlinder.h"

// System headers
#include <string>
#include <limits>

// Qweak headers
#include "QwLog.h"

// Maximum blinding asymmetry for additive blinding
const Double_t QwBlinder::kMaximumBlindingAsymmetry = 0.0; // ppm
const Double_t QwBlinder::kMaximumBlindingFactor = 0.0; // [fraction]

/**
 * Constructor using explicit seed string
 * @param seed Explicitly specified seed string
 * @param blinding_strategy Blinding strategy
 */
QwBlinder::QwBlinder(const TString& seed, const EQwBlindingStrategy blinding_strategy)
: fBlindingStrategy(blinding_strategy),
  fBlindingOffset(0.0),
  fBlindingFactor(1.0),
  fSeedID(0),
  fMaxTests(10)
{
  // Store seed string
  fSeed = seed;

  // Set up the blinder
  InitBlinders();
  SetTestValues(seed);
}


/**
 * Constructor using database connection
 * @param db Database connection
 * @param seed_id ID of the seed table
 * @param blinding_strategy Blinding strategy
 */
QwBlinder::QwBlinder(QwDatabase* db, const UInt_t seed_id, const EQwBlindingStrategy blinding_strategy)
: fBlindingStrategy(blinding_strategy),
  fBlindingOffset(0.0),
  fBlindingFactor(1.0),
  fSeedID(0),
  fMaxTests(10)
{
  // Set a default seed phrase.
  TString defaultseed  = "\"There is a condition worse than blindness, and that is, seeing something that isn't there.\" --- Thomas Hardy.";

  // Read the requested seed
  fSeedID = this->ReadSeed(db, seed_id);
  if (fSeedID == 0) {
    QwWarning << "QwBlinder::QwBlinder(): "
              << "Set seed to internal default value." << QwLog::endl;
    fSeed = fSeed + " " + defaultseed;
  }

  // Set up the blinder
  InitBlinders();
  SetTestValues(defaultseed);
};


/**
 * Destructor checks the validity of the blinding and unblinding
 */
QwBlinder::~QwBlinder()
{
  // Check the blinded values
  PrintFinalValues();
}



/*!-----------------------------------------------------------
 *------------------------------------------------------------
 * Function to read the seed in from the database.
 *
 * Parameters:
 *	seed_id = ID number of seed to use (0 = most recent seed)
 *
 * Return: Int_t
 *
 *------------------------------------------------------------
 *------------------------------------------------------------*/
Int_t QwBlinder::ReadSeed(QwDatabase* db, const UInt_t seed_id)
{
  string s_sql = "SELECT * FROM seeds ";
  Bool_t sql_status = db->Connect();
  if (sql_status)
    {
      if (fSeedID > 0)
        {
          // Use specified seed
          Char_t s_seed_id[10];
          sprintf(s_seed_id, "%d", seed_id);

          s_sql += "WHERE seed_id = ";
          s_sql += string(s_seed_id);
        }
      else
        {
          // Use most recent seed
          s_sql += "ORDER BY seed_id DESC LIMIT 1";
        }

      mysqlpp::Query query = db->Query();
      query<<s_sql;
      std::vector<QwParityDB::seeds> res;
      query.storein(res);

// Store seed_id and seed value in fSeedID and fSeed (want to store actual seed_id in those
// cases where only the most recent was requested (fSeedID = 0))
      // There should be one and only one seed_id for each seed.
      if (res.size() == 1)
        {
          // Store seed_id in fSeedID (want to store actual seed_id in those
          // cases where only the most recent was requested (fSeedID = 0))
          fSeedID = res[0].seed_id;

          // Store seed value
          if (!res[0].seed.is_null) {
            fSeed = res[0].seed.data;
          } else {
            std::cerr << "QwBlinder::ReadSeed(): ERROR:  Seed value came back NULL from the database." << std::endl;
            fSeedID = 0;
          }

          std::cout << "QwBlinder::ReadSeed():  Successfully read "
          << Form("the fSeed with ID %d from the database.", fSeedID)
          << std::endl;
        }
      else
        {
          // Error Condition.
          // There should be one and only one seed_id for each seed.
          fSeedID = 0;
          fSeed   = Form("ERROR:  There should be one and only one seed_id for each seed, but this had %d.",
                         res.size());
          std::cerr << "QwBlinder::ReadSeed(): "<<fSeed<<std::endl;
        }
      db->Disconnect();
    }
  else
    {
      //  We were unable to open the connection.
      fSeedID = 0;
      fSeed   = "ERROR:  Unable to open the connection to the database.";
      std::cerr << "QwBlinder::ReadSeed():  " << fSeed << std::endl;
    }

  return fSeedID;
} // End QwBlinder::ReadSeed



/**
 * Initialize the blinder parameters
 */
void QwBlinder::InitBlinders()
{
  if (fBlindingStrategy != kDisabled) {

      Int_t finalseed = UseMD5(fSeed);

      Double_t newtempout;
      if ((finalseed & 0x80000000) == 0x80000000) {
          newtempout = -1.0 * (finalseed & 0x7FFFFFFF);
      } else {
          newtempout =  1.0 * (finalseed & 0x7FFFFFFF);
      }


      /// The blinding constants are determined in two steps.
      ///
      /// First, the blinding asymmetry (offset) is determined.  It is
      /// generated from a signed number between +/- 0.244948974 that
      /// is squared to get a number between +/- 0.06 ppm.
      static Double_t maximum_asymmetry_sqrt = sqrt(kMaximumBlindingAsymmetry);
      Double_t tmp1 = maximum_asymmetry_sqrt * (newtempout / Int_t(0x7FFFFFFF));
      fBlindingOffset = tmp1 * fabs(tmp1) * 0.000001;

      //  Do another little calulation to round off the blinding asymmetry
      Double_t tmp2;
      tmp1 = fBlindingOffset * 4;    // Exactly shifts by two binary places
      tmp2 = tmp1 + fBlindingOffset; // Rounds 5*fBlindingOffset
      fBlindingOffset = tmp2 - tmp1; // fBlindingOffset has been rounded.

      /// Secondly, the multiplicative blinding factor is determined.  This
      /// number is generated from the blinding asymmetry between, say, 0.9 and 1.1
      /// by an oscillating but uniformly distributed sawtooth function.
      fBlindingFactor = 1.0;
      if (kMaximumBlindingAsymmetry > 0.0) {
        fBlindingFactor  = 1.0 + fmod(30.0 * fBlindingOffset, kMaximumBlindingAsymmetry);
        fBlindingFactor /= (kMaximumBlindingAsymmetry > 0.0 ? kMaximumBlindingAsymmetry : 1.0);
      }

      QwMessage << "QwBlinder::InitBlinders(): Blinding parameters have been calculated."<< QwLog::endl;

  } else {

      fSeed   = "";
      fSeedID = 0;
      fBlindingFactor = 1.0;
      fBlindingOffset = 0.0;
      fBlindingStrategy = kDisabled;
      QwWarning << "QwBlinder::InitBlinders(): Blinding parameters have been disabled!"<< QwLog::endl;
  }

  // Generate checksum
  TString hex_string;
  hex_string.Form("%.16llx%.16llx", *(ULong64_t*)(&fBlindingFactor), *(ULong64_t*)(&fBlindingOffset));
  fDigest = GenerateDigest(hex_string);
  fChecksum = "";
  for (size_t i = 0; i < fDigest.size(); i++)
    fChecksum += string(Form("%.2x",fDigest[i]));

}; // End of InitBlinders()


void  QwBlinder::WriteFinalValuesToDB(QwDatabase* db)
{
  this->WriteChecksum(db);
  if (!this->CheckTestValues())
    {
      std::cerr << "QwBlinder::WriteFinalValuesToDB():  "
      << "Blinded test values have changed; may be a problem in the analysis!!!"
      << std::endl;
    }
  this->WriteTestValues(db);
};



/**
 * Generate a set of test values of similar size as measured asymmetries
 * @param barestring Seed string
 */
void QwBlinder::SetTestValues(const TString& barestring)
{
  Int_t finalseed = UsePseudorandom(barestring);

  // If the blinding factor is one and blinding asymmetry zero, warn the user
  if (fBlindingFactor == 1.0 && fBlindingOffset == 0.0) {

      QwWarning << "QwBlinder::SetTestValues(): The blinding parameters have "
                << "not been calculated correctly!" << QwLog::endl;

  }

  // For each test case
  for (int i = 0; i < fMaxTests; i++) {

      // Generate a quasi-random number
      for (Int_t j = 0; j < 16; j++) {
         finalseed &= 0x7FFFFFFF;
          if ((finalseed & 0x800000) == 0x800000) {
              finalseed = ((finalseed ^ 0x00000d) << 1) | 0x1;
          } else {
              finalseed <<= 1;
          }
      }

      // Using end 3 digits (0x1 through 0xFFF) of the finalseed (1 - 4095)
      // This produces a value from 0.47 ppb to 953.4 ppb
      Double_t tempval = -1.0 * (finalseed & 0xFFF) / (1024.0 * 4096.0 * 1024.0);

      // Store the test values
      fTestNumber.push_back(i);
      fTestValue.push_back(tempval);
      this->BlindValue(tempval);
      fBlindTestValue.push_back(tempval);
      this->UnBlindValue(tempval);
      fUnBlindTestValue.push_back(tempval);
    }

  QwMessage << "QwBlinder::SetTestValues(): A total of " << std::dec << fMaxTests
            <<" test values have been calculated successfully." << QwLog::endl;

};

/**
 * Use string manipulation to get a number from the seed string
 * @param barestring Seed string
 * @return Integer number
 */
Int_t QwBlinder::UseStringManip(const TString& barestring)
{
  std::vector<UInt_t> choppedwords;
  UInt_t tmpword;
  Int_t finalseed = 0;

  for (Int_t i = 0; i < barestring.Length(); i++)
    {
      if (i % 4 == 0) tmpword = 0;
      tmpword |= (char(barestring[i]))<<(24-8*(i%4));
      if (i%4 == 3 || i == barestring.Length()-1)
        {
          choppedwords.push_back(tmpword);
          finalseed ^= (tmpword);
        }
    }
  for (Int_t i=0; i<64; i++)
    {
      finalseed &= 0x7FFFFFFF;
      if ((finalseed & 0x800000) == 0x800000)
        {
          finalseed = ((finalseed^0x00000d)<<1) | 0x1;
        }
      else
        {
          finalseed<<=1;
        }
    }
  if ((finalseed&0x80000000) == 0x80000000)
    {
      finalseed = -1 * (finalseed&0x7FFFFFFF);
    }
  else
    {
      finalseed = (finalseed&0x7FFFFFFF);
    }
  return finalseed;
};


/**
 * Use pseudo-random number generator to get a number from the seed string
 * @param barestring Seed string
 * @return Integer number
 */
Int_t QwBlinder::UsePseudorandom(const TString& barestring)
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
  for (Int_t i=0; i<barestring.Length(); i++)
    {
      if ( ((barestring[i])&0xC0)!=0 && ((barestring[i])&0xC0)!=0xC0)
        {
          finalseed = ((finalseed&longmask)<<1) | (((barestring[i])&0x40)>>6);
          bitcount++;
        }
      if ( ((barestring[i])&0x30)!=0 && ((barestring[i])&0x30)!=0x30)
        {
          finalseed = ((finalseed&longmask)<<1) | (((barestring[i])&0x10)>>4);
          bitcount++;
        }
      if ( ((barestring[i])&0xC)!=0 && ((barestring[i])&0xC)!=0xC)
        {
          finalseed = ((finalseed&longmask)<<1) | (((barestring[i])&0x4)>>2);
          bitcount++;
        }
      if ( ((barestring[i])&0x3)!=0 && ((barestring[i])&0x3)!=0x3)
        {
          finalseed = ((finalseed&longmask)<<1) | ((barestring[i])&0x1);
          bitcount++;
        }
    }
  for (Int_t i=0; i<(192-bitcount); i++)
    {
      if ((finalseed & 0x800000) == 0x800000)
        {
          finalseed = ((finalseed^0x00000d)<<1) | 0x1;
        }
      else
        {
          finalseed<<=1;
        }
    }
  tempout = (finalseed&0xFFFFFFFF) ^ ((finalseed>>32)&0xFFFFFFFF);
  if ((tempout&0x80000000) == 0x80000000)
    {
      tempout = -1 * (tempout&0x7FFFFFFF);
    }
  else
    {
      tempout =  1 * (tempout&0x7FFFFFFF);
    }
  return tempout;
};


/**
 * Use an MD5 checksum to get a number from the seed string
 * @param barestring Seed string
 * @return Integer number
 */
Int_t QwBlinder::UseMD5(const TString& barestring)
{
  Int_t temp = 0;
  Int_t tempout = 0;

  std::vector<UChar_t> digest = this->GenerateDigest(barestring);
  for (size_t i = 0; i < digest.size(); i++)
    {
      Int_t j = i%4;
      if (j == 0)
        {
          temp = 0;
        }
      temp |= (digest[i])<<(24-(j*8));
      if ( (j==3) || (i==(digest.size()-1)))
        {
          tempout ^= temp;
        }
    }

  if ((tempout & 0x80000000) == 0x80000000) {
      tempout = -1 * (tempout & 0x7FFFFFFF);
  } else {
      tempout = (tempout & 0x7FFFFFFF);
  }

  return tempout;
};



/*!-----------------------------------------------------------
 *------------------------------------------------------------
 * Function to write the checksum into the analysis table
 *
 * Parameters: void
 *
 * Return: void
 *
 * Note:  This function assumes that the analysis table has already
 *        been filled for the run.
 *------------------------------------------------------------
 *------------------------------------------------------------*/
void QwBlinder::WriteChecksum(QwDatabase* db)
{
  //----------------------------------------------------------
  // Construct SQL
  //----------------------------------------------------------
  Char_t s_number[20];
  string s_sql = "UPDATE analysis SET seed_id = ";
  sprintf(s_number, "%d", fSeedID);
  s_sql += string(s_number);
  s_sql += ", bf_checksum = ";
  s_sql += "\'" + fChecksum + "\'";
  s_sql += " WHERE analysis_id = ";
  sprintf(s_number, "%d", db->GetAnalysisID());
  s_sql += string(s_number);

  //----------------------------------------------------------
  // Execute SQL
  //----------------------------------------------------------
  db->Connect();
  mysqlpp::Query query = db->Query();
  query <<s_sql;
  query.execute();
  db->Disconnect();
} //End QwBlinder::WriteChecksum

/*!------------------------------------------------------------
 *------------------------------------------------------------
 * Function to write the test values into the database
 *
 * Parameters: void
 *
 * Return: void
 *------------------------------------------------------------
 *------------------------------------------------------------*/
void QwBlinder::WriteTestValues(QwDatabase* db)
{
  //----------------------------------------------------------
  // Construct Initial SQL
  //----------------------------------------------------------
  Char_t s_number[20];

  string s_sql_pre = "INSERT INTO bf_test (analysis_id, test_number, test_value) VALUES (";
  // analysis_id
  sprintf(s_number, "%d", db->GetAnalysisID());
  s_sql_pre += string(s_number);
  s_sql_pre += ", ";

  //----------------------------------------------------------
  // Construct Individual SQL and Execute
  //----------------------------------------------------------
  // Loop over all test values
  for (int i = 0; i < fMaxTests; i++)
    {
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
      db->Connect();
      mysqlpp::Query query = db->Query();
      query <<s_sql;
      query.execute();
      db->Disconnect();
    } // End loop over test values
} //End QwBlinder::WriteTestValues


/*!--------------------------------------------------------------
 *  This routines checks to see if the stored fBlindTestValue[i]
 *  match a recomputed blinded test value.  The values are cast
 *  into floats, and their difference must be less than a change
 *  of the least-significant-bit of fBlindTestValue[i].
 *--------------------------------------------------------------*/

Bool_t QwBlinder::CheckTestValues()
{
  Double_t checkval;
  Bool_t   status = kTRUE;

  double epsilon = std::numeric_limits<double>::epsilon();
  double test1, test2;

  for (int i = 0; i < fMaxTests; i++)
    {
      /// First test: compare a blinded value with a second computation

      checkval = fTestValue[i];
      this->BlindValue(checkval);

      test1 = fBlindTestValue[i];
      test2 = checkval;
      if ((test1 - test2) <= -epsilon || (test1 - test2) >= epsilon)
        {
          std::cerr << "QwBlinder::CheckTestValues():  Blinded test value "
          << std::dec << fTestNumber[i]
          << " does not agree with reblinded test value, "
          << "with a difference of "
          << (test1 - test2) << "."
          << std::endl;
          status = kFALSE;
        }


      /// Second test: compare the unblinded value with the original value

      test1 = fUnBlindTestValue[i];
      test2 = fTestValue[i];
      if ((test1 - test2) <= -epsilon || (test1 - test2) >= epsilon)
        {
          std::cerr << "QwBlinder::CheckTestValues():  Unblinded test value "
          << std::dec << fTestNumber[i]
          << " does not agree with original test value, "
          << "with a difference of "
          << (test1 - test2) << "."
          << std::endl;
          status = kFALSE;
        }
    }
  return status;
};


/**
 * Generate an MD5 digest of the blinding parameters
 * @param input
 * @return
 */
std::vector<UChar_t> QwBlinder::GenerateDigest(const TString& input)
{
  // Initialize MD5 checksum array
  const UInt_t length = 64;
  UChar_t value[length];
  for (UInt_t i = 0; i < length; i++)
    value[i] = 0;

  // Calculate MD5 checksum from input and store in md5_value
  TMD5 md5;
  md5.Update((UChar_t*) input.Data(), input.Length());
  md5.Final(value);

  // Copy the MD5 checksum in an STL vector
  std::vector<UChar_t> output;
  for (UInt_t i = 0; i < length; i++)
    output.push_back(value[i]);

  return output;
};


/**
 * Print a summary of the blinding/unblinding test
 */
void QwBlinder::PrintFinalValues()
{
  QwMessage << "QwBlinder::PrintCheckValues():  Begin summary"    << QwLog::endl;
  QwMessage << "================================================" << QwLog::endl;
  QwMessage << "The blinding parameters checksum for seed ID "
            << fSeedID << " is:" << QwLog::endl;
  QwMessage << fChecksum << QwLog::endl;
  QwMessage << "================================================" << QwLog::endl;
  CheckTestValues();
  QwMessage << "The test results are:" << QwLog::endl;
  QwMessage << std::setw(8)  << "Index"
            << std::setw(16) << "Original value"
            << std::setw(16) << "Blinded value"
            << std::setw(16) << "Unblinded value"
            << QwLog::endl;
  for (int i = 0; i < fMaxTests; i++) {
    QwMessage << std::setw(8)  << fTestNumber[i]
              << std::setw(16) << Form("% 9g ppb",fTestValue[i]*1e9)
              << std::setw(16) << Form(" [CENSORED]")
            //<< std::setw(16) << Form("% 9g ppb",fBlindTestValue[i]*1e9)
              << std::setw(16) << Form("% 9g ppb",fUnBlindTestValue[i]*1e9)
              << QwLog::endl;
  }
  QwMessage << "================================================" << QwLog::endl;
  QwMessage << "QwBlinder::PrintCheckValues():  End of summary"   << QwLog::endl;
};


/**
 * Write the blinding parameters to the database
 * @param db Database connection
 * @param datatype Datatype
 *
 * For each analyzed run the database contains a digest of the blinding parameters
 * and a number of blinded test entries.
 */
void QwBlinder::FillDB(QwDatabase *db, TString datatype)
{
  QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  QwMessage << "                         QwBlinder::FillDB                       " << QwLog::endl;
  QwMessage << " --------------------------------------------------------------- " << QwLog::endl;

  // Get the analysis ID
  UInt_t analysis_id = db->GetAnalysisID();


  // Fill the rows of the QwParityDB::bf_test table
  QwParityDB::bf_test bf_test_row(0);
  std::vector<QwParityDB::bf_test> bf_test_list;
  for (int i = 0; i < fMaxTests; i++) {
    bf_test_row.bf_test_id = 0;
    bf_test_row.analysis_id = analysis_id;
    bf_test_row.test_number = fTestNumber[i];
    bf_test_row.test_value  = fBlindTestValue[i];
    bf_test_list.push_back(bf_test_row);
  }


  // Connect to the database
  db->Connect();

  // Modify the seed_id and bf_checksum in the analysis table
  try {
    // Get the rows of the QwParityDB::analysis table
    mysqlpp::Query query = db->Query();
    query.execute(
      Form("select * from analysis where analysis_id = ",analysis_id));
    mysqlpp::StoreQueryResult analysis_res = query.store();
    QwParityDB::analysis analysis_row_orig = analysis_res[0];
    QwParityDB::analysis analysis_row_new  = analysis_res[0];

    // Modify the seed_id and bf_checksum
    analysis_row_new.seed_id = fSeedID;
    analysis_row_new.bf_checksum = fChecksum;

    // Update the analysis table
    query = db->Query();
    query.update(analysis_row_orig, analysis_row_new);
    query.execute();
  } catch (const mysqlpp::Exception& err) {
    QwError << err.what() << QwLog::endl;
  }

  // Add the bf_test rows
  try {
    if (bf_test_list.size()) {
      mysqlpp::Query query = db->Query();
      query.insert(bf_test_list.begin(), bf_test_list.end());
      query.execute();
    } else QwMessage << "QwBlinder::FillDB(): No bf_test entries to write." << QwLog::endl;
  } catch (const mysqlpp::Exception& err) {
    QwError << err.what() << QwLog::endl;
  }

  // Disconnect from database
  db->Disconnect();

}

