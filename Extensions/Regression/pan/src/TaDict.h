/********************************************************************
* TaDict.h
* CAUTION: DON'T CHANGE THIS FILE. THIS FILE IS AUTOMATICALLY GENERATED
*          FROM HEADER FILES LISTED IN G__setup_cpp_environmentXXX().
*          CHANGE THOSE HEADER FILES AND REGENERATE THIS FILE.
********************************************************************/
#ifdef __CINT__
#error TaDict.h/C is only for compilation. Abort cint.
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define G__ANSIHEADER
#define G__DICTIONARY
#define G__PRIVATE_GVALUE
#include "G__ci.h"
extern "C" {
extern void G__cpp_setup_tagtableTaDict();
extern void G__cpp_setup_inheritanceTaDict();
extern void G__cpp_setup_typetableTaDict();
extern void G__cpp_setup_memvarTaDict();
extern void G__cpp_setup_globalTaDict();
extern void G__cpp_setup_memfuncTaDict();
extern void G__cpp_setup_funcTaDict();
extern void G__set_cpp_environmentTaDict();
}


#include "TObject.h"
#include "TMemberInspector.h"
#include "TaBlind.hh"
#include "TaFileName.hh"
#include "TaIResultsFile.hh"
#include "TaLabelledQuantity.hh"
#include "TaOResultsFile.hh"
#include "TaStatistics.hh"
#include "TaString.hh"
#include "TaAnalysisManager.hh"
#include "TaDebugAna.hh"
#include "TaDevice.hh"
#include "TaDataBase.hh"
#include "TaCutInterval.hh"
#include "TaCutList.hh"
#include "VaEvent.hh"
#include "TaEvent.hh"
#include "TaSimEvent.hh"
#include "TaEpics.hh"
#include "TaMultiplet.hh"
#include "TaPairFromOctet.hh"
#include "TaPairFromPair.hh"
#include "TaPairFromQuad.hh"
#include "TaRun.hh"
#include "TaStandardAna.hh"
#include "TaFeedbackAna.hh"
#include "VaAnalysis.hh"
#include "VaPair.hh"
#include "TaADCCalib.hh"
#include "PanTypes.hh"
#include "DevTypes.hh"
#include <algorithm>
namespace std { }
using namespace std;

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__TaDictLN_TClass;
extern G__linked_taginfo G__TaDictLN_TBuffer;
extern G__linked_taginfo G__TaDictLN_TMemberInspector;
extern G__linked_taginfo G__TaDictLN_TObject;
extern G__linked_taginfo G__TaDictLN_vectorlEshortcOallocatorlEshortgRsPgR;
extern G__linked_taginfo G__TaDictLN_vectorlEshortcOallocatorlEshortgRsPgRcLcLiterator;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEshortcOallocatorlEshortgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_vectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgR;
extern G__linked_taginfo G__TaDictLN_vectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgRcLcLiterator;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_vectorlEunsignedsPintcOallocatorlEunsignedsPintgRsPgR;
extern G__linked_taginfo G__TaDictLN_vectorlEunsignedsPintcOallocatorlEunsignedsPintgRsPgRcLcLiterator;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEunsignedsPintcOallocatorlEunsignedsPintgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_vectorlEfloatcOallocatorlEfloatgRsPgR;
extern G__linked_taginfo G__TaDictLN_vectorlEfloatcOallocatorlEfloatgRsPgRcLcLiterator;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEfloatcOallocatorlEfloatgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_vectorlEdoublecOallocatorlEdoublegRsPgR;
extern G__linked_taginfo G__TaDictLN_vectorlEdoublecOallocatorlEdoublegRsPgRcLcLiterator;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEdoublecOallocatorlEdoublegRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_ios_base;
extern G__linked_taginfo G__TaDictLN_basic_istreamlEcharcOchar_traitslEchargRsPgR;
extern G__linked_taginfo G__TaDictLN_basic_ioslEcharcOchar_traitslEchargRsPgR;
extern G__linked_taginfo G__TaDictLN_basic_ostreamlEcharcOchar_traitslEchargRsPgR;
extern G__linked_taginfo G__TaDictLN_basic_ifstreamlEcharcOchar_traitslEchargRsPgR;
extern G__linked_taginfo G__TaDictLN_basic_ofstreamlEcharcOchar_traitslEchargRsPgR;
extern G__linked_taginfo G__TaDictLN_string;
extern G__linked_taginfo G__TaDictLN_stringcLcLiterator;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEstringcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_vectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgR;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEROOTcLcLTSchemaHelpercOallocatorlEROOTcLcLTSchemaHelpergRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_TString;
extern G__linked_taginfo G__TaDictLN_TaBlind;
extern G__linked_taginfo G__TaDictLN_EHelicity;
extern G__linked_taginfo G__TaDictLN_EPairSynch;
extern G__linked_taginfo G__TaDictLN_EMultipletSynch;
extern G__linked_taginfo G__TaDictLN_EPairType;
extern G__linked_taginfo G__TaDictLN_EFeedbackType;
extern G__linked_taginfo G__TaDictLN_TaFileName;
extern G__linked_taginfo G__TaDictLN_TaIResultsFile;
extern G__linked_taginfo G__TaDictLN_TaLabelledQuantity;
extern G__linked_taginfo G__TaDictLN_TaOResultsFile;
extern G__linked_taginfo G__TaDictLN_TaStatistics;
extern G__linked_taginfo G__TaDictLN_pairlEdoublecOdoublegR;
extern G__linked_taginfo G__TaDictLN_vectorlEpairlEdoublecOdoublegRcOallocatorlEpairlEdoublecOdoublegRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEpairlEdoublecOdoublegRcOallocatorlEpairlEdoublecOdoublegRsPgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_TaString;
extern G__linked_taginfo G__TaDictLN_vectorlEstringcOallocatorlEstringgRsPgR;
extern G__linked_taginfo G__TaDictLN_vectorlEstringcOallocatorlEstringgRsPgRcLcLiterator;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEstringcOallocatorlEstringgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_TaRun;
extern G__linked_taginfo G__TaDictLN_VaAnalysis;
extern G__linked_taginfo G__TaDictLN_TaAnalysisManager;
extern G__linked_taginfo G__TaDictLN_iteratorlEbidirectional_iterator_tagcOTObjectmUcOlongcOconstsPTObjectmUmUcOconstsPTObjectmUaNgR;
extern G__linked_taginfo G__TaDictLN_TTree;
extern G__linked_taginfo G__TaDictLN_TFile;
extern G__linked_taginfo G__TaDictLN_maplEstringcOintcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOintgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_maplEintcOintcOlesslEintgRcOallocatorlEpairlEconstsPintcOintgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_maplEstringcOTObjArraymUcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOTObjArraymUgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_TaCutList;
extern G__linked_taginfo G__TaDictLN_TaMultiplet;
extern G__linked_taginfo G__TaDictLN_VaEvent;
extern G__linked_taginfo G__TaDictLN_VaPair;
extern G__linked_taginfo G__TaDictLN_vectorlEintcOallocatorlEintgRsPgR;
extern G__linked_taginfo G__TaDictLN_vectorlEintcOallocatorlEintgRsPgRcLcLiterator;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEintcOallocatorlEintgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_vectorlEAnaListcOallocatorlEAnaListgRsPgR;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEAnaListcOallocatorlEAnaListgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_dequelEVaEventcOallocatorlEVaEventgRsPgR;
extern G__linked_taginfo G__TaDictLN_dequelEVaPairmUcOallocatorlEVaPairmUgRsPgR;
extern G__linked_taginfo G__TaDictLN_TaStandardAna;
extern G__linked_taginfo G__TaDictLN_TaDebugAna;
extern G__linked_taginfo G__TaDictLN_TDatime;
extern G__linked_taginfo G__TaDictLN_TArrayC;
extern G__linked_taginfo G__TaDictLN_dtype;
extern G__linked_taginfo G__TaDictLN_TaRootRep;
extern G__linked_taginfo G__TaDictLN_TaKeyMap;
extern G__linked_taginfo G__TaDictLN_maplEstringcOstringcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOstringgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_TaDataBase;
extern G__linked_taginfo G__TaDictLN_maplEintcOvectorlEintcOallocatorlEintgRsPgRcOlesslEintgRcOallocatorlEpairlEconstsPintcOvectorlEintcOallocatorlEintgRsPgRsPgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_vectorlETaStringcOallocatorlETaStringgRsPgR;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlETaStringcOallocatorlETaStringgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_vectorlEdtypemUcOallocatorlEdtypemUgRsPgR;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEdtypemUcOallocatorlEdtypemUgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_multimaplEstringcOvectorlEdtypemUcOallocatorlEdtypemUgRsPgRcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOvectorlEdtypemUcOallocatorlEdtypemUgRsPgRsPgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_maplEstringcOTaKeyMapcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOTaKeyMapgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_maplEstringcOTaKeyMapcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOTaKeyMapgRsPgRsPgRcLcLiterator;
extern G__linked_taginfo G__TaDictLN_maplEstringcOboolcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOboolgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_TVectorTlEfloatgR;
extern G__linked_taginfo G__TaDictLN_TVectorTlEdoublegR;
extern G__linked_taginfo G__TaDictLN_TH1F;
extern G__linked_taginfo G__TaDictLN_TaDevice;
extern G__linked_taginfo G__TaDictLN_vectorlETaLabelledQuantitycOallocatorlETaLabelledQuantitygRsPgR;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlETaLabelledQuantitycOallocatorlETaLabelledQuantitygRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_vectorlETaKeyMapcOallocatorlETaKeyMapgRsPgR;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlETaKeyMapcOallocatorlETaKeyMapgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_TaCutInterval;
extern G__linked_taginfo G__TaDictLN_pairlEunsignedsPintcOintgR;
extern G__linked_taginfo G__TaDictLN_vectorlEpairlEunsignedsPintcOintgRcOallocatorlEpairlEunsignedsPintcOintgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEpairlEunsignedsPintcOintgRcOallocatorlEpairlEunsignedsPintcOintgRsPgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_vectorlETaCutIntervalcOallocatorlETaCutIntervalgRsPgR;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlETaCutIntervalcOallocatorlETaCutIntervalgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_listlEunsignedsPintcOallocatorlEunsignedsPintgRsPgR;
extern G__linked_taginfo G__TaDictLN_TaEvent;
extern G__linked_taginfo G__TaDictLN_TRandom;
extern G__linked_taginfo G__TaDictLN_TaSimEvent;
extern G__linked_taginfo G__TaDictLN_vectorlEEpicsChancOallocatorlEEpicsChangRsPgR;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEEpicsChancOallocatorlEEpicsChangRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_maplEstringcOvectorlEEpicsChancOallocatorlEEpicsChangRsPgRcOlesslEstringgRcOallocatorlEpairlEconstsPstringcOvectorlEEpicsChancOallocatorlEEpicsChangRsPgRsPgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_TaEpics;
extern G__linked_taginfo G__TaDictLN_dequelEVaPaircOallocatorlEVaPairgRsPgR;
extern G__linked_taginfo G__TaDictLN_vectorlEdequelEVaPaircOallocatorlEVaPairgRsPgRcOallocatorlEdequelEVaPaircOallocatorlEVaPairgRsPgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEdequelEVaPaircOallocatorlEVaPairgRsPgRcOallocatorlEdequelEVaPaircOallocatorlEVaPairgRsPgRsPgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_vectorlEVaPaircOallocatorlEVaPairgRsPgR;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEVaPaircOallocatorlEVaPairgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_TaPairFromOctet;
extern G__linked_taginfo G__TaDictLN_TaPairFromPair;
extern G__linked_taginfo G__TaDictLN_TaPairFromQuad;
extern G__linked_taginfo G__TaDictLN_THaCodaData;
extern G__linked_taginfo G__TaDictLN_TaFeedbackAna;
extern G__linked_taginfo G__TaDictLN_TGraphErrors;
extern G__linked_taginfo G__TaDictLN_TaADCCalib;
extern G__linked_taginfo G__TaDictLN_vectorlEvectorlEintcOallocatorlEintgRsPgRcOallocatorlEvectorlEintcOallocatorlEintgRsPgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_vectorlEvectorlEintcOallocatorlEintgRsPgRcOallocatorlEvectorlEintcOallocatorlEintgRsPgRsPgRsPgRcLcLiterator;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEvectorlEintcOallocatorlEintgRsPgRcOallocatorlEvectorlEintcOallocatorlEintgRsPgRsPgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_vectorlEvectorlEdoublecOallocatorlEdoublegRsPgRcOallocatorlEvectorlEdoublecOallocatorlEdoublegRsPgRsPgRsPgR;
extern G__linked_taginfo G__TaDictLN_vectorlEvectorlEdoublecOallocatorlEdoublegRsPgRcOallocatorlEvectorlEdoublecOallocatorlEdoublegRsPgRsPgRsPgRcLcLiterator;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlEvectorlEdoublecOallocatorlEdoublegRsPgRcOallocatorlEvectorlEdoublecOallocatorlEdoublegRsPgRsPgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__TaDictLN_vectorlETStringcOallocatorlETStringgRsPgR;
extern G__linked_taginfo G__TaDictLN_vectorlETStringcOallocatorlETStringgRsPgRcLcLiterator;
extern G__linked_taginfo G__TaDictLN_reverse_iteratorlEvectorlETStringcOallocatorlETStringgRsPgRcLcLiteratorgR;

