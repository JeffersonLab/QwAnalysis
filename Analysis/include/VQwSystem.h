#ifndef __VQwSystem_h__
#define __VQwSystem_h__

// ROOT headers
#include <TROOT.h>
#include <TNamed.h>

/**
 *  \defgroup QwThreadSafe QwThreadSafe
 *  \brief A framework for thread-safe analysis
 *
 * To take advantage of the advances in CPU architectures, where multiple threads
 * of a program can be executed simultaneously to reduce the impact of IO waits,
 * we implement a structure of thread-safe analysis objects.  All specific
 * analyses (such as the tracking and parity analyses) will need to implement
 * an analysis object that inherits from a virtual analyzer.  Similarly, we will
 * provide a thread-safe dataserver.
 *
 *
 * \section QwThreadSafe_ClassStructure Class structure
 *
 * These two main types of objects (roughly similar to the two types of threads
 * that will be running) are shown in the following graph.  Multiple analysis
 * objects can be run simultaneously in different threads.
 * \dot
 * digraph system {
 *   node [shape=round, fontname=Helvetica, fontsize=10];
 *   VQwSystem [ label="VQwSystem" URL="\ref VQwSystem"];
 *   VQwDataserver [ label="VQwDataserver" URL="\ref VQwDataserver"];
 *   VQwAnalyzer1 [ label="VQwAnalyzer[1]" URL="\ref VQwAnalyzer"];
 *   VQwAnalyzer2 [ label="VQwAnalyzer[2]" URL="\ref VQwAnalyzer"];
 *   VQwAnalyzer3 [ label="VQwAnalyzer[3]" URL="\ref VQwAnalyzer"];
 *   VQwSystem -> VQwAnalyzer1;
 *   VQwSystem -> VQwAnalyzer2;
 *   VQwSystem -> VQwAnalyzer3;
 *   VQwSystem -> VQwDataserver;
 * }
 * \enddot
 *
 * The specific analysis object will be different depending on the analysis
 * needs (e.g. the tracking analysis probably does not need any information on
 * the helicity pattern, similarly the parity analysis does not need to do any
 * tracking).  Therefore, the actual analyzer objects inherit in the following
 * way:
 * \dot
 * digraph analyzer {
 *   node [shape=round, fontname=Helvetica, fontsize=10];
 *   VQwAnalyzer [ label="VQwAnalyzer" URL="\ref VQwAnalyzer"];
 *   QwParityAnalyzer [ label="QwAnalyzerParity" URL="\ref QwParityAnalyzer"];
 *   QwTrackingAnalyzer [ label="QwTrackingAnalyzer" URL="\ref QwTrackingAnalyzer"];
 *   VQwAnalyzer -> QwParityAnalyzer;
 *   VQwAnalyzer -> QwTrackingAnalyzer;
 * }
 * \enddot
 *
 * \note In this structure, the data server is somewhat like a shell around the
 * QwEventBuffer.  The specific class structure prevents us from implementing
 * a dataserver that serves MC events.  Personally, I (wdc) think that there is
 * not too much to be gained by putting the decoding inside the event buffer.
 * It seems that having multiple dataserver objects is difficult in a CODA file
 * scenario (although a ring buffer could probably support that).  If there is
 * only one dataserver, than we want it to be limited in scope because it will
 * be running in 'exclusive' mode.
 *
 *
 * \section QwThreadSafe_ExecutionStructure Execution structure
 *
 * There are two classes that implement the structure of the analysis: QwRoot
 * and QwControl.  The main routine creates a QwControl object (which is just
 * an interactive ROOT prompt with added functionality), and then instructs the
 * QwControl object to start an analyzer and dataserver.  After this, it hands
 * off control to the QwControl object.
 *
 * The QwControl object interprets an initial set of command line options and
 * sets some other general settings (search paths, etc).  Then it spins of the
 * main thread QwRoot, while QwControl stays active in the background for ROOT
 * commands.
 *
 * The QwRoot object implements the event loops.  There are two available loops:
 * QwRoot::OfflineLoop() and QwRoot::OnlineLoop().  Depending on the settings
 * parsed in QwControl, one of these loops is activated.
 *
 * In the inner part of the event loop, QwRoot requests a new event from the
 * dataserver, tells the dataserver to write the CODA stream to the analyzer,
 * and then instructs the analyzer to analyze this data in a new thread which
 * calls ProcessEvent().  While the analysis of this event is ongoing, the loop
 * continues and the next event is read from the CODA stream.  In multithread
 * mode (as opposed to single thread mode), a different thread can start with
 * the analysis of the new event while the previous event is still being
 * analyzed.
 *
 * One of the main concerns with thread programming is that one thread should
 * not modify variables in memory on which the other thread depends.  In this
 * model the only overlap in analyzer memory is when writing to the histograms
 * and trees.  It seems that ROOT histogram filling is thread-safe, but for all
 * safety we should lock and unlock the thread around the histogram filling.
 *
 * \note One problem in the current layout of multithreaded analysis is that
 * we have to ensure that subsystems don't create their own set of histograms
 * if a previous thread has already created them.  This might require changes
 * to the subsystem calls ConstructHistograms().  In particular, we should test
 * whether a histogram is already defined with that name, and if so, take that
 * pointer.
 */

/**
 *  \class VQwSystem
 *  \ingroup QwThreadSafe
 *  \brief Virtual base class for thread-safe analysis and dataserver objects
 *
 * This object is the basis for all thread-safe analysis and dataserver objects.
 * It does not contain any smarts, but sets up the inheritance structure, which
 * is then used in InheritsFrom() calls.  This ensures that only objects that
 * explicitly inherit from VQwSystem are used as thread-safe analyzers.
 */
class VQwSystem : public TNamed {

  public:
    /// Default constructor
    VQwSystem (const char* name = 0): TNamed (name, "Qweak-ROOT interface") { };
    /// Destructor
    virtual ~VQwSystem() { };

  ClassDef(VQwSystem,1);
  // Note: corresponding ClassImp in QwRoot.h

}; // class VQwSystem

#endif // __VQwSystem_h__
