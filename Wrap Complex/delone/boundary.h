/*
// Copyright 1996-97. Raindrop Geomagic, Inc.
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
*/

#ifndef __RGI_BOUNDARY_H__
#define __RGI_BOUNDARY_H__

#include <iostream>
#include "delone/dcomp.h"
#include "geometry/trist.h"
#include "geometry/ksimplex.h"
#include "geometry/geomutil.h"

class DBoundary : public TristModifier
{
public:
  // Constructors
  DBoundary (DComplex * dc);
  ~DBoundary (void) {  }

  int bdEnum (SimplexCB cbf, RgiObject * obj);
  int kBdEnum (Dim k, SimplexCB cbf, RgiObject * obj);

private:
  DComplex * dc_;
  RgiObject * obj_;
	SimplexCB cbf_;
  BVTag * vertTag_;
  BVTag * edgeTag_;
	int (DBoundary::*reportSimplex_)(OrTri);

/*---------------------------------------------------------------------------*/
  int enumerateBoundary_ (void);
  int reportSimplex0_ (OrTri ef);
  int reportSimplex1_ (OrTri ef);
  int reportSimplex2_ (OrTri ef);
  int reportSimplex3_ (OrTri ef);
  int reportSimplexAll_ (OrTri ef);
};

//------------------------------------------------------------------------

#endif // __BOUNDARY_H__
