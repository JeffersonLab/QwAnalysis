#ifndef __QWROOTFILE__
#define __QWROOTFILE__

// System headers
#include <typeinfo>

// ROOT headers
#include <TFile.h>
#include <TTree.h>
#include <TPRegexp.h>

// Qweak headers
#include "QwOptions.h"
#include "QwMapFile.h"


// If one defines more than this number of words in the full ntuple,
// the results are going to get very very crazy.
#define BRANCH_VECTOR_MAX_SIZE 6000


/**
 *  \class QwRootTree
 *  \ingroup QwAnalysis
 *  \brief A wrapper class for a ROOT tree
 *
 * This class provides the functionality to write to ROOT trees using a vector
 * of doubles.  The vector is part of this object, as well as a pointer to the
 * tree that contains the branches.  One ROOT tree can have multiple QwRootTree
 * objects, for example in tracking mode both parity and tracking detectors
 * can be stored in the same tree.
 */
class QwRootTree {

  public:

    /// Constructor with name, and description
    QwRootTree(const std::string& name, const std::string& desc)
    : fName(""),fDesc(""),fType("type undefined"),fCurrentEvent(0) {
      // Construct tree
      ConstructNewTree(name, desc);
    }

    /// Constructor with existing tree
    QwRootTree(const QwRootTree* tree)
    : fName(""),fDesc(""),fType("type undefined"),fCurrentEvent(0) {
      QwMessage << "Existing tree: " << tree->GetName() << ", " << tree->GetDesc() << QwLog::endl;
      fName = tree->GetName();
      fDesc = tree->GetDesc();
      fTree = tree->fTree;
    }

    /// Constructor with name, description, and object
    template < class T >
    QwRootTree(const std::string& name, const std::string& desc, T& detectors)
    : fName(""),fDesc(""),fType("type undefined"),fCurrentEvent(0) {
      // Construct tree
      ConstructNewTree(name, desc);

      // Construct branches and vector
      ConstructBranchAndVector(detectors);
    }

    /// Constructor with existing tree, and object
    template < class T >
    QwRootTree(const QwRootTree* tree, T& detectors)
    : fName(""),fDesc(""),fType("type undefined"),fCurrentEvent(0) {
      QwMessage << "Existing tree: " << tree->GetName() << ", " << tree->GetDesc() << QwLog::endl;
      fName = tree->GetName();
      fDesc = tree->GetDesc();
      fTree = tree->fTree;

      // Construct branches and vector
      ConstructBranchAndVector(detectors);
    }


    /// Construct the tree
    void ConstructNewTree(const std::string& name, const std::string& desc) {
      QwMessage << "New tree: " << name << ", " << desc << QwLog::endl;
      fName = name;
      fDesc = desc;
      fTree = new TTree(name.c_str(), desc.c_str());
    }

    /// Construct the branches and vector for generic objects
    template < class T >
    void ConstructBranchAndVector(T& detectors) {
      // Reserve space for the branch vector
      fVector.reserve(BRANCH_VECTOR_MAX_SIZE);
      // Associate branches with vector
      TString dummystr = "";
      detectors.ConstructBranchAndVector(fTree, dummystr, fVector);

      // Store type of object
      fType = typeid(detectors).name();

      // Check memory reservation
      if (fVector.size() > BRANCH_VECTOR_MAX_SIZE) {
        QwError << "The branch vector is too large: " << fVector.size() << " leaves!  "
                << "The maximum size is " << BRANCH_VECTOR_MAX_SIZE << "."
                << QwLog::endl;
        exit(-1);
      }
    }

    /// Fill the branches for generic objects
    template < class T >
    void FillTreeBranches(const T& detectors) {
      if (typeid(detectors).name() == fType) {
        // Fill the branch vector
        detectors.FillTreeVector(fVector);
      } else {
        QwError << "Attempting to fill tree vector for type " << fType << " with "
                << "object of type " << typeid(detectors).name() << QwLog::endl;
        exit(-1);
      }
    }

