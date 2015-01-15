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

inline ModifiedTriInfo::
ModifiedTriInfo (int size)
: parent_(size), 
  added_(size),
  deleted_(size),
  touched_(size),
  noInherit_(size),
  map_(NULL)
{

}

inline ModifiedTriInfo::
~ModifiedTriInfo (void)
{
  delete map_;
}

//-------------------------------------------------------

inline Bool ModifiedTriInfo::
isTouched (FIndex f)
{
  return touched_.test(f);
}

inline Bool ModifiedTriInfo::
isAdded (FIndex f)
{
  return added_.test(f);
}

inline Bool ModifiedTriInfo::
isDeleted (FIndex f)
{
  return deleted_.test(f);
}

//-------------------------------------------------------

inline FIndex ModifiedTriInfo::
getParent (FIndex f)
{
  if (added_.test(f))
	return parent_[f];
  
  return deleted_.test(f) ? 0 : f;
}

//-------------------------------------------------------

inline Bool ModifiedTriInfo::
isRemapped (void)
{
  return (map_ != NULL);
}

inline IMap * ModifiedTriInfo::
getMap (void)
{
  return map_;
}

inline void ModifiedTriInfo::
setMap (IMap * map)
{
  delete map_;
  map_ = map;
}

inline void ModifiedTriInfo::
setInheritance (FIndex f, int value)
{
  value ? noInherit_.clear (f) : noInherit_.set (f);
}

inline ModifiedTriInfo::InheritType ModifiedTriInfo::
getInheritType (FIndex f)
{
  return (noInherit_.test (f) ? NoInheritSelection : InheritSelection);
}

//-------------------------------------------------------

inline BitVector& ModifiedTriInfo::
getAddedBV (void)
{
  return added_;
}

inline BitVector& ModifiedTriInfo::
getDeletedBV (void)
{
  return deleted_;
}

inline BitVector& ModifiedTriInfo::
getTouchedBV (void)
{
  return touched_;
}


//-------------------------------------------------------

inline void ModifiedTriInfo::
reset (void)
{
  delete map_;
  map_ = NULL;
  added_.reset();
  deleted_.reset();
  touched_.reset();
  noInherit_.reset();
}

//-------------------------------------------------------

inline void ModifiedTriInfo::
add (FIndex f, FIndex parent)
{
  touched_.clear(f);
  added_.set(f);
  if (added_.test (parent))
  {
	  parent = parent_[parent];
	  parent_[f] = parent;
  }
  else
	  parent_[f] = parent;
}

inline void ModifiedTriInfo::
touch (FIndex f)
{
  touched_.set(f);
}

inline void ModifiedTriInfo::
touch (BitVector& modBV)
{
  touched_.or_op(modBV);
}

inline void ModifiedTriInfo::
del (FIndex f)
{
  // A triangle is marked as deleted only if it
  // was an original triangle deleted during the
  // current operation.  A triangle created during 
  // the current operation and then deleted is not
  // marked as deleted.
  
  if (!added_.test(f))
  	deleted_.set(f);
  added_.clear(f);
  touched_.clear(f);
}

inline void ModifiedTriInfo::
clearParent (FIndex f)
{
  if (added_.test(f))
	  parent_[f] = 0;
}

//-------------------------------------------------------

inline ostream& operator<< (ostream& s, ModifiedTriInfo& modtri)
{
	s << "-----------------------------------------------------" << endl << endl;
	s << "\nMod Tri Info:" << endl << endl;
	s << "Deleted triangles: " << endl << modtri.getDeletedBV () << endl;
	s << "Inserted triangles: " << endl;
	
	BitVectorIter iter (modtri.getAddedBV (), TRUE);
	for (iter.first (); ! iter.isDone(); iter.next ())
	{
		FIndex ix = iter.curItem ();
		FIndex parent = modtri.getParent (ix);
		s << "\t" << ix;
		if (parent)
			s << ": " << parent;
		s << endl;
	}
	s << endl;

	if (modtri.isRemapped ())
		s << "Indices were remapped." << endl << endl;
	s << "-----------------------------------------------------" << endl << endl;

	return s;
}
