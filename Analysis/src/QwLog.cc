/*!
 * \file   QwLog.cc
 * \brief  A logfile class, based on an identical class in the Hermes analyzer
 *
 * \author Wouter Deconinck
 * \date   2009-11-25
 */

#include "QwLog.h"

// System headers
#include <fstream>

// Qweak headers
#include "QwColor.h"
#include "QwOptions.h"

// Create the static logger object (with streams to screen and file)
QwLog gQwLog;

// Set the static flags
bool QwLog::fScreenAtNewLine = true;
bool QwLog::fScreenInColor = false;
bool QwLog::fFileAtNewLine = true;

// Log file open modes
const std::ios_base::openmode QwLog::kTruncate = std::ios::trunc;
const std::ios_base::openmode QwLog::kAppend = std::ios::app;

/*! The constructor initializes the screen stream and resets the file stream
 */
QwLog::QwLog()
: std::ostream(std::cout.rdbuf())
{
  fScreenThreshold = kMessage;
  fScreen = &std::cout;

  fFileThreshold = kMessage;
  fFile = 0;

  fLogLevel = kMessage;

  fUseColor = true;

  fPrintFunctionName = false;
  fPrintFunctionSignature = false;
}

/*! The destructor destroys the log file, if it was present
 */
QwLog::~QwLog()
{
  if (fFile) {
    delete fFile;
    fFile = 0;
  }
}


/**
 * Defines configuration options for logging class using QwOptions
 * functionality.
 *
 * Note: this uses a pointer as opposed to a reference, because as indicated
 * above the QwLog class cannot depend on the QwOptions class.  When using a
 * pointer we only need a forward declaration and we do not need to include
 * the header file QwOptions.h.
 *
 * @param options Options object
 */
void QwLog::DefineOptions(QwOptions* options)
{
  // Define the logging options
  options->AddOptions("Logging options")("QwLog.color",
                po::value<bool>()->default_value(true),
                "colored screen output");
  options->AddOptions("Logging options")("QwLog.logfile",
                po::value<string>(),
                "log file");
  options->AddOptions("Logging options")("QwLog.loglevel-file",
                po::value<int>()->default_value(kMessage),
                "log level for file output");
  options->AddOptions("Logging options")("QwLog.loglevel-screen",
                po::value<int>()->default_value(kMessage),
                "log level for screen output");
  options->AddOptions("Logging options")("QwLog.print-function",
                po::value<bool>()->default_value(false)->zero_tokens(),
                "print function on error or warning");
  options->AddOptions("Logging options")("QwLog.print-signature",
                po::value<bool>()->default_value(false)->zero_tokens(),
                "print signature on error or warning");
}


/**
 * Process configuration options for logging class using QwOptions
 * functionality.
 *
 * Note: this uses a pointer as opposed to a reference, because as indicated
 * above the QwLog class cannot depend on the QwOptions class.  When using a
 * pointer we only need a forward declaration and we do not need to include
 * the header file QwOptions.h.
 *
 * @param options Options object
 */
void QwLog::ProcessOptions(QwOptions* options)
{
  // Initialize log file
  if (options->HasValue("QwLog.logfile"))
    InitLogFile(options->GetValue<std::string>("QwLog.logfile"));

  // Set the logging thresholds
  SetFileThreshold(options->GetValue<int>("QwLog.loglevel-file"));
  SetScreenThreshold(options->GetValue<int>("QwLog.loglevel-screen"));

  // Set color flag
  SetScreenColor(options->GetValue<bool>("QwLog.color"));

  // Set the flags for function name and signature printing
  fPrintFunctionName      = options->GetValue<bool>("QwLog.print-function");
  fPrintFunctionSignature = options->GetValue<bool>("QwLog.print-signature");
}


/*! Initialize the log file with name 'name'
 */
void QwLog::InitLogFile(const string name, const std::ios_base::openmode mode)
{
  if (fFile) {
    delete fFile;
    fFile = 0;
  }

  std::ios_base::openmode flags = std::ios::out | mode;
  fFile = new std::ofstream(name.c_str(), flags);
  fFileThreshold = kMessage;
}

/*! Set the screen color mode
 */
void QwLog::SetScreenColor(bool flag)
{
  fUseColor = flag;
}

/*! Set the screen log level
 */
void QwLog::SetScreenThreshold(int thr)
{
  fScreenThreshold = QwLogLevel(thr);
}

/*! Set the file log level
 */
