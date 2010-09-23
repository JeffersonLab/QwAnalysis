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
#include "QwVQWK_Channel.h"

// Maximum blinding asymmetry for additive blinding
const Double_t QwBlinder::kMaximumBlindingAsymmetry = 0.6; // ppm
const Double_t QwBlinder::kMaximumBlindingFactor = 0.1; // [fraction]

// Default seed, associated with seed_id 0
const TString QwBlinder::kDefaultSeed = "Default seed, should not be used!";

//**************************************************//
void QwBlinder::DefineOptions(QwOptions &options){
  options.AddOptions("Blinder")("blinder.force-target-lh2", po::value<bool>()->default_value(false)->zero_tokens(),
		       "Forces the blinder to interpret the target position as LH2");
  options.AddOptions("Blinder")("blinder.force-target-out", po::value<bool>()->default_value(false)->zero_tokens(),
		       "Forces the blinder to interpret the target position as target-out");
  options.AddOptions("Blinder")("blinder.beam-current-threshold", po::value<double>()->default_value(1.0),
		       "Beam current in microamps below which data will not be blinded");
};


/**
 * Default constructor using optional database connection and blinding strategy
 * @param blinding_strategy Blinding strategy
 */
QwBlinder::QwBlinder(const EQwBlindingStrategy blinding_strategy):
  fTargetBlindability_firstread(kIndeterminate),
  fTargetBlindability(kIndeterminate),
  fTargetPositionForced(kFALSE),
  fBeamCurrentThreshold(1.0),
  fBeamIsPresent(kFALSE),
  fBlindingStrategy(blinding_strategy),
  fBlindingOffset(0.0),
  fBlindingFactor(1.0)
{
  // Set up the blinder with seed_id 0
  InitBlinders(0);

  // Calculate set of test values
  InitTestValues(10);
};


/**
 * Destructor checks the validity of the blinding and unblinding
 */
QwBlinder::~QwBlinder()
{
  // Check the blinded values
  PrintFinalValues();
}


/**
 * Update the blinder status with new external information
 *
 * @param options Qweak option handler
 */
void QwBlinder::ProcessOptions(QwOptions& options)
{
  if (options.GetValue<bool>("blinder.force-target-out")
      && options.GetValue<bool>("blinder.force-target-lh2")){
    QwError << "QwBlinder::Update:  Both blinder.force-target-lh2 and blinder.force-target-out are set.  "
	    << "Only one can be in force at one time.  Exit and choose one option."
	    << QwLog::endl;
    exit(10);
  } else if (options.GetValue<bool>("blinder.force-target-lh2")){
    fTargetPositionForced = kTRUE;
    SetTargetBlindability(QwBlinder::kBlindable);
  } else if (options.GetValue<bool>("blinder.force-target-out")){
    fTargetPositionForced = kTRUE;
    SetTargetBlindability(QwBlinder::kNotBlindable);
  }
  if (options.HasValue("blinder.beam-current-threshold")){
    fBeamCurrentThreshold = options.GetValue<double>("blinder.beam-current-threshold");
  }

}

/**
 * Update the blinder status with new external information
 *
 * @param db Database connection
 */
void QwBlinder::Update(QwDatabase* db)
{
  //  Update the seed ID then tell us if it has changed.
  UInt_t old_seed_id = fSeedID;
  ReadSeed(db);
  // If the blinding seed has changed, re-initialize the blinder
  if (fSeedID != old_seed_id) {
    QwWarning << "Changing blinder seed to " << fSeedID
              << " from " << old_seed_id << "." << QwLog::endl;
    InitBlinders(fSeedID);
    InitTestValues(10);
  }
}

/**
 * Update the blinder status with new external information
 *
 * @param detectors Current subsystem array
 */
void QwBlinder::Update(const QwSubsystemArrayParity& detectors)
{
  static QwVQWK_Channel q_targ("q_targ");
  if (fBlindingStrategy != kDisabled) {
    // Check for the target blindability flag
    

    // Check that the current on target is above acceptable limit
    Bool_t tmp_beam = kFALSE;
    if (detectors.ReturnInternalValue(q_targ.GetElementName(), &q_targ)) {
      if (q_targ.GetHardwareSum() > fBeamCurrentThreshold){
	tmp_beam = kTRUE;
      }
    }
    fBeamIsPresent &= tmp_beam;
  }
}

/**
 * Update the blinder status with new external information
 *
 * @param epics Current EPICS event
 */
