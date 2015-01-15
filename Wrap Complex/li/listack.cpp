#include "listack.h"

void LI_Stack::
pushtop (void)
{
  push (top ());
}

/*--------------------------------------------------------------------------*/

void LI_Stack::
plus (void)
     /* Stack addition:
        stack[..., (top-1), top]  -->  stack[..., (top-1) + top]. */
{
  Assert (size() > 1);
  (*this)[size() - 1] += (*this)[size()];
  pop();
}

/*--------------------------------------------------------------------------*/

void LI_Stack::
minus (void)
     /* Stack subtraction:
        stack[..., (top-1), top]  -->  stack[..., (top-1) - top]. */
{
  Assert (size() > 1);
  (*this)[size() - 1] -= (*this)[size()];
  pop();
}

/*--------------------------------------------------------------------------*/

void LI_Stack::
times (void)
     /* Stack multiplication:
        stack[..., (top-1), top]  -->  stack[..., (top-1) * top]. */
{
  Assert (size() > 1);
  (*this)[size() - 1] *= (*this)[size()];
  pop();
}

/*--------------------------------------------------------------------------*/

void LI_Stack::
ipower (int p)
     /* Uses the Lia stack to compute top^p:
        stack[..., top] --> stack[..., top ^ p] with p >= 0. */
{
  Assert (!empty());
  if (p == 0)
    /* (very) special case: stack[..., top] --> stack[..., 1] */
    top() = LI(1);
  else if (p == 1)
    { /* (normal) bottom of recursion: stack[..., top] --> stack[..., top] */
      return;
    }
  else if (Odd (p))
    {
      pushtop ();       /* --> stack[..., top, top]           */
      ipower (p - 1);   /* --> stack[..., top, top ^ (p - 1)] */
      times();  /* --> stack[..., top ^ p]            */
    }
  else
    { /* make time complexity logarithmic in p by squaring:             */
      ipower (p / 2);   /* --> stack[..., top ^ (p/2)]              */
      pushtop ();       /* --> stack[..., top ^ (p/2), top ^ (p/2)] */
      times();          /* --> stack[..., top ^ p]                  */
    }
}

/*--------------------------------------------------------------------------*/

void LI_Stack::
negtop (void)
{
  top().negate();
}
