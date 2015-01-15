/* lia/liaux.c  ---  Auxiliary routines for Lia. */

/*--------------------------------------------------------------------------*/

#include "lia.h"
#include "base.h"

/*--------------------------------------------------------------------------*/

Lia_info lia_common; /* NOTE: This is global because it's used internally
                        within the Lia Library; however, it isn't supposed
                        to be used from outside. (So, don't tell anybody. :) */

static int initialized = FALSE;

/*--------------------------------------------------------------------------*/

void lia_maximum (int length)
     /* Library initialization.
        Sets ABSOLUTE maximum number of Lia digits for lia_*() results.
        Can be called ONLY ONCE per execution!
        NOTE: Lia object Lia x[length] = x[0], x[1], ..., x[length-1]. */
{
  /*
  MIKE: Let's get rid of this...  BE VERY CAREFUL!!!!!!
  if (initialized)
    basic_error ("lia_maximum: can only be called once");
    */
  lia_common.max =    length;
  lia_common.length = length;
  lia_common.last =   length - 1;
  basic_counter_reset (&lia_common.mul_calls);
  basic_counter_reset (&lia_common.mul_elops);
  basic_counter_reset (&lia_common.padd_calls);
  basic_counter_reset (&lia_common.padd_elops);
  basic_counter_reset (&lia_common.psub_calls);
  basic_counter_reset (&lia_common.psub_elops);
  lia_common.maximum_digit = 0;
  initialized = TRUE;
}

/*--------------------------------------------------------------------------*/

void lia_length (int length)
     /* Change the maximum length, and if want: library initialization.
        Can be called more then once, but cannot increase the ABSOLUTE maximum,
        either set by lia_maximum() or by FIRST call of lia_length(). */
{
  /*
  MIKE: Let's get rid of this...
  if (not initialized)
    { /* (better call lia_maximum() first :) */
  lia_maximum (length);
      /*    } */
  lia_common.length = length;
  lia_common.last =   length - 1;
  if (lia_common.last >= lia_common.max)
    basic_error ("lia_length: length (%d) > maximum (%d)",
                length, lia_common.max);
}

/*--------------------------------------------------------------------------*/

int lia_get_maximum (void)
     /* Returns the absolut maximum length; cf, lia_maximum(). */
{
  return (lia_common.max);
}

/*--------------------------------------------------------------------------*/

int comp_beta (int nofBits)

{
  return COMP_BETA (nofBits);
}

/*--------------------------------------------------------------------------*/

int lia_get_length (void)
     /* Returns current maximum length; cf, lia_maximum() and lia_length(). */
{
  return (lia_common.length);
}

/*--------------------------------------------------------------------------*/

int lia_get_beta (void)
     /* Returns the BETA value of the implementation.
        Notice, that BASE == 2^BETA and DBASE == BASE^2. */
{
  return (BETA);
}

/*--------------------------------------------------------------------------*/

Lia_info* lia_info (void)
     /* Returns some Lia info. */
     /* NOTE: The returned address, which points to the info structure,
              is a constant.  DO NOT FREE() IT and consider the fields
              of the structure as read-only. */
{
  static Lia_info li;
  li = lia_common;
  return (&li);
}

/*--------------------------------------------------------------------------*/

void lia_fput (FILE *file, const Lia_obj longi)
     /* Prints internal representation of longi to file. */
{
  int i, ind = last_of (longi);
  fprintf (file, "%c:", If ((lia_sign (longi) == -1), '-', '+'));
  for (i=ind; i>=1; i--)
    fprintf (file, "%lu:", longi[i]);
  fprintf (file, "[%d]", ind);
}

/*--------------------------------------------------------------------------*/

