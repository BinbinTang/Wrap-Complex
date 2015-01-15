/* lia/lia.c  ---  Kernel of Lia (Long-integer arithmetic) Library */

/*---------------------------------------------------------------------------*/

const char lia__version[] = "";
const char lia__authors[] = "";
#ifdef __DEBUG__
const char lia__compile[] = "";
#endif

/* const */ int lia_magic = 130862003;

/*--------------------------------------------------------------------------*/

#include "lia.h"
#include "base.h"
     
/*--------------------------------------------------------------------------*/

static int  lia_ple  (const Lia_obj long1, const Lia_obj long2);
static void lia_padd (Lia_obj longi,
                      /*const */ Lia_obj long1, /* const */ Lia_obj long2);
static void lia_psub (Lia_obj longi, const Lia_obj long1, const Lia_obj long2);

/*--------------------------------------------------------------------------*/

/* AN IMPORTANT NOTE regarding output parameters (usually called 'longi') */

/* Lia objects do not store any information about their actually
 * allocated size.  Rather, it is the responsibility of the caller to
 * make sure that the passed 'longi' objects are allocated properly
 * (i.e., the "proper longi[]" assumption).  There is, however, a
 * reasonable amount of error catching (at run-time) and debugging
 * (only when compiled with -D__DEBUG__) available, but under the further
 * assumption that the caller tells Lia the size of the output objects
 * via the lia_length() procedure.
 */

/*--------------------------------------------------------------------------*/

void lia_assign (Lia_obj longi, const Lia_obj long2)
     /* Output: longi. */
     /* Long-integer assignment: longi := long2.
        NOTE: longi := "zero" if long2 == LIA_NULL.
        Assumes proper longi[], but at least long1[0..1]. */
{
  if (long2 == LIA_NULL)
    {
      Assert (lia_common.last >= 1);
      longi[0] = 2L;
      longi[1] = 0L;
    }
  else
    {
      int i, ind2 = last_of (long2);
      Assert (lia_common.last >= ind2);
      for (i=0; i<=ind2; i++)
        longi[i] = long2[i];
    }
}

/*--------------------------------------------------------------------------*/

void lia_chs (Lia_obj longi)
     /* Input/Output: longi. */
     /* Change of sign:  longi := -longi, if longi != 0. */
{
  if (is_nonzero (longi))
    chs (longi);
}

/*--------------------------------------------------------------------------*/

Lia_ptr lia_neg (Lia_obj longi)
     /* Input/Output: longi. */
     /* Like lia_chs() except that it returns a pointer to longi.
        USE WITH CARE, eg, like in "lia_push (lia_neg (longi));"
        which would push -longi on the stack. */
{
  if (is_nonzero (longi))
    chs (longi);
  return (longi);
}

/*--------------------------------------------------------------------------*/

int lia_sign (const Lia_obj longi)
     /* Returns the sign (longi) in {-1, 0, +1}. */
{
  if ((longi[0] == 2) and (longi[1] == 0))
    return (0);
  else if (is_negative (longi))
    return (-1);
  else
    return (1);
}
   
/*--------------------------------------------------------------------------*/

int lia_eq (const Lia_obj long1, const Lia_obj long2)
     /* Returns TRUE iff long1 == long2. */
{
  int result = FALSE;
  if (long1[0] == long2[0])
    {
      int i = last_of (long1);
      while ((i > 0) and (long1[i] == long2[i]))
        i --;
      result = (i <= 0);
    }
  return (result);
}

/*--------------------------------------------------------------------------*/

int lia_le (const Lia_obj long1, const Lia_obj long2)
     /* Returns TRIE iff long1 < long2. */
{
  int n_flag1 = is_negative (long1), n_flag2 = is_negative (long2);
  return (   ((n_flag1 != n_flag2) and (n_flag1 == 1))
          or (    (n_flag1 == n_flag2)
              and (   ((n_flag1 == 0) and lia_ple (long1, long2))
                   or ((n_flag1 == 1) and lia_ple (long2, long1)))));
}

/*--------------------------------------------------------------------------*/

static int lia_ple (const Lia_obj long1, const Lia_obj long2)
     /* Returns TRUE iff abs (long1) < abs (long2). */
{
  int stop_flag, result = FALSE;
  int ind1 = last_of (long1), ind2 = last_of (long2);
  if (ind1 != ind2)
    result = (ind1 < ind2);
  else
    {
      do
        {
          stop_flag = TRUE;
          if (long1[ind1] != long2[ind1])
            result = (long1[ind1] < long2[ind1]);
          else
            {
               ind1 --;
               stop_flag = FALSE;
            }
        } while (!((ind1 < 0) || stop_flag));
    }
  if (ind1 < 0)
    result = FALSE;
  return (result);
}

/*--------------------------------------------------------------------------*/

