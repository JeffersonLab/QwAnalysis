#include "QwEvent.h"
//ClassImp(QwEvent);


#include "QwTrackingTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwVertex.h"


QwEvent::QwEvent()
{
}


QwEvent::~QwEvent()
{
  for (int i = 0; i < kNumPackages; i++) {
    for (int j = 0; j < kNumRegions; j++) {
      for (int k = 0; k < kNumTypes; k++) {
        // Delete all those partial tracks
//         QwPartialTrack* pt = parttrack[i][j][k];
//         while (pt) {
//           QwPartialTrack* pt_next = pt->next;
//           //delete pt; // TODO disabled because it fails with wrong size
//           pt = pt_next;
//         }

        // Delete all those treelines
        for (int l = 0; l < kNumDirections; l++) {
          QwTrackingTreeLine* tl = treeline[i][j][k][l];
          while (tl) {
            QwTrackingTreeLine* tl_next = tl->next;
            delete tl;
            tl = tl_next;
          }
        } // end of loop over directions

      } // end of loop over types
    } // end of loop over regions
  } // end of loop over packages
}
