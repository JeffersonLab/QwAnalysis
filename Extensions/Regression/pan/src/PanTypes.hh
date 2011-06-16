////////////////////////////////////////////////////////////////////////
//
// PanTypes.hh
//
// Data types for pan 
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: PanTypes.hh,v 1.10 2010/02/10 18:19:55 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////

#ifndef PAN_PanTypes
#define PAN_PanTypes

typedef UInt_t RunNumber_t;
typedef UInt_t EventNumber_t;
typedef UInt_t SlotNumber_t;
typedef Int_t ErrCode_t;
typedef UInt_t Cut_t;
enum EHelicity { RightHeli, LeftHeli, UnkHeli };
enum EPairSynch { FirstPS, SecondPS };
enum EMultipletSynch { FirstMS, OtherMS };
enum EPairType { FromPair, FromQuad, FromOctet, FromMultiplet };
enum EFeedbackType { IA, PZTX, PZTY, PITA, IAHALLC};

#endif
