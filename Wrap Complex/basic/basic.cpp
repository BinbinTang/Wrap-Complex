/*                         
// Copyright 1996-97. Raindrop Geomagic, Inc.
// All Rights Reserved.
//
// This is unpublished proprietary source code of Raindrop Geomagic, Inc.; 
// the contents of this file may not be disclosed to third parties, copied 
// or duplicated in any form, in whole or in part, without the prior written 
// permission of Raindrop Geomagic, Inc.  
//
// This copyright notices may not be removed from this file at any time.
// 
// RESTRICTED RIGHTS LEGEND:
// Use, duplication or disclosure by the Government is subject to restrictions
// as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
// and Computer Software clause at DFARS 252.227-7013, and/or in similar or
// successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished-
// rights reserved under the Copyright Laws of the United States.
// 
//                 Raindrop Geomagic, Inc
//                 206 N. Randolph Street, Suite 520
//                 Champaign IL 61820-3898
//                 Phone: 217-239-2551
//                 Fax:   217-239-2556
//                 Email: inquiry@geomagic.com
// 
*/ 


/* basic/basic.c --- Basic C Library, core routines and version information. */

/*---------------------------------------------------------------------------*/

const char basic__version[] = " ";
const char basic__authors[] = " ";
#ifdef __DEBUG__
const char basic__compile[] = " ";
#endif

/*---------------------------------------------------------------------------*/

#include "basic.h"

/*---------------------------------------------------------------------------*/

const Basic_byte basic_charbit_mask__[8] = 
{
  (Basic_byte) 128, (Basic_byte) 64, (Basic_byte) 32, (Basic_byte) 16,
  (Basic_byte)   8, (Basic_byte)  4, (Basic_byte)  2, (Basic_byte)  1
};

const Basic_byte basic_charbit_mask_n[8] =
{
  (Basic_byte)~128, (Basic_byte)~64, (Basic_byte)~32, (Basic_byte)~16,
  (Basic_byte)~  8, (Basic_byte)~ 4, (Basic_byte)~ 2, (Basic_byte)~ 1
};

/* NOTE: The masks must be global.  I don't see any reasonable way around it,
   except, giving up the macros. */

/* The headerfile basic.h defines the following macros:

   * macro basic_charbit_on(I,C) returns TRUE iff bit I in char C is set to 1;
   * macro basic_charbit_s1(I,C) sets bit I in char C to 1;
   * macro basic_charbit_s0(I,C) sets bit I in char C to 0;
     usage eg.:  c = basic_charbit_s1 (2,c);
   */

/*---------------------------------------------------------------------------*/

static void (* hook) (const char []) = NULL;

/* Global constants used in definitions in basic.h  --- don't tell anybody! */

/*#ifdef __DEBUG__*/
const char basic__assert_frmt[] = 
  "FATAL ERROR: ASSERTION FAILED: file \"%s\", line %d.";
/*#else
const char basic__assert_frmt[] = "FATAL ERROR: ASSERTION FAILED: %d.";
#endif
*/
void (* basic__null_hook) () = NULL;  /* NOTE: const void blah blah...
					 gives a warning.  Why? */

/*--------------------------------------------------------------------------*/

void (*wrap_abort_fun) (void) = NULL;

void 
set_wrap_abort_fun (void (*wrap_abort_fun_p) (void))
{
  wrap_abort_fun = wrap_abort_fun_p;
}


/*--------------------------------------------------------------------------*/

void basic_error (const char *frmt, ...)
     /* Procedure basic_error (frmt [, arg] ...) has a variable number
	of arguments.  It produces an message just like fprint or vfprint.
	Then it either aborts execution or calls a given error hook. */
{ 
  va_list argp;
  char *msg=0;

      
  if (wrap_abort_fun != NULL)
    wrap_abort_fun ();
  else
  {
    va_start (argp, frmt);
    basic_cb_push_buffer ();
    (void) basic_cb_vprintf (frmt, argp);
    msg = STRDUP (basic_cb_str ());
    basic_cb_pop_buffer ();
    va_end (argp);
  }

  if (hook)
    { /* call hook */
      hook (msg);
    }
  else
    { /* print error message and abort execution (with coredump) */
      fprintf (stderr, "%s\n", msg);
#ifdef __DEBUG__
#ifdef sgi
      (void) abort ();
#else
      abort ();
#endif      
#else
       exit (1);
#endif
    }
  FREE (msg);
}  

/*--------------------------------------------------------------------------*/

