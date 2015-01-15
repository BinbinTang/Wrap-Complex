#include <basic/binio.h>
#include <basic/convert.h>

//#include <stdlib.h>
//#include <stdio.h>
#include <cstdlib>
#include <cstdio>
#include <fcntl.h>

#ifdef unix
#include <unistd.h>
#endif

#ifdef _WINDOWS
#include <io.h>
#else
#define _read read
#define _write write
#endif

/*----------------------------------------------------------------*/

/*
int BinInput::
readArray_ (char * buf, int eltSize, int nofNum, void* func)
{
  long res = -1;
#ifdef _WINDOWS
  unsigned long res2;
#endif
  char * bufp = buf_;
  int totSize = eltSize*nofNum;
  int readSize;

  if (eltSize > 1)
  {
    readSize = totSize < BUF_SIZE? totSize : BUF_SIZE;
  }
  else // special for char
  {
    readSize = totSize;
    bufp = buf;
  }
  
  switch (type_)
  {
    case FilePointer::IStream: 
	          //while(res < readSize) is_->get(bufp[res++]);
                  is_->read(bufp, res = readSize);
                  break;
#ifdef _WINDOWS
    case FilePointer::Handle:  
		  res = ReadFile(h_, bufp, readSize, &res2, NULL);
		  if (res)
			  res = res2;
                  break;
#endif
    case FilePointer::File:
                  // while(res < readSize) bufp[res++] = fgetc(fp_);
                  // res = _read(fileno(fp_), bufp, readSize);
                  res = fread (bufp, 1, readSize, fp_);
                  break;
    default:      assert (0);
  }

  assert(res >= 0);

  if(eltSize > 1)
  {
    xdrConvert(buf_, buf, eltSize, res/eltSize,
               XDR_DECODE, (xdrproc_t) func);
  }

  if (res == 0) return 0;
 
  else if (res == totSize) return res / eltSize;

  else if (res < totSize)
  return res/eltSize + readArray_(buf + res, eltSize,
                       nofNum - res/eltSize, func);
  else return -1;
}
*/

int BinInput::
readRawData (char **buf, int size)
{
	long res = -1;
#ifdef _WINDOWS
	unsigned long res2;
#endif
	int readSize = size < BUF_SIZE ? size : BUF_SIZE;
  
	switch (type_)
	{
	case FilePointer::IStream: 
		is_->read(buf_, res = readSize);
		break;
#ifdef _WINDOWS
	case FilePointer::Handle:  
		res = ReadFile(h_, buf_, readSize, &res2, NULL);
		if (res) res = res2;
		break;
#endif
	case FilePointer::File:
		res = fread (buf_, 1, readSize, fp_);
		break;
	default:
		assert (0);
	}

	assert(res >= 0);

	*buf = buf_;

	return res;
}

int BinInput::
readRawData (char *data, int size)
{
	long res = -1;
#ifdef _WINDOWS
	unsigned long res2;
#endif
  
	switch (type_)
	{
	case FilePointer::IStream: 
		is_->read(data, res = size);
		break;
#ifdef _WINDOWS
	case FilePointer::Handle:  
		res = ReadFile(h_, data, size, &res2, NULL);
		if (res) res = res2;
		break;
#endif
	case FilePointer::File:
		res = fread (data, 1, size, fp_);
		break;
	default:
		assert (0);
	}

	assert(res >= 0);

	return res;
}

int BinOutput::
writeRawData (char **buf, int size)
{
	if (size > BUF_SIZE)
		size = BUF_SIZE;
	*buf = buf_;
	return size;
}

int BinOutput::
flushRawData (int size)
{
	unsigned long result;

	switch (type_)
	{
	case FilePointer::OStream: 
		os_->write(buf_, result = size);
		break;
#ifdef _WINDOWS
	case FilePointer::Handle:
		WriteFile(h_, buf_, size, &result, (LPOVERLAPPED)0);
		break;
#endif
	case FilePointer::File:
		result = _write(_fileno(fp_), buf_, size);
		break;
	default:
		assert (0);
	}
	return result;
}

