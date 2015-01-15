// Ortri.hpp -- implementation of the oriented triangle class
//


/*-----------------------------------------------------------------------*/
/*------------------------------ CLASS ORTRI ----------------------------*/
/*-----------------------------------------------------------------------*/

OrTri::
OrTri(FIndex ix, int vers)
  : ef_ ((ix Mul8) | vers)
{
  Assert ((vers >= 0) && (vers <= 5));
}

/*-----------------------------------------------------------------------*/

OrTri::
OrTri(void)
{
}

/*-----------------------------------------------------------------------*/

unsigned OrTri::
getIntRep (void) const
{
  return ef_;
}
/*-----------------------------------------------------------------------*/

void OrTri::
setIntRep (int ef)
{
  ef_ = ef;
}

/*-----------------------------------------------------------------------*/

FIndex OrTri::
index (void) const
{
  return (ef_ Div8);
}

/*-----------------------------------------------------------------------*/

unsigned OrTri::
version (void) const
{
  return ef_ Mod8;
}

/*-----------------------------------------------------------------------*/

OrTri OrTri::
sym (void) const
{
  unsigned version = ef_ Mod8;
  version += (version & 1)? -1 : 1;
  return OrTri(ef_ Div8, version);
}

/*-----------------------------------------------------------------------*/

OrTri OrTri::
enext (void) const
{
  unsigned version = ef_ Mod8;
  version += (version & 1)? 4 : 2; // -2 + 6 : 2
  version %= 6;
  return OrTri(ef_ Div8, version);
}

/*-----------------------------------------------------------------------*/

OrTri OrTri::
enext2 (void) const
{
  unsigned version = ef_ Mod8;
  version += (version & 1)? 2 : 4; // -4 + 6 : 4
  version %= 6;
  return OrTri(ef_ Div8, version);
}
/*-----------------------------------------------------------------------*/


int OrTri::
operator== (const OrTri & f2) const
{ 
  return (ef_ == f2.ef_);
}

/*-----------------------------------------------------------------------*/

int OrTri::
operator!= (const OrTri & f2) const
{ 
  return (ef_ != f2.ef_);
}

/*-----------------------------------------------------------------------*/

int OrTri::
operator< (const OrTri & f2) const
{ 
  return (ef_ < f2.ef_);
}

/*-----------------------------------------------------------------------*/

ostream&
operator<< (ostream& s, const OrTri& ef)
{
  s << "(" << ef.index() << ", " << ef.version() << ")\n";
  return s;
}

/*-----------------------------------------------------------------------*/

void OrTri::
binDump (BinOutput& bo)
{
  bo.writeUnsignedInt(ef_);
}

Bool OrTri::
binInit (BinInput& bi)
{
  return bi.readUnsignedInts(&ef_, 1);
}

