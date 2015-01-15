/*
 * trist.hpp -- Implementation for the Trist class
 *
 * author -- Mike Facello
 *
 * date -- 1/19/96
 */

/*-----------------------------------------------------------------------*/
/*----------------------------- CLASS Trist -----------------------------*/
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Accessing information about triangles

inline int Trist::
maxTri (void)
{
	return triArr_.maxSize ();
}

/*-----------------------------------------------------------------------*/

inline int Trist::
numTri (void)
{
  return triArr_.numUsed ();
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/



// Basic navigation operations

inline VIndex Trist::
org (OrTri ef)
{
  FIndex t = ef.index ();
  int v = ef.version ();
  // Assert (triArr_.isValidIndex(t));
  return (triArr_.getData() + t)->origin_[vo[v]];
}

/*-----------------------------------------------------------------------*/

inline VIndex Trist::
dest (OrTri ef)
{
  FIndex t = ef.index ();
  int v = ef.version ();
  v += (v & 1)? -1 : 1;
  // Assert (triArr_.isValidIndex(t));
  return (triArr_.getData() + t)->origin_[vo[v]];
}


/*-----------------------------------------------------------------------*/

inline OrTri Trist::
enext2 (OrTri ef)
{
  return (OrTri (ef.index (), ve[ve[ef.version()]]));
}

/*-----------------------------------------------------------------------*/

inline OrTri Trist::
sym (OrTri ef)
{
  int v = ef.version ();
  v += (v & 1)? -1 : 1;
  return (OrTri (ef.index (), v));
}

/*-----------------------------------------------------------------------*/

inline OrTri Trist::
enext (OrTri ef)
{
  return (OrTri (ef.index (), ve[ef.version()]));
}

/*-----------------------------------------------------------------------*/

inline OrTri Trist::
fnext  (OrTri ef)
{
  FIndex t = ef.index ();
  Assert (triArr_.isValidIndex(t));
  return (triArr_.getData() + t)->fnext_[ef.version()];
}

/*-----------------------------------------------------------------------*/
// external checking is needed
inline OrTri Trist::
orient (OrTri ef, VIndex a0)
{
  if (a0 == org(ef))
    return ef;
  if (a0 == dest(ef))
    return enext(ef); 
  return enext2(ef);
}

inline OrTri Trist::
orient (FIndex f, VIndex a0)
{
  return orient(OrTri(f,0), a0);
}

inline void Trist::
flip (FIndex f)
{
  flip (OrTri(f, 0));
}

inline OrTri Trist::
fnextBV (OrTri ef, BitVector& bv, Bool rev, Bool useIntRep)
{
  int intRep;
  do {
	  ef = fnext(ef);
	  intRep = useIntRep? ef.getIntRep() : ef.index();
	} while (rev == bv.test(intRep));
	return ef;
}



