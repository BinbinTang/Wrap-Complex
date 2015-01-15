/*
 * dcbuilder.cpp -- definitions for the DCompBuilder class.
 *
 * author -- Mike Facello
 *        -- Damrong, edit for dynamic triangulation.
 *                  notice #ifdef _DYNAMIC_.    Feb 16, 1998
 *
 * date -- 10/8/96
 */

#include <basic/comment.h>
#include <basic/miscmath.h>
#include <delone/dcbuilder.h>
#include <basic/console.h>

#ifdef _DYNAMIC_ 
#include <geometry/simplexset.h>
#include <geometry/ihandler.h>
#endif

void testHidden (DComplex * dc);
/*-----------------------------------------------------------------------*/

#ifdef __DEBUG__
#define PROTO(PRINT_COMMAND)  if (debugFlag_) PRINT_COMMAND
#else
#define PROTO(PRINT_COMMAND)  ;
#endif

#define Turn     tr_->turn

// Constants for method mountT

#define INITIAL 'i'
#define NORMAL  'n'

#define NOTTESTED -1
#define CONCAVE 0
#define CONVEX 1

// A Static variable for class DCompBuilder

/*-----------------------------------------------------------------------*/
/*-------------------------- CLASS DCOMPBUILDER -------------------------*/
/*-----------------------------------------------------------------------*/
// NOTE: DCompBuilder is never owner of pArray_, rArray_ or tr_.
// dcomp_ is responsible for deleting them
DCompBuilder::
DCompBuilder (int maxBefore, int maxAfter, int arraySize)
  : converter_ (maxBefore + maxAfter, maxAfter),
  sEval_(converter_)
{
  if ((maxBefore + maxAfter) <= 7)
    rArray_ = new FloatPArrayW(arraySize);
  else
  rArray_ = new DoublePArrayW(arraySize);
  redundent_ = NULL;
//  rArray_ = new DoublePArray(arraySize);
  tr_ = new Trist(arraySize * 16); // in average, this complete triangulation size
  dcomp_ = new DComplex (tr_, rArray_, converter_);
  fh_ = new FlipHandler (tr_, rArray_, converter_);
  locator_ = new JWPLocate (tr_, rArray_, converter_);
  debugFlag_ = FALSE;
  triStatus_.reset ();
  firstTime_ = TRUE;
  skips_ = eflips_ = fflips_ = reds_ = greens_ = 0;
  mount1s_ = mount2s_ = mount3as_ = mount3bs_ = mount4s_ = 0;
  searchOutsiders_ = 0;
  dcompExported_ = FALSE;
  problemVertexIX_ = -1;

#ifdef _DYNAMIC_
  maintainIndex_ = FALSE;
#endif
}

/*-----------------------------------------------------------------------*/
  
DCompBuilder::
DCompBuilder (DComplex *comp)
  : TristModifier(comp->getTrist()),
    converter_ (comp->getConverter()),
    rArray_ (comp->getPArray()),
    dcomp_ (comp),
    fh_ (new FlipHandler (tr_, rArray_, converter_)),
    locator_ (new JWPLocate (tr_, rArray_, converter_)),
    sEval_(converter_)
{
  skips_ = eflips_ = fflips_ = reds_ = greens_ = 0;
  mount1s_ = mount2s_ = mount3as_ = mount3bs_ = mount4s_ = 0;
  searchOutsiders_ = 0;
  debugFlag_ = FALSE;
  firstTime_ = FALSE;
  dcompExported_ = TRUE;

#ifdef _DYNAMIC_
  maintainIndex_ = FALSE;
#endif
}

/*-----------------------------------------------------------------------*/

DCompBuilder::
~DCompBuilder (void)
{
  delete fh_;
  delete locator_;
  if (! dcompExported_)
    delete dcomp_;
}

/*-----------------------------------------------------------------------*/
#ifdef _DYNAMIC_

void DCompBuilder::
maintainIndex (void)
{
  if (maintainIndex_) return;

  /* setup communications : FlipHandler -> IndexHandler
   *                        DCompBuilder (peel,mount) -> IndexHandler
   */
   fh_ -> addTristModCB (Add, (TristModCB)& IndexHandler::addSimplices, dcomp_->ix_); // Flip 
   fh_ -> addTristModCB (Del, (TristModCB)& IndexHandler::delSimplices, dcomp_->ix_); // Flip
   fh_ -> addTristModCB (Upd, (TristModCB)& IndexHandler::updSimplices, dcomp_->ix_); // Flip

   this-> addTristModCB (Add, (TristModCB)& IndexHandler::addSimplices, dcomp_->ix_); // Mount, Peel
	 this-> addTristModCB (Del, (TristModCB)& IndexHandler::delSimplices, dcomp_->ix_); // Peel
   this-> addTristModCB (Upd, (TristModCB)& IndexHandler::updSimplices, dcomp_->ix_); // Peel

  maintainIndex_ = TRUE;
}

#endif
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Accessors