int BinOutput::
writeRawData (const char *data, int size)
{
	unsigned long result;

	switch (type_)
	{
	case FilePointer::OStream: 
		os_->write(data, result = size);
		break;
#ifdef _WINDOWS
	case FilePointer::Handle:
		WriteFile(h_, data, size, &result, (LPOVERLAPPED)0);
		break;
#endif
	case FilePointer::File:
		result = _write(_fileno(fp_), data, size);
		break;
	default:
		assert (0);
	}
	return result;
}

/*----------------------------------------------------------------*/

int BinInput::
queryInt (void)
{
  int res;
  if (readInts(&res, 1) != 1) return -1;

  int pos = 0;
  // putback filepointer
  switch (type_)
  {
    case FilePointer::IStream:
			   pos = is_->tellg();
			   pos -= INT_SIZE;
               //is_->seekg(is_->tellg() - INT_SIZE);
			   is_->seekg( pos );
               break;
#ifdef _WINDOWS
    case FilePointer::Handle:
               SetFilePointer(h_, -INT_SIZE, 0, FILE_CURRENT);
               break;
#endif
    case FilePointer::File:
               fseek(fp_, -INT_SIZE, SEEK_CUR);
               break;
    default: assert (0);
  }
  return res;
}


/*----------------------------------------------------------------*/
/*
int BinOutput::
writeArray_ (char * buf, int eltSize, int nofNum, void* func)
{
  unsigned long res = 0;
  char * bufp = buf_;
  unsigned long totSize = (unsigned long) eltSize*nofNum;
  int writeSize;
  
  if(eltSize > 1)
  {
    writeSize = totSize < BUF_SIZE? totSize : BUF_SIZE;
    xdrConvert(buf, buf_, eltSize, writeSize/eltSize,
               XDR_ENCODE, (xdrproc_t) func);
  }
  else // no conversion for char
  {
    writeSize = totSize;
    bufp = buf;
  }

  switch (type_)
  {
    case FilePointer::OStream: 
		  // while(res < writeSize)  os_->put(bufp[res++]);
                  os_->write(bufp, res = writeSize);
                  break;
#ifdef _WINDOWS
    case FilePointer::Handle:  
		  WriteFile(h_, bufp, writeSize, &res, NULL);
                  break;
#endif
    case FilePointer::File:
                  // while(res < writeSize) fputc(bufp[res++], fp_);
                  res = _write(fileno(fp_), bufp, writeSize);
                  break;

    default:      assert (0);
  }

  assert(res != 0);
  if (res == totSize) return res/eltSize;
  else if (res < totSize)
  return res/eltSize + writeArray_(buf + res, eltSize,
                       nofNum - res/eltSize, func);
  else return -1;
 
}
*/

/*----------------------------------------------------------------*/

// Magic number manipulators

Magic::Magic (BinInput &bi, int magic) 
   { 
       magic_ = bi.readInt(); 
       assert(magic_ == magic);
   }

Magic::Magic (FilePointer s, int magic)
   { 
     BinInput bi(s);
     magic_ = bi.readInt();
       assert(magic_ == magic);
   }

Magic::Magic (int magic) 
: magic_(magic) { }

Magic::Magic (int id, int version)
{
  assert(version >= 0 && version <= 255);
  if(id < 0)
    magic_ = id;
  else
    magic_ = ((id << 8) | version);
}

Magic::~Magic (void)  { }

int  Magic::getId(void) const
{ 
  return magic_;
}

int Magic::index (void) const
{
  return (magic_ >= 0)? (magic_ >> 8) : -1;
}

int Magic::version(void) const
{
  return (magic_ >= 0)? (magic_ & 255) : -1;
}

Bool Magic::binInit(FilePointer s, int versionedMagic) 
{ 
  BinInput bi(s); magic_ = bi.readInt(); return (magic_ == versionedMagic); 
}

Bool Magic::binInit (BinInput& bi, int versionedMagic) 
{ 
  magic_ = bi.readInt(); return (magic_ == versionedMagic); 
}

Bool Magic::binInit(FilePointer s) 
{ 
  BinInput bi(s); int magic = bi.readInt(); return (magic_ == magic); 
}

