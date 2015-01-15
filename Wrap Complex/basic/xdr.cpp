
#ifdef WRAP_DEBUG
#define __DEBUG__
#endif

#include "basic.h"
#include "xdr.h"

/*---------------------------------------------------------------------------*/

static void
xdrSingleVectorIO (PFILE fp, char *i, int eltSize, int nofNum, 
		   enum xdr_op xdrReadWrite, xdrproc_t xdrFun)

{
#ifdef WIN32
	char buf[16];
	DWORD res;
	int k; /* chars within elements */
	int n; /* element counter */
	switch(xdrReadWrite) {
	case XDR_ENCODE:
		for(n=0;n<eltSize*nofNum;n+=eltSize) {
		    for(k=0;k<eltSize;k++) {
			    buf[k] = i[n+eltSize-1-k];
		    }
		    WriteFile(fp, buf, eltSize, &res, NULL);
		}
		break;
	case XDR_DECODE:
		for(n=0;n<eltSize*nofNum;n+=eltSize) {
			ReadFile(fp, buf, eltSize, &res, NULL);
			for(k=0;k<eltSize;k++) {
				i[n+eltSize-1-k] = buf[k];
			}
		}
		break;
	default: 
		break;
	}
#else /* for Unix - use standard xdr */
  typedef size_t (*readFunType) (void *, size_t, size_t, FILE *);
  typedef size_t (*writFunType) (const void *, size_t, size_t, FILE *);
  static readFunType fReadWrite[2] = { (readFunType) fwrite, fread };
    
  XDR xdrEncodeStream;

  Assert (nofNum >= 0);

  if (nofNum == 0) return;

  /* chars are always transportable as they are. xdr uses 4 bytes for */
  /* every char. so we do our own thing */

  if (eltSize == 1) {
    if (fReadWrite[xdrReadWrite == XDR_ENCODE ? 0 : 1] 
	  (i, eltSize, nofNum, fp) != nofNum)
      basic_error ("ERROR: xdrSingleVectorIO: problem reading/writing chars");
    return;
  }

  xdrstdio_create (&xdrEncodeStream, fp, xdrReadWrite);

  if (! xdr_vector (&xdrEncodeStream, i, (u_int) nofNum, eltSize, xdrFun)) 
    basic_error ("ERROR: xdrSingleVectorIO: problem writing vector");

  xdr_destroy (&xdrEncodeStream);
#endif
}

/*---------------------------------------------------------------------------*/

static void (*xdrVectorIO) (PFILE, char *, int, int, enum xdr_op, xdrproc_t)
                           = xdrSingleVectorIO;
static PFILE multipleFP;

#ifndef WIN32
static XDR xdrEncodeStreamMultiple;
#endif

/*---------------------------------------------------------------------------*/

static void
xdrMultipleVectorIO (PFILE fp, char *i, int eltSize, int nofNum, 
		     enum xdr_op xdrReadWrite, xdrproc_t xdrFun)

{
  Assert (fp == multipleFP);
  Assert (nofNum >= 0);
  Assert (eltSize > 1);

  if (nofNum == 0) return;

#ifdef WIN32 /* multiple and single vector IOs are identical */
  xdrSingleVectorIO(fp, i, eltSize, nofNum, xdrReadWrite, xdrFun);
#else
  if (! xdr_vector (&xdrEncodeStreamMultiple, i, (u_int) nofNum, eltSize, 
		    xdrFun)) 
    basic_error ("ERROR: xdrMultipleVectorIO: problem writing vector");
#endif
}

/*---------------------------------------------------------------------------*/

static void
xdrBeginMultipleIO (PFILE fp, enum xdr_op xdrReadWrite)

{
  Assert (xdrVectorIO != xdrMultipleVectorIO);

  multipleFP = fp;
#ifndef WIN32
  xdrstdio_create (&xdrEncodeStreamMultiple, fp, xdrReadWrite);
#endif
  xdrVectorIO = xdrMultipleVectorIO;
}

/*---------------------------------------------------------------------------*/

