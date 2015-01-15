#include <basic/comment.h>
#include <basic/convert.h>

//----------------------------------------------------------------------------
// Utility functions
//----------------------------------------------------------------------------

#ifdef __DEBUG__
static BOOL checkDefines ()
{
//	COMMENT ("checking machine word sizes");
	Assert (sizeof (short) == MACHINE_SHORT_SIZE);
	Assert (sizeof (unsigned short) == MACHINE_SHORT_SIZE);
	Assert (sizeof (long) == MACHINE_LONG_SIZE);
	Assert (sizeof (unsigned long) == MACHINE_LONG_SIZE);
#if defined(IEEE_754)
	Assert (sizeof (float) == 4);
	Assert (sizeof (double) == 8);
#endif
	return TRUE;
}

static BOOL foo = checkDefines ();
#endif

static inline void
swapBytes (char *data, size_t size)
{
	for (size_t i=0; i<size/2; i++) {
		char tmp = data[i];
		data[i] = data[size-1-i];
		data[size-1-i] = tmp;
	}
}

static inline void
fromLittleEndian (char *data, size_t size)
{
#if defined(BIG_ENDIAN)
	swapBytes (data, size);
#endif
}

static inline void
fromBigEndian (char *data, size_t size)
{
#if defined(LITTLE_ENDIAN)
	swapBytes (data, size);
#endif
}

//----------------------------------------------------------------------------
// Reading little endian
//----------------------------------------------------------------------------

template<class T>
BOOL
readIntegerLI (FILE *f, T *t, size_t size)
{
	unsigned char b[8];
	if (!fread (b, size, 1, f)) {
		return FALSE;
	}
	*t = 0;
	for (int i=0; i<size; i++) {
		*t = ((*t) << 8) | (T)b[size-i-1];
	}
	return TRUE;
}

BOOL
readShortLI (FILE *f, short *s)
{
#if (MACHINE_SHORT_SIZE == 2)
	BOOL result = fread (s, 2, 1, f);
	fromLittleEndian ((char *)s, 2);
	return result;
#else
	return readIntegerLI (f, s, 2);
#endif
}

BOOL
readShortsLI (FILE *f, short *s, size_t count) {
	while (count-- != 0) {
		if (!readShortLI (f, s++)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL
readShortLI (FILE *f, unsigned short *s)
{
#if (MACHINE_SHORT_SIZE == 2)
	BOOL result = fread (s, 2, 1, f);
	fromLittleEndian ((char *)s, 2);
	return result;
#else
	return readIntegerLI (f, s, 2);
#endif
}

BOOL
readShortsLI (FILE *f, unsigned short *s, size_t count) {
	while (count-- != 0) {
		if (!readShortLI (f, s++)) {
			return FALSE;
		}
	}
	return TRUE;
}
BOOL
readLongLI (FILE *f, long *l)
{
#if (MACHINE_LONG_SIZE == 4)
	BOOL result = fread (l, 4, 1, f);
	fromLittleEndian ((char *)l, 4);
	return result;
#else
	return readIntegerLI (f, l, 4);
#endif
}

BOOL
readLongsLI (FILE *f, long *l, size_t count) {
	while (count-- != 0) {
		if (!readLongLI (f, l++)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL
readLongLI (FILE *f, unsigned long *l)
{
#if (MACHINE_LONG_SIZE == 4)
	BOOL result = fread (l, 4, 1, f);
	fromLittleEndian ((char *)l, 4);
	return result;
#else
	return readIntegerLI (f, l, 4);
#endif
}

BOOL
readLongsLI (FILE *f, unsigned long *l, size_t count) {
	while (count-- != 0) {
		if (!readLongLI (f, l++)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL
readFloatLI (FILE *f, float *s)
{
	BOOL result = fread (s, 4, 1, f);
	fromLittleEndian ((char *)s, 4);
	return result;
}

BOOL
readFloatsLI (FILE *f, float *s, size_t count) {
	while (count-- != 0) {
		if (!readFloatLI (f, s++)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL
readDoubleLI (FILE *f, double *d)
{
	BOOL result = fread (d, 8, 1, f);
	fromLittleEndian ((char *)d, 8);
	return result;
}

BOOL
readDoublesLI (FILE *f, double *d, size_t count) {
	while (count-- != 0) {
		if (!readDoubleLI (f, d++)) {
			return FALSE;
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------
// Reading big endian
//----------------------------------------------------------------------------

template<class T>
BOOL
readIntegerBI (FILE *f, T *t, size_t size)
{
	unsigned char b[8];
	if (!fread (b, size, 1, f)) {
		return FALSE;
	}
	*t = 0;
	for (int i=0; i<size; i++) {
		*t = ((*t) << 8) | (T)b[i];
	}
	return TRUE;
}

BOOL
readShortBI (FILE *f, short *s)
{
#if (MACHINE_SHORT_SIZE == 2)
	BOOL result = fread (s, 2, 1, f);
	fromBigEndian ((char *)s, 2);
	return result;
#else
	return readIntegerBI (f, s, 2);
#endif
}

BOOL
readShortsBI (FILE *f, short *s, size_t count) {
	while (count-- != 0) {
		if (!readShortBI (f, s++)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL
readShortBI (FILE *f, unsigned short *s)
{
#if (MACHINE_SHORT_SIZE == 2)
	BOOL result = fread (s, 2, 1, f);
	fromBigEndian ((char *)s, 2);
	return result;
#else
	return readIntegerBI (f, s, 2);
#endif
}

BOOL
readShortsBI (FILE *f, unsigned short *s, size_t count) {
	while (count-- != 0) {
		if (!readShortBI (f, s++)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL
readLongBI (FILE *f, long *l)
{
#if (MACHINE_LONG_SIZE == 4)
	BOOL result = fread (l, 4, 1, f);
	fromBigEndian ((char *)l, 4);
	return result;
#else
	return readIntegerBI (f, l, 4);
#endif
}

BOOL
readLongsBI (FILE *f, long *l, size_t count) {
	while (count-- != 0) {
		if (!readLongBI (f, l++)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL
readLongBI (FILE *f, unsigned long *l)
{
#if (MACHINE_LONG_SIZE == 4)
	BOOL result = fread (l, 4, 1, f);
	fromBigEndian ((char *)l, 4);
	return result;
#else
	return readIntegerBI (f, l, 4);
#endif
}

BOOL
readLongsBI (FILE *f, unsigned long *l, size_t count) {
	while (count-- != 0) {
		if (!readLongBI (f, l++)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL
readFloatBI (FILE *f, float *s)
{
	BOOL result = fread (s, 4, 1, f);
	fromBigEndian ((char *)s, 4);
	return result;
}

BOOL
readFloatsBI (FILE *f, float *s, size_t count) {
	while (count-- != 0) {
		if (!readFloatBI (f, s++)) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL
readDoubleBI (FILE *f, double *d)
{
	BOOL result = fread (d, 8, 1, f);
	fromBigEndian ((char *)d, 8);
	return result;
}

BOOL
readDoublesBI (FILE *f, double *d, size_t count) {
	while (count-- != 0) {
		if (!readDoubleBI (f, d++)) {
			return FALSE;
		}
	}
	return TRUE;
}