    /// Fill the tree
    Int_t Fill() {
      // Tree prescaling
      if (fNumEventsCycle > 0) {
        fCurrentEvent++;
        fCurrentEvent %= fNumEventsCycle;
        if (fCurrentEvent > fNumEventsToSave)
          return 0;
      }
      // Fill the tree
      Int_t retval = fTree->Fill();
      // Check for errors
      if (retval < 0) {
        QwError << "Writing tree failed!  Check disk space or quota." << QwLog::endl;
        exit(retval);
      }
      return retval;
    }


    /// Print the tree name and description
    void Print() const {
      QwMessage << GetName() << ", " << GetType() << QwLog::endl;
    }

    /// Get the tree pointer for low level operations
    TTree* GetTree() const { return fTree; };


  friend class QwRootFile;

  private:

    /// Tree pointer
    TTree* fTree;
    /// Vector of leaves
    std::vector<Double_t> fVector;

    /// Name, description, and type
    std::string fName;
    std::string fDesc;
    std::string fType;

    /// Get the object type
    std::string GetType() const { return fType; };
    /// Get the name of the tree
    std::string GetName() const { return fName; };
    /// Get the description of the tree
    std::string GetDesc() const { return fDesc; };


    /// Tree prescaling parameters
    UInt_t fCurrentEvent;
    UInt_t fNumEventsToSave;
    UInt_t fNumEventsToSkip;
    UInt_t fNumEventsCycle;

    /// Set tree prescaling parameters
    void SetPrescaling(UInt_t num_to_save, UInt_t num_to_skip) {
      fNumEventsToSave = num_to_save;
      fNumEventsToSkip = num_to_skip;
      fNumEventsCycle = fNumEventsToSave + fNumEventsToSkip;
    }


    /// Maximum tree size, autoflush and autosave
    Long64_t fMaxTreeSize;
    Long64_t fAutoFlush;
    Long64_t fAutoSave;

    /// Set maximum tree size
    void SetMaxTreeSize(Long64_t maxsize = 1900000000) {
      fMaxTreeSize = maxsize;
      if (fTree) fTree->SetMaxTreeSize(maxsize);
    }

    /// Set autoflush size
    void SetAutoFlush(Long64_t autof = 30000000) {
      fAutoFlush = autof;
      #if ROOT_VERSION_CODE >= ROOT_VERSION(5,26,00)
        if (fTree) fTree->SetAutoFlush(autof);
      #endif
    }

    /// Set autosave size
    void SetAutoSave(Long64_t autos = 300000000) {
      fAutoSave = autos;
      if (fTree) fTree->SetAutoSave(autos);
    }

    //Set circular buffer size for the memory resident tree
    void SetCircular(Long64_t buff = 100000) {
      if (fTree) fTree->SetCircular(buff);
    }
};



/**
 *  \class QwRootFile
 *  \ingroup QwAnalysis
 *  \brief A wrapper class for a ROOT file or memory mapped file
 *
 * This class functions as a wrapper around a ROOT TFile or a TMapFile.  The
 * common inheritance of both is only TObject, so there is a lot that we have
 * to wrap (rather than inherit).  Theoretically you could have both a TFile
 * and a TMapFile represented by an object of this class at the same time, but
 * that is untested.
 *
 * The functionality of writing to the file is done by templated functions.
 * The objects that are passed to these functions have to provide the following
 * functions:
 * <ul>
 * <li>ConstructHistograms, FillHistograms, DeleteHistograms
 * <li>ConstructBranchAndVector, FillTreeVector
 * </ul>
 *
 * The class keeps track of the registered tree names, and the types of objects
 * that have branches constructed in those trees (via QwRootTree).  In most
 * cases it should be possible to just call FillTreeBranches with only the object,
 * although in rare cases this could be ambiguous.
 *
 * The proper way to register a tree is by either calling ConstructTreeBranches
 * of NewTree first.  Then FillTreeBranches will fill the vector, and FillTree
 * will actually fill the tree.  FillTree should be called only once.
 */
