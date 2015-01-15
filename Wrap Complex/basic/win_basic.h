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

#ifndef __RGI_WIN_BASIC_H__
#define __RGI_WIN_BASIC_H__

#ifdef  _WIN32

//#ifdef  _MSC_VER
//#undef  _MSC_VER
//#define _MSC_VER 1000
//#endif

#include <windows.h>
//#include <mapiwin.h>
#include <assert.h>

/*
#ifndef CONST
#define CONST const
#endif
#ifndef BOOL
typedef int BOOL;
#endif
#ifndef INT
typedef int INT;
#endif
#ifndef BYTE
typedef unsigned char BYTE;
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
*/

#define BZERO(P,T,N)  do { if ((N) > 0) ZeroMemory((void*)(P), (DWORD) sizeof(T)*(N)); } while(0)
#define BCOPY(S,D,L)  do { if ((L) > 0) CopyMemory((void*)(D), (void*)(S), (DWORD) (L)); } while (0)
#define strcasecmp(S1,S2)  _stricmp(S1, S2)
#define strncasecmp(S1, S2, N) _strnicmp(S1, S2, N)

#ifdef __DEBUG__
#define Assert(ex)  assert(ex)
#define Assert_if(FLAG,EX)  do { if (FLAG) assert(EX); } while (0)
#else
#define Assert(EX)
#define Assert_if(FLAG,EX)
#endif

#define Assert_always(EX) assert(EX)

#endif /* WIN32 */
#endif /* __WIN_BASIC_H__ */
