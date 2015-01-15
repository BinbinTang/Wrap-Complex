//
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
// 

#ifndef __RGI_TRITYPE_H__
#define __RGI_TRITYPE_H__

enum TriangleType { 
  ALL_TRI = 0, VOLUME_TRI = 1, SURFACE_TRI = 2,
  CLOSED_TRI = 3,
  COMPLETE_TRI = 4,
  PRINCIPAL_TRI = 5,
  INTERIOR_TRI = 6,
  CLOSED_AND_INTERIOR_TRI = 7,
  COMPLETE_AND_INTERIOR_TRI = 8,
  MAX_TRI_TYPE = 9
};

// This is a small hack for now...  This file should be renamed...

// Gui animate modes will be either "Animate" or "Step".  The
// following specify whether to use these methods or to override
// them.
enum AnimateMode 
		{ BeginAnimate,
      Default,           // Use the default mode
		  Animate,          // 
		  Pause,
      EndAnimate
		};


#endif // __TRITYPE_H__