class QwRootFile {

  public:

    /// \brief Constructor with run label
    QwRootFile(const TString& run_label);
    /// \brief Destructor
    virtual ~QwRootFile();


    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);


    /// Is the ROOT file active?
    Bool_t IsRootFile() const { return (fRootFile); };
    /// Is the map file active?
    Bool_t IsMapFile()  const { return (fMapFile); };


    /// \brief Construct the tree branches of a generic object
    template < class T >
    void ConstructTreeBranches(const std::string& name, const std::string& desc, T& detectors);
    /// \brief Fill the tree branches of a generic object by tree name
    template < class T >
    void FillTreeBranches(const std::string& name, const T& detectors);
    /// \brief Fill the tree branches of a generic object by type only
    template < class T >
    void FillTreeBranches(const T& detectors);


    /// \brief Construct the histograms of a generic object
    template < class T >
    void ConstructHistograms(const std::string& name, T& detectors);
    /// Fill histograms of the subsystem array
    template < class T >
    void FillHistograms(T& detectors) {
      // Update regularly
      static Int_t update_count = 0;
      update_count++;
      if (update_count % fUpdateInterval == 0) Update();
      if (! HasDirByType(detectors)) return;
      // Fill histograms
      detectors.FillHistograms();

    };
    /// Delete histograms of the subsystem array
    template < class T >
    void DeleteHistograms(T& detectors) {
      if (! HasDirByType(detectors)) return;
      // Delete histograms
      detectors.DeleteHistograms();
    }


    /// Create a new tree with name and description
    void NewTree(const std::string& name, const std::string& desc) {
      this->cd();
      if (! HasTreeByName(name)) {
        fTreeByName[name].push_back(new QwRootTree(name,desc));
      } else {
        fTreeByName[name].push_back(new QwRootTree(fTreeByName[name].front()));
      }
      fTreeByType["type undefined"].push_back(name);
    }

    /// Get the tree with name
    TTree* GetTree(const std::string& name) {
      if (! HasTreeByName(name)) return 0;
      else return fTreeByName[name].front()->GetTree();
    }

    /// Fill the tree with name
    Int_t FillTree(const std::string& name) {
      if (! HasTreeByName(name)) return 0;
      else return fTreeByName[name].front()->Fill();
    }

    /// Fill all registered trees
    Int_t FillTrees() {
      // Loop over all registered tree names
      Int_t retval = 0;
      std::map< const std::string, std::vector<QwRootTree*> >::iterator iter;
      for (iter = fTreeByName.begin(); iter != fTreeByName.end(); iter++) {
        retval += iter->second.front()->Fill();
      }
      return retval;
    }

    /// Print registered trees
    void PrintTrees() const {
      QwMessage << "Trees: " << QwLog::endl;
      // Loop over all registered tree names
      std::map< const std::string, std::vector<QwRootTree*> >::const_iterator iter;
      for (iter = fTreeByName.begin(); iter != fTreeByName.end(); iter++) {
        QwMessage << iter->first << ": " << iter->second.size()
                  << " objects registered" << QwLog::endl;
        // Loop over all registered objects for this tree
        std::vector<QwRootTree*>::const_iterator tree;
        for (tree = iter->second.begin(); tree != iter->second.end(); tree++) {
          (*tree)->Print();
        }
      }
    }
    /// Print registered histogram directories
    void PrintDirs() const {
      QwMessage << "Dirs: " << QwLog::endl;
      // Loop ove rall registered directories
      std::map< const std::string, TDirectory* >::const_iterator iter;
      for (iter = fDirsByName.begin(); iter != fDirsByName.end(); iter++) {
        QwMessage << iter->first << QwLog::endl;
      }
    }


    /// Write any object to the ROOT file (only valid for TFile)
    template < class T >
    Int_t WriteObject(const T* obj, const char* name, Option_t* option = "", Int_t bufsize = 0) {
      Int_t retval = 0;
      // TMapFile has no suport for WriteObject
      if (fRootFile) retval = fRootFile->WriteObject(obj,name,option,bufsize);
      return retval;
    }


    // Wrapped functionality
    void Update() { if (fMapFile) fMapFile->Update(); } // not for TFile
    void Close()  { if (fMapFile) fMapFile->Close();  if (fRootFile) fRootFile->Close(); }
    void Print()  { if (fMapFile) fMapFile->Print();  if (fRootFile) fRootFile->Print(); }
    void ls()     { if (fMapFile) fMapFile->ls();     if (fRootFile) fRootFile->ls(); }

    // Wrapped functionality
    Bool_t cd(const char* path = 0) {
      Bool_t status = kTRUE;
      if (fMapFile)  status &= fMapFile->cd(path);
      if (fRootFile) status &= fRootFile->cd(path);
      return status;
    }

    // Wrapped functionality
    TDirectory* mkdir(const char* name, const char* title = "") {
      // TMapFile has no suport for mkdir
      if (fRootFile) return fRootFile->mkdir(name, title);
      else return 0;
    }

    // Wrapped functionality
    Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0) {
      Int_t retval = 0;
      // TMapFile has no suport for Write
      if (fRootFile) retval = fRootFile->Write(name, option, bufsize);
      return retval;
    }


  private:

    /// Private default constructor
    QwRootFile() { };


    /// ROOT file
    TFile* fRootFile;

    /// ROOT file stem
    TString fRootFileStem;

    /// Map file
    QwMapFile* fMapFile;
    Bool_t fEnableMapFile;
    Int_t fUpdateInterval;
    Int_t fAutoFlush;
    Int_t fAutoSave;


  private:

    /// List of excluded trees
    std::vector< TPRegexp > fDisabledTrees;
    std::vector< TPRegexp > fDisabledHistos;

    /// Add regexp to list of disabled trees names
    void DisableTree(const TString& regexp) {
      fDisabledTrees.push_back(regexp);
    }
    /// Does this tree name match a disabled tree name?
    bool IsTreeDisabled(const std::string& name) {
      for (size_t i = 0; i < fDisabledTrees.size(); i++)
        if (fDisabledTrees.at(i).Match(name)) return true;
      return false;
    }
    /// Add regexp to list of disabled histogram directories
    void DisableHisto(const TString& regexp) {
      fDisabledHistos.push_back(regexp);
    }
    /// Does this histogram directory match a disabled histogram directory?
    bool IsHistoDisabled(const std::string& name) {
      for (size_t i = 0; i < fDisabledHistos.size(); i++)
        if (fDisabledHistos.at(i).Match(name)) return true;
      return false;
    }


  private:

    /// Tree names and types
    std::map< const std::string, std::vector<QwRootTree*> > fTreeByName;
    std::map< const std::string, std::vector<std::string> > fTreeByType;

    /// Is a tree registered for this name
    bool HasTreeByName(const std::string& name) {
      if (fTreeByName.count(name) == 0) return false;
      else return true;
    }
     /// Is a tree registered for this type
    template < class T >
    bool HasTreeByType(const T& object) {
      std::string type = typeid(object).name();
      if (fTreeByType.count(type) == 0) return false;
      else return true;
    }

    /// Directories
    std::map< const std::string, TDirectory* > fDirsByName;
    std::map< const std::string, std::vector<std::string> > fDirsByType;

    /// Is a tree registered for this name
    bool HasDirByName(const std::string& name) {
      if (fDirsByName.count(name) == 0) return false;
      else return true;
    }
    /// Is a directory registered for this type
    template < class T >
    bool HasDirByType(const T& object) {
      std::string type = typeid(object).name();
      if (fDirsByType.count(type) == 0) return false;
      else return true;
    }


  private:

    /// Prescaling of events written to tree
    UInt_t fNumEventsCycle;
    UInt_t fNumEventsToSkip;
    UInt_t fNumEventsToSave;
    UInt_t fCircularBufferSize;
    UInt_t fCurrentEvent;

    /// Maximum tree size
    static const Long64_t kMaxTreeSize;
};