int lia_leq (const Lia_obj long1, const Lia_obj long2)
     /* Returns TRUE iff long1 <= long2. */
{
  return (lia_le (long1, long2) or lia_eq (long1, long2));
}

/*--------------------------------------------------------------------------*/

void lia_add (Lia_obj longi, const Lia_obj long1, const Lia_obj long2)
     /* Output: longi. */
     /* Long-integer addition: longi := long1 + long2.
        Assumes proper longi[], but at least longi[0..1]. */
{
  int n_flag1 = is_negative (long1), n_flag2 = is_negative (long2);
  if (n_flag1 == n_flag2)
    {
      lia_padd (longi, long1, long2);
      longi[0] += n_flag1;
    }
  else if (n_flag1 == 0)
    {
      if (lia_ple (long1, long2))
        {
          lia_psub (longi, long2, long1);
          longi[0] ++;
        }
      else
        lia_psub (longi, long1, long2);
    }
  else if (lia_ple (long1, long2))
    lia_psub (longi, long2, long1);
  else
    {
      lia_psub (longi, long1, long2);
      if (is_nonzero (longi))
        longi[0] ++;
    }
  Assert (lia_common.max >= 1);
  if ((longi[1] == 0) and (longi[0] == 3))
    /* "-0" correction;
       don't really know why/whether this is still needed, 
       but it seems to work as it is */
    longi[0] = 2;
}

/*--------------------------------------------------------------------------*/

void lia_sub (Lia_obj longi, const Lia_obj long1, const Lia_obj long2)
     /* Output: longi. */
     /* Long-integer subtraction: longi := long1 - long2.
        Assumes proper longi[]. */
{
  int max = last_of (long1);

  /* printf ("lia_sub max, len2 %d %d\n", max, len2); */
  Assert(max < 10);

  if (long1 == long2)
    { /* special case 1: 0 = a - a */
      lia_assign (longi, lia_const (0));
    }
  else if (is_nonzero (long2))
    { /* normal case: */
      chs (long2);
      lia_add (longi, long1, long2);
      chs (long2);
    }
  else
    { /* special case 2: a = a - 0 */
      lia_assign (longi, long1);
    }
  /* NOTE: Above case analysis could be done slightly different and
           with less overhead if you call lia_padd/psub() directly.
           >>>> Think about it! <<< */
}

/*--------------------------------------------------------------------------*/

static void lia_padd (Lia_obj longi,
                      /*const*/ Lia_obj long1, /*const*/ Lia_obj long2)
     /* Output: longi. */
     /* NOTE: long1 and long2 are unchanged! */
     /* Long-integer abs-value addition: longi := abs (long1) + abs (long2).
        Assumes proper longi[]; error/overflow-checking at run-time! */
{
  Lia sum, carry = 0;
  int i, max = last_of (long1), len2 = last_of (long2);
  if (max < len2)
    { /* switch long1[] <--> long2[] */
      Lia_ptr aux = long1;
      int aux2 = max;
      long1 = long2;
      long2 = aux;
      max = len2;
      len2 = aux2;  /* NOTE: pointers are switched, but contents stay const */
    }
  /* lia_high (long1) >= lia_high (long2) */
  basic_counter_plus (&lia_common.padd_calls, 1);
  basic_counter_plus (&lia_common.padd_elops, max);
  for (i=1; i<=len2; i++)
    {
      sum = long1[i] + long2[i] + carry;
      longi[i] = sum mod_DBASE;
      carry = sum div_DBASE;
    }
  for (i=len2 + 1; i<=max; i++)
    {
      sum = long1[i] + carry;
      longi[i] = sum mod_DBASE;
      carry = sum div_DBASE;
    }
  if (carry)
    {
      max ++;
      if (max > lia_common.last)
        basic_error ("lia_add or lia_sub: overflow; %d+1 > %d",
                     max, lia_common.length);
      longi[max] = 1;
    }
  lia_common.maximum_digit = (lia_common.maximum_digit > max)? lia_common.maximum_digit : max;
  longi[0] = (unsigned) max times_2;
}

/*--------------------------------------------------------------------------*/

