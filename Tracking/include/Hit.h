//
// C++ Interface: Hit
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef HIT_H
#define HIT_H

/// First declare the Hit and Det are objects, because they contain
/// pointers to each other.
class Det;

/*! \class Hit
    \brief Contains hit data and information regarding the hit detector

    Hits for individual detector elements are strung together.  They also
    have a pointer back to the detector in case more information about the
    hit is needed.  The various position values are used in multiple ways,
    and therefore are not strictly defined.

    \todo This should be replaced by the QwHit class.  Probably QwHit will
    have to be extended for this.  Possibly QwHitContainer can be used right
    away instead of going with a linked list of QwHit objects.  But first
    just QwHit, I would say. (wdc)
 */
class Hit {

  public:

    int wire;			/*!< wire ID                           */
    double Zpos;		/*!< Z position of hit                 */
    double rPos1;		/*!< rPos1 and                         */
    double rPos2;		/*!< rPos2 from level II decoding      */
    double Resolution;		/*!< resolution of this specific hit   */
    double rTrackResolution;	/*!< tracking road width around hit    */
    Det *detec;			/*!< which detector                    */
    Hit *next, *nextdet;	/*!< next hit and next hit in same detector */
    int ID;			/*!< event ID                          */
    int  used;			/*!< hit is used by a track            */
    double rResultPos;		/*!< Resulting hit position            */
    double rPos;		/*!< Position of from track finding    */

  private:

};


#endif // HITDET_H