static void
xdrEndMultipleIO (PFILE fp)

{
  Assert (xdrVectorIO == xdrMultipleVectorIO);
  Assert (fp == multipleFP);
#ifndef WIN32
  xdr_destroy(&xdrEncodeStreamMultiple);
#endif
  xdrVectorIO = xdrSingleVectorIO;
}

/*---------------------------------------------------------------------------*/

void
xdrBeginMultipleWrites (PFILE fp) { xdrBeginMultipleIO (fp, XDR_ENCODE); }
void
xdrBeginMultipleReads (PFILE fp) { xdrBeginMultipleIO (fp, XDR_DECODE); }
void
xdrEndMultipleWrites (PFILE fp) { xdrEndMultipleIO (fp); }
void
xdrEndMultipleReads (PFILE fp) { xdrEndMultipleIO (fp); }

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
xdrWriteInts (PFILE fp, int *dataArray, int nofNum)

{
  xdrVectorIO (fp, (char *) dataArray, sizeof (int), nofNum, XDR_ENCODE, 
	       (xdrproc_t) xdr_int);
}

/*---------------------------------------------------------------------------*/

void
xdrReadInts (PFILE fp, int *dataArray, int nofNum)

{
  xdrVectorIO (fp, (char *) dataArray, sizeof (int), nofNum, XDR_DECODE, 
	       (xdrproc_t) xdr_int);
} 

/*---------------------------------------------------------------------------*/

void
xdrWriteUnsignedInts (PFILE fp, unsigned int *dataArray, int nofNum)

{
  xdrVectorIO (fp, (char *) dataArray, sizeof (unsigned int), nofNum, 
	       XDR_ENCODE, (xdrproc_t) xdr_u_int);
}

/*---------------------------------------------------------------------------*/

void
xdrReadUnsignedInts (PFILE fp, unsigned int *dataArray, int nofNum)

{
  xdrVectorIO (fp, (char *) dataArray, sizeof (unsigned int), nofNum, 
	       XDR_DECODE, (xdrproc_t) xdr_u_int);
} 

/*---------------------------------------------------------------------------*/

void
xdrWriteShorts (PFILE fp, short *dataArray, int nofNum)

{
  xdrVectorIO (fp, (char *) dataArray, sizeof (short), nofNum, XDR_ENCODE, 
	       (xdrproc_t) xdr_short);
}

/*---------------------------------------------------------------------------*/

void
xdrReadShorts (PFILE fp, short *dataArray, int nofNum)

{
  xdrVectorIO (fp, (char *) dataArray, sizeof (short), nofNum, XDR_DECODE, 
	       (xdrproc_t) xdr_short);
} 

/*---------------------------------------------------------------------------*/

void
xdrWriteFloats (PFILE fp, float *dataArray, int nofNum)

{
  xdrVectorIO (fp, (char *) dataArray, sizeof (float), nofNum, XDR_ENCODE, 
	       (xdrproc_t) xdr_float);
}

/*---------------------------------------------------------------------------*/

void
xdrReadFloats (PFILE fp, float *dataArray, int nofNum)

{
  xdrVectorIO (fp, (char *) dataArray, sizeof (float), nofNum, XDR_DECODE, 
	       (xdrproc_t) xdr_float);
} 

/*---------------------------------------------------------------------------*/

void
xdrWriteDoubles (PFILE fp, double *dataArray, int nofNum)

{
  xdrVectorIO (fp, (char *) dataArray, sizeof (double), nofNum, XDR_ENCODE, 
	       (xdrproc_t) xdr_double);
}

/*---------------------------------------------------------------------------*/

void
xdrReadDoubles (PFILE fp, double *dataArray, int nofNum)

{
  xdrVectorIO (fp, (char *) dataArray, sizeof (double), nofNum, XDR_DECODE, 
	       (xdrproc_t) xdr_double);
} 

/*---------------------------------------------------------------------------*/

void
xdrWriteChars (PFILE fp, char *dataArray, int nofNum)

