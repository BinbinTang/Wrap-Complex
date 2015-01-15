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
#ifndef __RGI_XDR_H__
#define __RGI_XDR_H__

#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#define PFILE HANDLE
#else
#define PFILE FILE *
typedef unsigned int HANDLE;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
  enum xdr_op {
	XDR_ENCODE = 0,
	XDR_DECODE = 1,
	XDR_FREE = 2
};
typedef int (*xdrproc_t)();

#define xdr_char     0
#define xdr_u_char   0
#define xdr_short    0
#define xdr_u_short  0
#define xdr_int      0
#define xdr_u_int    0
#define xdr_float    0
#define xdr_double   0

#else /* Unix */

#include <sys/types.h>
#include <unistd.h>
#include <rpc/types.h>
#include <rpc/xdr.h>

#endif


void xdrWriteShorts (PFILE fp, short *dataArray, int nofNum);
void xdrReadShorts (PFILE fp, short *dataArray, int nofNum);
void xdrWriteInts (PFILE fp, int *dataArray, int nofNum);
void xdrReadInts (PFILE fp, int *dataArray, int nofNum);
void xdrWriteUnsignedInts (PFILE fp, unsigned int *dataArray, int nofNum);
void xdrReadUnsignedInts (PFILE fp, unsigned int *dataArray, int nofNum);
void xdrWriteFloats (PFILE fp, float *dataArray, int nofNum);
void xdrReadFloats (PFILE fp, float *dataArray, int nofNum);
void xdrWriteDoubles (PFILE fp, double *dataArray, int nofNum);
void xdrReadDoubles (PFILE fp, double *dataArray, int nofNum);
void xdrWriteChars (PFILE fp, char *dataArray, int nofNum);
void xdrReadChars (PFILE fp, char *dataArray, int nofNum);
void xdrWriteUnsignedChars (PFILE fp, unsigned char *dataArray, int nofNum);
void xdrReadUnsignedChars (PFILE fp, unsigned char *dataArray, int nofNum);

void xdrBeginMultipleWrites (PFILE fp);
void xdrBeginMultipleReads (PFILE fp);
void xdrEndMultipleWrites (PFILE fp);
void xdrEndMultipleReads (PFILE fp);

void xdrConvertToXdrInts (int *src, int *dst, int nofNum);
void xdrConvertFromXdrInts (int *src, int *dst, int nofNum);
void xdrConvertToXdrUnsignedInts (unsigned int *src, unsigned int *dst, 
				  int nofNum);
void xdrConvertFromXdrUnsignedInts (unsigned int *src, unsigned int *dst, 
				    int nofNum);
void xdrConvert (char *src, char *dst, int eltSize, int nofNum, 
	        enum xdr_op xdrReadWrite, xdrproc_t xdrFun);

#ifdef __cplusplus
};
#endif

#endif /* __XDR_H__ */
