// boxes.cpp
// Afra Zomorodian
// January 13, 1999
//
// Copyright 1996-99. Raindrop Geomagic, Inc.
// All Rights Reserved.
//
// This is unpublished proprietary source code of Raindrop Geomagic, Inc.;
// the contents of this file may not be disclosed to third parties, copied
// or duplicated in any form, in whole or in part, without the prior written
// permission of Raindrop Geomagic, Inc.
//
// This copyright notices may not be removed from this file at any time.
//
// RESTRICTED RIGHTS LEGEND:
// Use, duplication or disclosure by the Government is subject to restrictions
// as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
// and Computer Software clause at DFARS 252.227-7013, and/or in similar or
// successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished-
// rights reserved under the Copyright Laws of the United States.
//
//                 Raindrop Geomagic, Inc
//                 206 N. Randolph Street, Suite 520
//                 Champaign IL 61820-3898
//                 Phone: 217-239-2551
//                 Fax:   217-239-2556
//                 Email: inquiry@geomagic.com
//
//

/* Libraries */
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "boxes.h"

/* Exported Functions */

boxesT *CreateBoxesT(int numBoxes)
{
  boxesT *boxes;
  int dim;

  boxes = new boxesT;
  boxes->boxes = new boxT[numBoxes+2];
  boxes->numBoxes = numBoxes+2;

  /* Special boxes 
   * As numbering boxes is for order only, start at -1
   */
  boxes->boxes[0].num = -1;
  boxes->boxes[numBoxes+1].num = numBoxes;
  for(dim = 0; dim < kNumDim; dim++) {
    boxes->boxes[0].ints[dim].lo = boxes->boxes[0].ints[dim].hi = INT_MIN;
    boxes->boxes[numBoxes+1].ints[dim].lo = 
      boxes->boxes[numBoxes+1].ints[dim].hi = INT_MAX;
  }
  return boxes;
}

void FreeBoxes(boxesT *boxes)
{
  delete [] boxes->boxes;
  delete boxes;
}

/* BoxCompare
 *
 * For sorting purposes.
 */
int BoxCompare (const void *box1, const void *box2)
{
  if(IsLoLessLo((boxT *)box1, (boxT *)box2, 0))
    return -1;
  else
    return 1;
  return 0; /* to stop wailing of wall */
}
