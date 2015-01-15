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

 
/* basic/basic.h --- Headerfile for The Basic C Library & Macro Extensions. */

/* NOTE: Use "cc -D__DEBUG__" when debugging! */

#ifndef __BASIC_H__  /* Include this file only once! */
#define __BASIC_H__

#include <math.h>       /* math lib: sin(), ...; use with -lm */

//#include <stdio.h>      /* standard IO: FILE, NULL (*), EOF, ... */
//#include <stdlib.h>     /* standard lib: abort(), system(), getenv(), ... */

//#include <ctype.h>      /* char macros: isalpha(), ... */
//#include <sys/types.h>  /* system types definitions: time_t, ... */
//#include <stdarg.h>     /* for variable-length argument lists */

//#include <string.h>    /* strings */

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <typeinfo>
#include <string>
#include <assert.h>
#include <crtdbg.h>

using namespace std;

/* Can't include other files while inside an extern "C" as there
  may be templates there. -swp */
#ifdef _WIN32
#include "win_basic.h"
#include "windows.h"
#else
#include "unix_basic.h"
#endif

/*            (*) NOTE: The constant NULL designates a nonexistent pointer. */

/*--------------------------------------------------------------------------*/

/* Substitutes for definitions that are, for some reason,  missing in
   some systems' standard header files: i.e., <values.h>, <sys/param.h> */

#ifndef BITSPERBYTE
# if gcos
#  define BITSPERBYTE     9
# else
#  define BITSPERBYTE     8
# endif
#endif

#ifndef  BITS
# define BITS(type)  (BITSPERBYTE * (int) sizeof (type))
#endif

/* It seems as if 'gcc -Wall' on some installations has badly (well, not
   pedantically correctly) defined HIBIT* in <values.h>.  They should be
   like below! If there are warnings, use -D__USE_FIXED_PROTOTYPES__ */
#if defined (__GNUC__) && defined (__USE_FIXED_PROTOTYPES__)
#  undef HIBITS
#  undef HIBITI
#  undef HIBITL
#endif

/* Pedantically correctly (PC :) defined HIBIT* ... */
#ifndef  HIBITI
# define HIBITS  ((short)(1 << (BITS(short) - 1)))
# define HIBITI  (1 << (BITS(int) - 1))
# define HIBITL  (1L << (BITS(long) - 1))
#endif

#ifndef MAXINT
#define MAXINT (~HIBITI)
#endif

#  include <float.h>

/* Can't include other files while inside an extern "C" as there
  may be templates there. -swp */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef MAXFLOAT
#undef MAXFLOAT
#endif
# define MAXFLOAT FLT_MAX
#ifdef MINFLOAT
#undef MINFLOAT
#endif
# define MINFLOAT FLT_MIN
 
#ifndef  HZ
# define HZ  60  /* Just a guess! */
#endif

#ifndef  MAXPATHLEN  
# define MAXPATHLEN  256  /* Or even 1024? */
#endif

/*---------------------------------------------------------------------------*/

/* Substitute declarations of a couple of commonly used functions
   either sometimes missing in the standard header files (e.g., for ANSI),
   or with possibly nonstandard prototypes;
   cf. man pages */
#ifdef WIN32
 extern long random (void);
 extern int srandom (int seed);
#endif

/*--------------------------------------------------------------------------*/

/* Boolean (int) constants and 1-byte type. */

#define Basic_byte  unsigned char

#ifdef  TRUE
# undef TRUE
#endif
#define TRUE   1

#ifdef  FALSE
#undef FALSE
#endif
#define FALSE  0

#define YES    TRUE
#define NO     FALSE

#define ON     TRUE
#define OFF    FALSE

/*--------------------------------------------------------------------------*/

/* Human-readable logical operators and functional in-line if. */

#define and    &&
#define or     ||
#define not    !
#define mod    %

#define If(COND,THEN,ELSE)  ((COND) ? (THEN) : (ELSE))