double lia_real (const Lia_obj longi)
     /* Converts longi to floating-point. */
{
  /* NOTES:
     (1) This is not too efficient.
     (2) Is it accurate? It seems so, but we could use sprint()/sscan(), too??
     (3) A 'double' corresponds to roughly LIA_DOUBLE_MAXDECIMALS (== 16)
     decimals of accuracy.   Ie., in 32-bit mode the conversion from 'double'
     to 'Lia' is not problematic: DBASE==1073741824.  However, in 64-bit mode,
     DBASE==4611686018427387904, 19 decimals, and this could mean trouble.  So,
     let's be bit paranoid, and slow the whole thing down by a factor of 2...
     */
#if 1 || defined (is_64_bit_ARCH)
  /* New code: Splits Lia digits in half like lia_mul(); cf. note (3).
   */
  int i, ind = last_of (longi);
  double base = (double) BASE, b = 1.0, r = 0.0;
  Lia a1, a2;
  for (i=1; i<=ind; i++)
    {
      a1 = longi[i] div_BASE;
      a2 = longi[i] mod_BASE;
      ;
      r += ((double) a2) * b;
      b *= base;
      ;
      r += ((double) a1) * b;
      b *= base;
    }
#else
  /* Old code: Works fine for 32-bits; probably also works for 64-bits.
     Empirically, both codes give 15 to 17 decimals accuracy; however,
     the new code above seems to be 1 decimal better "on average," both
     in 32- aw well as in 64-bit mode...
     */
  int i, ind = last_of (longi);
  double dbase = (double) DBASE, b = 1.0, r = 0.0;
  for (i=1; i<=ind; i++)
    {
      r += ((double) longi[i]) * b;
      b *= dbase;
    }
#endif
  return (r * (double) lia_sign (longi));
}

/*--------------------------------------------------------------------------*/

void
lia_exportBinString (const Lia_obj longi, unsigned char out[], 
		     const int lengthOut)

     /* exports "longi" in binary format in "out". "lengthOut" specifies the
	size of "out" and is assumed to be at least sizeof (Lia) times
	the length of "longi".
     */

{
  unsigned char c;
  int offset, max, i, j;
  Lia longTmp;
#if defined (is_64_bit_ARCH)
  static int offs[8] = {8, 8, 8, 8, 8, 8, 8, 2 * BETA - 56};
#else
  static int offs[4] = {8, 8, 8, 2 * BETA - 24};
#endif

#if defined (is_64_bit_ARCH)
  Assert_always (2 * sizeof (int) == sizeof (long));
  Assert_always (sizeof (Lia) == sizeof (long));
#else
  Assert_always (sizeof (int) == sizeof (long));
#endif

  for (i = 0; i < lengthOut; i++) out[i] = 0;

  max = last_of (longi);
  longTmp = longi[0];
  *out++ = (unsigned char) (longTmp % 256); longTmp = longTmp / 256;
  *out++ = (unsigned char) longTmp % 256; longTmp = longTmp / 256;
  *out++ = (unsigned char) longTmp % 256; longTmp = longTmp / 256;
  *out++ = (unsigned char) longTmp; Assert_always (longTmp / 256 == 0);
  Assert_always (lengthOut / 4 >= max);
  offset = 0;

  for (i=1; i<=max; i++) {
    longTmp = longi[i];
    for (j = 0; j < sizeof (Lia); j++) {
      c = (unsigned char) (longTmp % 256);
      longTmp = longTmp / 256;

      if (offset == 0) {
	*out = c;
	offset = offs[j] % 8;
	if (offset == 0) out++;
      } else {
	*out |= (unsigned char) c << offset;
	offset = (offset + offs[j]) % 8;
	if (offset < offs[j]) {
	  out++;
	  if (offset != 0) *out = c >> (offs[j] - offset); 
	}
      }
    }
  }
}

/*--------------------------------------------------------------------------*/

void
lia_loadBinString (const unsigned char in[], const int lengthIn, 
		   Lia_obj longi)

     /* copies lia stored in binary format in "in" into "longi". the
	size of "longi" is assumed to be "lengthIn" (this includes the
	header word.
     */