void QwBlinder::Update(const QwEPICSEvent& epics)
{
  // Check for the target information
  // Position:
  //     QWtgt_name == "HYDROGEN-CELL" || QWTGTPOS > 350
  // Temperature:
  //     QWT_miA < 22.0 && QWT_moA < 22.0
  // Pressure:
  //     QW_PT3 in [20,35] && QW_PT4 in [20,35]
  if (fBlindingStrategy != kDisabled && !(fTargetPositionForced) ) {
    Double_t tgt_pos   = epics.GetDataValue("QWTGTPOS");
    Double_t tgt_temperture = epics.GetDataValue("QWT_miA");
    Double_t tgt_pressure   = epics.GetDataValue("QW_PT3"); 
    if (tgt_pos > 350. 
	&& (tgt_temperture>18.0 && tgt_temperture<22.0)
	&& (tgt_pressure>20.0 && tgt_pressure < 35.0)){
      SetTargetBlindability(QwBlinder::kBlindable);
    } else if (tgt_pos == 0 
	       || (tgt_temperture==0.0)
	       || (tgt_pressure==0.0)){
      SetTargetBlindability(QwBlinder::kIndeterminate);
    } else {
      SetTargetBlindability(QwBlinder::kNotBlindable);
    }
  }
}

/*!-----------------------------------------------------------
 *------------------------------------------------------------
 * Function to read the seed in from the database.
 *
 * Parameters:
 *
 * Return: Int_t 
 *
 *------------------------------------------------------------
 *------------------------------------------------------------*/
Int_t QwBlinder::ReadSeed(QwDatabase* db)
{
  // Return unchanged if no database specified
  if (! db) {
    QwWarning << "QwBlinder::ReadSeed(): No database specified" << QwLog::endl;
    fSeedID = 0;
    fSeed   = "default seed";
    return 0;
  }

  // Try to connect to the database
  if (db->Connect()) {

    // Build up query
    string s_sql = "SELECT seed_id, seed FROM seeds, run as rf, run as rl WHERE seeds.first_run_id = rf.run_id AND seeds.last_run_id = rl.run_id ";
    s_sql += "AND rf.run_number <= ";
    s_sql += db->GetRunNumber();
    s_sql += " AND rl.run_number >= ";
    s_sql += db->GetRunNumber();

    // Send query
    mysqlpp::Query query = db->Query();
    query << s_sql;
    mysqlpp::StoreQueryResult res = query.store();

    // Store seed_id and seed value in fSeedID and fSeed (want to store actual seed_id in those
    // cases where only the most recent was requested (fSeedID = 0))
    // There should be one and only one seed_id for each seed.
    if (res.size() == 1) {
      // Store seed_id in fSeedID (want to store actual seed_id in those
      // cases where only the most recent was requested (fSeedID = 0))
      fSeedID = res[0]["seed_id"];

      // Store seed value
      if (!res[0]["seed"].is_null()) {
        fSeed = res[0]["seed"].data();
      } else {
        QwError << "QwBlinder::ReadSeed(): Seed value came back NULL from the database." << QwLog::endl;
        fSeedID = 0;
        fSeed = kDefaultSeed;
      }

      std::cout << "QwBlinder::ReadSeed():  Successfully read "
      << Form("the fSeed with ID %d from the database.", fSeedID)
      << std::endl;

    } else {
      // Error Condition.
      // There should be one and only one seed_id for each seed.
      fSeedID = 0;
      fSeed   = Form("ERROR:  There should be one and only one seed_id for each seed, but this had %d.",
                     res.size());
      std::cerr << "QwBlinder::ReadSeed(): "<<fSeed<<std::endl;
    }

    // Disconnect from database
    db->Disconnect();

  } else {

    //  We were unable to open the connection.
    fSeedID = 0;
    fSeed   = "ERROR:  Unable to open the connection to the database.";
    QwError << "QwBlinder::ReadSeed(): Unable to open connection to database." << QwLog::endl;
  }

  return fSeedID;
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
  // Return unchanged if no database specified
  if (! db) {
    QwWarning << "QwBlinder::ReadSeed(): No database specified" << QwLog::endl;
    fSeedID = 0;
    fSeed   = "default seed";
    return 0;
  }

  // Try to connect to the database
  if (db->Connect()) {

    // Build up query
    string s_sql = "SELECT * FROM seeds ";
    if (fSeedID > 0) {
      // Use specified seed
      Char_t s_seed_id[10];
      sprintf(s_seed_id, "%d", seed_id);
      s_sql += "WHERE seed_id = ";
      s_sql += string(s_seed_id);
    } else {
      // Use most recent seed
      s_sql += "ORDER BY seed_id DESC LIMIT 1";
    }

    // Send query
    mysqlpp::Query query = db->Query();
    query << s_sql;
    std::vector<QwParityDB::seeds> res;
    query.storein(res);

    // Store seed_id and seed value in fSeedID and fSeed (want to store actual seed_id in those
    // cases where only the most recent was requested (fSeedID = 0))
    // There should be one and only one seed_id for each seed.
    if (res.size() == 1) {
      // Store seed_id in fSeedID (want to store actual seed_id in those
      // cases where only the most recent was requested (fSeedID = 0))
      fSeedID = res[0].seed_id;

      // Store seed value
      if (!res[0].seed.is_null) {
        fSeed = res[0].seed.data;
      } else {
        QwError << "QwBlinder::ReadSeed(): Seed value came back NULL from the database." << QwLog::endl;
        fSeedID = 0;
        fSeed = kDefaultSeed;
      }

      std::cout << "QwBlinder::ReadSeed():  Successfully read "
      << Form("the fSeed with ID %d from the database.", fSeedID)
      << std::endl;

    } else {
      // Error Condition.
      // There should be one and only one seed_id for each seed.
      fSeedID = 0;
      fSeed   = Form("ERROR:  There should be one and only one seed_id for each seed, but this had %d.",
                     res.size());
      std::cerr << "QwBlinder::ReadSeed(): "<<fSeed<<std::endl;
    }

    // Disconnect from database
    db->Disconnect();

  } else {

    //  We were unable to open the connection.
    fSeedID = 0;
    fSeed   = "ERROR:  Unable to open the connection to the database.";
    QwError << "QwBlinder::ReadSeed(): Unable to open connection to database." << QwLog::endl;
  }

  return fSeedID;
}