/*--------------------------------------------------------------------------*/

#if defined (powerof2)
#undef powerof2 /* unfortunate coincidence (so far on __alpha only) */
#endif

/* #define Square(X)  ((X) * (X)) */

#define powerof2(EXP)  (1L << (EXP))  /* returns: long */
#define bitsof(TYPE)   (BITS (TYPE))  /* returns: int  */

#define Addr(PTR)  ((unsigned long) (PTR))
/* Here, we're a bit paranoid when casting pointers to integers;  eg, to print
   out addresses, only use 'printf ("... 0x%lx ...", ... Addr (PTR) ...);' */

/* IMPORTANT NOTES AD POINTER VS. INTEGERS
   |
   | (1) In general, we can't just cast a pointer to an integer!  Logically
   |     speaking, we should use size_t (as defined in <stddef.h>) for it, but
   |     but we're talking C here, so what does this have to do with logic?
   |     The problem is that size_t might be either int, unsigned int, or
   |     unsigned long, depending on architecture, and so we don't know how
   |     to print it.
   |       My solution is this: make sure that sizeof (unsigned long) ==
   |     sizeof (void *) --- cf, basic_types_okay() --- and cast pointers
   |     always to unsigned long, ie, use the Addr() macro... and hope that
   |     <stddef.h> is smart enough to define the proper size_t.
   |
   | (2) One more time: Whenever we're talking about pointers/addresses,
   |     we're talking unsigend long integers.
   |
   | (3) The sizeof() operator returns size_t... so, if you want an int,
   |     you need to cast: "(int) sizeof (...)"
   */

/*--------------------------------------------------------------------------*/

/* Usually, the return values of ?printf() and ?scanf() are useless. */
/* so what if they are useless to you? -swp */
#ifdef THIS_IS_DUMB 
#define  print  (void) printf
#define fprint  (void) fprintf
#define sprint  (void) sprintf
#define  scan   (void)  scanf
#define fscan   (void) fscanf
#define sscan   (void) sscanf
#endif

#ifndef __cplusplus
#define flush() fflush (stdout)
#endif

/*--------------------------------------------------------------------------*/

/* Sun's CAT operator.  Eg, CAT(CAT(pr,i),nt) ("Hello world."); */

#ifndef __GNUC__  /* Since this won't work in GNU C; use stringifiers! */
# ifndef CAT
#  define CAT_DEFINED_IN_BASIC
#  undef  CAT_IDENT
#  define CAT_IDENT(X) X
#  define CAT(A,B) CAT_IDENT(A)B
# else
# endif
#endif


/*--------------------------------------------------------------------------*/

extern void (*wrap_abort_fun) (void);
extern void set_wrap_abort_fun (void (*wrap_abort_fun_p) (void));

/*--------------------------------------------------------------------------*/

/* "Touching" unused arguments... */

/* It sometimes happens, in particular, in connection with above Assert*()
   macros, that some of the arguments to a function are unused...  *and*
   there happen to be compilers that bitch about this. Rather than to
   globally dissabel the "argument unused" warning, it's better to
   selectively avoidng the warning by "touching" the arguments. */

/* The statement "Touch_args (a && b && c && ...);" tells the compiler that
   the arguments "a, b, c, ..." are not necessarily used in the body of the
   function.  Yes, it's ugly, but it seems to work well, and the macro is
   implemented such that, I believe, it doesn't cost too much time during
   execution.  Hopefully, it's portable, too.  --Ernst. */

#define Touch_args(ARG_CONJUNCTION)  (void) (0 and (ARG_CONJUNCTION))

/*---------------------------------------------------------------------------*/

/* MODULE basic/basic.c --- some core routines */

extern const Basic_byte basic_charbit_mask__[];
extern const Basic_byte basic_charbit_mask_n[];

