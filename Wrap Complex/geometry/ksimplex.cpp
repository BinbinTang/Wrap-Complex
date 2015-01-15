
#include "ksimplex.h"

int Ksimplex::
operator== (Ksimplex sigma) 
{ 
  return rep_ == sigma.rep_; 
}

ostream & 
operator<< (ostream & s, Ksimplex & sigma) 
{ 
  s << "[" << sigma.getFace() << "," << sigma.getDim() << "]"; 
  return s; 
}

//----------------------------------------------------------------------