{
  int offset, i, maxNonZero, j;
  unsigned char c[8];
#if defined (is_64_bit_ARCH)
  static int offs[8] = {8, 8, 8, 8, 8, 8, 8, 2 * BETA - 56};
#else
  static int offs[4] = {8, 8, 8, 2 * BETA - 24};
#endif

#if defined (is_64_bit_ARCH)
  Assert_always (2 * sizeof (int) == sizeof (long));
  Assert_always (sizeof (Lia) == sizeof (long));
#else
  Assert_always (sizeof (int) == sizeof (long));
#endif

  longi[0] = in[3];
  longi[0] = longi[0] * 256 + in[2];
  longi[0] = longi[0] * 256 + in[1];
  longi[0] = longi[0] * 256 + in[0];
  in += sizeof (int);
  offset = 0;

  maxNonZero = 0;
  for (i=1; i<lengthIn; i++) {
    for (j = 0; j < sizeof (Lia); j++) {
      if (offset == 0) {
	c[j] = *in % (1 << offs[j]);
	offset = offs[j] % 8;
	if (offset == 0) in++;
      } else {
	c[j] = (*in >> offset) % (1 << offs[j]);
	offset = (offset + offs[j]) % 8;
	if (offset < offs[j]) {
	  in++;
	  if (offset != 0) 
	    c[j] |= (*in % powerof2 (offset)) << (offs[j] - offset);
	}
      }
    }

    longi[i] = 0;
    for (j = sizeof (Lia) - 1; j >= 0; j--) {
      longi[i] *= 256;
      longi[i] += c[j];
    }

    if (longi[i] != 0) maxNonZero = i;
  }
  if (maxNonZero == 0) 
    lia_load (longi, 0);
  else
    longi[0] = (longi[0] & 1L) + (maxNonZero times_2);
}

/*--------------------------------------------------------------------------*/

int lia_high (const Lia_obj longi)
     /* Returns number of highest lia_digit used in longi:
        longi == [0... lia_high (long)]. */
{
  return (last_of (longi));
}

/*--------------------------------------------------------------------------*/

Lia lia_digit (const Lia_obj longi, int d)
     /* Returns longi[d] without any further checking. */
{
 return (longi[d]);
}

/*--------------------------------------------------------------------------*/

void lia_load (Lia_obj longi, int shorti)
     /* Output: longi. */
     /* Converts int to Lia:
        longi := shorti.
        Assumes at least longi[0..2], no matter whether 32- or 64-bit mode! */
{
  Lia abs_value = (shorti >= 0)? shorti : -shorti;
  Assert (lia_common.last >= 2);
  longi[1] = abs_value mod_DBASE;
  longi[2] = abs_value div_DBASE;
  Assert ((not basic_is_64_bit_ARCH) or (longi[2] == 0));
  /* NOTE:
     longi[2] should always be zero in 64-bit mode!
     That is, w/ 'cc -64', we COULD actually lift the longi[0..2] requirement!
     This is not done at this point for sake of consistency.
     */
  longi[0] = If ((longi[2] == 0), 1 times_2, 2 times_2);
  if (shorti < 0)
    longi[0] ++;
}

/*--------------------------------------------------------------------------*/

