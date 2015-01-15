  
#ifndef __RGI_UNIX_BASIC_H__
#define __RGI_UNIX_BASIC_H__

#include <sys/param.h>  /* constants like MAXPATHLEN, hopefully */
# include <strings.h>
# include <values.h>   /* for MAXINT, MAXFLOAT, BITS() ... */

#ifndef USESQT
#include <X11/Xmd.h> /* X11 Machine Dependent stuff */
#else
typedef char BYTE;
typedef int BOOL;
#endif

/* The rest are not defined in Xmd.h */
#ifndef USHRT_MAX
#define USHRT_MAX 65535
#endif
#ifndef CONST
#define CONST const
#endif
#ifndef INT
typedef int INT;
#endif
#ifndef DWORD
typedef unsigned long DWORD;
#endif
#ifndef WORD
typedef unsigned short WORD;
#endif
#ifndef FLOAT
typedef float FLOAT;
#endif
#ifndef PFLOAT
typedef float *PFLOAT;
#endif
#ifndef LPVOID
typedef void *LPVOID;
#endif
#ifndef LPCVOID
typedef CONST void *LPCVOID;
#endif
#ifndef LPSTR
typedef char *LPSTR;
#endif
#ifndef LPCSTR
typedef CONST char *LPCSTR;
#endif
#ifndef UINT
typedef unsigned int UINT;
#endif
#ifndef PUINT
typedef unsigned int *PUINT;
#endif
#ifndef LONG
typedef long LONG;
#endif
#ifndef ULONG
typedef unsigned long ULONG;
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* First, let's use SGI's symbols __sgi and sgi as synonyms. */
#if defined (__sgi) && !defined (sgi)
# define sgi
#endif
#if defined (sgi) && !defined (__sgi)
# define __sgi
#endif

/* ANSI C on an SGI? */
#if defined (sgi) && !defined (_SGI_SOURCE) && defined (__ANSI_CPP__)
# define is_ANSI_C
#endif

/* ...via gcc (eg NeXT, SUN)? */
#if (defined (__STRICT_ANSI__))
# define is_ANSI_C
#endif

#if defined (__alpha)
# define is_ANSI_C
#endif

/* Let's try to figure out if we have a 64-bit architecture... */
#if defined (_MIPS_SZPTR) && (_MIPS_SZPTR == 64)
# define is_64_bit_ARCH
#endif

#if defined (__alpha)
# define is_64_bit_ARCH
#endif

#ifdef __cplusplus
}
#endif

#if 0
# include <bstring.h>
#endif

/* These are already declared --swp
int bzero (const void *b, int length);
int bcopy (const void *src, void *dst, int length);
*/

#define BZERO(P,T,N)  bzero ((void*)P, (int)sizeof(T) * (N))
#define BCOPY(S,D,L)  bcopy ((void*)S, (void*)D, (int)L)

#ifdef __DEBUG__
#define Assert_always(EX) \
do { \
     if (not (EX)) basic_error (basic__assert_frmt, __FILE__, __LINE__); \
   } while (0)
#else
#define Assert_always(EX) \
do { \
     if (not (EX)) basic_error (basic__assert_frmt, __LINE__); \
   } while (0)
#endif

#ifdef __DEBUG__
# define Assert(EX)          Assert_always (EX)
# define Assert_if(FLAG,EX)  do { if (FLAG) Assert (EX); } while (0)
#else
# define Assert(EX)       
# define Assert_if(FLAG,EX) 
#endif


#endif /* __UNIX_BASIC_H__ */
