
#ifndef __RGI_KSIMPLEX_H__
#define __RGI_KSIMPLEX_H__

#define NUM_BITS  32

#include <iostream>
#include "basic/basic.h"
#include "geometry/trist.h"
#include "geometry/vertarray.h"
#include "geometry/simplex.h"

//----------------------------------------------------------------------

class Ksimplex
{
public:
  Ksimplex (void);
  Ksimplex (OrTri face, Dim dim);

  OrTri getFace (void) const;
  Dim getDim (void) const;
  int getIntRep (void);
  void setIntRep (int i);
  int operator== (Ksimplex sigma2);
  friend ostream & operator<< (ostream & s, Ksimplex & sigma);
  
private:
  unsigned int rep_;
};

inline Ksimplex::
Ksimplex (void) { }

inline Ksimplex::
Ksimplex (OrTri face, Dim dim) : rep_((face.getIntRep() << 2) | (dim & 03)) { }

inline OrTri Ksimplex::
getFace (void) const { OrTri ef; ef.setIntRep(rep_ >> 2); return ef; }

inline Dim Ksimplex::
getDim (void) const { return rep_ & 03; }

inline int Ksimplex::
getIntRep (void) { return rep_; }

inline void Ksimplex::
setIntRep (int rep) { rep_ = rep; }


//----------------------------------------------------------------------

typedef IStackIter<Ksimplex> KSArrayIter;

//----------------------------------------------------------------------

class 
KsimplexArray : public IterStack <Ksimplex>

{
public:
  SArrayIter * iter (void) 
  {
    return (SArrayIter *) new IStackIter<Ksimplex>(*this);
  }
};

//----------------------------------------------------------------------

#endif // __KSIMPLEX_H__