/**
 * Initialize the blinder parameters
 */
void QwBlinder::InitBlinders(const UInt_t seed_id)
{
  // If the blinding strategy is disabled
  if (fBlindingStrategy == kDisabled) {

      fSeed = kDefaultSeed;
      fSeedID = 0;
      fBlindingFactor = 1.0;
      fBlindingOffset = 0.0;
      QwWarning << "Blinding parameters have been disabled!"<< QwLog::endl;

  // Else blinding is enabled
  } else {

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

    QwMessage << "Blinding parameters have been calculated."<< QwLog::endl;

  }

  // Generate checksum
  TString hex_string;
  hex_string.Form("%.16llx%.16llx", *(ULong64_t*)(&fBlindingFactor), *(ULong64_t*)(&fBlindingOffset));
  fDigest = GenerateDigest(hex_string);
  fChecksum = "";
  for (size_t i = 0; i < fDigest.size(); i++)
    fChecksum += string(Form("%.2x",fDigest[i]));
};


void  QwBlinder::WriteFinalValuesToDB(QwDatabase* db)
{
  WriteChecksum(db);
  if (! CheckTestValues()) {
    QwError << "QwBlinder::WriteFinalValuesToDB():  "
            << "Blinded test values have changed; may be a problem in the analysis!!!"
            << QwLog::endl;
  }
  WriteTestValues(db);
};



/**
 * Generate a set of test values of similar size as measured asymmetries
 * @param barestring Seed string
 */
void QwBlinder::InitTestValues(const int n)
{
  // Use the stored seed to get a pseudorandom number
  Int_t finalseed = UsePseudorandom(fSeed);

  fTestValues.clear();
  fBlindTestValues.clear();
  fUnBlindTestValues.clear();

  // For each test case
  for (int i = 0; i < n; i++) {

    // Generate a pseudorandom number
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
    fTestValues.push_back(tempval);
    BlindValue(tempval);
    fBlindTestValues.push_back(tempval);
    UnBlindValue(tempval);
    fUnBlindTestValues.push_back(tempval);
  }

  QwMessage << "QwBlinder::InitTestValues(): A total of " << fTestValues.size()
            << " test values have been calculated successfully." << QwLog::endl;
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

  std::vector<UChar_t> digest = GenerateDigest(barestring);
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
  for (size_t i = 0; i < fTestValues.size(); i++)
    {
      string s_sql = s_sql_pre;

      // test_number
      sprintf(s_number, "%d", (int) i);
      s_sql += string(s_number);
      s_sql += ", ";

      // test_value
      sprintf(s_number, "%9g", fBlindTestValues[i]);
      s_sql += s_number;
      s_sql += ")";

      // Execute SQL
      db->Connect();
      mysqlpp::Query query = db->Query();
      query <<s_sql;
      query.execute();
      db->Disconnect();
    }
}