void DCompBuilder::
cleanTriangulation (void)
{
  peelFlatTetras ();

#ifndef _DYNAMIC_
  tr_->pack ();
#else
  if (!maintainIndex_)
  {
    tr_->pack ();
  }
  else
  {
    IMap * m;
    dcomp_->ix_->reMap (m = tr_->pack (TRUE));
    delete m;
  }
#endif
}
/*-----------------------------------------------------------------------*/
void DCompBuilder::
reindexTriangles (IMap& newLoc)
{
  tr_->reindexTriangles (newLoc);

#ifdef _DYNAMIC_
	if (maintainIndex_)
  {
		  dcomp_->ix_->reMap (& newLoc);
	}
#endif
}
/*-----------------------------------------------------------------------*/
int DCompBuilder::
initIndices (void)
{
  if (dcomp_->ix_ != NULL)
  {
      delete dcomp_->ix_;
#ifdef _DYNAMIC_
      fh_ -> resetTristModCB ();    // reset communications to 
      this-> resetTristModCB ();    // IndexHandler
      maintainIndex_ = FALSE;
#endif
  }

  dcomp_->ix_ = new IndexHandler (dcomp_);
  setProgressCBto(dcomp_->ix_);
  return dcomp_->ix_->init();
}
/*-----------------------------------------------------------------------*/
DComplex *DCompBuilder::
getComplex (void)
{
  dcompExported_ = TRUE;
  return dcomp_;
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Operations on complex

int DCompBuilder::
getSize ()
{
	return rArray_->size();
}

double* DCompBuilder::
getPoint (int v)
{
	double* x = new double [4]; 
    rArray_->getPoint(v, x);
	return x;
}


int DCompBuilder::
addPoints (RealPArray * rArr, BitVector& skip)
{
  VIndex newIX;
  int rsize = rArr->size();
  int size = rsize - skip.getNumSet() + 1;
  rArray_->resize(size);
  
  // change converter to L.0 format
  double rcoef = 1.0/converter_.getScale();

  uninserted_.resize(size);
  double x[4]; 
  for (int v=1; v<=rsize; v++)
  {
    if (skip.test(v))
      continue;
    rArr->getPoint(v, x);
      // do the rounding, for the sake of stability
    converter_.truncMul(x);
    vmulw(x, rcoef);
    newIX = rArray_->addPoint(x);
    uninserted_.push(newIX);
  }
  return rArray_->size();
}

/*-----------------------------------------------------------------------*/

int DCompBuilder::
addPoint (double x[4])
{
	converter_.truncMulDiv(x);
  VIndex newIX = rArray_->addPoint(x);
  uninserted_.push (newIX);
  return newIX;
}

/*-----------------------------------------------------------------------*/

//
// NOTE:  This function currently assumes that if this is the first call
//        to compute, then at least *four* vertices are to be inserted.
//

#ifdef DEBUG
#ifdef UNIX
ofstream dCompLog_ ("/tmp/dcomplog.txt");
#else
ofstream dCompLog_ ("c:\\dcomplog.txt");
#endif
#endif

int DCompBuilder::
compute  (void)
{
  OrTri ef, aux;
  int n = rArray_->size ();
  OrTriStack visList (2 * n);
  OrTri h;
  FlipEvent fEvent;

  if(redundent_)
	  delete redundent_;
  redundent_ = new BitVector;
  redundent_->reset();

  removeDuplicates (uninserted_);
  randomize ();

  int n0 = uninserted_.size();


//  testHidden (dcomp_);

  if (firstTime_)
  {
      if (n0 <4)
      {
          // basic_error ("Point set must have at least four points\n");
          return 0;
      }
  }

  	getPArray()->getBBox(BBoxMin_,BBoxMax_);

	for(int yy= 0 ; yy < 3 ; yy++){
		BBoxMin_[yy] = BBoxMin_[yy]-100;
		BBoxMax_[yy] = BBoxMax_[yy]+100;
	}

  
 /* 
  double k1[4];
  k1[0] = BBoxMin_[0]; k1[1] = BBoxMin_[1]; k1[2] = BBoxMin_[2]; k1[3] = -10; addPoint(k1);
  k1[0] = BBoxMin_[0]; k1[1] = BBoxMin_[1]; k1[2] = BBoxMax_[2]; k1[3] = -10; addPoint(k1);
  k1[0] = BBoxMin_[0]; k1[1] = BBoxMax_[1]; k1[2] = BBoxMin_[2]; k1[3] = -10; addPoint(k1);
  k1[0] = BBoxMin_[0]; k1[1] = BBoxMax_[1]; k1[2] = BBoxMax_[2]; k1[3] = -10; addPoint(k1);
  k1[0] = BBoxMax_[0]; k1[1] = BBoxMin_[1]; k1[2] = BBoxMin_[2]; k1[3] = -10; addPoint(k1);
  k1[0] = BBoxMax_[0]; k1[1] = BBoxMin_[1]; k1[2] = BBoxMax_[2]; k1[3] = -10; addPoint(k1);
  k1[0] = BBoxMax_[0]; k1[1] = BBoxMax_[1]; k1[2] = BBoxMin_[2]; k1[3] = -10; addPoint(k1);
  k1[0] = BBoxMax_[0]; k1[1] = BBoxMax_[1]; k1[2] = BBoxMax_[2]; k1[3] = -10; addPoint(k1);
*/
  
  if (firstTime_)
  {
    initial (uninserted_.pop(), uninserted_.pop(), uninserted_.pop(), uninserted_.pop(), h);
    firstTime_ = FALSE;
  }
  
  n0 -= 4;
  float rn0 = n0? 1.0f/n0 : 1.0f;
  float fraction = rn0;
  
 //  upfor (index, start, n0)
  while (!uninserted_.empty())
  {
    /*
    if ((n0 - uninserted_.size ()) % 100 == 0)
    {
#ifdef DEBUG
      dCompLog_ << "Number of points inserted = " << n0 - uninserted_.size () << endl;
      dCompLog_ << "\tNumber of triangles = " << tr_->numTri () << endl;
#endif
    }
    */

    Assert (visList.empty ());
    //      Assert_if (PEDANTIC_TEST, dt_test ());
    VIndex v = uninserted_.pop();
    ef = locator_->locate (v);
	int a,b,c;
	tr_->triangle(ef,a,b,c);
//	rArray_->getPoint(v,x);
    if (tr_->isOnHull (ef))
    {
      searchOutsiders_ ++;
      collectVisibles (v, ef, visList);
      hullInsert (v, visList, h);
    }
    else
    {
	  if(sEval_.inSphereP (*rArray_, v, a, b, c, Dest_(Enext_(Fnext_(ef)))))
	  {
		  visList.push (ef);
		  OrTri tmp;
		  visList.push (tmp = Sym_ (aux = Fnext_ (ef)));
		  visList.push (tmp = Sym_ (aux = Turn (aux)));
		  visList.push (tmp = Sym_ (aux = Turn (aux)));
		  OrTri tmp2;
		  Assert ((tmp2 = Turn (aux)) == (tmp = Fnext_ (ef)));
		  fh_->insert (ef, v, fEvent);
	  }
	  else
		  redundent_->set(v);
    }

    (void) processFlips (visList, v);
		if (problemVertexIX_ != -1)
		{
			print("SHIT\n");
			return 0;
		}

    if (!progressUpdate(fraction))
      return 0;
    fraction += rn0;
  }

  return 1;
}

/*-----------------------------------------------------------------------*/

void DCompBuilder::
debugOn (void)
{
  debugFlag_ = TRUE;
}

/*-----------------------------------------------------------------------*/

void DCompBuilder::
debugOff (void)
{
  debugFlag_ = FALSE;
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Private methods

void DCompBuilder::
initial (VIndex a, VIndex b, VIndex c, VIndex o, OrTri& h)
     /* Output: *h. */
     /* ... creates initial triangulation by building tetrahedron (a, b, c, o);
        outputs *h, a face on CH */
{
  OrTri aux = tr_->makeTri (a, b, c);
  if (not sEval_.positive3 (*rArray_, o, a, b, c))
    aux = Sym_ (aux);
  tr_->hullFacetSet (Sym_ (aux), TRUE);
  mountT (INITIAL, o, aux);
  restoreHullBits (o, h = Enext2_(Fnext_ (aux)));
}

/*-----------------------------------------------------------------------*/

static DCompBuilder * curBuilder_ = NULL;

void DCompBuilder::
removeDuplicates (IndexStack& verts)
{
  int i, j = 1;
  int n0 = verts.size ();

  double xi[4], xj[4];
  
  lexiComparisons_ = 0;
  curBuilder_ = this;
  basic_qsort ((int *) &(verts[1]), 0, n0 - 1, lexiCompare);
  curBuilder_ = NULL;
  for (i=2; i<=n0; i++)
    {
      rArray_->getPoint(verts[i], xi);
      rArray_->getPoint(verts[j], xj);
      vsubw(xi, xj);
      converter_.truncMul(xi);
/*
      if (xi[0] || xi[1] || xi[2])
		{
			j ++;
			if (j < i)
			  verts[j] = verts[i];
		}
//	  else
//		  Assert(0);
*/

      if (xi[0] || xi[1] || xi[2] || xi[3])
      {
		if(xi[0]||xi[1]||xi[2])
		{
			j ++;
			if (j < i)
			  verts[j] = verts[i];
		}
		else
		{
			if (j<i)
			{
				redundent_->set(verts[j]);
				verts[j]=verts[i];
			}
		}
      }
	  else
	  {
		  redundent_->set(verts[i]);
	  }

    }

  for (i=1; i<=n0 - j; i++)
    verts.pop ();
}

/*--------------------------------------------------------------------------*/

Bool lexiCompare (const int *i, const int *j)
{
  return curBuilder_->compare(*i, *j);
}

Bool DCompBuilder::
compare (int i, int j)
{
//     COMMENT("curPArray in lC: " << DCompBuilder::curPArray_);
  double xi[4], xj[4];
  rArray_->getPoint(i, xi);
  rArray_->getPoint(j, xj);
  vsubw(xi, xj);
  converter_.truncMul(xi);
  int res;
  if (xi[0] == 0.0)
    {
      if (xi[1] == 0.0)
        {
          if (xi[2] == 0.0)
		  {
			if (xi[3]==0.0)
				res = 0;
			else if ( xi[3] <= 0.0)
				res = -1;
			else
				res = 1;
		  }
          else if ( xi[2] <= 0.0)
            res = -1;
          else
            res = 1;
        }
      else if (xi[1] <= 0.0)
       res = -1;
      else
       res = 1;
    }
  else if (xi[0] <= 0.0)
   res = -1;
  else
   res = 1;
  return res;
}

/*-----------------------------------------------------------------------*/

void DCompBuilder::
randomize (void)
{
  int i, j;
  int n = uninserted_.size();
  VIndex temp;
  
  for (i = n; i >= 2; i--) {
    j = (getRandom() % i) + 1;    /* 1 <= j <= i */
    temp = uninserted_[j];
    uninserted_[j] = uninserted_[i];
    uninserted_[i] = temp;
  }
}

/*-----------------------------------------------------------------------*/

void DCompBuilder::
hullInsert (VIndex v, OrTriStack& theList, OrTri& h)
     /* Output: *h. */
     /* ... inserts v into the triangulation by mounting v on each triangle
        in the_list; if the hull_flag is set, it is assumed that v was
        outside the (old) CH, and a new CH face, *h, is set. */
     /* NOTE: Here, it is also assumed that the_list is in DFS order
      *       (see delaunay_collect_visibles ()).  Since we use get() & push()
      *       to scan through it, this gives us the same order below!
      * ALSO: This is being quietly exported for code sharing!
      */
{
  if (debugFlag_)
  {
    COMMENT("\thullInsert (" << (int) v << ")");
  }

  int m = theList.size ();

#ifdef _DYNAMIC_
  simplexSet_.clear ();
#endif

  for (register int i = 1; i <= m; i++)
  {
    mountT (NORMAL, v, theList[i]);

#ifdef _DYNAMIC_
    simplexSet_.pushSimplex (theList[i], 3);  // new tetrahedron
#endif
  }

  restoreHullBits (v, h = getHull ());

#ifdef _DYNAMIC_
  // Callback for post-mount : Add vertex v, new tetrahedra, new edges.
  if (maintainIndex_)
  {
		  simplexSet_.pushSimplex (Enext2_ (Fnext_ (theList [1])), 0);  // vertex v
      callTristModCB (Add, & simplexSet_);

      if (debugFlag_)
      {
          COMMENT("Check after callTristModCB in hullInsert.....");
          for (int i=1; i <=m ; i++)
          {
              VIndex a,b,c;
              COMMENT("check OrTri " << theList[i]);
              tr_->triangle (theList [i], a,b,c);
              if ( (dcomp_->ix_->ix_v (v, a) < 0) ||
                   (dcomp_->ix_->ix_v (v, b) < 0) ||
                   (dcomp_->ix_->ix_v (v, c) < 0)   )
              {
                 COMMENT("...edges Fail");
              }
              else
              {
                 COMMENT("...edges Success");
              }
              if ( dcomp_->ix_->ix_v (v, a, b, c) < 0)
              {
                 COMMENT("...tetrahedron Fail");
              }
              else
              {
                 COMMENT("...tetrahedron Success");
              }
          }
          COMMENT(" ");
      }
  }
#endif
}

/*-----------------------------------------------------------------------*/

void DCompBuilder::
restoreHullBits (VIndex v, OrTri hf)
     /* ... restores hull bits in Trist after mounting of v,
        assuming: hf is on CH *AND* Org_ (hf) == v. */
{ 
#define next_CH(E)   Sym_ (Enext2_(Fnext_ (E)))
/* Gives next CH EdFacet with same Org_ as E (which is assumed to be on CH). */

  int counter = 0;
  OrTri h = hf;

  if (debugFlag_)
    {
      COMMENT("\trestoreHullBits for vertex" << (int) v << ", triangle ");
      tr_->printTriangle (cout, h);
    }
  
  Assert_always (Org_ (hf) == v);
  Assert (isHullFacet (hf));
  do
    {
      tr_->hullFacetSet (h, TRUE);
      h = next_CH (h);
      counter ++;
      //      Assert_always (counter <= trist_last ());  /* Emergency break! */
    } while (h != hf);
#undef next_CH
}

/*-----------------------------------------------------------------------*/

Bool DCompBuilder::
isHullFacet (OrTri h)
     /* ... checks locally (!) whether edfacet (!) h lies on the CH of
        the triangulation built so far; uses geometric primitives for its
        decision! */
     /* NOTE: I think we could also do this by checking the existence of
        incident tetrahedra; note, however, that this isn't called very
        often. */
{
  VIndex x, y, z, u, v, w;
  tr_->triangle (h, x, y, z);
  u = Dest_ (Enext_ (Fnext_ (h)));
  v = Dest_ (Enext_ (Fnext_ (h = Enext_ (h))));
  w = Dest_ (Enext_ (Fnext_ (Enext_ (h))));
  if (sEval_.positive3 (*rArray_, u, x, y, z) or
      sEval_.positive3 (*rArray_, v, x, y, z) or
      sEval_.positive3 (*rArray_, w, x, y, z))
    return (FALSE);
  else
    return (TRUE);
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
collectVisibles (VIndex v, OrTri ef, OrTriStack& v_list)
     /* Output: v_list. */
     /* GLOBAL: rx. */
     /* ... collects all CH facets visible from v and stores then in v_list.
        Assumption: hx is on CH and visible from v. */
{
#ifdef __DEBUG__
  VIndex a, b, c;
  tr_->triangle (ef, a, b, c);
  Assert (isHullFacet (ef) and sEval_.positive3 (*rArray_, v, a, b, c));
#endif
  Assert_always (v_list.empty ());
  tr_->hullFacetSet (ef, FALSE);
  v_list.push (ef);
/*
  if(v==710)
	  vlink.set(ef.index());
*/
  collectVisiblesDFS (v, v_list, Fnext_(ef).sym());
  collectVisiblesDFS (v, v_list, Fnext_(ef = ef.enext()).sym());
  collectVisiblesDFS (v, v_list, Fnext_(ef = ef.enext()).sym());

#if 0
@DT
  if (debugFlag_)
    COMMENT("\tVisibles for vertex " << (int) v << ":" << v_list);
#endif
}

/*-----------------------------------------------------------------------*/

void DCompBuilder::
collectVisiblesDFS (VIndex v, OrTriStack& v_list, OrTri ef)
     // Output: v_list. 
     // GLOBAL: rx. 
     // Depth-first serach. 
{
  dfsStack_.reset();
  dfsStack_.push(ef);
  VIndex i, j, k;
  while(!dfsStack_.empty())
  {
    ef = dfsStack_.pop();
    if (!tr_->isOnHull (ef))
      continue;
    tr_->triangle (ef, i, j, k);
    if (sEval_.positive3 (*rArray_, v, i, j, k))
    {
      tr_->hullFacetSet (ef, FALSE);
      v_list.push (ef);
/*
	  if(v==710)
		  vlink.set(ef.index());
*/
      dfsStack_.push(Fnext_ (ef = ef.enext()).sym());
      dfsStack_.push(Fnext_ (ef = ef.enext()).sym());
    }
    else
      rx_ = ef;  // set the reference facet! 
  }
}




/*-----------------------------------------------------------------------*/

OrTri DCompBuilder::
getHull (void)
{
  Assert (rx_.getIntRep ());
  return (Sym_ (Enext_ (Fnext_ (rx_))));
}

/*-----------------------------------------------------------------------*/

int DCompBuilder::
processFlips (OrTriStack& ndList, VIndex v)
     /* Flips non-Delaunay triangles in given Trist until all triangles
        are locally Delaunay (cf, delaunay_test() and delaunay_test_put()).
        ASSUMPTION: Trist is non-Delaunay after inserting point v,
        AND: all possibly non-Delaunay link-facets after are on nd_list.
        Upon success (which should always happen, by now :) delaunay_flip()
        returns the number of necessary flips. As a side effect, nd_list
        will be cleared, and current Trist updated accordingly. */
{
  int f0 = eflips_ + fflips_;
  int n = rArray_->size ();
  int xyConv,yzConv,zxConv;

  vlink.reset();

  maxFlips_ = n * (n + 3) + 1;  /* UB on total number of flips;
           see Herbert's Triangulation notes, p95 */
  if (maxFlips_ <= 0)
    { /* Overflow! */
      maxFlipsOverflowFlag_ = TRUE;
      maxFlips_ = MAXINT;
    }
  else
    maxFlipsOverflowFlag_ = FALSE;

  theV = new_v = v;
  double x[4];
  rArray_->getPoint(v,x);
  while (not ndList.empty ())
    addToFlipList (ndList.pop ());

  Assert ((reds_ == 0) and (greens_ == flipList_.size ()));
  {
    OrTri xyz;
    while (nextNDFacet (xyz))
      {
		xyConv = NOTTESTED;
		yzConv = NOTTESTED;
		zxConv = NOTTESTED;
        Assert_always (maxFlips_ >= 0);  /* emergency break! */
		if(not flipVertice(xyz))
		  if (not flipEdge (xyz,xyConv,yzConv,zxConv))
			if (not flipEdge (Enext_ (xyz),yzConv,zxConv,xyConv))
              if (not flipEdge (Enext2_ (xyz),zxConv,xyConv,yzConv))
                if (not flipTriangle (Enext2_(xyz),xyConv,yzConv,zxConv))
				{ /* non-transformable triangle */
                    skips_ ++;
                    markTriangle (xyz.index (), Red);
					theRedTri_ = xyz.index();
				}
      }
  }
  if (reds_ or greens_)
	{
	    int v1 = Dest_(Fnext_(OrTri(theRedTri_,0)));
	    int v2 = Dest_(Fnext_(OrTri(theRedTri_,0).sym()));
		problemVertexIX_ = v;
	}
  if (maxFlipsOverflowFlag_)
    maxFlips_ = MAXINT;
  return (eflips_ + fflips_ - f0);
}


/*-----------------------------------------------------------------------*/

Bool DCompBuilder::
nextNDFacet (OrTri& ef)
     /* Takes facets from flipList_ and returns first one with
        delaunayTest (ef) == FALSE. Returns FALSE if none such is found. */
{
  FIndex t;
  OrTri curEF;

  while (not flipList_.empty ())
    {
      curEF = flipList_.pop ();      /*** flipList_ is a stack! ***/
      t = curEF.index ();
      if ((not tr_->deleted (t)) and (getColor (t) == Green))
        { /* ef is still on flipList_; check NOW if it's still non-Delaunay */
          if (delaunayTest (curEF))
            markTriangle (t, White);
          else
      {
        ef = curEF;
        return (TRUE);
      }
        }
    }
  return (FALSE);
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
addToFlipList (OrTri ef)
     /* Adds ef to flipList_, but only if it is
        (a) not on flipList_ already (ie, not GREEN),
        (b) not on CH, and
        (c) a link facet. */
{
  VIndex a, b, c;
  FIndex t = ef.index ();
  Assert(Dest_(Fnext_((ef)).enext())==new_v);
  vlink.set(ef.index());

  if ((getColor (t) != Green) and (not tr_->isOnHull (t)))
    {
      tr_->triangle (ef, a, b, c);
      if ((new_v != a) and (new_v != b) and (new_v != c))
        {
          flipList_.push (ef);   /*** flipList_ is a stack! ***/
          markTriangle (ef.index (), Green);
        }
	  else Assert(0);
    }
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
mountT (char code, VIndex p, OrTri a)
     /* Mounts tetrahedron connecting new point v = p with ijk = a, some
        visible face on CH.  Distinguishes a couple of cases; cf mount* (). */
     /* GLOBAL: hull, v, i, j, k. */
{

  OrTri jki, kij, ijv, ivj, vji, vkj, vki, jkv, kiv;  
  Bool b1, b2, vau, cov;
  /* NOTE: a was on hull but hull bit might already have been deleted */
  ijk_ = a;
  v_ = p;
  tr_->triangle (ijk_, i_, j_, k_);
  Assert (sEval_.positive3 (*rArray_, v_, i_, j_, k_));
  if (code == INITIAL)
    {
      mount1 ();
      return;
    }
  jki = Enext_ (ijk_);
  kij = Enext_ (jki);
  ijv = Fnext_ (ijk_);
  jkv = Fnext_ (jki);
  kiv = Fnext_ (kij);
  vji = Sym_ (Enext_ (ijv));
  vau = (Org_ (vji) == v_);
  cov = ((Turn (ijv) == jkv) and (Turn (jkv) == kiv) and (Turn (kiv) == ijv));
  if (not (vau and cov))
    {  /* act iff not already mounted w/ tetrahedron w/ top = v */
      vkj = Sym_ (Enext_ (jkv));
      vki = Enext2_(kiv);
      b1 = (Org_ (vkj) == v_);
      b2 = (Org_ (vki) == v_);
      ivj = Enext_ (Sym_ (ijv));
      PROTO (COMMENT ("mountT [" << ijk_ << "]=t" << ijk_.index() << " = " 
        << i_ << "," << j_ << "," << k_ << " visible from " << v_ 
        << " (" << Org_ (vkj) << "," << Org_ (vki) << "," << vau << ")"));
      Assert_if (debugFlag_ and vau,
                 (    tr_->testTriangle (ijk_, i_, j_, k_)
                  and tr_->testTriangle (vji, v_, j_, i_)
                  and tr_->testTriangle (ijv, i_, j_, v_)
                  and tr_->testTriangle (ivj, i_, v_, j_)));
      Assert (vau or (not (b1 or b2)));
      /* > NOTE: This assumption is essential because of the "if" below.
         >       It is true since facets are processed in DFS order! */
      if (not vau)
        mount1 ();
      else if (b1)
        {
          if (b2)
            mount4 (vkj, vki, ivj, vji);
          else 
            mount3a (ijv, vkj, vji);
        }
      else if (b2)
        mount3b (ijv, ivj, vki);
      else
        mount2 (jki, kij, ivj, vji);
      Assert (    (Org_ (Enext_ (Fnext_ (jki))) == k_)
              and (Org_ (Enext_ (Sym_ (Fnext_ (kij)))) == k_)
              and (Org_ (Sym_ (Enext_ (Fnext_ (jki)))) == v_)
              and (Org_ (Enext2_(Fnext_ (kij))) == v_));
    }
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
mount1 (void)
     /* GLOBAL: v_, ijk_, i_, j_, k_. */
     /* Mounts v on "top" of ijk_; creates 3 * 3 new edfacets;
        assumes: edge-ring of ijk_ is seen making a CCW turn from vertex v. */
{ 
  OrTri ijv, jkv, kiv, jki = Enext_ (ijk_), kij = Enext_ (jki); 
  mount1s_ ++;
  ijv = tr_->makeTri (i_, j_, v_);
  jkv = tr_->makeTri (j_, k_, v_);
  kiv = tr_->makeTri (k_, i_, v_);
  Fsplice_ (ijk_, ijv);
  Fsplice_ (jki, jkv);
  Fsplice_ (kij, kiv);
  Fsplice_ (Enext_ (ijv), Enext_ (Sym_ (jkv)));
  Fsplice_ (Enext_ (jkv), Enext_ (Sym_ (kiv)));
  Fsplice_ (Enext_ (kiv), Enext_ (Sym_ (ijv)));
  PROTO (COMMENT ("mount1 done: ijk_=[" << ijk_ << "] tetra=(" 
    << i_ << "," << j_ << "," << k_ << "," << v_ << ")"));
  Assert_if (debugFlag_,
             (    sEval_.positive3 (*rArray_, v_, i_, j_, k_)
              and tr_->testOpenTetra (ijv, i_, j_, k_, v_)
              and tr_->testOpenTetra (Enext_ (Sym_ (ijk_)), i_, k_, v_, j_)));

#ifdef _DYNAMIC_
  simplexSet_.pushSimplex (Enext_(ijv), 1);    // new edge (j v)i
  simplexSet_.pushSimplex (Enext_(jkv), 1);    // new edge (k v)j
  simplexSet_.pushSimplex (Enext_(kiv), 1);    // new edge (i v)k
#endif
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
mount2 (OrTri jki, OrTri kij, OrTri ivj, OrTri vji)
     /* GLOBAL: v_, ijk_, i_, j_, k_. */
     /* Mounts 2 triangular facets given by edge-rings represented by GLOBAL
        ijk_ and Sym_ (Fnext_ (ijk_)); assumes: sEval_.positive3 (v_, i_, j_, k_);
        and: mounted facets are on hull; and: Sym_ (Fnext_ (ijk_)) == jiv. */
{
  OrTri jkv, kiv, ivk, kvj;
  mount2s_ ++;
  jkv = tr_->makeTri (j_, k_, v_);
  kiv = tr_->makeTri (k_, i_, v_);
  ivk = Enext_ (kiv);
  kvj = Enext_ (jkv);
  Fsplice_ (jki, jkv);
  Fsplice_ (kij, kiv);
  Fsplice_ (ivj, ivk);
  Fsplice_ (kvj, Sym_ (Enext_ (ivk)));
  Fsplice_ (vji, Enext_ (kvj));
  PROTO (COMMENT ("mount2 done: ijk_=[" << ijk_ << "] edge=(" 
    << v_ << "," << k_ << ") tetra=("
    << i_ << "," << j_ << "," << k_ << "," << v_ << ")"));
//  PROTO (printf ("mount2 done: ijk_=[%d] edge=(%d,%d) tetra=(%d,%d,%d,%d)\n",
//                ijk_, v_, k_, i_, j_, k_, v_));
  Assert_if (debugFlag_,
             tr_->testOpenTetra (kiv, k_, i_, j_, v_));

#ifdef _DYNAMIC_
  simplexSet_.pushSimplex (kvj, 1);           // new edge (k v)j
#endif
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
mount3a (OrTri ijv, OrTri vkj, OrTri vji)
     /* GLOBAL: v_, ijk_, i_, j_, k_. */
     /* Nearly same prereqisites as mount2()... */
{
  OrTri vki;
  mount3as_ ++;
  vki = tr_->makeTri (v_, k_, i_);
  Fsplice_ (vkj, vki);
  Fsplice_ (Enext2_(ijk_), Enext_ (vki));
  Fsplice_ (Enext_ (Sym_ (ijv)), Enext2_(vki));
  OrTri vkjse = Enext_ (Sym_ (vkj));
  if (Fnext_ (vji) != vkjse)
    Fmerge_ (vji, vkjse);
  PROTO (COMMENT ("mount3a done: ijk_=[" << ijk_ << "] vkj=[" << vkj << "] tetra=("
    << i_ << "," << j_ << "," << k_ << "," << v_ << ")"));
//  PROTO (printf ("mount3a done: ijk_=[%d] vkj=[%d] tetra=(%d,%d,%d,%d)\n",
//                ijk_, vkj, i_, j_, k_, v_));
  Assert_if (debugFlag_,
             tr_->testOpenTetra (ijv, i_, j_, k_, v_));
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
mount3b (OrTri ijv, OrTri ivj, OrTri vki)
     /* GLOBAL: v_, ijk_, i_, j_, k_. */
     /* Nearly same prereqisites as mount2()... */
{
  OrTri vkj;
  mount3bs_ ++;
  vkj = tr_->makeTri (v_, k_, j_);
  Fsplice_ (Sym_ (vki), Sym_ (vkj));
  Fsplice_ (Enext_ (ijk_), Sym_ (Enext_ (vkj)));
  Fsplice_ (Sym_ (Enext_ (ijv)), Enext_ (Sym_ (vkj)));
  OrTri vkie2 = Enext2_(vki);
  if (Fnext_ (ivj) != vkie2)
    Fmerge_ (ivj, vkie2);
  PROTO (COMMENT ("mount3b done: ijk_=[" << ijk_ << "] vki=[" << vki << "] tetra=("
    << i_ << "," << j_ << "," << k_ << "," << v_ << ")"));
//  PROTO (printf ("mount3b done: ijk_=[%d] vki=[%d] tetra=(%d,%d,%d,%d)\n",
//                ijk_, vki, i_, j_, k_, v_));
  Assert_if (debugFlag_,
             tr_->testOpenTetra (ijv, i_, j_, k_, v_));
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
mount4 (OrTri vkj, OrTri vki, OrTri ivj, OrTri vji)
     /* GLOBAL: v_, ijk_, i_, j_, k_. */
     /* No comment. */
{
  Bool a = (Fnext_ (vkj) != vki);
  OrTri vkjse = Enext_ (Sym_ (vkj));
  Bool b = (Fnext_ (vji) != vkjse);
  OrTri vkie2 = Enext2_(vki);
  Bool c = (Fnext_ (ivj) != vkie2);
  mount4s_ ++;
  if (a)
    Fmerge_ (vkj, vki);
  if (b)
    Fmerge_ (vji, Enext_ (Sym_ (vkj)));
  if (c)
    Fmerge_ (ivj, Enext2_(vki));
  PROTO (COMMENT ("mount4 done: vkj=[" << vkj << "] vki=[" << vki << "] tetra=("
    << i_ << "," << j_ << "," << k_ << "," << v_ << ")"));
//  PROTO (printf ("mount4 done: vkj=[%d] vki=[%d] tetra=(%d,%d,%d,%d)\n",
//                vkj, vki, i_, j_, k_, v_));
  Assert_if (debugFlag_,
             (    (a or b or c)
              and tr_->testTriangle (vki, v_, k_, i_)
              and tr_->testTriangle (vkj, v_, k_, j_)
              and tr_->testOpenTetra (Sym_ (ijk_), j_, i_, v_, k_)));
}


/*--------------------------------------------------------------------------*/

Bool DCompBuilder::
flipVertice (OrTri xyz)
    /*  try to flip vertex x,y or z */
{
  FlipEvent fEvent;
  OrTri flipv;
  OrTri t1,t2,t3,t4,t5;
  Bool xRd = FALSE; // x redundent
  Bool yRd = FALSE;
  Bool zRd = FALSE;
  Bool xyDeg3 = (xyz.index()==Fnext_(Fnext_(Fnext_(xyz))).index());
  Bool yzDeg3 = (Enext_(xyz).index()==Fnext_(Fnext_(Fnext_(Enext_(xyz)))).index());
  Bool zxDeg3 = (Enext2_(xyz).index()==Fnext_(Fnext_(Fnext_(Enext2_(xyz)))).index());

  if(tr_->isOnHull(xyz))
	  return false;

    if(yzDeg3&&zxDeg3) // yz and zx concave
	{
		flipv = Enext2_(xyz);
		if(!tr_->isOnHull(Fnext_(flipv)))
			if(!tr_->isOnHull(Fnext_(Sym_(flipv))))
				if(!tr_->isOnHull(Fnext_(Enext2_(flipv))))
					if(!tr_->isOnHull(Fnext_(Sym_(Enext2_(flipv)))))
						if(!tr_->isOnHull(Fnext_(Enext_(Fnext_(Enext2_(flipv))))))
							// z is redundent
							zRd=TRUE;
	}
	
    if(zxDeg3&&xyDeg3&&!zRd) // zx and xy concave
	{
		flipv = xyz;
		if(!tr_->isOnHull(Fnext_(flipv)))
			if(!tr_->isOnHull(Fnext_(Sym_(flipv))))
				if(!tr_->isOnHull(Fnext_(Enext2_(flipv))))
					if(!tr_->isOnHull(Fnext_(Sym_(Enext2_(flipv)))))
						if(!tr_->isOnHull(Fnext_(Enext_(Fnext_(Enext2_(flipv))))))
							// x is redundent
							xRd=TRUE;
	}

    if(yzDeg3&&xyDeg3&(!zRd)&&!xRd) // yz and xy concave
	{
        flipv = Enext_(xyz);
		if(!tr_->isOnHull(Fnext_(flipv)))
			if(!tr_->isOnHull(Fnext_(Sym_(flipv))))
				if(!tr_->isOnHull(Fnext_(Enext2_(flipv))))
					if(!tr_->isOnHull(Fnext_(Sym_(Enext2_(flipv)))))
						if(!tr_->isOnHull(Fnext_(Enext_(Fnext_(Enext2_(flipv))))))
							// y is redundent
							yRd=TRUE;
	}

    if(! (xRd||yRd||zRd) )
		return (FALSE);

	if(xRd)
		flipv = xyz;
	else if(yRd)
		flipv = Enext_(xyz);
	else flipv = Enext2_(xyz);

	t1 = Fnext_(flipv);
	t2 = Fnext_(Sym_(flipv));
	t3 = Fnext_(Enext2_(flipv));
	t4 = Fnext_(Sym_(Enext2_(flipv)));
	t5 = Fnext_(Enext_(Fnext_(Enext2_(flipv))));
	if (tr_->isOnHull(t1) or
		tr_->isOnHull(t2) or
		tr_->isOnHull(t3) or
		tr_->isOnHull(t4) or
		tr_->isOnHull(t5) )
		return FALSE;

	markTriangle (flipv.index(),White);
	markTriangle (t1.index(),White); 
	markTriangle (t2.index(),White); 
	markTriangle (t3.index(),White); 
	markTriangle (t4.index(),White); 
	markTriangle (t5.index(),White); 

	redundent_->set(Org_(flipv));
/*
	if(Org_(flipv)!=new_v)
		flipv = flipv.enext().sym();
*/
	fh_->flipVertex(Enext2_(flipv),fEvent);
	Assert(!vlink.test(fEvent.getTri(8).index()));
	Assert(!vlink.test(fEvent.getTri(6).index()));
	Assert(!vlink.test(fEvent.getTri(7).index()));
	Assert(!vlink.test(fEvent.getTri(9).index()));
	Assert(vlink.test(fEvent.getTri(5).index()));
	Assert(vlink.test(fEvent.getTri(0).index()));
	Assert(vlink.test(fEvent.getTri(2).index()));
	vlink.clear(fEvent.getTri(2).index());
	vlink.clear(fEvent.getTri(0).index());
	vlink.clear(fEvent.getTri(5).index());
	Assert(!vlink.test(fEvent.getTri(1).index()));
	Assert(!vlink.test(fEvent.getTri(3).index()));
	Assert(!vlink.test(fEvent.getTri(4).index()));

	addToFlipList (fEvent.getTri (8));
	Assert(vlink.test(fEvent.getTri(8).index()));
/*
	for(j=0;j<=9;j++)
		vlink.clear(fEvent.getTri(j).index());
	vlink.set(fEvent.getTri(9).index());
*/
	return (TRUE);
}


/*--------------------------------------------------------------------------*/

Bool DCompBuilder::
flipEdge (OrTri pox,int& poConv,int& oxConv,int& xpConv)
     /* Flips edge pox to triangle xyz, in case it's transformable;
        returns FALSE otherwise. Assumes: delaunay_test (pox) == FALSE. */
     /* NOTE: Transformability of edge can be tested WITHOUT geometric tests.
              We just have to make sure, it's not a CH edge. */
{
  // VIndex x, y, z, o, p;
  OrTri poy, poz;
  int j;
  Bool isTransformable;
  FlipEvent fEvent;

  Assert (    (getColor (pox.index ()) == Green)
          and (not delaunayTest (pox)));
  poy = Fnext_ (pox);
  poz = Fnext_ (poy);

  VIndex x,y,z,o,p;
  p = Org_(pox);
  o = Dest_(pox);
  x = Dest_(Enext_(pox));
  y = Dest_(Enext_(poy));
  z = Dest_(Enext_(poz));
  
//  Assert(sEval_.positive3 (*rArray_, o, x, y, z));
//  Assert(sEval_.positive3 (*rArray_, p, y, x, z));


	if (Fnext_ (poz) != pox) 
		isTransformable = FALSE;  /* no degree-3 edge */
	else {
		// we need ox and xp convex to flip the edge

/*
		if(oxConv==NOTTESTED)
			oxConv= sEval_.positive3 (*rArray_,z,o,y,x);
		if(!oxConv)
			return FALSE;

		if(xpConv==NOTTESTED)
			xpConv= sEval_.positive3 (*rArray_,z,p,x,y);
		if(!xpConv)
			return FALSE;

		if((!xpConv)||(!oxConv))
			isTransformable = FALSE;
*/
/*
		if((!sEval_.positive3 (*rArray_,o,x,y,z)) ||
			!sEval_.positive3 (*rArray_,p,x,z,y))
			isTransformable = FALSE;
*/
		if(sEval_.positive3(*rArray_,o,x,y,z)==sEval_.positive3(*rArray_,p,x,y,z))
			isTransformable = FALSE;
		else 
		isTransformable = ((not (tr_->isOnHull (poy)))
           and (not (tr_->isOnHull (poz))));
		
	}
  
  if (isTransformable)
    {
      /* Edge-to-triangle flip:
         assumig sos_positive3 (o, x, y, z) and sos_positive3 (p, y, x, z). */
      eflips_ ++;
      maxFlips_ --;


      /*
      trist_triangle (pox, &p,  &o,  &x);
      y = Dest_ (Enext_ (poy));
      z = Dest_ (Enext_ (poz));
      Assert ((y == new_v) or (z == new_v));
      Assert_if (delaunay_flip_test_flag,
                 (    sos_positive3 (o, x, y, z)
                  and sos_positive3 (p, y, x, z)
                  and (not sos_positive3 (p, y, z, o))
                  and (not sos_positive3 (p, z, x, o))
                  and (not sos_positive3 (p, x, y, o))));
     PROTO (COMMENT ("FLIP edge to tri [%d|%d|%d] :: %d,%d,%d;%d,%d\n",
                    pox, poy, poz, x, y, z, o, p));
      xyo = Enext_ (Fnext_ (Enext_ (pox)));
      delete_triangle (pox);
      delete_triangle (poy);
      delete_triangle (poz);
      load (xyo, field, x, y, z, o, p);
      cut (field, x, y, z, o, p);
      */

      markTriangle (pox.index (), White);
      markTriangle (poy.index (), White);
      markTriangle (poz.index (), White);

	  Assert(y==new_v);
      fh_->flipEdge (pox, fEvent);
	  Assert(vlink.test(fEvent.getTri (0).index()));
	  vlink.clear(fEvent.getTri(0).index());
	  Assert(!vlink.test(fEvent.getTri (1).index()));
	  Assert(vlink.test(fEvent.getTri (2).index()));
	  vlink.clear(fEvent.getTri(2).index());
	  Assert(!vlink.test(fEvent.getTri (4).index()));
	  Assert(!vlink.test(fEvent.getTri (5).index()));
	  Assert(!vlink.test(fEvent.getTri (6).index()));
	  Assert(!vlink.test(fEvent.getTri (7).index()));
	  Assert(!vlink.test(fEvent.getTri (8).index()));
	  Assert(!vlink.test(fEvent.getTri (9).index()));
      for (j=8; j<=9; j++)
        addToFlipList (fEvent.getTri (j));
	  Assert(vlink.test(fEvent.getTri (8).index()));
	  Assert(vlink.test(fEvent.getTri (9).index()));
      //      if (hook)
      //        {
      //          int xyz = Sym_ (Fnext_ (Sym_ (xyo)));
      //          Assert (dt_test_triangle (xyz, x, y, z));
      //          hook (DT_FLIP_EDGE, x, y, z, o, p, xyz);                
      //        }

      return (TRUE);
    }
  else
    return (FALSE);
}

/*--------------------------------------------------------------------------*/

Bool DCompBuilder::
flipTriangle (OrTri xyz,int xyConv,int yzConv,int zxConv)
    /* Flips triangle xyz to edge pox, in case it's transformable.
        Assumption: delaunay_test (xyz) == FALSE. */
{
  VIndex x, y, z, o, p;

  OrTri xyo = Fnext_ (xyz);
  int j;
  Bool isConcave;
  FlipEvent fEvent;

  tr_->triangle (xyz, x, y, z);
  o = Dest_ (Enext_ (xyo));
  p = Dest_ (Enext_ (Fnext_ (Sym_ (xyz))));

  /*
  if(xyConv==NOTTESTED)
	  xyConv=sEval_.positive3 (*rArray_,p,x,o,y);
  if(!xyConv)
	  return FALSE;

  if(yzConv==NOTTESTED)
	  yzConv=sEval_.positive3 (*rArray_,p,o,z,y);
  if(!yzConv)
	  return FALSE;

  if(zxConv==NOTTESTED)
	  zxConv=sEval_.positive3 (*rArray_,p,x,z,o);
  if(!zxConv)
	  return FALSE;

  Assert(xyConv!=NOTESTED);
  Assert(yzConv!=NOTESTED);
  Assert(zxConv!=NOTESTED);

  isConcave= !(xyConv&&yzConv&&zxConv);
*/
  isConcave = sEval_.positive3 (*rArray_, p, y, z, o) or
			sEval_.positive3 (*rArray_, p, z, x, o) or
			sEval_.positive3 (*rArray_, p, x, y, o) ;


  Assert  ((o == new_v) or (p == new_v));
  Assert_if (debugFlag_, 
             (    sEval_.positive3 (*rArray_, o, x, y, z)
              and (not sEval_.positive3 (*rArray_, p, x, y, z))
              and tr_->testTriangle (xyo, x, y, o)
              and tr_->testTriangle (Fnext_ (Sym_ (xyz)), y, x, p)));
  PROTO (COMMENT (If (isConcave, "SKIP tetra", "FLIP tri to edge") 
    << " [" << xyz << "] t" << xyz.index () << " = " 
    << x << "," << y << "," << z << ";" << p << "," << o));
//  PROTO (printf ("%s [%d] t%d = %d,%d,%d;%d,%d\n",
//                If (isConcave, "SKIP tetra", "FLIP tri to edge"),
//                xyz, xyz.index (), x, y, z, p, o));

  if (isConcave)
    return (FALSE);
  else
    { /* Triangle-to-edge flip:
         assumig sos_positive3 (o, x, y, z) and sos_positive3 (p, y, x, z). */
      fflips_ ++;
      maxFlips_ --;


      /*
      delete_triangle (xyz);
      load (xyo, field, x, y, z, o, p);
      diagonalize (field, x, y, z, o, p);
      */

	  Assert(o==new_v);
      markTriangle (xyz.index (), White);

      fh_->flipTriangle (xyz, fEvent);
	  Assert(vlink.test(fEvent.getTri(0).index()));
	  vlink.clear(fEvent.getTri(0).index());
	  Assert(!vlink.test(fEvent.getTri(4).index()));
	  Assert(!vlink.test(fEvent.getTri(5).index()));
	  Assert(!vlink.test(fEvent.getTri(6).index()));
	  Assert(!vlink.test(fEvent.getTri(7).index()));
	  Assert(!vlink.test(fEvent.getTri(8).index()));
	  Assert(!vlink.test(fEvent.getTri(9).index()));

      for (j=7; j<=9; j++)
//      for (j=4; j<=9; j++)
        addToFlipList (fEvent.getTri (j));
	  Assert(vlink.test(fEvent.getTri(7).index()));
	  Assert(vlink.test(fEvent.getTri(8).index()));
	  Assert(vlink.test(fEvent.getTri(9).index()));
      //      if (hook)
      //        {
      //          int pox = Sym_ (Enext_ (Fnext_ (Enext_ (Sym_ (xyo)))));
      //          Assert (dt_test_triangle (pox, p, o, x));
      //          hook (DT_FLIP_TRIANGLE, x, y, z, o, p, pox);            
      //        }
      
      return (TRUE);
    }
}

/*--------------------------------------------------------------------------*/

Bool DCompBuilder::
delaunayTest (OrTri ef)
     /* Tests triangle of ef for local Delaunayhood. Either closest-point
        (which is default) or furthest-point; see delaunay_test_set_fp(). */
{
  VIndex x, y, z, o, p;
  if (tr_->isOnHull (ef.index ()))
    return (TRUE);
  tr_->triangle (ef, x, y, z);
  o = Dest_ (Enext_ (Fnext_ (ef)));
  p = Dest_ (Enext_ (Fnext_ (Sym_ (ef))));
  /*
    Assert_if (debugFlag_,
             (    sEval_.positive3 (o, x, y, z)
              and (not sEval_.positive3 (p, x, y, z))
              and tr_->testTriangle (Fnext_ (ef), x, y, o)
              and tr_->testTriangle (Fnext_ (Sym_ (ef)), y, x, p)));
        */
#ifdef __DEBUG__
/*
  Bool res = FALSE;
  res = sEval_.positive3 (*rArray_, o, x, y, z);
  Assert (res);
  res = sEval_.positive3 (*rArray_, p, x, y, z);
  Assert (not res);
  Assert (tr_->testTriangle (Fnext_ (ef), x, y, o));
  Assert (tr_->testTriangle (Fnext_ (Sym_ (ef)), y, x, p));
*/
#endif
  return (sEval_.inSphereP (*rArray_, o, x, y, z, p));
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

// Triangle status

DCompBuilder::Color DCompBuilder::
getColor (FIndex t)
{
  //  int c1 = If (triStatus_.test (2 * t), 1, 0);
  //  int c2 = If (triStatus_.test (2 * t + 1), 1, 0);

  if (! triStatus_.test (2 * t))
    if (! triStatus_.test (2 * t + 1))
      return White;
    else
      return Green;
  else
    if (! triStatus_.test (2 * t + 1))
      return Red;
    else
      {
				basic_error ("Illegal Color\n");
				return Red;
      }
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
setColor (FIndex t, Color color)
{
  switch (color)
    {
    case White:
      triStatus_.clear (2 * t);
      triStatus_.clear (2 * t + 1);
      break;
    case Green:
      triStatus_.clear (2 * t);
      triStatus_.set   (2 * t + 1);
      break;
    case Red:
      triStatus_.set  (2 * t);
      triStatus_.clear (2 * t + 1);
      break;
    }
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
markTriangle (FIndex t, Color color)
  /* Marks triangle t with given color. */
{
  switch (color)
    {
     case White:
      {
        switch (getColor (t))
          {
           case Red:
            reds_ --;
            setColor (t, White);
      //      COMMENT("UN-Marking red triangle ");
      //      tr_->printTriangle (cout, OrTri (t, 0));
            break;
           case Green:
            greens_ --;
            setColor (t, White);
      //      COMMENT("UN-Marking green triangle ");
      //      tr_->printTriangle (cout, OrTri (t, 0));
            break;
           default:
            ; /* don't do anything if it wasn't RED or GREEN */
          }
  //  COMMENT("Marking white triangle ");
  //  tr_->printTriangle (cout, OrTri (t, 0));
        break;
      }
     case Green:
      {
        Assert (getColor (t) != Green);
        if (getColor (t) == Red)
          reds_ --;
        setColor (t, Green);
        greens_ ++;
  //  COMMENT("Marking green triangle ");
  //  tr_->printTriangle (cout, OrTri (t, 0));
        break;
      }
     case Red:
      {
		Assert(getColor(t)!=Red);
        if (getColor (t) == Green)
    {
      greens_ --;
      //      COMMENT("UN-Marking green triangle ");
      //      tr_->printTriangle (cout, OrTri (t, 0));
    }
        setColor (t, Red);
        reds_ ++;
  //  COMMENT("Marking red triangle ");
  //  tr_->printTriangle (cout, OrTri (t, 0));
        break;
      }
     default:
      Assert_always (FALSE);
    }
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
peelFlatTetras (void)
  /* Peels all flat tetrahedra from the convex hull of the triangulation. */
{
  OrTriStack q;
  
  if (debugFlag_)
  {
      COMMENT("> Removing SoS artifacts ...");
  }

  TristHullIter  iter (*tr_);
  for (iter.first (); ! iter.isDone (); iter.next ())
  {
			OrTri ef  = Sym_ (iter.curOrTri ());		 // flip triangle inward
			                                       // to represent Hull Tetrahedron  
      LI_Point liPoints[4];
      liPoints[0].alloc(); liPoints[1].alloc(); 
      liPoints[2].alloc(); liPoints[3].alloc();
      pushIfFlat (q, ef, liPoints);
  }

  if (debugFlag_)
  {
      COMMENT("Hull Triangles of Flat tetrahedra found: " << q.size ());
  }

  int cnt = 0;

  for (int i=1; i <= q.size (); i++)      // re-evaluate q.size () for q can grow
  {
      OrTri ef = q[i];
      
      if (tr_->deleted (ef.index ()))  continue;

      // workaround for an SGI compiler
      if (!tr_->deleted (ef.index ())) {

      /* the flat tetrahedron is incident to ef */
      Assert (tr_->isOnHull (Sym_ (ef)));
      cnt ++;

      OrTri ff [4];
      ff[0] = Sym_ (ef);
      ff[1] = Fnext_ (ef);
      ff[2] = Fnext_ (Enext_ (ef));
      ff[3] = Fnext_ (Enext2_(ef));


      LI_Point liPoints[4];
      liPoints[0].alloc(); liPoints[1].alloc(); 
      liPoints[2].alloc(); liPoints[3].alloc();

      for (int j=0; j <= 3; j++)
      {
          if (tr_->deleted (ff[j].index ())) continue;
      
          if (tr_->isOnHull (ff[j]))
          {

#ifdef _DYNAMIC_
              if (maintainIndex_)    prePeelDelTri_ (ff[j]);
#endif
              tr_->hullFacetSet (ff[j], FALSE);
              tr_->delTri (ff[j]);
          }
          else
          {
              tr_->hullFacetSet (Sym_ (ff[j]), TRUE);
              pushIfFlat (q, ff[j], liPoints);
          }
      }/* for int j */

      // workaround, see above
      }
  }/* for int i */

#ifdef _DYNAMIC_
	if (maintainIndex_)    postPeelAddVertex_ ();
#endif

  if (debugFlag_)
  {
    COMMENT("> Successful: t=t-" << cnt << ".");
  }
}

/*--------------------------------------------------------------------------*/
#ifdef _DYNAMIC_

void DCompBuilder::
prePeelDelTri_ (OrTri ef)
{
  simplexSet_ .clear ();                                  // for deletion
	simplexSet2_.clear ();                                  // for update

	simplexSet_.pushSimplex (Sym_ (ef), 3);									// del flat tetrahedron

  OrTri e1 = ef;
  do
  {		  
      OrTri e2 = Fnext_ (e1);

      simplexSet_.pushSimplex (e1, 0);                    // Delete Vertex
                                                          // might add it back later
      if (e2 == e1)
      {
					simplexSet_.pushSimplex (e2, 1);    						// Delete edge
			}
			else
      {
				  simplexSet2_.pushSimplex (e2, 1);               // Update edge
      }
      e1 = Enext_ (e1);
  }while (e1 != ef);

	callTristModCB (Del, & simplexSet_);
	callTristModCB (Upd, & simplexSet2_);
}

#endif
/*--------------------------------------------------------------------------*/
#ifdef _DYNAMIC_

void DCompBuilder::
postPeelAddVertex_ (void)
{
	simplexSet_.clear ();

	TristHullIter iter (*tr_);
	for (iter.first (); ! iter.isDone (); iter.next ())
	{
		  OrTri ef = iter.curItem ();
      
			OrTri e1 = ef;
			do
			{
				simplexSet_.pushSimplex (e1, 0);                  // Add vertex
				e1 = Enext_ (e1);
			} while (e1 != ef);
	}

  callTristModCB (Add, & simplexSet_);
}

#endif
/*--------------------------------------------------------------------------*/

void DCompBuilder::
pushIfFlat (OrTriStack& q, OrTri ef, LI_Point liPoints[4])
     /* Refinement. */
{
  VIndex a, b, c, d;
  tr_->triangle (ef, a, b, c);
  d = Dest_ (Enext_ (Fnext_ (ef))); 

  double xa[4], xb[4], xc[4], xd[4];
  rArray_->getPoint(a, xa);
  rArray_->getPoint(b, xb);
  rArray_->getPoint(c, xc);
  rArray_->getPoint(d, xd);

  LI_Point& A = liPoints[0];
  LI_Point& B = liPoints[1];
  LI_Point& C = liPoints[2];
  LI_Point& D = liPoints[3];
  converter_.toLi(xa, 1, A);
  converter_.toLi(xb, 1, B);
  converter_.toLi(xc, 1, C);
  converter_.toLi(xd, 1, D);

  mEval_.minor4 (A, B, C, D, 1, 2, 3, 0, wSpace_);

  if (wSpace_.sign () == 0)
    q.push (ef);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

// Testing Routines

void DCompBuilder::
testDC (void)
{
#if 1
  return;
#else
  COMMENT ("testDC()...\n");
  testComplex ();
  testTriangulation ();
  { /* test (convex) hull */
    FIndex t;
    OrTri e;
    TristIter tIter (*tr_);

    tIter.first ();
    while (! tIter.isDone ())
      {
  t = tIter.curItem ();
  if (tr_->isOnHull (t))
    {
      if (tr_->isOnHull (e = OrTri (t, 0)))
        testHull (e);
      else if (tr_->isOnHull (e = OrTri (t, 1)))
        testHull (e);
      else
        Assert_always (FALSE);  /* should never be reached */
    }
  tIter.next ();
      }
  }
#endif
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
testHull (OrTri h)
{
  BitVector hullbit (tr_->maxTri ());
  Bool is_wrong = FALSE;
  FIndex t = h.index ();
  TristIter tIter (*tr_);
  
  hullbit.set (t);
  Assert_always (tr_->isOnHull (h));
  testHullRecursive (hullbit, Sym_ (Fnext_ (h)));
  testHullRecursive (hullbit, Sym_ (Fnext_ (Enext_ (h))));
  testHullRecursive (hullbit, Sym_ (Fnext_ (Enext2_(h))));

  tIter.first ();
  while (! tIter.isDone ())
    {
      t = tIter.curItem ();
      if (   (tr_->isOnHull (t) and (not hullbit.test (t)))
       or (hullbit.test (t) and (not tr_->isOnHull (t))))
      {
        COMMENT("test_hull: Ochsenscheisse: t" << t);
        is_wrong = TRUE;
      }
      tIter.next ();
    }
       
  Assert_always (not is_wrong);
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
testHullRecursive (BitVector& hb, OrTri h)
{
  FIndex t = h.index ();
  if (not hb.test (t))
    {
      hb.set (t);
      Assert_always (tr_->isOnHull (h));
      testHullRecursive (hb, Sym_ (Fnext_ (h)));
      testHullRecursive (hb, Sym_ (Fnext_ (Enext_ (h))));
      testHullRecursive (hb, Sym_ (Fnext_ (Enext2_(h))));
    }
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
testComplex (void)
{
  FIndex t;
  int v;
  OrTri i;
  OrTri a, b;
  unsigned int length;
  VIndex origin;
  OrTri max = OrTri (tr_->maxTri (), 5);
  TristIter tIter (*tr_);
  
  COMMENT("  [topology]");
  tIter.first ();
  while (! tIter.isDone ())
    {
      t = tIter.curItem ();
      for (v=0; v<=5; v++)
  {
    i = OrTri (t, v);
    origin = Org_ (i);
    Assert_always ((rArray_->size () >= origin) and
       (origin != Dest_ (i)));
    a = Fnext_ (Sym_ (Fnext_ (Sym_ (i))));
    b = Sym_ (Fnext_ (Sym_ (Fnext_ (i))));
    Assert_always ((a == b) and (a == i));
    length = 0;
    a = i;
    b = Sym_ (i);
    while ((length < max.getIntRep () + 1) and (i != (a = Fnext_ (a))))
      {
        length ++;
        b = Fnext_ (b);
        Assert_always ((i != b) and (origin == Org_ (a)));
      }
    Assert_always (length <= max.getIntRep ());
  }
      tIter.next ();
    }
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
testTriangulation (void)
{
  OrTri i, a, s;
  FIndex t;
  VIndex x, y, z, o, x1, y1, p, x2, y2;
  TristIter tIter (*tr_);

  COMMENT("  [geometry]");

  tIter.first ();
  while (! tIter.isDone ())
    {
      t = tIter.curItem ();
      if (tr_->isOnHull (i = OrTri (t, 0)))
        i = Sym_ (i);
      tr_->triangle (i, x, y, z);
      a = Fnext_ (i);
      s = Sym_ (i);
      tr_->triangle (a, x1, y1, o);
      tr_->triangle (Fnext_ (s), y2, x2, p);
      Assert_always ((x1 == x) and (y1 == y) and (x2 == x) and (y2 == y));
      (void) tr_->testOpenTetra (a, x, y, z, o);
      (void) tr_->testTriangle (Turn (s), x, o, z);
      (void) tr_->testTriangle (Turn (a), y, z, o);
      Assert_always (sEval_.positive3 (*rArray_, o, x, y, z));
      if (not (tr_->isOnHull (s)))
        Assert_always (sEval_.positive3 (*rArray_, p, y, x, z));
      tIter.next ();
    }
}

/*--------------------------------------------------------------------------*/

void DCompBuilder::
testTetraGlobal (VIndex a, VIndex b, VIndex c, VIndex d,
     int maxVert, Bool delaunay_flag)
{
  int i;
  VIndex ell;
  Bool pos_a, pos_b, pos_c, pos_d;

  cerr << "T";
  pos_a = sEval_.positive3 (*rArray_, a, b, c, d);
  pos_b = sEval_.positive3 (*rArray_, b, a, c, d);
  pos_c = sEval_.positive3 (*rArray_, c, a, b, d);
  pos_d = sEval_.positive3 (*rArray_, d, a, b, c);
  for (i=1; i<=maxVert; i++)
    {
      ell = i;
      if ((ell != a) and (ell != b) and (ell != c) and (ell != d))
        {
          /* no point inside */
          Assert_always (not (    (pos_a ==
           sEval_.positive3 (*rArray_, ell, b, c, d))
            and (pos_b ==
           sEval_.positive3 (*rArray_, ell, a, c, d))
                              and (pos_c ==
           sEval_.positive3 (*rArray_, ell, a, b, d))
                              and (pos_d ==
           sEval_.positive3 (*rArray_, ell, a, b, c))));
          if (delaunay_flag)
            /* Delaunay hood */
            Assert_always (! sEval_.inSphere (*rArray_, a, b, c, d, ell));
        }
    }
}


/*-----------------------------------------------------------------------------*/



void testHidden (DComplex * dc)
{
  SizeEvaluator& eval = dc->getSizeEvaluator();
  RealPArray * rArray = dc->getPArray();
  LI_Converter& converter = dc->getConverter();
  int n = rArray->size();
  int v1, v2, v3, v4;
  for (int i=0; i<50000; i++)
  {
    v1 = getRandom() % n + 1;
    v2 = getRandom() % n + 1;
    v3 = getRandom() % n + 1;
    v4 = getRandom() % n + 1;
    if ((v1 == v2) || (v2 == v3) || (v3 == v1))
      i--;
    else
    {
      eval.hidden(rArray, converter, v1, v2, v3);
      if ((v4 != v1) && (v4 != v2) && (v4 != v3))
        eval.hidden(rArray, converter, v1, v2, v3, v4);
    }
  }
}

