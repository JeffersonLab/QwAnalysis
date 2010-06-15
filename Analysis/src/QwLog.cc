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

// Reset the end-of-line flags
static Bool_t QwLogScreenAtNewLine = kTRUE;
static Bool_t QwLogFileAtNewLine = kTRUE;

// Reset the color flag
bool QwLog::fUseColor = true;

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

  QwLogScreenAtNewLine = kTRUE;
  QwLogFileAtNewLine = kTRUE;
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
 * Defines configuration options for QwDatabase class using QwOptions
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
  options->AddOptions()("QwLog.color", po::value<bool>()->default_value(true),
                "colored screen output");
  options->AddOptions()("QwLog.logfile", po::value<string>(), "log file");
  options->AddOptions()("QwLog.loglevel-file", po::value<int>()->default_value(4),
                "log level for file output");
  options->AddOptions()("QwLog.loglevel-screen", po::value<int>()->default_value(2),
                "log level for screen output");
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
QwLog& QwLog::operator()(QwLogLevel level)
{
  fLogLevel = level;

  if (fScreen && fLogLevel <= fScreenThreshold) {
    if (QwLogScreenAtNewLine) {
      // Put something at the beginning of a new line
      switch (level) {
      case kError:
        if (fUseColor)
          *(fScreen) << QwColor(Qw::kRed) << "Error: ";
        else
          *(fScreen) << "Error: ";
        break;
      case kWarning:
        if (fUseColor)
          *(fScreen) << QwColor(Qw::kRed) << "Warning: " << QwColor(Qw::kNormal);
        else
          *(fScreen) << "Warning: ";
        break;
      default:
        break;
      }
    }
    QwLogScreenAtNewLine = kFALSE;
  }

  if (fFile && fLogLevel <= fFileThreshold) {
    if (QwLogFileAtNewLine) {
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
      QwLogFileAtNewLine = kFALSE;
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
    if (fUseColor)
      *(gQwLog.fScreen) << QwColor(Qw::kNormal) << std::endl;
    else
      *(gQwLog.fScreen) << std::endl;
    QwLogScreenAtNewLine = kTRUE;
  }
  if (gQwLog.fFile && gQwLog.fLogLevel <= gQwLog.fFileThreshold) {
    *(gQwLog.fFile) << std::endl;
    QwLogFileAtNewLine = kTRUE;
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
