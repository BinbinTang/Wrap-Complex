#ifndef __RGI_CONVERT_H__
#define __RGI_CONVERT_H__

// This is written in the hope that it will be portable. The code won't
// run on all machines yet, but it should fail on compile time.

// Wrap/Decimator/Shape use the following format for binary I/O:
//   + integer values are stores in the 2-complement format, big endian
//     - sizeof(char)   == 1
//     - sizeof(short)  == 2
//     - sizeof(int)    == 4
//     - sizeof(long)   == 4
//   + float and double are stores in IEEE754 format, big endian
//     - sizeof(float)  == 4
//     - sizeof(double) == 8

#include <string>
#include <basic/basic.h>

#undef LITTLE_ENDIAN
#undef BIG_ENDIAN
#undef IEEE_754

// Enter missing machine descriptions here

#if defined(linux)
#define LITTLE_ENDIAN
#define IEEE_754
#define MACHINE_SHORT_SIZE 2
#define MACHINE_LONG_SIZE 4
#endif

#if defined(WIN32) && defined(_M_IX86)
#define LITTLE_ENDIAN
#define IEEE_754
#define MACHINE_SHORT_SIZE 2
#define MACHINE_LONG_SIZE 4
#endif

#if defined(sgi)
#define BIG_ENDIAN
#define IEEE_754
#define MACHINE_SHORT_SIZE 2
#define MACHINE_LONG_SIZE 4
#endif

#if defined(LITTLE_ENDIAN) && defined(BIG_ENDIAN)
Endianess problem
#endif

#if !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
Endianess problem
#endif

#if !defined(IEEE_754)
Unknown floating point format
#endif

const int CHAR_SIZE = 1;
const int SHORT_SIZE = 2;
const int INT_SIZE = 4;
const int LONG_SIZE = 4;
const int FLOAT_SIZE = 4;
const int DOUBLE_SIZE = 8;

template<class T>
void decodeInteger (T *target, char *source, size_t size, size_t count);

template<class T>
void decodeFloating (T *target, char *source, size_t size, size_t count);

template<class T>
void encodeInteger (char *target, T *source, size_t size, size_t count);

template<class T>
void encodeFloating (char *target, T *source, size_t size, size_t count);

BOOL readShortLI (FILE *, short *);
BOOL readShortsLI (FILE *, short *, size_t count);
BOOL readShortLI (FILE *, unsigned short *);
BOOL readShortsLI (FILE *, unsigned short *, size_t count);
BOOL readLongLI (FILE *, long *);
BOOL readLongsLI (FILE *, long *, size_t count);
BOOL readLongLI (FILE *, unsigned long *);
BOOL readLongsLI (FILE *, unsigned long *, size_t count);
BOOL readFloatLI (FILE *, float *);
BOOL readFloatsLI (FILE *, float *, size_t count);
BOOL readDoubleLI (FILE *, double *);
BOOL readDoublesLI (FILE *, double *, size_t count);

BOOL readShortBI (FILE *, short *);
BOOL readShortsBI (FILE *, short *, size_t count);
BOOL readShortBI (FILE *, unsigned short *);
BOOL readShortsBI (FILE *, unsigned short *, size_t count);
BOOL readLongBI (FILE *, long *);
BOOL readLongsBI (FILE *, long *, size_t count);
BOOL readLongBI (FILE *, unsigned long *);
BOOL readLongsBI (FILE *, unsigned long *, size_t count);
BOOL readFloatBI (FILE *, float *);
BOOL readFloatsBI (FILE *, float *, size_t count);
BOOL readDoubleBI (FILE *, double *);
BOOL readDoublesBI (FILE *, double *, size_t count);

#include <basic/convert.hpp>

#endif

