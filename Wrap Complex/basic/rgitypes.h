/*
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
*/

#ifndef __RGI_RGITYPES_H__
#define __RGI_RGITYPES_H__

  const unsigned int 
  CLOSED_COMPONENT = 0,
  HAS_BOUNDARY = 1,
  NOT_ORIENTED = 2, 
  HAS_NMEDGES = 4, 
  MULTIPLE_COMPONENTS = 8;

	enum UnitType { Inches=0, Millimeters, Feet, Centimeters,
		Meters, Kilometers, Microns, UNKNOWN, MaxUnitTypes };
  
//-------------------- History Mode for history usage -------------------------

enum HistoryMode { Inactive = 0, OneLevel = 1, MultipleLevel = 2 };

enum FloatType { Float = 0, Double = 1 };

#endif // __RGI_TYPES_H__