/**
 * Construct the tree branches of a generic object
 * @param detectors Subsystem array
 */
template < class T >
void QwRootFile::ConstructTreeBranches(
        const std::string& name,
        const std::string& desc,
        T& detectors)
{
  // Return if we do not want this tree information
  if (IsTreeDisabled(name)) return;

  // If the tree does not exist yet, create it
  if (fTreeByName.count(name) == 0) {

    // Go to top level directory
    this->cd();

    // Add the branches to the list of trees by name
    fTreeByName[name].push_back(new QwRootTree(name, desc, detectors));

    // Settings only relevant for new trees
    fTreeByName[name].back()->SetPrescaling(fNumEventsToSave, fNumEventsToSkip);
    fTreeByName[name].back()->SetMaxTreeSize(kMaxTreeSize);
    #if ROOT_VERSION_CODE >= ROOT_VERSION(5,26,00)
      fTreeByName[name].back()->SetAutoFlush(fAutoFlush);
    #endif
    fTreeByName[name].back()->SetAutoSave(fAutoSave);

    if (fEnableMapFile && fCircularBufferSize>0)
      fTreeByName[name].back()->SetCircular(fCircularBufferSize);

  } else {

    // Add the branches to the list of trees by name
    fTreeByName[name].push_back(new QwRootTree(fTreeByName[name].front(), detectors));
  }

  // Add to the list of trees by type
  std::string type = typeid(detectors).name();
  fTreeByType[type].push_back(name);
}