{
  Assert (xdrVectorIO == xdrSingleVectorIO);

  xdrVectorIO (fp, dataArray, sizeof (char), nofNum, XDR_ENCODE, 
	       (xdrproc_t) xdr_char);
}

/*---------------------------------------------------------------------------*/

void
xdrReadChars (PFILE fp, char *dataArray, int nofNum)

{
  Assert (xdrVectorIO == xdrSingleVectorIO);

  xdrVectorIO (fp, dataArray, sizeof (char), nofNum, XDR_DECODE, 
	       (xdrproc_t) xdr_char);
} 

/*---------------------------------------------------------------------------*/

void
xdrWriteUnsignedChars (PFILE fp, unsigned char *dataArray, int nofNum)

{
  Assert_always (xdrVectorIO == xdrSingleVectorIO);

  xdrVectorIO (fp, (char *) dataArray, sizeof (unsigned char), nofNum, 
	       XDR_ENCODE, (xdrproc_t) xdr_u_char);
}

/*---------------------------------------------------------------------------*/

void
xdrReadUnsignedChars (PFILE fp, unsigned char *dataArray, int nofNum)

{
  Assert (xdrVectorIO == xdrSingleVectorIO);

  xdrVectorIO (fp, (char *) dataArray, sizeof (unsigned char), nofNum, 
	       XDR_DECODE, (xdrproc_t) xdr_u_char);
} 

/*---------------------------------------------------------------------------*/

void
xdrConvert (char *src, char *dst, int eltSize, int nofNum, 
	    enum xdr_op xdrReadWrite, xdrproc_t xdrFun)

{
#ifdef WIN32
int n, k;

  Assert (nofNum >= 0);

  if (nofNum == 0) return;

  for(n=0; n<eltSize*nofNum; n+=eltSize) {
	  for(k=0;k<eltSize;k++) {
		  dst[n+k] = src[n+eltSize-1-k];
	  }
  }

#else
  XDR xdrEncodeStream;

  Assert (nofNum >= 0);

  if (nofNum == 0) return;

  xdrmem_create (&xdrEncodeStream, (xdrReadWrite == XDR_ENCODE ? dst : src), 
		 eltSize * nofNum, xdrReadWrite);

  if (! xdr_setpos (&xdrEncodeStream, 0)) 
    basic_error ("ERROR: xdrConvert: problem positioning xdr");

  if (! xdr_vector (&xdrEncodeStream, (xdrReadWrite == XDR_ENCODE ? src : dst),
		    (u_int) nofNum, eltSize, xdrFun)) 
    basic_error ("ERROR: xdrConvert: problem converting vector");

  xdr_destroy (&xdrEncodeStream);
#endif
}

/*---------------------------------------------------------------------------*/

void
xdrConvertToXdrInts (int *src, int *dst, int nofNum)

{
  xdrConvert ((char *) src, (char *) dst, sizeof (int), nofNum, 
	      XDR_ENCODE, (xdrproc_t) xdr_int);
} 

/*---------------------------------------------------------------------------*/

void
xdrConvertFromXdrInts (int *src, int *dst, int nofNum)

{
  xdrConvert ((char *) src, (char *) dst, sizeof (int), nofNum, 
	      XDR_DECODE, (xdrproc_t) xdr_int);
} 

/*---------------------------------------------------------------------------*/

void
xdrConvertToXdrUnsignedInts (unsigned int *src, unsigned int *dst, int nofNum)

{
  xdrConvert ((char *) src, (char *) dst, sizeof (unsigned int), nofNum, 
	      XDR_ENCODE, (xdrproc_t) xdr_u_int);
} 

/*---------------------------------------------------------------------------*/

void
xdrConvertFromXdrUnsignedInts (unsigned int *src, unsigned int *dst, 
			       int nofNum)

{
  xdrConvert ((char *) src, (char *) dst, sizeof (unsigned int), nofNum, 
	      XDR_DECODE, (xdrproc_t) xdr_u_int);
} 

/*---------------------------------------------------------------------------*/




