/*
 * vertarray.C -- C++ Implementation file for the VertArray class
 *
 * author -- Mike Facello
 *
 * date -- 12/18/96
 */

#include "vertarray.h"

/*----------------------------------------------------------------------*/
/*------------------------- CLASS VERTARRAY ----------------------------*/
/*----------------------------------------------------------------------*/

VertArray::
VertArray ()
  : dim_(0)
    //    vert_(4)
{
}

/*----------------------------------------------------------------------*/

VertArray::
VertArray (VIndex ix1)
  : dim_(0)
  //    vert_(4)
{
  setVertices (ix1);
}

/*----------------------------------------------------------------------*/

VertArray::
VertArray (VIndex ix1, VIndex ix2)
  : dim_(1)
  //    vert_(4)
{
  setVertices (ix1, ix2);
}

/*----------------------------------------------------------------------*/

VertArray::
VertArray (VIndex ix1, VIndex ix2, VIndex ix3)
  : dim_(2)
  //    vert_(4)
{
  setVertices (ix1, ix2, ix3);
}

/*----------------------------------------------------------------------*/

VertArray::
VertArray (VIndex ix1, VIndex ix2, VIndex ix3, VIndex ix4)
  : dim_(3)
  //    vert_(4)
{
  setVertices (ix1, ix2, ix3, ix4);
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

void VertArray::
setVertices (VIndex ix1)
{
  vert_[1] = ix1;
  dim_ = 0;
}

/*----------------------------------------------------------------------*/

void VertArray::
setVertices (VIndex ix1, VIndex ix2)
{
  vert_[1] = ix1;
  vert_[2] = ix2;
  dim_ = 1;
}

/*----------------------------------------------------------------------*/

void VertArray::
setVertices (VIndex ix1, VIndex ix2, VIndex ix3)
{
  vert_[1] = ix1;
  vert_[2] = ix2;
  vert_[3] = ix3;
  dim_ = 2;
}

/*----------------------------------------------------------------------*/

void VertArray::
setVertices (VIndex ix1, VIndex ix2, VIndex ix3, VIndex ix4)
{
  vert_[1] = ix1;
  vert_[2] = ix2;
  vert_[3] = ix3;
  vert_[4] = ix4;
  dim_ = 3;
}

/*----------------------------------------------------------------------*/

void VertArray::
setVertices (IndexStack * v)
{
  dim_ = v->size () - 1;
  
  for (int d = 1; d <= v->size (); d++) 
    vert_[d] = (*v)[d];
}

/*----------------------------------------------------------------------*/

Bool VertArray::
operator== (VertArray& varr2)
{
  if (dim_ != varr2.dim_)
    return FALSE;

  for (register int i = 1; i <= dim_ + 1; i++)
    if (vert_[i] != varr2.vert_[i])
      return FALSE;

  return TRUE;
}


/*----------------------------------------------------------------------*/

ostream&
operator<< (ostream& s, VertArray& vArray)
{
  for (register int i = 1; i <= vArray.dim_; i++)
    s << (int) vArray.getVertex (i) << " ";
  s << (int) vArray.getVertex (vArray.dim_ + 1) << endl;

  return s;
}

/*----------------------------------------------------------------------*/

Bool VertArray::
isPrefix (VertArray& varr2)
{
  if (dim_ > varr2.dim_)
    return FALSE;

  register int a, b;
  a = b = 0;

  while (1) {
    if (vert_[a + 1] == varr2.vert_[b + 1]) {
      a++;
      if (a > dim_) return TRUE;
    }
    b++;
    if (b > varr2.dim_) return FALSE;
  }
}

/*----------------------------------------------------------------------*/

void VertArray::
exchangeVertices_ (int i, int j)
{
  register VIndex tmp = vert_[i];
  vert_[i] = vert_[j];
  vert_[j] = tmp;
}

/*----------------------------------------------------------------------*/

void VertArray::
exchangeVertices_ (int i, int j, int k)
{
  register VIndex tmp = vert_[i];
  vert_[i] = vert_[j];
  vert_[j] = vert_[k];
  vert_[k] = tmp;
}

/*----------------------------------------------------------------------*/

void VertArray::
exchangeVertices_ (int i, int j, int k, int l)
{
  register VIndex tmp = vert_[i];
  vert_[i] = vert_[j];
  vert_[j] = vert_[k];
  vert_[k] = vert_[l];
  vert_[l] = tmp;
}

/*----------------------------------------------------------------------*/

void VertArray::
sort ()

{
#define AssertOrder(x) 

  if (dim_ == 0) return;

  if (dim_ == 1) {
    if (vert_[1] > vert_[2]) exchangeVertices_ (2, 1);
    return;
  }

  if (vert_[1] > vert_[2]) {
    if (vert_[1] > vert_[3]) {
      if (vert_[2] > vert_[3]) { /* 3 < 2 < 1 */
	AssertOrder ((vert_[3] < vert_[2]) && (vert_[2] < vert_[1]));
	exchangeVertices_ (3, 1);
      } else { /* 2 < 3 < 1 */
	AssertOrder ((vert_[2] < vert_[3]) && (vert_[3] < vert_[1]));
	exchangeVertices_ (3, 1, 2);
      }
    } else { /* 2 < 1 < 3*/
      AssertOrder ((vert_[2] < vert_[1]) && (vert_[1] < vert_[3]));
      exchangeVertices_ (2, 1);
    }
  } else {
    if (vert_[2] > vert_[3]) {
      if (vert_[1] > vert_[3]) { /* 3 < 1 < 2*/
	AssertOrder ((vert_[3] < vert_[1]) && (vert_[1] < vert_[2]));
	exchangeVertices_ (3, 2, 1);
      } else { /* 1 < 3 < 2*/
	AssertOrder ((vert_[1] < vert_[3]) && (vert_[3] < vert_[2]));
	exchangeVertices_ (2, 3);
      }
    }
  }

  AssertOrder ((vert_[1] < vert_[2]) && (vert_[2] < vert_[3]));
  if (dim_ == 2) return;

  if (vert_[4] > vert_[2]) {
    if (vert_[4] < vert_[3]) { /* 1 < 2 < 4 < 3 */
      AssertOrder ((vert_[1] < vert_[2]) && (vert_[2] < vert_[4]) && 
		   (vert_[4] < vert_[3]));
      exchangeVertices_ (3, 4);
    }
  } else {
    if (vert_[4] > vert_[1]) { /* 1 < 4 < 2 < 3 */
      AssertOrder ((vert_[1] < vert_[4]) && (vert_[4] < vert_[2]) && 
		   (vert_[2] < vert_[3]));
      exchangeVertices_ (4, 3, 2);
    } else { /* 4 < 1 < 2 < 3 */
      AssertOrder ((vert_[4] < vert_[1]) && (vert_[1] < vert_[2]) && 
		   (vert_[2] < vert_[3]));
      exchangeVertices_ (4, 3, 2, 1);
    }
  }

  AssertOrder ((vert_[1] < vert_[2]) && (vert_[2] < vert_[3]) && 
	       (vert_[3] < vert_[4]));
#undef AssertOrder
}

/*----------------------------------------------------------------------*/

