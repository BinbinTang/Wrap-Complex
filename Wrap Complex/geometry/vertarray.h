/*
 * comp.h -- C++ Header file for the Complex class
 *
 * author -- Mike Facello
 *
 * date -- 10/24/96
 */

#ifndef __RGI_VERTARRAY_H__
#define __RGI_VERTARRAY_H__

#include <basic/dumpable.h>

class IndexStack;

/************************************************************************/

/*
 * Class VertArray
 *
 * This class is used to represent a sequence of vertices corresponding
 * to a simplex.
 *
 */

class VertArray
{
private:
  Dim dim_;
  VIndex vert_[5];

  inline void exchangeVertices_ (int i, int j);
  inline void exchangeVertices_ (int i, int j, int k);
  inline void exchangeVertices_ (int i, int j, int k, int l);

public:
  friend class DComplex;

  VertArray ();
  VertArray (VIndex ix1);
  VertArray (VIndex ix1, VIndex ix2);
  VertArray (VIndex ix1, VIndex ix2, VIndex ix3);
  VertArray (VIndex ix1, VIndex ix2, VIndex ix3, VIndex ix4);

  void setVertices (VIndex ix1);
  void setVertices (VIndex ix1, VIndex ix2);
  void setVertices (VIndex ix1, VIndex ix2, VIndex ix3);
  void setVertices (VIndex ix1, VIndex ix2, VIndex ix3, VIndex ix4);
  void setVertices (IndexStack * v);

  VIndex& getVertex (int index) 
  { 
    Assert ((1 <= index) and (index <= dim_+1));
    return vert_[index];
  }

  VIndex *getArray (void) { return (vert_ + 1); }
                                  
  Dim getDim (void) { return dim_; }

  Bool operator== (VertArray& varr2);

  friend ostream& operator<< (ostream& s, VertArray& vArray);

  Bool isPrefix (VertArray& varr2);
  void sort (void);
};


#endif // __VERTARRAY_H__
