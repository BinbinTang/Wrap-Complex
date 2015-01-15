// boxes.h
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

#ifndef __RGI_BOXES_H__
#define __RGI_BOXES_H__

/* Constants */
#define kNumDim 3

/* Data Structures */

/* An Interval */
typedef struct{
  int lo, hi;
} intervalT;

/* A Box */
typedef struct {
  intervalT ints[kNumDim];
  int num;
} boxT;

/* Many Boxes! */
typedef struct{
  boxT *boxes;
  int numBoxes;
} boxesT;

/* Boolean Type */
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/* Macros */
#define GetLo(b,d) ((b)->ints[d].lo)
#define GetHi(b,d) ((b)->ints[d].hi)
#define GetNum(b)  ((b)->num)
#define GetBoxArray(x) ((x)->boxes+1)
#define GetPosInfBox(x) ((x)->boxes[(x)->numBoxes-1])
#define GetNumBoxes(x) ((x)->numBoxes-2)
#define SetNumBoxes(x,n) ((x)->numBoxes = (n)+2)

#define IsLoLessLo(b1, b2, d) (GetLo(b1,d) < GetLo(b2,d) || \
			                        (GetLo(b1,d) == GetLo(b2,d)  \
			                         && GetNum(b1) < GetNum(b2)))

#define IsHiLessLo(b1, b2, d) (GetHi(b1,d) <= GetLo(b2,d))

#define IsLoLessHi(b1, b2, d) (GetLo(b1,d) < GetHi(b2,d))

#define IsHiLessHi(b1, b2, d) (GetHi(b1,d) < GetHi(b2,d))

#define ContainsLoPoint(b1, b2, d) (!IsLoLessLo(b2, b1, d) && \
                                       IsLoLessHi(b2, b1, d))

#define Intersects(b1, b2, d) (!IsHiLessLo(b1, b2, d) && \
			       !IsHiLessLo(b2, b1, d))

/* CreateBoxesT
 * Usage:  boxes = CreateBoxesT(5);
 * -----------------------------------
 * Creates a boxesT structure for numBoxes boxes
 * in numDim dimensions.  (Adds 2 special boxes at infinity)
 */
boxesT *CreateBoxesT(int numBoxes);

/* FreeBoxes
 * Usage:  boxes = FreeBoxes(b);
 * ----------------------------
 * Frees the boxes.
 */
void FreeBoxes(boxesT *boxes);

/* BoxCompare */
int BoxCompare (const void *box1, const void *box2);

#endif /* _BOXES_H_ */

