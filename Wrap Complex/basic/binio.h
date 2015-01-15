#ifndef __RGI_BINIO_H__
#define __RGI_BINIO_H__
/******************************************************
*
*
*    class BinInput
*
*
*/

//#include <fstream.h>
#include <iostream>
#include <fstream>
#include <strstream>
#include <basic/basic.h>

using namespace std;

#if defined(GNU_CC_28)
#define VOID_PTR (void *)
#else
#define VOID_PTR
#endif


class FilePointer
{
public:
 union {
 istream * is;
 ostream * os;
#ifdef _WINDOWS
 HANDLE h;
#endif
 FILE *  fp;
 };
 enum IOType {IStream,OStream, File, Handle};
 IOType iotype;
 FilePointer (istream &s): is(&s),iotype(IStream) {  }
 FilePointer (ostream &s): os(&s),iotype(OStream) {  }
#ifdef _WINDOWS
 FilePointer (HANDLE handle): h(handle),iotype(Handle) { }
#endif
 FilePointer (FILE * file): fp(file),iotype(File)  {  }

 ~FilePointer (void) { }
};


/******************************************************
*
*
*    class BinInputOutput
*
*
*/

#define BUF_SIZE 1024

class BinInputOutput
{
protected:
 union {
  istream * is_;
  ostream * os_;
#ifdef _WINDOWS
  HANDLE h_;
#endif
  FILE * fp_;
  };
  FilePointer::IOType type_;
  char buf_[BUF_SIZE];
public:
  BinInputOutput (FilePointer s):is_(s.is),type_(s.iotype){ }
  ~BinInputOutput (void) { }
};

/******************************************************
*
*
*    class BinInput
*
*
*/


class BinInput : public BinInputOutput
{
public:
  BinInput (FilePointer s);
  ~BinInput (void);
  int readInts (int * buf, int numElem);
  int readUnsignedInts(unsigned int * buf, int numElem);
  int readChars(char * buf, int numElem);
  int readUnsignedChars(unsigned char * buf, int numElem); 
  int readFloats(float * buf, int numElem);
  int readDoubles(double * buf,int numElem);
  int readShorts(short * buf, int numElem);
  int readUnsignedShorts(unsigned short * buf, int numElem);
  int queryInt (void);

  int readInt (void);
  unsigned int readUnsignedInt (void);
  char readChar (void);
  unsigned char readUnsignedChar (void);
  float readFloat (void);
  double readDouble (void);
  short readShort (void);
  unsigned short readUnsignedShort (void);

  int readRawData (char **buf, int size);
  int readRawData (char *buf, int size);
};

/******************************************************
*
*
*    class BinOutput
*
*
*/


class BinOutput : public BinInputOutput
{
public:
  BinOutput (FilePointer s);
  ~BinOutput (void);
  int writeInts (int * buf, int numElem);
  int writeUnsignedInts(unsigned int * buf, int numElem);
  int writeChars(char * buf, int numElem);
  int writeUnsignedChars(unsigned char * buf, int numElem);
  int writeFloats(float * buf, int numElem);
  int writeDoubles(double * buf,int numElem);
  int writeShorts(short * buf, int numElem);
  int writeUnsignedShorts(unsigned short * buf, int numElem);

  int writeInt (int i);
  int writeUnsignedInt (unsigned int i);
  int writeChar (char c);
  int writeUnsignedChar (unsigned char c);
  int writeShort (short s);
  int writeUnsignedShort (unsigned short s); 
  int writeFloat (float f);
  int writeDouble (double d);

  int writeRawData (char **buf, int size);
  int writeRawData (const char *buf, int size);
  int flushRawData (int size);
};

/******************************************************
*
*
*    class Magic
*
*
*/

class Magic
{
public:
  Magic (BinInput &bi, int magic);
  Magic (FilePointer s, int magic);
  Magic (int magic = -1);
  Magic (int id, int version);
  ~Magic (void);
  int  getId(void) const;
  int index (void) const;
  int version (void) const;
  Bool binInit (FilePointer s, int magic);
  Bool binInit (BinInput& bi, int magic);
  Bool binInit (FilePointer s);
  Bool binInit (BinInput& bi);
  void binDump(BinOutput& bo);
  void binDump(FilePointer s);
  friend ostream&  operator<< (ostream& s, const Magic& m);
private:
  int magic_;
};

#include <basic/binio.hpp>

#endif // __BINIO_H__
