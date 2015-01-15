
#include "simplex.h"

int Simplex::
operator== (const Simplex & sigma2) const
{ 
  return rep_ == sigma2.rep_; 
}

int Simplex::
operator!= (const Simplex & sigma2) const
{ 
  return rep_ != sigma2.rep_;
}

ostream & 
operator<< (ostream & s, Simplex & sigma) 
{ 
  s << "[" << sigma.getIndex() << "," << sigma.getDim() << "]"; 
  return s; 
}

//----------------------------------------------------------------------