#define basic_charbit_on(I,CVAR)  ((basic_charbit_mask__[I] & CVAR) != 0)
#define basic_charbit_s1(I,CVAR)  ((char) (basic_charbit_mask__[I] | CVAR))
#define basic_charbit_s0(I,CVAR)  ((char) (basic_charbit_mask_n[I] & CVAR))

#define basic_intbit_on(I,IVAR) ((powerof2 (I) & IVAR) != 0)

#define NULL_HOOK    basic__null_hook
extern const char    basic__assert_frmt[];
extern       void (* basic__null_hook) ();

void          basic_error      (const char *frmt, ...);
void          basic_error_hook (void (* user_error) (const char msg[]));
unsigned long basic_kbytes     (unsigned long bytes);
double        basic_mbytes     (unsigned long bytes);
char*         basic_strip      (const char *strng);
void          basic_system     (const char *frmt, ...);
int           basic_types_okay (void);
int           basic_is_64_bit_ARCH (void);

/*--------------------------------------------------------------------------*/

/* basic/files.c --- encapsulated file utilities */

/* robust fopen */
FILE*  rgi_fopen     (const char path_name[], const char mode[]);

FILE*  basic_fopen   (const char path_name[], const char mode[]);
void   basic_fclose  (FILE *file);
int    basic_access  (const char path_name[]);
time_t basic_modtime (const char path_name[]);
void   basic_rm      (const char path_name[]);

extern char *basic_fopen_zpath;

/*--------------------------------------------------------------------------*/

/* MODULE basic/malloc.c --- encapsulated malloc */

/* Macros for dynamic memory alloctaion:
   
   (T *)    MALLOC (T, N)      ... allocate N elements of type T
   void     REALLOC (P, T, N)  ... re-allocate memory object (char *) P
   void     FREE (P)           ... deallocate memory object (char *) P
   void     BZERO (P, T, N)    ... clear bytes of memory object (char *) P

   The parameters T and N above always denote a type and an integer number.
   Momory objects (char *) P are assumed to be allocated with this module.
   NOTE that REALLOC() is a void function... in contrast to realloc()!

   Macros for string allocation:

   (char *) STRDUP (S) ... duplicate string (char *) S and return pointer
   STRSET (S1, S2)     ... sets S1 = STRDUP (S2);  assumes S1 either NULL
                           or result of earlier STRDUP()

   The macros below are ignored for basic_malloc_debug (0), which is the
   default.

   void     HIDE (P)           ... mark memory object P as "hidden"
   void     MARK (P, M)        ... mark memory object P with (int) M

   Cf, malloc(), realloc(), strdup(), free(), bzero() of standard library.

   */

#define MALLOC(T,N) \
  (T*) basic_malloc ((int) sizeof(T) * (N), __FILE__, __LINE__)

#define REALLOC(P,T,N) \
  P = (T*) basic_realloc ((char*) (P), (int) sizeof(T) * (N), __FILE__, __LINE__)

#define FREE(P) \
  do { basic_free ((char*) (P), __FILE__, __LINE__);  P = NULL; } while (0)


#define HIDE(P)    basic_malloc_mark ((char *) P, -1)
#define MARK(P,M)  basic_malloc_mark ((char *) P,  M)

#define STRDUP(S) \
  basic_strdup (S, __FILE__, __LINE__)

#define STRSET(S1,S2) \
  do { FREE (S1);  S1 = STRDUP (S2); } while (0)

typedef struct basic_malloc_info_struct
{
  unsigned long total;
  unsigned long in_use;
  unsigned long hidden;
  long arena;
  unsigned long used;
  unsigned long used_sml;
  unsigned long used_ord;
} Basic_malloc_info;

Basic_malloc_info* basic_malloc_info (void);
void               basic_malloc_debug        (int level);
void               basic_malloc_mark         (void *ptr, int mark);
unsigned long      basic_malloc_marked_bytes (int a, int b);
void               basic_malloc_list_print   (FILE *file);
void               basic_malloc_info_print   (FILE *file);