Bool Magic::binInit (BinInput& bi) 
{ 
  int versionedMagic = magic_;
  magic_ = bi.readInt(); 
  return (magic_ == versionedMagic); 
}

void Magic::binDump(BinOutput& bo) 
{ 
  bo.writeInt(magic_); 
}

void Magic::binDump(FilePointer s) 
{ 
  BinOutput bo (s); binDump (bo); 
}


ostream&  operator<< (ostream& s, const Magic& m) 
{ 
  s << (m.magic_ >> 8) << " " << (m.magic_ & 255); return s; 
}

//---------------------------------------------------------------------------
// Low level read/write
//---------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Integer input
//--------------------------------------------------------------------------

/*
template<class T>
void decodeInteger (T *target, char *source, size_t size, size_t count)
{
	while (count-- > 0)
	{
		T result = ((T)source[0] >= 0) ? (T)0 : (T)-1;
		for (size_t i=0; i<size; i++)
		{
			result <<= 8;
			char next = source[i];
			unsigned char tmp = (unsigned char) next;
			T add = (T) tmp;
			result |= add;
		}
		*(target++) = result;
		source += size;
	}
}
*/

template<class T>
int readInteger (BinInput *binIO, T *buf, size_t size, size_t count)
{
	size_t have = 0;
	while (count > 0)
	{
		char *source;
		size_t read = binIO->readRawData (&source, size * count) / size;
		Assert (read <= count);
		if (!read) return 0;
//		decodeInteger<T> (buf+have, source, size, read);
		decodeInteger(buf+have, source, size, read);
		have += read;
		count -= read;
	}
	return 1;
}

int BinInput::
readInts (int *buf, int count)
{
//	return readInteger<int> (this, buf, INT_SIZE, count);
	return readInteger(this, buf, INT_SIZE, count);
}

int BinInput::
readUnsignedInts (unsigned int *buf, int count)
{
//	return readInteger<unsigned int> (this, buf, INT_SIZE, count);
	return readInteger(this, buf, INT_SIZE, count);
}

int BinInput::
readChars (char *buf, int count)
{
//	return readInteger<char> (this, buf, CHAR_SIZE, count);
	return readInteger(this, buf, CHAR_SIZE, count);
}

int BinInput::
readUnsignedChars (unsigned char *buf, int count)
{
//	return readInteger<unsigned char> (this, buf, CHAR_SIZE, count);
	return readInteger(this, buf, CHAR_SIZE, count);
}

int BinInput::
readShorts (short *buf, int count)
{
//	return readInteger<short> (this, buf, SHORT_SIZE, count);
	return readInteger(this, buf, SHORT_SIZE, count);
}

int BinInput::
readUnsignedShorts (unsigned short *buf, int count)
{
//	return readInteger<unsigned short> (this, buf, SHORT_SIZE, count);
	return readInteger(this, buf, SHORT_SIZE, count);
}

//--------------------------------------------------------------------------
// Floating Point input
//--------------------------------------------------------------------------

/*
template<class T>
void decodeFloating (T *target, char *source, size_t size, size_t count)
{
	while (count-- > 0)
	{
		char *tmp = (char *)target;
		for (size_t i=0; i<size; i++)
		{
#if defined(BIG_ENDIAN)
			tmp[i] = source[i];
#else
			tmp[i] = source[size-1 - i];
#endif
		}
		target++;
		source += size;
	}
}
*/

template<class T>
int readFloating (BinInput *binIO, T *buf, size_t size, size_t count)
{
	size_t have = 0;
	while (count > 0)
	{
		char *source;
		size_t read = binIO->readRawData (&source, size * count) / size;
		Assert (read <= count);
		if (!read) return 0;
//		decodeFloating<T> (buf+have, source, size, read);
		decodeFloating(buf+have, source, size, read);
		have += read;
		count -= read;
	}
	return 1;
}

int BinInput::
readFloats (float *buf, int count)
{
//	return readFloating <float> (this, buf, FLOAT_SIZE, count);
	return readFloating(this, buf, FLOAT_SIZE, count);
}