/* STUB derived class for protected member access */
typedef vector<short,allocator<short> > G__vectorlEshortcOallocatorlEshortgRsPgR;
typedef vector<unsigned short,allocator<unsigned short> > G__vectorlEunsignedsPshortcOallocatorlEunsignedsPshortgRsPgR;
typedef vector<unsigned int,allocator<unsigned int> > G__vectorlEunsignedsPintcOallocatorlEunsignedsPintgRsPgR;
typedef vector<float,allocator<float> > G__vectorlEfloatcOallocatorlEfloatgRsPgR;
typedef vector<double,allocator<double> > G__vectorlEdoublecOallocatorlEdoublegRsPgR;
typedef vector<string,allocator<string> > G__vectorlEstringcOallocatorlEstringgRsPgR;
typedef vector<int,allocator<int> > G__vectorlEintcOallocatorlEintgRsPgR;
typedef vector<vector<int,allocator<int> >,allocator<vector<int,allocator<int> > > > G__vectorlEvectorlEintcOallocatorlEintgRsPgRcOallocatorlEvectorlEintcOallocatorlEintgRsPgRsPgRsPgR;
typedef vector<vector<double,allocator<double> >,allocator<vector<double,allocator<double> > > > G__vectorlEvectorlEdoublecOallocatorlEdoublegRsPgRcOallocatorlEvectorlEdoublecOallocatorlEdoublegRsPgRsPgRsPgR;
typedef vector<TString,allocator<TString> > G__vectorlETStringcOallocatorlETStringgRsPgR;
