#ifndef __RGI_ORINDEX_H__
#define __RGI_ORINDEX_H__

#include <basic/dumpable.h>


class OrIndex
{
public:
  OrIndex (Index pathId = 0, Bool reversed = 0);
  OrIndex (const OrIndex& other);
  Bool version (void) const;
  Index index (void) const;
  OrIndex sym (void) const;
  unsigned getIntRep (void) const;
  void setIntRep (unsigned rep);
  Bool operator!= (const OrIndex& other) const;
  Bool operator== (const OrIndex& other) const;
  Bool binInit (BinInput& bi);
  void binDump (BinOutput& bo);
protected:
  unsigned rep_;
};


// Oriented path
class OrPath : public OrIndex
{
public:
  OrPath (Index pathId = 0, Bool reversed = 0);
  OrPath (const OrIndex& other);
	OrPath sym (void) const;
};

// Oriented position
class OrPos : public OrIndex
{
public:
  OrPos (Index pos = 0, Bool reversed = 0);
  OrPos (const OrIndex& other);
	OrPos sym (void) const;
};

typedef Bool (RgiObject::*OrIndexCB) (OrIndex);
typedef Bool (RgiObject::*OrPathCB) (OrPath);
typedef Bool (RgiObject::*OrPosCB) (OrPos);

#include <basic/orindex.hpp>

#endif // __ORINDEX_H__