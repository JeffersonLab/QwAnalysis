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
#include "QwOptions.h"

// Create the static logger object (with streams to screen and file)
QwLog gQwLog;

// Reset the end-of-line flags
static Bool_t QwLogScreenAtNewLine = kTRUE;
static Bool_t QwLogFileAtNewLine = kTRUE;

/*! The constructor initializes the screen stream and resets the file stream
 */
QwLog::QwLog()
: std::ostream(std::cerr.rdbuf())
{
  // Define some options
  gQwOptions.AddOptions()("logfile", po::value<string>(), "log file");
  gQwOptions.AddOptions()("loglevel-file", po::value<int>()->default_value(4), "log level for file output");
  gQwOptions.AddOptions()("loglevel-screen", po::value<int>()->default_value(2), "log level for screen output");

  fScreenThreshold = kMessage;
  fScreen = &std::cerr;

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


/*! Initialize the log file with name 'name'
 */
void QwLog::InitLogFile(const string name)
{
  if (fFile) {
    delete fFile;
    fFile = 0;
  }

  fFile = new std::ofstream(name.c_str(), std::ios::app | std::ios::out);
  fFileThreshold = kMessage;
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
      switch (level) {
      case kError:   *(fScreen) << "Error: "; break;
      case kWarning: *(fScreen) << "Warning: "; break;
      default: break;
      }
      QwLogScreenAtNewLine = kFALSE;
    }
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
      default: *(fFile) << "  "; break;
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