/*!--------------------------------------------------------------
 *  This routines checks to see if the stored fBlindTestValues[i]
 *  match a recomputed blinded test value.  The values are cast
 *  into floats, and their difference must be less than a change
 *  of the least-significant-bit of fBlindTestValues[i].
 *--------------------------------------------------------------*/

Bool_t QwBlinder::CheckTestValues()
{
  Bool_t status = kTRUE;

  double epsilon = std::numeric_limits<double>::epsilon();
  for (size_t i = 0; i < fTestValues.size(); i++) {

    /// First test: compare a blinded value with a second computation
    double checkval = fTestValues[i];
    BlindValue(checkval);

    double test1 = fBlindTestValues[i];
    double test2 = checkval;
    if ((test1 - test2) <= -epsilon || (test1 - test2) >= epsilon) {
      QwError << "QwBlinder::CheckTestValues():  Blinded test value "
              << i
              << " does not agree with reblinded test value, "
              << "with a difference of "
              << (test1 - test2) << "." << QwLog::endl;
      status = kFALSE;
    }

    /// Second test: compare the unblinded value with the original value
    test1 = fUnBlindTestValues[i];
    test2 = fTestValues[i];
    if ((test1 - test2) <= -epsilon || (test1 - test2) >= epsilon) {
      QwError << "QwBlinder::CheckTestValues():  Unblinded test value "
              << i
              << " does not agree with original test value, "
              << "with a difference of "
              << (test1 - test2) << "." << QwLog::endl;
      status = kFALSE;
    }
  }
  return status;
};


/**
 * Generate an MD5 digest of the blinding parameters
 * @param input Input string
 * @return MD5 digest of the input string
 */
std::vector<UChar_t> QwBlinder::GenerateDigest(const TString& input) const
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
  for (size_t i = 0; i < fTestValues.size(); i++) {
    QwMessage << std::setw(8)  << i
              << std::setw(16) << Form("% 9g ppb",fTestValues[i]*1e9)
              << std::setw(16) << Form(" [CENSORED]")
            //<< std::setw(16) << Form("% 9g ppb",fBlindTestValues[i]*1e9)
              << std::setw(16) << Form("% 9g ppb",fUnBlindTestValues[i]*1e9)
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
  for (size_t i = 0; i < fTestValues.size(); i++) {
    bf_test_row.bf_test_id = 0;
    bf_test_row.analysis_id = analysis_id;
    bf_test_row.test_number = i;
    bf_test_row.test_value  = fBlindTestValues[i];
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

void QwBlinder::SetTargetBlindability(QwBlinder::EQwBlinderStatus status){
  fTargetBlindability = status;
  if (fTargetBlindability_firstread == QwBlinder::kIndeterminate
      && fTargetBlindability != QwBlinder::kIndeterminate)
    fTargetBlindability_firstread = fTargetBlindability;
};

QwBlinder::EQwBlinderStatus QwBlinder::CheckBlindability(){
  EQwBlinderStatus status = QwBlinder::kBlindableFail;
  if (fBlindingStrategy == kDisabled) {
    status = QwBlinder::kNotBlindable;
  } else if (fTargetBlindability == QwBlinder::kIndeterminate) {
    QwError  << "QwBlinder::CheckBlindability:  The target blindability is not determined.  "
	     << "Fail this event." << QwLog::endl;
    status = QwBlinder::kBlindableFail;
  } else if (fTargetBlindability!=fTargetBlindability_firstread) {
    QwError << "QwBlinder::CheckBlindability:  The target blindability has changed.  "
	    << "Fail this event." << QwLog::endl;
    status = QwBlinder::kBlindableFail;
  } else if (fTargetBlindability==kNotBlindable) {
    //  This isn't a blindable target, so don't do anything.
    status = QwBlinder::kNotBlindable;
  } else if (fTargetBlindability==kBlindable && fBeamIsPresent) {
    //  This is a blindable target and the beam is sufficent.
    status = QwBlinder::kBlindable;
  } else if (fTargetBlindability==kBlindable && ! fBeamIsPresent) {
    //  This is a blindable target but there is insufficent beam present
    status = QwBlinder::kBlindableFail;
  } else {
    QwError << "QwBlinder::CheckBlindability:  The event blindability is unclear.  "
	     << "Fail this event." << QwLog::endl;
    status = QwBlinder::kBlindableFail;
  }
  //
  fBlinderIsOkay = (status != QwBlinder::kBlindableFail);

  return status;
}
