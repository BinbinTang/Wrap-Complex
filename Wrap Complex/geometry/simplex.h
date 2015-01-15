
#ifndef __RGI_SIMPLEX_H__
#define __RGI_SIMPLEX_H__

#define NUM_BITS  32

#include "basic/index.h"
#include "basic/basic.h"
#include "geometry/vertarray.h"

//----------------------------------------------------------------------

/*
 * Class Simplex
 *
 * 
 *
 */

class Simplex
{
public:
  Simplex (void);
  Simplex (Index index, Dim dim);

  Index getIndex (void) const;
  Dim getDim (void) const;
  int getIntRep (void);
  void setIntRep (int i);
  int operator== (const Simplex & sigma2) const;
  int operator!= (const Simplex & sigma2) const;
  friend ostream & operator<< (ostream & s, Simplex & sigma);
  
private:
  unsigned int rep_;
};

inline Simplex::
Simplex (void) { }

inline Simplex::
Simplex (Index index, Dim dim) : rep_((index << 2) | (dim & 03)) { }

inline int Simplex::
getIndex (void) const { return rep_ >> 2; }

inline Dim Simplex::
getDim (void) const { return rep_ & 03; }

inline int Simplex::
getIntRep (void) { return  rep_; }

inline void Simplex::
setIntRep (int rep) { rep_ = rep; }

//----------------------------------------------------------------------

typedef IStackIter<Simplex> SArrayIter;

//----------------------------------------------------------------------

class 
SimplexArray : public IterStack <Simplex>

{
public:
  SArrayIter * iter (void) 
  { 
    return (SArrayIter *) new IStackIter<Simplex>(*this);
  }
};

//----------------------------------------------------------------------

#endif // __SIMPLEX_H__
