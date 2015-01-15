/*
 * trist.cpp -- Method declarations for the Trist class
 *
 * author -- Mike Facello
 *
 * modified -- Damrong
 * 1. Just fix bug by adding tIter.next() in the loop of Trist::numVertices()
 *    and Trist::iter().
 *
 * date -- 7/1/97
 */

#include <geometry/trist.h>
#include <geometry/bvtag.h>
#include <geometry/geomutil.h>
#include <geometry/tristmodifier.h>
#include <basic/util.h>
#include <basic/comment.h>
#include <iostream>
#include <fstream>
#include <strstream>
//#include <CompDB/CompDB.h>

/*-----------------------------------------------------------------------*/

/* origins wrt triangle version */
int Trist::vo[6] = { 0, 1, 1, 2, 2, 0 };
/* enexts  ... */
int Trist::ve[6] = { 2, 5, 4, 1, 0, 3 };  


const int * TristModifierAdmin::vo = Trist::vo;
const int * TristModifierAdmin::ve = Trist::ve;

/*-----------------------------------------------------------------------*/
/*---------------------------- CLASS TristNum ---------------------------*/
/*-----------------------------------------------------------------------*/

Bool TristNum::
operator== (TristNum& tn)
{
  return ((t_ == tn.t_) and (f_ == tn.f_) and (e_ == tn.e_) and (v_ == tn.v_)
	  and (fh_ == tn.fh_) and (eh_ == tn.eh_) and (vh_ == tn.vh_));
}

/*-----------------------------------------------------------------------*/

Bool TristNum::
hullEqual (TristNum& tn)
{
  return ((fh_ == tn.fh_) and (eh_ == tn.eh_) and (vh_ == tn.vh_));
}

/*-----------------------------------------------------------------------*/

void TristNum::
check (void)
{
#define equality(N, L, R)  if ((L) != (R)) basic_error ("equality (%d) violated", N)

  equality (1,  vh_ - eh_ + fh_,  2);
  equality (2,  3 * fh_,  2 * eh_);
  equality (3,  v_ - e_ + f_ - t_,  1);
  equality (4,  4 * t_,  fh_ + 2 * (f_ - fh_));
  equality (5,  fh_,  2 * vh_ - 4);
  equality (6,  f_ - fh_,  vh_ + 2 * ((e_ - eh_) - (v_ - vh_)) - 4);
  equality (7,  t_,  e_ - (v_ + vh_) + 3);
}


/*-----------------------------------------------------------------------*/
/*-------------------------- CLASS TRISTINFO  --------------------------*/
/*-----------------------------------------------------------------------*/

TristInfo::
TristInfo (void)
{
  basic_counter_reset (&orgs_);
  basic_counter_reset (&syms_);
  basic_counter_reset (&enexts_);
  basic_counter_reset (&fnexts_);
  basic_counter_reset (&fsplices_);
  minEfFnexts_ = 0;
}

/*-----------------------------------------------------------------------*/

Bool TristInfo::
binInit (BinInput& bi)
{
  orgs_.a = bi.readInt();     orgs_.b = bi.readInt();
  syms_.a = bi.readInt();     syms_.b = bi.readInt();
  enexts_.a = bi.readInt();   enexts_.b = bi.readInt();
  fnexts_.a = bi.readInt();   fnexts_.b = bi.readInt();
  fsplices_.a = bi.readInt(); fsplices_.b = bi.readInt();
  minEfFnexts_ = bi.readInt();
  bpt_ = bi.readInt();
  bytes = bi.readUnsignedInt();
  maxbytes_ = bi.readUnsignedInt();
  return TRUE;
}

void TristInfo::
binDump (BinOutput& bo)
{
  bo.writeInt(orgs_.a);     bo.writeInt(orgs_.b);
  bo.writeInt(syms_.a);     bo.writeInt(syms_.b);
  bo.writeInt(enexts_.a);   bo.writeInt(enexts_.b);
  bo.writeInt(fnexts_.a);   bo.writeInt(fnexts_.b);
  bo.writeInt(fsplices_.a); bo.writeInt(fsplices_.b);
  bo.writeInt(minEfFnexts_);
  bo.writeInt(bpt_);
  unsigned int ibytes = bytes;
  unsigned int imaxbytes = maxbytes_;
  bo.writeUnsignedInt(ibytes);
  bo.writeUnsignedInt(imaxbytes);
}

/*-------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/*----------------------------- CLASS Trist -----------------------------*/
/*-----------------------------------------------------------------------*/

const int TRIST_MAGIC_OLD = 1234567;
const int TRIST_MAGIC = 1234569;


Trist::
Trist (int size)
    : magic_(TRIST_MAGIC),
     onHull_ (new BitVector(2 * size)),
     triArr_(size)
{
  debugFlag_ = FALSE;
}

/*-----------------------------------------------------------------------*/

Bool Trist::
binInitOld (BinInput& bi)
{
  Magic magic(TRIST_MAGIC_OLD);
  if (!magic.binInit (bi))
    return FALSE;
  if (!ti_.binInit(bi))
    return FALSE;
  if (!onHull_->binInit(bi))
    return FALSE;

  int ind[3], ort[6],j;
  VIndex vind[3];
  OrTri ortri[6];
  unsigned int arrSize = bi.readUnsignedInt(); // construct trist array
  triArr_.resize(arrSize);
  float rSize = (float) (arrSize? 1.0/arrSize : 1.0);
  float fraction = rSize;
  for(unsigned int i=1; i<=arrSize; i++)
  {
    bi.readInts(ind, 3);
    bi.readInts(ort, 6);
    for(j=0;j<3;j++) vind[j] = ind[j];
    for(j=0;j<6;j++) ortri[j].setIntRep (ort[j]);
    
    makeTri (vind, ortri);
    progressUpdate (fraction);
    fraction += rSize;
  }

  debugFlag_ = FALSE;
  return TRUE;
}