/* NOTE: The routines below should be invoked only in connection */
/*       with the above mentioned allocation macros!             */
void* basic_malloc  (int n, const char *f, int l);
void* basic_realloc (void *ptr, int n, const char *f, int l);
void  basic_free    (void *ptr, const char *f,  int l);
char* basic_strdup  (const char *s1, const char *f, int l);


extern void (*malloc_fail_handler) (void);
extern void set_malloc_fail_handler (void (*malloc_fail_handler_fun) (void));

/*--------------------------------------------------------------------------*/

/* MODULE basic/getarg.c --- parse command-line arguments */

/* This was originally written by D'Arcy J M Cain. */

int          basic_getarg_init  (int argc, char **argv);
int          basic_getarg_inite (int argc, char **argv);
int          basic_getarg       (const char *opts);
extern char* basic_getarg_optarg;
extern int   basic_getarg_optind;

/*--------------------------------------------------------------------------*/

/* MODULE basic/cb.c --- a (stack of) dynamically sized char buffer(s) */
void  basic_cb_putc  (char c);
char* basic_cb_str   (void);
void  basic_cb_clear (void);
int   basic_cb_len   (void);
int   basic_cb_size  (void);

int   basic_cb_addalinef (FILE *file);
char* basic_cb_getline   (FILE *file);

int   basic_cb_printf (const char *frmt, ...);
char* basic_cb_frmt   (const char *frmt, ...);

#define basic_cb_addaline  (void) basic_cb_addalinef
#define basic_cb_print     (void) basic_cb_printf

#define basic_cb_vprintf   basic_cb_doprnt

void  basic_cb_push_buffer (void);
void  basic_cb_pop_buffer  (void);

/* NOTE: This module uses Steve Summit's _doprnt() code: basic/cb_doprnt.c */
int basic_cb_doprnt (const char *fmt, va_list argp);

/*--------------------------------------------------------------------------*/

/* MODULE basic/isort --- inplace (insertion) sorting of very small lists */

int basic_isort2  (int *a, int*b);
int basic_isort3  (int *a, int *b, int *c);
int basic_isort4p (int *a, int *b, int *c, int *d);
int basic_isort4  (int *a, int *b, int *c, int *d);
int basic_isort5p (int *a, int *b, int *c, int *d, int *e);

/*--------------------------------------------------------------------------*/

/* MODULE basic/prime.c --- prime numbers */

int basic_prime_successor (int n);
int basic_prime_test      (int n);

typedef struct basic_prime_info_struct
{
  int tests, mods;
} Basic_prime_info;

Basic_prime_info* basic_prime_info (void);

/*--------------------------------------------------------------------------*/

/* MODULE basic/time.c --- system time etc */

double basic_utime    (void);
char*  basic_hostname (void);
char*  basic_date     (void);
void   basic_daytime  (int *sec, int *micros, int *minwest, int *dst);
double basic_seconds  (void);
int    basic_seed     (void);

/*--------------------------------------------------------------------------*/

/* MODULE basic/counter.c --- macros & routines for counters: big, int, N/A */

typedef struct basic_counter_record
{
  int a, b;
} Basic_counter;

/* Never use these functions directly; use below macros! */
char *basic_counter__ (Basic_counter counter);
char *basic_counter_  (int counter);
void  basic_counter_reset__ (Basic_counter *counter);
void  basic_counter_plus__ (Basic_counter *counter, int increment);

#define BASIC_COUNTER_MODE 1

#if (BASIC_COUNTER_MODE == 2)
#  define basic_counter_plus(CPTR,I)  basic_counter_plus__ (CPTR, I)
#  define basic_counter_reset(CPTR)   basic_counter_reset__ (CPTR)
#  define basic_counter(C)            basic_counter__ (C)
#endif

#if (BASIC_COUNTER_MODE == 1)
#  define basic_counter_plus(CPTR,I)  (CPTR)->a += (I)
#  define basic_counter_reset(CPTR)   (CPTR)->a = 0
#  define basic_counter(C)            basic_counter_ (C.a)
#endif