void basic_error_hook (void (* user_error) (const char msg[]))
     /* To specify an error hook call basic_error_hook (my_error) assuming
	my_error is a pointer to a function:
	.
	.            void my_error (const char error_message[])
	.            { ... }
	.
	With this, basic_error() will generate the error_message and pass it
	to my_error().  This function might then do some cleanup and/or cause
	segmentation fault for dbx.  Use basic_error_hook (NULL_HOOK) to get
	default behaviour back. */
{
  hook = user_error;
}

/*---------------------------------------------------------------------------*/

unsigned long basic_kbytes (unsigned long bytes)
     /* Converts bytes to kilobytes (going to next higher integer). */
{
  return (If ((bytes > 0), bytes / 1024 + 1, bytes));
}

/*--------------------------------------------------------------------------*/

double basic_mbytes (unsigned long bytes)
     /* Converts bytes to megabytes. */
{
  return ((double) bytes / 1024.0 / 1024.0);
}

/*--------------------------------------------------------------------------*/

char* basic_strip  (const char *strng)
     /* Returns a pointer to the first character in strng that is not
	a special  character, or NULL  if no such  character exists.
	Cf, man  strpbrk(). */
{
#define ALPH "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
  return ((char*) strpbrk (strng, ALPH));
}

/*--------------------------------------------------------------------------*/

void basic_system (const char *frmt, ...)
{ 
  va_list argp;
  char *cmd;
  int result;
  va_start (argp, frmt);
  basic_cb_push_buffer ();
  (void) basic_cb_vprintf (frmt, argp);
  cmd = STRDUP (basic_cb_str ());
  basic_cb_pop_buffer ();
  va_end (argp);
  result = system (cmd);
  if (result != 0)
    printf ("WARNING: basic_system(\"%s\") status=%d\n", cmd, result);
  FREE (cmd);
}

/*--------------------------------------------------------------------------*/

int basic_types_okay (void)
     /* Returns TRUE iff the size of base types is satisfactory;
	otherwise, issues a fatal error warning... */
{
  int okay = TRUE;
  ;
  if ((bitsof (char) != 8) or (bitsof (short) != 16) or (bitsof (int) != 32))
    okay = FALSE;
  else if (not ((bitsof (void *) == 32) or (bitsof (void *) == 64)))
    okay = FALSE;
  else if (not ((bitsof (long) == 32) or (bitsof (long) == 64)))
    okay = FALSE;
  else if (bitsof (long) != bitsof (void *))
    okay = FALSE;
#if defined (__LONGLONG)
  else if (not ((bitsof (long long) == 32) or (bitsof (long long) == 64)
		or (bitsof (long long) == 128)))
    okay = FALSE;
#endif
  else if (bitsof (float) != 32)
    okay = FALSE;
  else if (bitsof (double) != 64)
    okay = FALSE;
  ;
  if (not okay)
    {
#define PRT(B_STR,B_SIZE,SHOULD_BE) \
      fprintf (stderr, "%4d == %s ... should be %s\n", B_SIZE, B_STR, SHOULD_BE)
      ;
      fprintf (stderr, "\nFATAL ERROR: basic_types_okay() returns FALSE!\n");
      fprintf (stderr, "\n");
      PRT ("bitsof (char)     ", bitsof (char),   " 8");
      PRT ("bitsof (short)    ", bitsof (short),  "16");
      PRT ("bitsof (int)      ", bitsof (int),    "32");
      fprintf (stderr, "\n");
      PRT ("bitsof (long)   | ", bitsof (long)  , "32 or 64");
      PRT ("bitsof (void *) | ", bitsof (void *), "32 or 64");
      fprintf (stderr, "\n");
#if defined (__LONGLONG)
      PRT ("bitsof (long long)", bitsof (long long), "32 or 64 or 128");
#endif
      fprintf (stderr, "\n");
      PRT ("bitsof (float)    ", bitsof (float),     "32");
      PRT ("bitsof (double)   ", bitsof (double),    "64");
      fprintf (stderr, "\n");
#undef PRT
    }
  return (okay);
}

/*--------------------------------------------------------------------------*/

int basic_is_64_bit_ARCH (void)
     /* Returns non-zero #if defined (is_64_bit_ARCH); see basic.h. */
{
#if defined (is_64_bit_ARCH)
  return (TRUE);
#else
  return (FALSE);
#endif
}

#ifdef _WINDOWS
char * index (const char *s, int c)
{
	return (char *) strchr(s, (char) c);
}

char * rindex (const char *s, int c)
{
    return (char *) strrchr(s, (char) c);
}
#endif
