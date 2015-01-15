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

#ifndef __RGI_MODTRINFO_H__
#define __RGI_MODTRINFO_H__

#include <basic/rarray.h>
#include <basic/bitvector.h>
#include <basic/dumpable.h>

class ModifiedTriInfo
{
public:
  ModifiedTriInfo (int size = 20);
  ~ModifiedTriInfo (void);
  // export info functions
  Bool isAdded (FIndex f);
  Bool isDeleted (FIndex f);
  Bool isTouched (FIndex f);
  FIndex getParent (FIndex f);
  Bool isRemapped (void);
  IMap * getMap (void);
  BitVector& getAddedBV (void);
  BitVector& getDeletedBV (void);
  BitVector& getTouchedBV (void);

  // maintainence support
  void reset (void);
  void add (FIndex f, FIndex parent);
  void touch (FIndex f);
  void touch (BitVector& modBV);
  void del (FIndex f);
  void clearParent (FIndex f);
  void setMap (IMap * map);

  enum InheritType { InheritSelection, NoInheritSelection };
  void setInheritance (FIndex f, int value);
  InheritType getInheritType (FIndex f);

  friend ostream&   operator<< (ostream& s, const ModifiedTriInfo& modtri);

private:
  RArray<int> parent_;
  BitVector added_;
  BitVector deleted_;
  BitVector touched_;
  BitVector noInherit_;
  IMap * map_;
};

#include <geometry/modtrinfo.hpp>

#endif // __MODIFIED_TRI_INFO__H__