void lia_strload (Lia_obj longi, const char string[], const char frmt[])
     /* Output: longi. */
     /* Converts an integer string to Lia:
        longi := "decimal integer" string for frmt[] == "%d";
        longi := "hex.    integer" string for frmt[] == "%x".
        Assumes longi[] of proper size. */
{
  Lia basis[3], ziffer[3];  /* cf. lia_load() */
  char digit[2];
  int d, i = 0;
  int n_flag = FALSE;
  static Lia *aux = NULL;
  if (not aux)
    {
      aux = MALLOC (Lia, lia_common.max);
      //MARK (aux, -lia_magic);
	  MARK (aux, -130862003);
    }
  if ((frmt[0] != '%') or (frmt[2] != 0))
    basic_error ("lia_strload: totally wrong frmt: \"%s\"", frmt);
  switch (frmt[1])
    {
     case 'd':
      lia_load (basis, 10);
      break;
     case 'x':
      lia_load (basis, 16);
      break;
     default:
      basic_error ("lia_strload: wrong format: \"%s\"", frmt);
    }
  lia_assign (longi, LIA_NULL);
  digit[1] = 0;
  while (string[i] == ' ')
    i ++;
  if (string[i] == '-')
    {
      n_flag = TRUE;
      i ++;
    }
  else if (string[i] == '+')
    i ++;
  while (isdigit (digit[0] = string[i++]))
    {
      //sscanf (digit, frmt, &d);
	  sscanf_s (digit, frmt, &d);
      lia_load (ziffer, d);
      lia_mul (aux, longi, basis);
      lia_add (longi, aux, ziffer);   /* longi := basis * longi + ziffer */
    }
  if (n_flag)
    lia_chs (longi);
}

/*--------------------------------------------------------------------------*/

void lia_ffpload (Lia_obj longi, int w, int a, double value)
     /* Output: longi. */
     /* Conversion: Floating-point (actually, fix-point) to Lia:
        longi := integer (value * 10^a).
        That is, longi := value IN "fix-point" format "%<w>.<a>f" TIMES 10^a.
        Assumes at least longi[Lia_DIGITS(LIA_DOUBLE_MAXDECIMALS)];
        and: a < w <= LIA_DOUBLE_MAXDECIMALS  (== 16),
        which is fine since 64-bit FP has approx 16 decimal digits.
        Overflow check at run-time! */
{
#define FFPLOAD_MAXLEN 20
#define FFPLOAD_SAVETY 20
  /* The underlying assumption here is that a 64-bit double has no more that
     16 significant decimal digits.  So, 20 characters for the format should
     be sufficient, right? But since I'm paranoid, let's just double it for
     savety.
     */
  static char buf[FFPLOAD_MAXLEN+FFPLOAD_SAVETY];
  static double mult_prime = 1;
  static int a_prime = 0;
  double floorVal;
  if (a_prime != a)
    {
      mult_prime = pow (10.0, (double) a);
      a_prime = a;
    }
  floorVal = floor (value * mult_prime + 0.5);
  //sprintf (buf, "%0.1f", floorVal);
  sprintf_s (buf, FFPLOAD_MAXLEN+FFPLOAD_SAVETY, "%0.1f", floorVal);
  if (((int) strlen (buf) > w + 1 + 2) or (w + 1 + 2 + 1 > FFPLOAD_MAXLEN))
    basic_error ("lia_ffpload: overflow: %d.%d, %f, \"%s\"", a, w, value, buf);
  /* strload() will ignore everything after the comma */
  lia_strload (longi, buf, "%d");
}

/*--------------------------------------------------------------------------*/