void QwLog::SetFileThreshold(int thr)
{
  fFileThreshold = QwLogLevel(thr);
}

/*! Set the stream log level
 */
QwLog& QwLog::operator()(
  const QwLogLevel level,
  const std::string func_sig,
  const std::string func_name)
{
  fLogLevel = level;

  if (fScreen && fLogLevel <= fScreenThreshold) {
    if (fScreenAtNewLine) {
      // Put something at the beginning of a new line
      switch (level) {
      case kError:
        if (fUseColor) {
          *(fScreen) << QwColor(Qw::kRed);
          fScreenInColor = true;
        }
        if (fPrintFunctionSignature)
          *(fScreen) << "Error (in " << func_sig << "): ";
        else if (fPrintFunctionName)
          *(fScreen) << "Error (in " << func_name << "): ";
        else
          *(fScreen) << "Error: ";
        break;
      case kWarning:
        if (fUseColor) {
          *(fScreen) << QwColor(Qw::kRed);
          fScreenInColor = true;
        }
        if (fPrintFunctionSignature)
          *(fScreen) << "Warning (in " << func_sig << "): ";
        else if (fPrintFunctionName)
          *(fScreen) << "Warning (in " << func_name << "): ";
        else
          *(fScreen) << "Warning: ";
        if (fUseColor) {
          *(fScreen) << QwColor(Qw::kNormal);
          fScreenInColor = false;
        }
        break;
      default:
        fScreenInColor = false;
        break;
      }
    }
    fScreenAtNewLine = false;
  }

  if (fFile && fLogLevel <= fFileThreshold) {
    if (fFileAtNewLine) {
      *(fFile) << GetTime();
      switch (level) {
      case kError:   *(fFile) << " EE"; break;
      case kWarning: *(fFile) << " WW"; break;
      case kMessage: *(fFile) << " MM"; break;
      case kVerbose: *(fFile) << " VV"; break;
      case kDebug:   *(fFile) << " DD"; break;
      default: *(fFile) << "   "; break;
      }
      *(fFile) << " - ";
      fFileAtNewLine = false;
    }
  }

  return *this;
}

#if (__GNUC__ >= 3)
/*!
 */
QwLog& QwLog::operator<<(std::ios_base& (*manip) (std::ios_base&))
{
  if (fScreen && (fLogLevel <= fScreenThreshold || fLogLevel <= fFileThreshold) ) {
    *(fScreen) << manip;
  }

// The following solution leads to double calls to QwLog::endl
//   if (fScreen && fLogLevel <= fScreenThreshold) {
//     *(fScreen) << manip;
//   }
//   if (fFile && fLogLevel <= fFileThreshold) {
//     *(fFile) << manip;
//   }

  return *this;
}
#endif

/*!
 */
QwLog& QwLog::operator<<(std::ostream& (*manip) (std::ostream&))
{
  if (fScreen && (fLogLevel <= fScreenThreshold || fLogLevel <= fFileThreshold) ) {
    *(fScreen) << manip;
  }

// The following solution leads to double calls to QwLog::endl
//   if (fScreen && fLogLevel <= fScreenThreshold) {
//     *(fScreen) << manip;
//   }
//   if (fFile && fLogLevel <= fFileThreshold) {
//     *(fFile) << manip;
//   }

  return *this;
}

/*! End of the line
 */
std::ostream& QwLog::endl(std::ostream& strm)
{
  if (gQwLog.fScreen && gQwLog.fLogLevel <= gQwLog.fScreenThreshold) {
    if (fScreenInColor)
      *(gQwLog.fScreen) << QwColor(Qw::kNormal) << std::endl;
    else
      *(gQwLog.fScreen) << std::endl;
    fScreenAtNewLine = true;
    fScreenInColor = false;
  }
  if (gQwLog.fFile && gQwLog.fLogLevel <= gQwLog.fFileThreshold) {
    *(gQwLog.fFile) << std::endl;
    fFileAtNewLine = true;
  }

  return strm;
}

/*! Flush the streams
 */
QwLog& QwLog::flush()
{
  if (fScreen) {
    *(fScreen) << std::flush;
  }
  if (fFile) {
    *(fFile) << std::flush;
  }
  return *this;
}

/*! Get the local time
 */
const char* QwLog::GetTime()
{
  time_t now = time(0);
  if (now >= 0) {
    struct tm *currentTime = localtime(&now);
    strftime(fTimeString, 128, "%Y-%m-%d, %T", currentTime);
    return fTimeString;
  } else {
    return "";
  }
}