Trist::
Trist ( Trist& tr): magic_(TRIST_MAGIC),
  onHull_(new BitVector(*tr.onHull_)),
  triArr_(tr.triArr_)
{
  debugFlag_ = FALSE;
}

/*-----------------------------------------------------------------------*/

Trist::
~Trist (void)
{
  delete onHull_;
}

/*-----------------------------------------------------------------------*/

void Trist::
binDumpOld (BinOutput& bo) // trist is assumed to be packed
{
  Magic magic(TRIST_MAGIC_OLD);
  magic.binDump(bo);
  ti_.binDump(bo);
  

  int origin[3], fn[6],j;

  TrImp tri;
  int num = triArr_.numUsed();
 // int arrSize = triArr_.maxSize();
  
  onHull_->binDump(bo);
  bo.writeInt(num);
  float rSize = (float) (num? 1.0/num : 1.0);
  float fraction = rSize;
  for(int i = 1; i <= num; i++)
  {
    tri = triArr_[i];
    for(j=0;j<3;j++) origin[j] = tri.origin_[j];
    for(j=0;j<6;j++) fn[j] = tri.fnext_[j].getIntRep ();
    bo.writeInts(origin, 3);
    bo.writeInts(fn, 6);
    progressUpdate (fraction);
    fraction += rSize;
  }
}

//----------------------------------------------------------------------------------

void Trist::
binDump (BinOutput& bo) // trist is not assumed to be packed, use for shape2d
{
  magic_.binDump(bo);
  onHull_->binDump(bo);
  setProgressCBto(&triArr_);
  triArr_.binDump(bo);
  triArr_.removeAllCallbacks();
}

Bool Trist::
binInit (BinInput& bi)
{
  int id = bi.queryInt();
  if (id==TRIST_MAGIC_OLD)
    return binInitOld(bi);

  if (!magic_.binInit(bi))
    return FALSE;
  if(!onHull_->binInit(bi)) // dummy bitvector beyond wrap phase, but for legacy we should maintain it
    return FALSE;
  setProgressCBto(&triArr_);
  Bool res = triArr_.binInit(bi);
  triArr_.removeAllCallbacks();
  return res;
}

/*-----------------------------------------------------------------------*/

// Debugging variables

void Trist::
debugOn (void)
{
  debugFlag_ = TRUE;
}

/*-----------------------------------------------------------------------*/

void Trist::
debugOff (void)
{
  debugFlag_ = FALSE;
}



/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Creating and Deleting triangles

OrTri Trist::
makeTri(VIndex *ind, OrTri *ortri)
{
  FIndex ix = triArr_.newLoc ();
  int i;
  for(i=0; i<3; i++) triArr_[ix].origin_[i] = ind[i];
  for(i=0; i<6; i++) triArr_[ix].fnext_[i] = ortri[i];
  return ortri[0];
}

/*-----------------------------------------------------------------------*/

void Trist::
delTri (FIndex ix)
{
  delTri (OrTri (ix, 0));
}


/*-----------------------------------------------------------------------*/


void Trist::
delTri (OrTri ef)
{
  OrTri d = enext (ef);
  OrTri f = enext2 (ef);
  int ix = ef.index ();
  int t = ix << 1;
  
  if (onHull_->test(t))
    onHull_->clear (t);
  if (onHull_->test(t | 1))
    onHull_->clear (t | 1);

  fsplice (sym (fnext (sym (ef))), ef);
  fsplice (sym (fnext (sym (d))), d);
  fsplice (sym (fnext (sym (f))), f);

  triArr_.delElt(ef.index ());
}


/*-----------------------------------------------------------------------*/

void Trist::
delTri (IterStack<FIndex>& triStack)
{
  FIndex fInd;
  IStackIter<FIndex> iter(triStack);
  for(iter.first(); !iter.isDone(); iter.next())
  {
     fInd = iter.curItem();
	 delTri (fInd);
  }
  
}

/*-----------------------------------------------------------------------*/

void Trist::
delTri (IterStack<OrTri>& triStack)
{
  OrTri ef;
  IStackIter<OrTri> iter(triStack);
  for(iter.first(); !iter.isDone(); iter.next())
  {
     ef = iter.curItem();
	 delTri (ef);
  }
  
}

void Trist::
delTri (BitVector& bv)
{
  BitVectorIter iter(bv);
  for(iter.first(); !iter.isDone(); iter.next())
	  delTri(iter.curItem());
}