static void lia_psub (Lia_obj longi, const Lia_obj long1, const Lia_obj long2)
     /* Output: longi. */
     /* Long-integer abs-value subtraction: longi := abs (long1) - abs (long2),
        provided that abs (long1) >= abs (long2).
        Assumes proper longi[]. */
{
  Lia b, carry = 0;
  int i, j = 1, max = last_of (long1), len2 = last_of (long2);
  Assert ((max >= len2) and (max <= lia_common.last));
  basic_counter_plus (&lia_common.psub_calls, 1);
  basic_counter_plus (&lia_common.psub_elops, max);

  /* printf ("lia max, len2 %d %d\n", max, len2); */

  for (i=1; i<=len2; i++)
    {
      b = long2[i] + carry;
      if (b > long1[i])
        {
          longi[i] = ((long1[i] + (DBASE - b)) mod_DBASE);
          carry = 1;
        }
      else
        {
          longi[i] = long1[i] - b;
          carry = 0;
        }
      if (longi[i] > 0)
        j = i;
    }
  for (i=len2 + 1; i<=max; i++)
    {
      /* b == 0 + carry */
      if ((long1[i] == 0) and carry)
        /* ie, carry > long1[i]; this is rare... but possible !?! */
        {
          longi[i] = ((DBASE - carry) mod_DBASE);
          carry = 1;
        }
      else
        {
          longi[i] = long1[i] - carry;
          carry = 0;
        }
      if (longi[i] > 0)
        j = i;
    }
  longi[0] = (unsigned) j times_2;
  /* Note: lia_psub() can't change lia_common.maximum_digit. */

  /* printf ("lia leni %d\n", last_of (longi)); */
}

/*--------------------------------------------------------------------------*/

void lia_mul (Lia_obj longi, const Lia_obj long1, const Lia_obj long2)
     /* Output: longi. */
     /* Long-integer multiplication: longi := long1 * long2.
        Assumes proper longi[]; error/overflow-checking at run-time!
        V1 -- inner-loop multiplication by breaking up into half digits */
{
  Lia a1, a2, b1, b2, x1, x2, carry = 0;
  int i, j, h;
  int ind1 = last_of (long1), ind2 = last_of (long2);
  basic_counter_plus (&lia_common.mul_calls, 1);
  if (ind1 + ind2 - 1 > lia_common.last)
    basic_error ("lia_mul: (a priory) overflow; %d+1 > %d",
                ind1 + ind2 - 1, lia_common.length);
  if (((ind1 == 1) and (long1[1] == 0)) or ((ind2 == 1) and (long2[1] == 0)))
    /* either long1 is zero or long2 is zero; in other words:
       (not (is_nonzero (long1) and is_nonzero (long2))) is true */
    lia_load (longi, 0);
  else
    {
      basic_counter_plus (&lia_common.mul_elops, ind1 * ind2);
      for (i=1; i<ind1 + ind2; i++)
        /* Note: This is actually faster than BZERO(longi,Lia,ind1+ind2). */
        longi[i] = 0;
      longi[0] = ind1 + ind2 - 1;  /* expected length */
      for (i=1; i<=ind1; i++)
        {
          carry = 0;
          h = i;
          for (j=1; j<=ind2; j++)
            {
              /* V1 -- inner loop:
                 compute (hi,lo) = long1[i] * long2[j] + carry + longi[h]
                 using double precision (long) multiplication;
                 note: h == i + j - 1 */
              /* break up into half digits (a1,a2) and (b1,b2) */
              a1 = long1[i] div_BASE;
              a2 = long1[i] mod_BASE;
              b1 = long2[j] div_BASE;
              b2 = long2[j] mod_BASE;
              /* (x1,x2) = (a1,a2) * (b1,b2) */
              x1 = a1 * b1;
              x2 = a2 * b2;
              a1 = a1 * b2;
              a2 = a2 * b1;
              /* note: (a1,a2) below used as (a1b2,a2b1) */
              x1 +=  (a1 div_BASE) + (a2 div_BASE);
              x2 += ((a1 mod_BASE) + (a2 mod_BASE)) times_BASE;
              /* move higher bits of x2 to x1 since only 2 overflow bits */
              x1 += (x2 div_DBASE);
              x2  = (x2 mod_DBASE);
              /* (x1,x2) = (x1,x2) + carry + longi[h] */
              x2 += carry + longi[h];   /* note: order is important */
              x1 += (x2 div_DBASE);     /* note: x1 is always < DBASE */
              x2 = (x2 mod_DBASE);
              /* store in digit and carry */
              longi[h] = x2;
              carry = x1;
              h ++;
            }
          if (i < ind1)
            longi[h] = carry;
          else if (carry > 0)
            {
              if (h > lia_common.last)
                basic_error ("lia_mul: (last carry) overflow; %d+1 > %d",
                            h, lia_common.length);
              longi[h] = carry;
            }
        }
      if (carry > 0)
        longi[0] ++; /* since last carry > 0 */
      lia_common.maximum_digit = ((lia_common.maximum_digit > (int) longi[0])?
        lia_common.maximum_digit : (int) longi[0]);
      longi[0] = ((longi[0] times_2)
                  + ((is_negative (long1) + is_negative (long2)) mod_2));
    }
}

/*--------------------------------------------------------------------------*/

/*             ............................................................
               "Beware of bugs in the above code;  I've only tested it, not 
               proved it correct."  [moi, twisting a quote of Donald Knuth]
               ............................................................ */