int BinInput::
readDoubles (double *buf, int count)
{
//	return readFloating<double> (this, buf, DOUBLE_SIZE, count);
	return readFloating(this, buf, DOUBLE_SIZE, count);
}


//--------------------------------------------------------------------------
// Integer output
//--------------------------------------------------------------------------

/*
template<class T>
void encodeInteger (char *target, T *source, size_t size, size_t count)
{
	while (count-- > 0)
	{
		T next = *source;
		for (size_t i=0; i<size; i++)
		{
			target[size-1 - i] = next & ((1<<8) - 1);
			next >>= 8;
		}
		target += size;
		source++;
	}
}
*/

template<class T>
int writeInteger (BinOutput *binIO, T *buf, size_t size, size_t count)
{
	size_t have = 0;
	while (count > 0)
	{
		char *target;
		size_t write = binIO->writeRawData (&target, size * count) / size;
		Assert (write <= count);
		if (!write) return 0;
//		encodeInteger<T> (target, buf+have, size, write);
		encodeInteger(target, buf+have, size, write);
		int ok = binIO->flushRawData (write * size) / size;
		Assert (ok == write);
		have += write;
		count -= write;
	}
	return 1;
}

int BinOutput::
writeInts (int *buf, int count)
{
//	return writeInteger<int> (this, buf, INT_SIZE, count);
	return writeInteger(this, buf, INT_SIZE, count);
}

int BinOutput::
writeUnsignedInts (unsigned int *buf, int count)
{
//	return writeInteger<unsigned int> (this, buf, INT_SIZE, count);
	return writeInteger(this, buf, INT_SIZE, count);
}

int BinOutput::
writeChars (char *buf, int count)
{
//	return writeInteger<char> (this, buf, CHAR_SIZE, count);
	return writeInteger(this, buf, CHAR_SIZE, count);
}

int BinOutput::
writeUnsignedChars (unsigned char *buf, int count)
{
//	return writeInteger<unsigned char> (this, buf, CHAR_SIZE, count);
	return writeInteger(this, buf, CHAR_SIZE, count);
}

int BinOutput::
writeShorts (short *buf, int count)
{
//	return writeInteger<short> (this, buf, SHORT_SIZE, count);
	return writeInteger(this, buf, SHORT_SIZE, count);
}

int BinOutput::
writeUnsignedShorts (unsigned short *buf, int count)
{
//	return writeInteger<unsigned short> (this, buf, SHORT_SIZE, count);
	return writeInteger(this, buf, SHORT_SIZE, count);
}

//--------------------------------------------------------------------------
// Floating Point output
//--------------------------------------------------------------------------

/*
template<class T>
void encodeFloating (char *target, T *source, size_t size, size_t count)
{
	while (count-- > 0)
	{
		char *tmp = (char *)source;
		for (size_t i=0; i<size; i++)
		{
#if defined(BIG_ENDIAN)
			target[i] = tmp[i];
#else
			target[i] = tmp[size-1 - i];
#endif
		}
		target += size;
		source ++;
	}
}
*/

template<class T>
int writeFloating (BinOutput *binIO, T *buf, size_t size, size_t count)
{
	size_t have = 0;
	while (count > 0)
	{
		char *target;
		size_t write = binIO->writeRawData (&target, size * count) / size;
		Assert (write <= count);
		if (!write) return 0;
//		encodeFloating<T> (target, buf+have, size, write);
		encodeFloating(target, buf+have, size, write);
		int ok = binIO->flushRawData (write * size) / size;
		Assert (ok == write);
		have += write;
		count -= write;
	}
	return 1;
}

int BinOutput::
writeFloats (float *buf, int count)
{
//	return writeFloating <float> (this, buf, FLOAT_SIZE, count);
	return writeFloating(this, buf, FLOAT_SIZE, count);
}

int BinOutput::
writeDoubles (double *buf, int count)
{
//	return writeFloating<double> (this, buf, DOUBLE_SIZE, count);
	return writeFloating(this, buf, DOUBLE_SIZE, count);
}