void Trist::
delTriReset (BitVector& bv)
{
  BitVectorIter iter(bv);
	int ix;
  for(iter.first(); !iter.isDone(); iter.next())
	{
	  ix = iter.curItem();
	  delTri(ix);
		bv.clear(ix);
	}
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

void Trist::
tetra (OrTri ef, VIndex& i, VIndex& j, VIndex& k, VIndex& o)
{
  triangle (ef, i, j, k);
  o = If ((! isOnHull (ef)), dest (enext (fnext (ef))), (VIndex) 0);
}

/*-----------------------------------------------------------------------*/

void Trist::
printTriangle (ostream& s, FIndex ix)
{
  VIndex a, b, c;

  triangle (OrTri (ix, 0), a, b, c);
  s << a << " " << b << " " << c << endl;
}

/*-----------------------------------------------------------------------*/

void Trist::
printTriangle (ostream& s, OrTri ef)
{
  VIndex a, b, c;

  triangle (ef, a, b, c);
  s << a << " " << b << " " << c << endl;
}

/*-----------------------------------------------------------------------*/

Bool Trist::
deleted (FIndex ix)
{
  return (! triArr_.isValidIndex (ix));
}

/*-----------------------------------------------------------------------*/

void Trist::
hullFacetSet (OrTri ef, Bool flag)
{
  int tr = ef.index ();
  int side = If (Odd (ef.version ()), 1, 0);

  if (flag)
    onHull_->set ((tr << 1) | side);
  else
    onHull_->clear ((tr << 1) | side);
}

/*-----------------------------------------------------------------------*/

Bool Trist::
isOnHull (FIndex tr)
{
  return (onHull_->test ((tr << 1) | 0) or
	  onHull_->test ((tr << 1) | 1));
}

/*-----------------------------------------------------------------------*/

Bool Trist::
isOnHull (OrTri  ef)
{
  FIndex tr = ef.index ();
  int side = If (Odd (ef.version ()), 1, 0);
  return (onHull_->test ((tr << 1) | side));
}

/*-----------------------------------------------------------------------*/

OrTri Trist::
tetraMinEf (OrTri ef)
{
  OrTri x, a, b, c, d, min_ef;
  a = x = ef;
  b = sym (fnext (x));
  c = sym (fnext (x = enext (x)));
  d = sym (fnext (x = enext (x)));
  x.setIntRep (MIN(a.getIntRep (), b.getIntRep (), c.getIntRep (), d.getIntRep ()));

  // MIKE::: Let's see if this actually works here...
  min_ef = OrTri (x.index (), If (Odd (x.getIntRep ()), 1, 0));
  Assert (min_ef.getIntRep () != 0);
  return (min_ef);
}

/*-----------------------------------------------------------------------*/

OrTri Trist::
edgeMinEf (OrTri ef)
{
  OrTri min_ef, aux;
  min_ef = ef;
  aux = fnext (ef);
  ti_.minEfFnexts_ ++;
  while (aux != ef)
    {
      if (aux < min_ef)
        min_ef = aux;
      aux = fnext (aux);
      ti_.minEfFnexts_ ++;
    }
  if (Odd (min_ef.getIntRep ()))
    min_ef = sym (min_ef);
  Assert (min_ef.getIntRep () != 0);
  return (min_ef);
}
 


/*-----------------------------------------------------------------------*/

OrTri Trist::
turn   (OrTri ef)
{
  /* Returns Enext (Fnext (Sym (Enext (e)))); abbrev: Turn (e). */

  FIndex t = ef.index ();
  int v = ef.version ();
  OrTri ef2;

  basic_counter_plus (&ti_.enexts_, 2);
  basic_counter_plus (&ti_.fnexts_, 1);
  basic_counter_plus (&ti_.syms_, 1);
  v = ve[v];
  v = If (Odd (v), (v - 1), (v + 1));
  // v = sym_macro (v);
  ef2 = triArr_[t].fnext_[v];
  t = ef2.index ();
  v = ef2.version ();
  return (OrTri (t, ve[v]));
}

/*-----------------------------------------------------------------------*/
int Trist::degree(OrTri abc)
{
	int i=0;
	OrTri start = abc;
	OrTri current = start;

	do {
		current = fan(current);
		i++;
	} while (current!=start);
	return i;
}

/*-----------------------------------------------------------------------*/

OrTri Trist::
fan   (OrTri ef,int i)
{
	OrTri ef2 = ef;
	for(int j=0;j<i;j++)
		ef2 = fan(ef2);
	return ef2;
}
		
/*-----------------------------------------------------------------------*/

OrTri Trist::
fan   (OrTri ef)
{
	return enext(fnext(sym(ef)));
}

/*-----------------------------------------------------------------------*/

OrTri Trist::
afan   (OrTri ef,int i)
{
	OrTri ef2 = ef;
	for(int j=0;j<i;j++)
		ef2 = afan(ef2);
	return ef2;
}
		
/*-----------------------------------------------------------------------*/

OrTri Trist::
afan   (OrTri ef)
{
	return fnext(enext(sym(ef)));
}

/*-----------------------------------------------------------------------*/

OrTri Trist::lmSearch(OrTri start,VIndex a)
{
	BitVector tbv;
	IterStack<OrTri> ts;
	OrTri ef,ef2;
	int i;
	ts.push(start);
	tbv.set(start.index());
	while(!ts.empty())
	{
		ef = ts.pop();
		for(i=0;i<3;i++)
		{
			if(org(ef)==a)
				return ef;
			ef2 = fnext(ef.sym());
			if(!tbv.test(ef2.index()))
			{
				ts.push(ef2);
				tbv.set(ef2.index());
			}
			ef = ef.enext();
		}

	}
	Assert(0);
	return ef;
}
/*-----------------------------------------------------------------------*/

void Trist::
fmerge (OrTri d, OrTri e)
     /* Merges 2 (nonoverlapping) Facet rings s.t. fnext (d) == e,
        assuming fnext (d) != e. */
{
  Assert ((org (d) == org (e)) and (dest (d) == dest (e)));
  fsplice (d, sym (fnext (sym (e))));
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


Bool Trist::
testTriangle (OrTri ef, VIndex x, VIndex y, VIndex z)
{
  VIndex vx, vy, vz;
  if (debugFlag_)
    printf ("dt_test_triangle: [%ud] = %d,%d,%d\n", ef.getIntRep(), x, y, z);
  triangle (ef, vx, vy, vz);
  Assert_always ((x == vx) and (y == vy) and (z == vz));
  return (TRUE);
}

/*--------------------------------------------------------------------------*/

Bool  Trist::
testOpenTetra (OrTri a, VIndex x, VIndex y, VIndex z, VIndex o)
{
  OrTri b, c;
  OrTri a1;
  if (debugFlag_)
    printf ("dt_test_open_tetra: [%ud] = %d,%d,%d;%d\n", a.getIntRep(), x, y, z, o);
  b = turn (a);
  c = turn (b);
  testTriangle (a, x, y, o);
  testTriangle (b, y, z, o);
  testTriangle (c, z, x, o);
  a1 = turn (c);
  Assert_always (a == a1);  /* a == a.Turn.Turn.Turn */
  return (TRUE);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

// Getting information about the structure
//
// These two methods should probably be moved to Delaunay Complex...

int Trist::
numVertices (void)
{
  TristIter tIter (*this);
  BitVector inTrist;
  VIndex a, b, c;
  int v;
  
  v = 0;

  tIter.first ();
  while (! tIter.isDone ())
    {
      triangle (OrTri (tIter.curItem (), 0), a, b, c);
      if (! inTrist.test (a)) {
	inTrist.set (a);
	v++;
      }
      if (! inTrist.test (b)) {
	inTrist.set (b);
	v++;
      }
      if (! inTrist.test (c)) {
	inTrist.set (c);
	v++;
      }
      tIter.next();
    }
  
  return (v);
}


/*-----------------------------------------------------------------------*/

TristNum Trist::
tristNum  (void)
{
  TristNum tn;

  // This should be moved to DComplex

  tn.v_ = 0;

  return tn;
}

/*-----------------------------------------------------------------------*/

void Trist::
updateStarCenter(OrTri ef, VIndex a)
{
 OrTri ef1 = ef;
 FIndex t;
 int v;
 do {
      t = ef1.index();
      v = ef1.version();
      // a1 = org(ef1);
      // Assert(a1 == a0);
      triArr_[t].origin_[vo[v]] = a; 
      ef1 = fnext(sym(enext2(ef1)));
    } while(ef1 != ef);

}


/*-----------------------------------------------------------------------*/

void Trist::
updateStarCenter(OrTri ef, VIndex a, BVTag& tag)
{
 OrTri ef1 = ef;
 VIndex a0 = org(ef);
 FIndex t;
 int v;
 do {
      t = ef1.index();
	  Assert(!tag.test(t));
      v = ef1.version();
      Assert(org(ef1) == a0);
      triArr_[t].origin_[vo[v]] = a; 
      ef1 = fnext(sym(enext2(ef1)));
    } while(ef1 != ef);

}

/*-----------------------------------------------------------------------*/
// BVTAg uses integer representation of OrTri
void Trist::
updateStarCenterOrTri (StackBV& efStar, VIndex a)
{
  // efStar contains complete star of the org(ef)
  OrTri ef1;
  int ix;
  StackBVIter iter (efStar);
  for (iter.first(); !iter.isDone(); iter.next())
  {
    ix = iter.curItem();
    ef1.setIntRep (ix);
    triArr_[ef1.index()].origin_[vo[ef1.version()]] = a; 
  }
}

//---------------------------------------------------------------------
// BVTAg uses index of the triangle
void Trist::
updateStarCenterFIndex (StackBV& efStar, VIndex b, VIndex a)
{
  // efStar contains complete star of the org(ef)
  OrTri ef1;
  int fInd;
  StackBVIter iter (efStar);
  for (iter.first(); !iter.isDone(); iter.next())
  {
    fInd = iter.curItem();
    Assert (!deleted(fInd));
    ef1 = orient (OrTri(fInd, 0), b);
#ifdef __DEBUG__
    VIndex a1, b1, c1;
    triangle (ef1, a1, b1, c1);
#endif
    VIndex& v = triArr_[ef1.index()].origin_[vo[ef1.version()]];
    /**/
//	if (v == b);
      v = a; 
  }
}

//-------------------------------------------------------------

void Trist::
getSimpleStar (OrTri ef, StackBV& tag, Bool useIntRep)
{
  OrTri ef1 = ef;
  FIndex fInd;
  do {
    fInd = useIntRep? ef1.getIntRep(): ef1.index();
    tag.set(fInd);
    ef1 = fnext(sym(enext2(ef1)));
  } while (ef1 != ef);
}

//-------------------------------------------------------------

void Trist::
getCompleteStar (OrTri ef, StackBV& tag, Bool useIntRep)
{
  OrTri ef1, ef2;
  int efInd;
  OrTriStack s;
  s.push(ef);
  while (!s.empty())
  {
    ef1 = s.pop();
    efInd = useIntRep? ef1.getIntRep(): ef1.index();
    
    if (!tag.test(efInd))
    {
      
      tag.set(efInd);
      
      // walk through both edges of the popped triangle
      ef2 = fnext(ef1);
      efInd = useIntRep? ef2.getIntRep(): ef2.index();
      do {
        if (!tag.test(efInd))
          s.push(ef2);
        ef2 = fnext(ef2);
      } while (ef2 != ef1);
      
      ef1 = sym(enext2(ef1));
      ef2 = fnext(ef1);
      efInd = useIntRep? ef2.getIntRep(): ef2.index();
      do {
        if (!tag.test(efInd))
          s.push(ef2);
        ef2 = fnext(ef2);
      } while (ef2 != ef1);
      
    }
  }
}



void Trist::
getCompleteStar (OrTri ef, StackBV& tag, BitVector & bv, Bool rev)
{
  OrTri ef1, ef2;
  int efInd;
  OrTriStack s;
  s.push(ef);
  while (!s.empty())
  {
	ef1 = s.pop();
  efInd = ef1.index();

	if (!tag.test(efInd))
	{
       
		tag.set(efInd);

	   // walk through both edges of the popped triangle
	   ef2 = fnextBV(ef1, bv, rev);
     efInd = ef2.index();
	   do {
		    if (!tag.test(efInd))
				s.push(ef2);
		    ef2 = fnextBV(ef2, bv, rev);
	      } while (ef2 != ef1);

	   ef1 = sym(enext2(ef1));
	   ef2 = fnextBV(ef1, bv, rev);
     efInd = ef2.index();
       do {
		    if (!tag.test(efInd))
				s.push(ef2);
		    ef2 = fnextBV(ef2, bv, rev);
	      } while (ef2 != ef1);

	}
  }
	
}



/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Iteration
//
// These functions iterate through all (non-deleted) triangles in the
// triangle array, returning a FIndex for each.  Two iteration methods
// are provided:  a callback iteration and an external iteration.

void Trist::
iter(void (* func) (FIndex ix))
{
  TristIter tIter (*this);

  tIter.first ();
  while (! tIter.isDone ()){
    func (tIter.curItem ());
    tIter.next();
  }
}


/*-----------------------------------------------------------------------*/

TristIter *Trist::
newIter (void)
{
  return (new TristIter (*this));
}


/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Packing
//
// NOTE:  this will use the pack procedure in the FreeArray, but after
// this is finished, the fnext_ pointers need to be adjusted.  Trist is
// responsible for doing this.

/*
IMap * Trist::
pack(Bool returnFlag)
{
  IMap *map;
  OrTri fNext;
  int nUsed;
  int t2;
  map = triArr_.packAndResize ();
  nUsed = triArr_.numUsed ();
  
  BitVector *newBv  = new BitVector(triArr_.numUsed()*2 + 1, 
				    BitVector::AdjustableSize);

  for (register int i = 1; i < map->size(); i++)
    if ((*map)[i] != 0)
      {
	t2 = (*map) [i] << 1;

	if (onHull_->test (i << 1))
	  newBv->set (t2);
 
	if (onHull_->test ((i << 1) | 1))
	  newBv->set (t2 | 1);
      }

  delete onHull_;
  onHull_ = newBv;

  for (register int t = 1; t <= nUsed; t++)
    {
      for (register int j = 0; j <= 5; j++)
	{
	  fNext = triArr_[t].fnext_[j];
	  triArr_[t].fnext_[j] = OrTri ((*map) [fNext.index ()],
					fNext.version ());
	}
    }

  if(returnFlag)
  {
    return map;
  }
  else
  {
    delete map;
  }

  return NULL;
}
*/

IMap * Trist::
pack(Bool returnFlag)
{
  IMap * mapptr = new IMap(triArr_.maxSize()+1);
  IMap& map = *mapptr;
  TrImp * triArrData = triArr_.getData();
  int nUsed = 0;
  BitVector& exists = triArr_.getBitVector();

  // set up IMap
  BitVectorIter iter(exists);
  int cur;
  for(iter.first(); !iter.isDone(); iter.next())
  {
    map[iter.curItem()] = ++nUsed;
  }

  Assert (nUsed == exists.getNumSet());

  // remap onHull bitvector
  BitVector * newBV = new BitVector(2*exists.getNumSet() + 1);
  for (iter.first(); !iter.isDone(); iter.next())
  {
    cur = iter.curItem();
	  int t2 = map[cur] << 1;
	  if (onHull_->test (cur << 1))
	    newBV->set (t2);
    if (onHull_->test ((cur << 1) | 1))
	    newBV->set (t2 | 1);
  }
  delete onHull_;
  onHull_ = newBV;

  // map the Trist itself
  for (iter.first(); !iter.isDone(); iter.next())
  {
    cur = iter.curItem();
    TrImp& src = triArrData[cur];
    TrImp& dst = triArrData[map[cur]];
    if (&src != &dst)
    {
      dst.origin_[0] = src.origin_[0];
      dst.origin_[1] = src.origin_[1];
      dst.origin_[2] = src.origin_[2];
    }
    for(int j=0; j<=5; j++)
    {
      OrTri ef = src.fnext_[j];
      dst.fnext_[j] = OrTri(map[ef.index()], ef.version());
    }
  }

  triArr_.resize(nUsed);
  triArr_.resetReusable ();

  // remap exists bitvector to already known packed state
  exists.resize(nUsed);
  exists.setAll();
  exists.clear(0);
  int maxn = exists.size();
  for(cur=nUsed+1; cur<= maxn; cur++)
    exists.clear(cur);

  if(returnFlag)
  {
    return mapptr;
  }
  else
  {
    delete mapptr;
  }

  return NULL;
}

/*-----------------------------------------------------------------------*/
// let's assume Trist is packed already
void Trist::
reindexTriangles (IMap& map)
{
  // map is indexed by old location and returns new location

  // remap onHull
  BitVector * newBV = new BitVector(2*getTriangleBV().getNumSet() + 1);
  BitVector traverse(getTriangleBV());
  BitVectorIter iter(traverse);

  for (iter.first(); !iter.isDone(); iter.next())
  {
    int cur = iter.curItem();
	  int t2 = map[cur] << 1;
	  if (onHull_->test (cur << 1))
	    newBV->set (t2);
    if (onHull_->test ((cur << 1) | 1))
	    newBV->set (t2 | 1);
  }
  delete onHull_;
  onHull_ = newBV;

  TrImp tmpTrImp[2];
  TrImp * src = &tmpTrImp[0];
  TrImp * dst = &tmpTrImp[1];
  for (iter.first(); !iter.isDone(); iter.next())
  {
      FIndex f = iter.curItem();
      *src = triArr_[f];
      while (traverse.test(f))
      {
        FIndex newf = map[f];
        traverse.clear(f);
        TrImp& trImp = triArr_[newf];
        *dst = trImp;
        
        trImp.origin_[0] = src->origin_[0];
        trImp.origin_[1] = src->origin_[1];
        trImp.origin_[2] = src->origin_[2];
        for(int j=0; j<=5; j++)
        {
          OrTri ef = src->fnext_[j];
          trImp.fnext_[j] = OrTri(map[ef.index()], ef.version());
        }
        TrImp * tmp = src; src = dst; dst = tmp;
        f = newf;
    }
  }
}


/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Output operators

void Trist::
printInfo (ostream &s)
{
  int hullCount;
  TristIter tIter (*this);
  
  hullCount = 0;
  
  tIter.first ();
  while (! tIter.isDone ())
    {
      if (isOnHull (tIter.curItem ()))
	hullCount ++;
      tIter.next ();
    }

  s.width (12);
  s << triArr_.numUsed () << " . Number of triangles\n";
  s.width (12);
  s << hullCount << " . Number of hull triangles\n";
}


/*-----------------------------------------------------------------------*/

ostream&
operator<< (ostream& s, Trist& trist)
{
  TristIter tIter (trist);
  OrTri ef;
  
  s << "\n(Oriented) Hull Triangles\n\n";

  tIter.first ();
  while (! tIter.isDone ())
    {
      ef = OrTri (tIter.curItem (), 0);
      if (trist.isOnHull (ef))
	  trist.printTriangle (s, ef);
      else if (trist.isOnHull (trist.sym (ef)))
	  trist.printTriangle (s, trist.sym (ef));
      tIter.next ();
    }

  s << "\nInterior Triangles\n\n";
  
  tIter.first ();
  while (! tIter.isDone ())
    {
      if (! trist.isOnHull (tIter.curItem ()))
	trist.printTriangle (s, tIter.curItem ());
      tIter.next ();
    }

  return s;
}


/*-----------------------------------------------------------------------*/


// make triangle ijk with predefined version
OrTri Trist::
makeTri (VIndex i, VIndex j, VIndex k, int version)
{
  Assert (version >= 0 && version <= 5);
  FIndex ix = triArr_.newLoc ();
  int v = version;
  triArr_[ix].origin_[vo[v]] = i;
  v = ve[v];
  triArr_[ix].origin_[vo[v]] = j;
  v = ve[v];
  triArr_[ix].origin_[vo[v]] = k;

  onHull_->clear (ix << 1);
  onHull_->clear ((ix << 1) | 1);

  for (v = 0; v <= 5; v++)
    triArr_[ix].fnext_[v] = OrTri(ix, v);

  return OrTri(ix, version);

}

OrTri Trist::
flipEdge(OrTri abc,OrTri& adc,OrTri& bcd,BitVector* triBV)
{

	OrTri abd=fnext(abc);
	VIndex ai,bi,ci,di;
	triangle(abc,ai,bi,ci);
	di = dest(enext(abd));

	Assert(di!=  dest(enext(fnext(abc.enext2()))));
	Assert(di!=  dest(enext(fnext(abc.enext()))));
	Assert(ci!=  dest(enext(fnext(abd.enext()))));
	Assert(ci!=  dest(enext(fnext(abd.enext2()))));
	adc = makeTri(ai,di,ci);
	bcd = makeTri(bi,ci,di);
	fmerge(sym(enext(bcd)),enext(adc));
	fmerge(adc,enext(sym(abd)));
	fmerge(enext(sym(adc)),enext(sym(abc)));
	fmerge(bcd,enext(abc));
	fmerge(enext(sym(bcd)),enext(abd));
	delTri(abc);
	delTri(abd);
	if(triBV)
	{
		triBV->clear(abc.index());
		triBV->clear(abd.index());
		triBV->set(adc.index());
		triBV->set(bcd.index());
	}
	return adc.enext();
}

void Trist::
splitEdge(OrTri abc,VIndex vi,OrTri& cvb,OrTri& cav,OrTri& adv,OrTri& dbv)
{
	int ai = org(abc);
	int bi = dest(abc);
	int ci = dest(abc.enext());
	OrTri bad = fnext(abc.sym());
	int di = dest(bad.enext());
/*
	OrTri bdf,cbe;
	bdf = fnext(bad.enext2().sym());
	cbe = fnext(abc.enext().sym());
*/
	cvb = makeTri(ci,vi,bi);
	cav = makeTri(ci,ai,vi);
	adv = makeTri(ai,di,vi);
	dbv = makeTri(di,bi,vi);

	// the cross
	fmerge(cvb,cav.sym().enext());
	fmerge(cav.enext(),adv.sym().enext());
	fmerge(adv.enext(),dbv.sym().enext());
	fmerge(dbv.enext(),cvb.enext().sym());

	// the boundary
	fmerge(cvb.enext2(),abc.enext());
	fmerge(cav,abc.enext2());
	fmerge(adv,bad.enext());
	fmerge(dbv,bad.enext2());
	delTri(abc);
	delTri(bad);
}

void Trist::
insertVertex(OrTri abc,VIndex vi,OrTri& abv,OrTri& bcv,OrTri& cav,BitVector* triBV)
{
	VIndex ai,bi,ci;
	triangle(abc,ai,bi,ci);
	abv=makeTri(ai,bi,vi);
	bcv=makeTri(bi,ci,vi);
	cav=makeTri(ci,ai,vi);

	fmerge(enext(abv),enext(sym(bcv)));
	fmerge(enext(bcv),enext(sym(cav)));
	fmerge(enext(cav),enext(sym(abv)));
	fmerge(abv,abc);
	fmerge(bcv,enext(abc));
	fmerge(cav,enext2(abc));
	delTri(abc);
	if(triBV)
	{
		triBV->set(abv.index());
		triBV->set(bcv.index());
		triBV->set(cav.index());
		triBV->clear(abc.index());
	}

    Assert((abv.version()&1)==0);
    Assert((bcv.version()&1)==0);
    Assert((cav.version()&1)==0);
}

void Trist::
flip (OrTri ef)
{
#ifdef __TO_BE_DEBUGGED
// there is a bug here it should be fixed
  FIndex ix = ef.index();
	OrTri efTmp;
	int v1, v2;
	TrImp& t = triArr_[ix];
	// swap 2 first indices
  VIndex vTmp = t.origin_[0];
	t.origin_[0] = t.origin_[1];
	t.origin_[1] = vTmp;
#define SWAPF(V1, V2) \
    efTmp = t.fnext_[v1]; \
	  t.fnext_[v1] = t.fnext_[v2]; \
	  t.fnext_[v2] = efTmp
	// swap ab fnexts
  v1 = ef.version();
	v2 = sym(ef).version();
	SWAPF(v1, v2);
	v1 = ve[v1]; v2 = ve[v2];
  SWAPF(v1, v2);
	v1 = ve[v1]; v2 = ve[v2];
  SWAPF(v1, v2);
	// now make adjacent triangles consistent
	for(v1=0; v1<=5; v1++)
	{
    ef = sym(t.fnext_[v1]); // sym(fnext(ef));
		triArr_[ef.index()].fnext_[ef.version()] = sym(OrTri(ix, v1));
	}
#undef SWAPF
#endif
// this is slower version, but not much slower, since
// usually it doesn't trigger resizing
	FIndex f = ef.index();
	OrTri ef1(f, 1); // negative
	VIndex v1, v2, v3;
	triangle(ef1, v1, v2, v3);
	OrTri ef2 = makeTri(v1, v2, v3); // positive
	fmerge(ef1, ef2);
	fmerge(enext(ef1), enext(ef2));
	fmerge(enext2(ef1), enext2(ef2));
	delTri (ef1);
	// now create copy of ef2 with old index
	OrTri ef3 = makeTri(v1, v2, v3); // positive
    fmerge(ef2, ef3);
	fmerge(enext(ef2), enext(ef3));
	fmerge(enext2(ef2), enext2(ef3));
	delTri (ef2);
	Assert (f == ef3.index());
}

/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

void Trist::
triangle (OrTri ef, VIndex& i, VIndex& j, VIndex& k)
{
  FIndex t = ef.index ();
  int v = ef.version ();

  i = triArr_[t].origin_[vo[v]];
  v = ve[v];
  j = triArr_[t].origin_[vo[v]];
  v = ve[v];
  k = triArr_[t].origin_[vo[v]];

#ifdef __DEBUG__
  if (debugFlag_)
    if ((i == j) or (i == k) or (k == j) or (ve[v] != (int)(ef.version ())))
      basic_error ("Triangle: Given edge-ring is not triangular.");
#endif
}





// Linking and unlinking triangles

inline void swapfnext(OrTri A, OrTri B, Trist::TrImp * triArr)
{
  Index ta = A.index (),  va = A.version (),
        tb = B.index (),  vb = B.version ();
  OrTri aux = triArr[ta].fnext_[va]; 
  triArr[ta].fnext_[va] = triArr[tb].fnext_[vb];
  triArr[tb].fnext_[vb] = aux;
}

void Trist::
fsplice (OrTri a, OrTri b)
{
  OrTri alpha = fnext(a).sym(), beta = fnext(b).sym();
  Trist::TrImp * data = triArr_.getData();
  swapfnext (a, b, data);
  swapfnext (alpha, beta, data);
}

//---------------------------------------------------------------------------

void Trist::
getVertices (OrTri ef, Dim dim, VertArray & v)
{
  int a, b, c, d;
  switch (dim)
  {
  case 0:
    v.setVertices (org (ef));
    return;
  case 1:
    v.setVertices (org (ef), dest (ef));
    return;
  case 2:
    triangle (ef, a, b, c);
    v.setVertices (a, b, c);
    return;
  case 3:
    tetra (ef, a, b, c, d);
    v.setVertices (a, b, c, d);
    return;
  default:
	  Assert(0);
  }
}

//---------------------------------------------------------------------------

void Trist::
getSortedVertices (OrTri ef, Dim d, VertArray & v)
{
  getVertices (ef, d, v);
  v.sort();
}

//---------------------------------------------------------------------------

void Trist::
reindexVertices (IMap& newVert)
{
  TristIter iter(*this);
  for(iter.first(); !iter.isDone(); iter.next())
  {
    TrImp& elt = triArr_[iter.curItem()];
    elt.origin_[0] = newVert[elt.origin_[0]];
    elt.origin_[1] = newVert[elt.origin_[1]];
    elt.origin_[2] = newVert[elt.origin_[2]];
  }

}

//--------------------------------------------------------

int Trist::
enumVertices (BitVector * triangles, OrTriCB cbFunc, RgiObject * obj)
{
  if (!triangles) triangles = &getTriangleBV();
	BitVector vertTag;
  BitVectorIter iter(*triangles);
	for (iter.first(); !iter.isDone(); iter.next())
	{
    OrTri abc(iter.curItem(), 0);
		for (int i=0; i<3; i++, abc = enext(abc))
		{
      VIndex v = org(abc);
			if (vertTag.test(v))
			  continue;
			vertTag.set(v);
      if (cbFunc && !(obj->*cbFunc)(abc))
				return 0;
		}
	}
  return cbFunc? 1 : vertTag.getNumSet();
}

int Trist::
enumEdges (BitVector * triangles, OrTriCB cbFunc, RgiObject * obj)
{
  BitVector edgeBV;
  FIndex f;
  OrTri abc;
  int count = 0;
	if (!triangles) triangles = &getTriangleBV();

  BitVectorIter iter(*triangles);
  for (iter.first (); !iter.isDone(); iter.next ())
  {    
     f = iter.curItem();
     abc = OrTri(f, 0);
     // go through all edges of the triangle
     for(int i=0; i<3; i++, abc = enext(abc))
     {
			 if (edgeBV.test(abc.getIntRep() >> 1)) // edge is visited
				 continue;
			 OrTri abd = abc; // mark edge visited
			 do {
				  edgeBV.set(abd.getIntRep() >> 1);
					abd = fnext(abd);
			 } while (abd != abc);
			 count++;
			 if (cbFunc && !(obj->*cbFunc)(abc))
				 return 0;
     }     
  }

  return cbFunc? 1 : count;
}

int Trist::
enumTriangles (BitVector * triangles, OrTriCB cbFunc, RgiObject * obj)
{
  int count = 0;
	if (!triangles) triangles = &getTriangleBV();

  BitVectorIter iter(*triangles);
  for (iter.first (); !iter.isDone(); iter.next ())
  {   
     OrTri curEF(iter.curItem(), 0);
		 count++;
		 if (cbFunc && !(obj->*cbFunc)(curEF))
			 return 0;   
  }

  return cbFunc? 1 : count;
}

inline int frep (OrTri ef) { return ((ef.index() << 1) | (ef.getIntRep() & 1)); }
inline OrTri efrep(int r) { return OrTri(r >> 1, r & 1); }

int Trist::
enumTetra (OrTriCB cbFunc, RgiObject * obj)
{
  int count = 0;
  BitVector tetra(maxTri()*2 +1);
  BitVectorIter tIter(getTriangleBV());
  for (tIter.first(); !tIter.isDone(); tIter.next())
  {
    int t = tIter.curItem() << 1;
    tetra.set(t);
    tetra.set(t | 1);
  }
  tetra.sub_op(getHullBV());
  BitVectorIter iter(tetra);
  for (iter.first(); !iter.isDone(); iter.next())
  {
    int index = iter.curItem();
    OrTri curEF = efrep(index);
    count++;
    if (cbFunc && !(obj->*cbFunc)(curEF))
			return 0;
    tetra.clear (index);
    tetra.clear (frep(sym(fnext(curEF))));
    tetra.clear (frep(sym(fnext(enext(curEF)))));
    tetra.clear (frep(sym(fnext(enext2(curEF)))));
  }
  return cbFunc? 1 : count;
}



Index Trist::
ix (OrTri ef,Dim d)
{
	FIndex fidx = ef.index();
	int version = ef.version();
	Index res;

	switch (d) {
	case 0:
		res = org(ef);
		break;
	case 1:
		res = version >> 1;
		res = triArr_[fidx].ei_[version>>1];
		break;
	case 2:
		res = fidx;
		break;
	case 3:
		res = version%2;
		res = triArr_[fidx].ti_[version%2];
		break;
	default:
		Assert(0);
	}
	return res;
}

void Trist::
setPackedIndex(FIndex fi,EIndex ei0,EIndex ei1,EIndex ei2,TIndex ti0,TIndex ti1)
{
	triArr_[fi].ei_[0] = ei0;
	triArr_[fi].ei_[1] = ei1;
	triArr_[fi].ei_[2] = ei2;
	triArr_[fi].ti_[0] = ti0;
	triArr_[fi].ti_[1] = ti1;
}
