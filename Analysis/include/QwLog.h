#ifndef QWLOG_HH
#define QWLOG_HH

#include <iostream>
#include <iomanip>

#include "QwTypes.h"

/*! \def QwError
 *  \brief Predefined log drain for errors
 */
#define QwError    gQwLog(QwLog::kError)

/*! \def QwWarning
 *  \brief Predefined log drain for warnings
 */
#define QwWarning  gQwLog(QwLog::kWarning)

/*! \def QwMessage
 *  \brief Predefined log drain for regular messages
 */
#define QwMessage  gQwLog(QwLog::kMessage)

/*! \def QwVerbose
 *  \brief Predefined log drain for verbose messages
 */
#define QwVerbose  gQwLog(QwLog::kVerbose)

/*! \def QwDebug
 *  \brief Predefined log drain for debugging output
 */
#define QwDebug    gQwLog(QwLog::kDebug)


/**
 *  \class QwLog
 *  \ingroup QwAnalysis
 *  \brief A logfile class
 *
 * This class should not be used directly. Instead one can write text to the
 * screen or a log file via the predefined log drains QwError, QwWarning,
 * QwMessage, QwVerbose and QwDebug.
 *
 * An example could be:
\verbatim
 QwMessage << "Hello World !!!" << QwLog::endl;
\endverbatim
 */
class QwLog : public std::ostream {

  public:

    //! Loglevels
    /*! enum of possible log levels */
    enum QwLogLevel {
      kError     =  0, /*!< Error loglevel */
      kWarning   =  1, /*!< Warning loglevel */
      kMessage   =  2, /*!< Message loglevel */
      kVerbose   =  3, /*!< Verbose loglevel */
      kDebug     =  4  /*!< Debug loglevel */
    };

    /*! \brief The constructor
     */
    QwLog();

    /*! \brief The destructor
     */
    ~QwLog();

    /*! \brief Initialize the log file with name 'name'
     */
    void                        InitLogfile(const char* name);

    /*! \brief Set the screen log level
     */
    void                        SetScreenThreshold(int thr);

    /*! \brief Set the file log level
     */
    void                        SetFileThreshold(int thr);

    /*! \brief Set the stream log level
     */
    QwLog&                      operator()(QwLogLevel level);

    /*! \brief Stream an object to the output stream
     */
    template <class T> QwLog&   operator<<(const T &t) {
      if (fScreen && fLogLevel <= fScreenThreshold) {
        *(fScreen) << t;
      }
      if (fFile && fLogLevel <= fFileThreshold) {
        *(fFile) << t;
      }
      return *this;
    }

    /*! \brief Pass the ios_base manipulators
     */
#if (__GNUC__ >= 3)
    QwLog&                      operator<<(std::ios_base & (*manip) (std::ios_base &));
#endif
    QwLog&                      operator<<(std::ostream & (*manip) (std::ostream &));

    /*! \brief End of the line
     */
    static std::ostream&        endl(std::ostream&);

    /*! \brief Flush the streams
     */
    QwLog&                      flush();

  private:

    /*! \brief Get the local time
     */
    const char *                GetTime();
    char                        fTimeString[128];

    //! Screen thresholds and stream
    QwLogLevel                  fScreenThreshold;
    std::ostream               *fScreen;
    //! File thresholds and stream
    QwLogLevel                  fFileThreshold;
    std::ostream               *fFile;
    //! Log level of this stream
    QwLogLevel                  fLogLevel;

};

extern QwLog gQwLog;

#endif