/**
 * Fill the tree branches of a generic object by name
 * @param detectors Subsystem array
 */
template < class T >
void QwRootFile::FillTreeBranches(
        const std::string& name,
        const T& detectors)
{
  // If this name has no registered trees
  if (! HasTreeByName(name)) return;
  // If this type has no registered trees
  if (! HasTreeByType(detectors)) return;

  // Get the type of the object
  std::string type = typeid(detectors).name();

  // Fill the tree with the correct type
  for (size_t tree = 0; tree < fTreeByName[name].size(); tree++) {
    if (fTreeByName[name].at(tree)->GetType() == type) {
      fTreeByName[name].at(tree)->FillTreeBranches(detectors);
    }
  }
}


/**
 * Fill the tree branches of a generic object by type only
 * @param detectors Subsystem array
 */
template < class T >
void QwRootFile::FillTreeBranches(
        const T& detectors)
{
  // If this type has no registered trees
  if (! HasTreeByType(detectors)) return;

  // Get the type of the object
  std::string type = typeid(detectors).name();

  // Fill the tree with the correct type
  for (size_t name = 0; name < fTreeByType[type].size(); name++) {
    FillTreeBranches(fTreeByType[type].at(name), detectors);
  }
}


/**
 * Construct the histogram of a generic object
 * @param detectors Subsystem array
 */
template < class T >
void QwRootFile::ConstructHistograms(const std::string& name, T& detectors)
{
  // Return if we do not want this histogram information
  if (IsHistoDisabled(name)) return;

  // Create the histograms in a directory
  if (fRootFile) {
    std::string type = typeid(detectors).name();
    fDirsByName[name] = fRootFile->GetDirectory("/")->mkdir(name.c_str());
    fDirsByType[type].push_back(name);
    detectors.ConstructHistograms(fDirsByName[name]);
  }

  // No support for directories in a map file
  if (fMapFile) {
    std::string type = typeid(detectors).name();
    fDirsByName[name] = fMapFile->GetDirectory()->mkdir(name.c_str());
    fDirsByType[type].push_back(name);
    //detectors.ConstructHistograms(fDirsByName[name]);
    detectors.ConstructHistograms();
  }
}


#endif // __QWROOTFILE__
