inline BinInput::
BinInput (FilePointer s): BinInputOutput(s) 
{
	//assert(type_ != FilePointer::OStream);
	assert(type_ != FilePointer::OStream);
}

inline BinInput::
~BinInput (void)
{ }

inline int BinInput::
readInt (void) { int i; readInts(&i, 1); return i; }

inline unsigned int BinInput::
readUnsignedInt (void)
{ unsigned int i; readUnsignedInts(&i, 1); return i; }

inline char BinInput::
readChar (void) { char c; readChars(&c, 1); return c; }

inline unsigned char BinInput::
readUnsignedChar (void)
{ unsigned char c; readUnsignedChars(&c, 1); return c; }

inline float BinInput::
readFloat (void) { float f; readFloats(&f, 1); return f; }

inline double BinInput::
readDouble (void) { double d; readDoubles(&d, 1); return d; }

inline short BinInput::
readShort (void) { short s; readShorts(&s, 1); return s; }

inline unsigned short BinInput::
readUnsignedShort (void)
{ unsigned short s; readUnsignedShorts(&s, 1); return s; }

//-----------------------------------------------------------------------------

inline BinOutput::
BinOutput (FilePointer s) : BinInputOutput(s) 
{ assert(type_ != FilePointer::IStream); }

inline BinOutput::
~BinOutput (void)
{ }

inline int BinOutput::
writeInt (int i) { return writeInts(&i, 1); }

inline int BinOutput::
writeUnsignedInt (unsigned int i) { return writeUnsignedInts (&i, 1); }

inline int BinOutput::
writeChar (char c) { return writeChars (&c, 1); }

inline int BinOutput::
writeUnsignedChar (unsigned char c) { return writeUnsignedChars (&c, 1); }

inline int BinOutput::
writeShort (short s) { return writeShorts (&s, 1); }

inline int BinOutput::
writeUnsignedShort (unsigned short s) { return writeUnsignedShorts(&s, 1); }

inline int BinOutput::
writeFloat (float f) { return writeFloats (&f, 1); }

inline int BinOutput::
writeDouble (double d) { return writeDoubles (&d, 1); }

//-------------------------------------------------------------------------------

