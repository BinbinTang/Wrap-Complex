/* basic/math2.c --- Substitutes for missing math funcs PLUS some int math. */

/*--------------------------------------------------------------------------*/

#include "basic.h"

#define Odd(v) (v & 1)

/*--------------------------------------------------------------------------*/

int basic_ipower (int x, int y)
     /* Returns (int) x "to the power of" y, assuming y >= 0.
        Time complexity: O (log_2 (y)).
        NOTE: You better make sure that abs (x^y) < MAXINT. */
{
  int aux;
  if (y == 0)
    return (1);
  else if (Odd (y))
    return (x * basic_ipower (x, y - 1));
  else
    {
      aux = basic_ipower (x, y / 2);
      return (aux * aux);
    }
}

/*--------------------------------------------------------------------------*/

double log2 (double x)
{
  return (log (x) / log (2.0));
}

double exp2 (double x)
{
  return (exp (x * log (2.0)));
}

double exp10 (double x)
{
  return (exp (x * log (10.0)));
}



/*--------------------------------------------------------------------------*/

#if defined (is_ANSI_C)

double cbrt (double x)
{
  static double one_third = 1.0 / 3.0;
  return (pow (x, one_third));
}

#endif