#if (BASIC_COUNTER_MODE == 0)
#  define basic_counter_plus(CPTR,I)  /* nothing */
#  define basic_counter_reset(CPTR)   /* nothing */
#  define basic_counter(C)            "N/A"
#endif

/* NOTE: basic_counter_plus() behaves like a function but is a macro, such
 *       that you can redefine it in case you don't bounther counting at all.
 */

/*--------------------------------------------------------------------------*/

/* MODULE basic/arg.c --- Routines for long command-line arguments */

void   basic_arg_open         (int arg_c, char **arg_v);
void   basic_arg_close        (void);
char*  basic_arg_string       (int i);
int    basic_arg_match        (int i, const char string[]);
int    basic_arg_match2int    (int i, const char string[], int *val);
int    basic_arg_match2float  (int i, const char string[], float *val);
int    basic_arg_match2double (int i, const char string[], double *val);
int    basic_arg_match2string (int i, const char string[],
                               char buffer[], int length);
int    basic_arg_increment    (void);
int    basic_arg_find         (const char string[], int arg_c,
                               /*const*/ char *arg_v[]);

/*--------------------------------------------------------------------------*/

/* Miscellaneous routines. */

/* basic/qsort.c */
void basic_qsort (int table[], int  i, int j,
                  int (*compare) (const int *, const int *)); 

/* basic/tokenize.c */
int  basic_tokenize (char string[], char *token[], int maximum);    

/*--------------------------------------------------------------------------*/

/* basic/math2.c --- math substitutes and some integer math */

int basic_ipower (int x, int y);

 double log2  (double x);
 double exp2  (double x);
 double exp10 (double x);

#if defined (is_ANSI_C)
 double cbrt (double x);
#endif

/*--------------------------------------------------------------------------*/

/* MODULE basic/iit.c --- Integer interval tree [**] */

typedef  char*  Basic_iit;  /* Abstract data type! */

typedef struct basic_iit_info_type
{
  int n;
  int nodes;
  double lowsort_time;
  double buildup_time;
  int bytes, bytes_tree, bytes_array;
  int max_depth, filled_depth;
  double avg_depth;
  double avg_nodesize;
} Basic_iit_info;

Basic_iit       basic_iit_build (int n, int e[],
                                 int (* low) (int), int (* high) (int),
                                 int (* is_contained) (int, int));
void            basic_iit_query (Basic_iit t, int x, void (* report) (int));
void            basic_iit_kill  (Basic_iit t);
Basic_iit_info* basic_iit_info  (const Basic_iit t);
extern int      basic_iit_proto_flag;

/*--------------------------------------------------------------------------*/

/* MODULE basic/uf.c -- Union/find data structure [**] */

typedef struct basic_uf_record_type       /* union and find data structure */
{
  int set;  /* pointer to root; for root, it is (negative) num. of elements */
  int next; /* next pointer */
} Basic_uf_record;

typedef struct basic_set_queue_type
{
  int num_elts;             /* Number of elements */
  int num_sets;             /* Number of sets */
  Basic_uf_record *ufelts;  /* Array of elements and their sets */
  int *root_list;           /* Array of roots */
} Basic_set_queue;

void             basic_uf_set      (Basic_set_queue *sq);
void             basic_uf_create   (int n);
int              basic_uf_find     (int i);
void             basic_uf_add      (int i);
void             basic_uf_delete   (int i);
void             basic_uf_union    (int i, int j);
Basic_set_queue* basic_uf_collect  (void);
int              basic_uf_set_size (int i);
int              basic_uf_num_sets (void);
void             basic_uf_free     (Basic_set_queue *sq);
void             basic_uf_print    (void);

#ifdef __cplusplus
}
#endif

#ifndef Bool
typedef int Bool;
#endif

#endif  /* #ifndef __BASIC_H__ */
