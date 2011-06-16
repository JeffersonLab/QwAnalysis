//////////////////////////////////////////////////////////////////////////
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//        TaLinkDef.hh  
//        ^^^^^^^^^^^^^
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaLinkDef.hh,v 1.19 2009/08/06 18:36:03 rsholmes Exp $
//
//////////////////////////////////////////////////////////////////////////
//
//    Full list of classes for root dictionary generation.
//
//////////////////////////////////////////////////////////////////////////

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class TaADCCalib;
#pragma link C++ class TaAnalysisManager;
#pragma link C++ class TaDataBase;
#pragma link C++ class TaBlind;
#pragma link C++ class TaCutInterval;
#pragma link C++ class TaCutList;
#pragma link C++ class TaDebugAna;
#pragma link C++ class TaDevice;
#pragma link C++ class VaEvent;
#pragma link C++ class TaEpics;
#pragma link C++ class TaEvent;
#pragma link C++ class TaIResultsFile;
#pragma link C++ class TaSimEvent;
#pragma link C++ class TaFileName;
#pragma link C++ class TaLabelledQuantity;
#pragma link C++ class TaMultiplet;
#pragma link C++ class TaOResultsFile;
#pragma link C++ class TaPairFromOctet;
#pragma link C++ class TaPairFromPair;
#pragma link C++ class TaPairFromQuad;
#pragma link C++ class TaRootRep;
#pragma link C++ class TaRun;
#pragma link C++ class TaStandardAna;
#pragma link C++ class TaFeedbackAna;
#pragma link C++ class TaStatistics;
#pragma link C++ class TaString;
#pragma link C++ class VaAnalysis;
#pragma link C++ class VaPair;

#ifdef __MAKECINT__
#pragma link C++ class std::string;
#pragma link C++ class std::vector<short>;
#pragma link C++ class std::vector<int>;
#pragma link C++ class std::vector<unsigned short>;
#pragma link C++ class std::vector<unsigned int>;
#pragma link C++ class std::vector<float>;
#pragma link C++ class std::vector<double>;
#pragma link C++ class std::vector<std::string>;
#pragma link C++ class std::vector<std::vector<int>>;
#pragma link C++ class std::vector<std::vector<double>>;
#pragma link C++ class std::vector<TString>;
#endif

#endif