int lia_fixload (Lia_obj longi, int w, int a, char fixfrmt[])
     /* Output: longi. */
     /* Conversion: (unlimited) fix-point format to Lia:
        longi := integer ("value" of fixfrmt[] * 10^a).
        That is, longi := value IN "fix-point" format "%<w>.<a>f" TIMES 10^a.
        NOTE: since we no longer go through 'double': w > 16 is valid!
        Assumes proper longi[]; here: >= Lia_DIGITS (w) long.
        If successful: returns 1; otherwise: 0. */
{
  char decimals[] = "+-.1234567890";
  char *ptr = strpbrk (fixfrmt, decimals);
  char *dot = strpbrk (fixfrmt, ".");
  size_t ell, ell2, len = strspn (fixfrmt, decimals);
  int i = 0, cnt = 0, j;
  static char *buffer = NULL;
  static int w_buffer = 0;
  ;
  /* a-priory check of input */
  if ((a >= w) or (ptr == NULL) or (len > (unsigned int) (w + 2) ))
    return (0);
  ;
  /* copy into buffer, and correct fix-point format */
  if (w_buffer < w)
    {
      FREE (buffer);
      buffer = MALLOC (char, w + 6);  /* 6 for safety ... "save 6" */
      HIDE (buffer);
      w_buffer = w;
    }
  //sprintf (buffer, "%s", ptr);
  sprintf_s (buffer, w + 6, "%s", ptr);
  buffer[len] = '\0';
  ell = strlen (buffer) - 1;
  if (dot == NULL)
    { /* attach '.' */
      ell ++;
      buffer[ell] = '.';
      buffer[ell + 1] = '\0';
      len ++;
    }
  ptr = buffer;
  dot = strpbrk (buffer, ".");
  len = strlen (buffer);
  ell2 = len - ((long) dot - (long) ptr) - 1;
  if (ell2 > (unsigned int) a)
    { /* cut off */
      dot[a + 1] = '\0';  /* look at this! :) */
      ell2 = a;
      len = strlen (buffer);
    }
  else
    {
      while (ell2 < (unsigned int) a)
        { /* attach '0' */
          ell2 ++;
          ell ++;
          buffer[ell] = '0';
          buffer[ell + 1] = '\0';
          len ++;
        }
    }
  ;
  /* second check of corrected input */
  if (len > (unsigned int) (w + 2) )
    return (0);
  ;
  /* remove the '.' */
  /* NOTE: this loop is useless... why don't jump to dot immediately ??? */
  while (*ptr != '.')  
    {
      cnt ++;
      if ((unsigned int) cnt > len)
        return (0);
      /* NOTE: at this point, buffer[i] == *ptr */
      i ++;
      ptr ++;
    }
  cnt ++;
  ptr ++;
  for (j=1; j<=a; j++)
    {
      cnt ++;
      if ((unsigned int) cnt > len)
        return (0);  /* shouldn't happen! */
      buffer[i] = *ptr;  /* i.e., buffer[i] = buffer[i+1] */
      i ++;
      ptr ++;
    }
  if (cnt != len)
    return (0);  /* shouldn't happen! */
  buffer[i] = '\0';
  ;
  /* finally: string --> Lia */
  lia_strload (longi, buffer, "%d");
  return (1);
}

/*--------------------------------------------------------------------------*/

Lia_ptr lia_const (int i)
     /* Returns pointer to TEMPORARY Lia object representing int i.
        It's the users responsibility to save it via lia_copy() or
        lia_assign(), where needed.  The resulting Lia object will
        need at most 3 Lia digits!  Be careful: Don't ever use the
        resulting pointer other than read-only! */
{
  static Lia aux[3];  /* 3 digits are enough for an int; cf lia_load() */
  lia_load (aux, i);
  return (aux);
}

/*--------------------------------------------------------------------------*/

void lia_sdiv (Lia_obj result, unsigned long *remainder,
               const Lia_obj longi, long shorti)
     /* Output: result, *remainder. */
     /* Does simple divison  longi / shorti  with  0 < shorti < BASE.
        NOTE: The result and longi arguments may denote the same Lia object! */
{
  Lia i, r, s, t, e, f, u, v, ind = last_of (longi);
  int z_flag = TRUE;  /* for the time being, assume zero */
  int n_flag = is_negative (longi);
  s = DBASE / shorti;
  t = DBASE mod shorti;
  r = 0;
  for (i=ind; i>=1; i--)
    {
      e = longi[i] / shorti;
      f = longi[i] mod shorti;
      u = r * s + e;
      v = r * t + f;   /* NOTE: overflow in v is possible when shorti > BASE */
      result[i] = u + v / shorti;
      r = v mod shorti;
    }
  *remainder = If (n_flag, -((long) r), r);
  for (i=ind; i>=1; i--)
    if (result[i] > 0)
      {
        result[0] = i times_2;
        z_flag = FALSE;
        break;
      }
  if (n_flag)
    result[0] ++;
  if (z_flag)
    result[0] = 2;
}
