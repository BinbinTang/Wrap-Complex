/*
 * locate.h -- C++ Header file for Point Location in a Delaunay Complex
 *
 * author -- Mike Facello
 *
 * date -- 10/24/96
 */

#include "locate.h"

/*-----------------------------------------------------------------------*/
/*--------------------------- CLASS PLTechnique -------------------------*/
/*-----------------------------------------------------------------------*/

PLTechnique::
PLTechnique (Trist  * tr, RealPArray * rArray, LI_Converter& converter)
  : TristModifier(tr),
    rArray_ (rArray),
    converter_ (converter)
{
  // Do nothing...
}

/*-----------------------------------------------------------------------*/
/*---------------------------- CLASS JWPLocate --------------------------*/
/*-----------------------------------------------------------------------*/

JWPLocate::
JWPLocate (Trist  * tr, RealPArray * rArray, LI_Converter& converter)
    : PLTechnique (tr, rArray, converter), sEval_(converter)
{
  positive3Tests_ = 0;
  tetsVisited_ = 0;
}

/*--------------------------------------------------------------------------*/

/* Sample size for jwSearch(). */

float search_kf = 7.0;

#define SEARCH_K(N) (int) (search_kf * powf ((float) N, (float) 0.25))

/*-----------------------------------------------------------------------*/

OrTri JWPLocate::
locate (VIndex p)
{
  return jwSearch (p, SEARCH_K (rArray_->size ()));
}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

// Private Methods for JPWLocate

/*--------------------------------------------------------------------------*/


OrTri JWPLocate::
jwSearch (VIndex p, int k)
     /* Point location for query point p in (Delaunay) triangulation of
        current Trist.  Uses jump'n'walk strategy, with a random sample of
        size k.  Use k=O(n^{1/4}), n == num. of points in triangulation; to
        get an expected search time of O(n^{1/4}).
         Assumes: k > 0 and srandom (seed).
         The result is an edgefacet ef with p in ef^+.  If it's a hull facet,
        then p lies outside the convex hull; otherwise, the point lies inside
        the tetrahedron represented by ef. */
{

  VIndex a, b, c;
  OrTri  result, ef, ef_prime;
  FIndex ix;
  int i, last = tr_->maxTri ();
  double dist, dist_prime;
  ;
#define GET_RANDOM_FACET(X)  /* Input/output: int x. */               \
  do                                                                   \
    {                                                                   \
      X = ( getRandom() mod last) + 1; /* Result: random triangle */  \
    } while (tr_->deleted (X));
  ;
  /* Jump: Select "good" candidate using k random samples, taking the closest
   *       one useing quick'n'dirty floating-point distance()
   */
  GET_RANDOM_FACET (ix);
  ef_prime = OrTri (ix, 0);
  dist_prime = distance (ef_prime, p);
  for (i=2; i<=k; i++)
    {
      GET_RANDOM_FACET (ix);
      ef = OrTri (ix, 0);
      dist = distance (ef, p);
      if (dist < dist_prime)
        {
          ef_prime = ef;
          dist_prime = dist;
        }      
    }
  Triangle_ (ef_prime, a, b, c);
  if (not sEval_.positive3 (*rArray_, p, a, b, c))
    {
      ef_prime = Sym_ (ef_prime);
      Triangle_ (ef_prime, a, b, c);
    }
  positive3Tests_ ++;
  ;
  /* Walk: start search at "good" candidate, "walk" towards target,
   *       and return result.
   */
  JWSearchData data(p, ef_prime, a, b, c);
  result = jwSearchWalk (data);
  ;
#if __DEBUG__
  VIndex o;

  Triangle_ (result, a, b, c);
  if (tr_->isOnHull (result))
  {
    /* p is on CH and visible from v, right? */
    Assert (sEval_.positive3 (*rArray_, p, a, b, c));
  }
  else
    { /* p is inside tetrahedron ef^+, right? */
      o = Dest_ (Enext_ (Fnext_ (result)));
      (void) tr_->testOpenTetra (Fnext_ (result), a, b, c, o);
      Assert (    sEval_.positive3 (*rArray_, p, a, b, c)
              and sEval_.positive3 (*rArray_, p, b, a, o)
              and sEval_.positive3 (*rArray_, p, c, b, o)
              and sEval_.positive3 (*rArray_, p, a, c, o));
      }
#endif
  return (result);
}

/*--------------------------------------------------------------------------*/



OrTri JWPLocate::
jwSearchWalk (JWPLocate::JWSearchData& d)
// (VIndex p, OrTri ef, VIndex i, VIndex j, VIndex k)
     /* The "walk" routine. -- Right now implemented with tail recursion! --
        It locates point p within (Delaunay) triangulation of current Trist,
        assuming ef == (i,j,k) and sEval_.positive3 (p, i, j, k) == TRUE. 
        The result... see jwSearch(). */
     /*
      * NOTE: This still needs some modification to make it work for
      *       non-Delaunay triangulations with "high" probability!
      */
{
  Assert (jwStack_.empty());
  jwStack_.push(d);

  while (!jwStack_.empty())
  {
	d = jwStack_.pop();
	VIndex i = d.i, j = d.j, k = d.k, p = d.p;
	OrTri ef = d.ef;
    Assert (tr_->testTriangle (ef, i, j, k)
	  and sEval_.positive3 (*rArray_, p, i, j, k));
    if (tr_->isOnHull (ef))
    {
	  jwStack_.reset();
      return ef;   /* Found!  Outside convex hull. */
    }
    else
    {
      OrTri ef_1 = Fnext_ (ef);
      OrTri ef_2 = Fnext_ (ef = Enext_ (ef));
      OrTri ef_3 = Fnext_ (ef = Enext_ (ef));
      VIndex o = Dest_ (Enext_ (ef_1));
      tetsVisited_ ++;
      if (sEval_.positive3 (*rArray_, p, i, j, o))
        {
          positive3Tests_ += 1;
          // return (jwSearchWalk (p, ef_1, i, j, o));
		  d.set(p, ef_1, i, j, o);
          jwStack_.push(d);
        }
      else if (sEval_.positive3 (*rArray_, p, j, k, o))
        {
          positive3Tests_ += 2;
          // return (jwSearchWalk (p, ef_2, j, k, o));
		  d.set(p, ef_2, j, k, o);
          jwStack_.push(d);
        }
      else if (sEval_.positive3 (*rArray_, p, k, i, o))
        {
          positive3Tests_ += 3;
          // return (jwSearchWalk (p, ef_3, k, i, o));
		  d.set(p, ef_3, k, i, o);
          jwStack_.push(d);
        }
      else
        {
          /* Found! Inside tetrahedron. */
          positive3Tests_ += 3;
		  jwStack_.reset();
          return (ef);    
        }
    }
  }

  return OrTri(0, 0);
}

/*--------------------------------------------------------------------------*/

double JWPLocate::
distance (OrTri ef, VIndex p)
     /* Returns FP "distance" of edfacet ef to p. */
{
	/*
  VIndex a, b, c;
  double d, da, db, dc;
  double p1 = (*pArray_)[p][1].real ();
  double p2 = (*pArray_)[p][2].real ();
  double p3 = (*pArray_)[p][3].real ();
  Triangle_ (ef, a, b, c);
  da = (  square (p1 - (*pArray_)[a][1].real ())
        + square (p2 - (*pArray_)[a][2].real ())
        + square (p3 - (*pArray_)[a][3].real ()));
  db =  (  square (p1 - (*pArray_)[b][1].real ())
         + square (p2 - (*pArray_)[b][2].real ())
         + square (p3 - (*pArray_)[b][3].real ()));
  dc = (  square (p1 - (*pArray_)[c][1].real ())
        + square (p2 - (*pArray_)[c][2].real ())
        + square (p3 - (*pArray_)[c][3].real ()));
  d = Min (da, db);
  return (Min (d, dc));
	*/
  VIndex a, b, c;
	Triangle_ (ef, a, b, c);
	double xp[4], xa[4], xb[4], xc[4];
	rArray_->getPoint(a, xa);
	rArray_->getPoint(b, xb);
	rArray_->getPoint(c, xc);
	rArray_->getPoint(p, xp);
	vsub(xa, xp); vsub(xb, xp); vsub(xc, xp);
	double da = vlength2(xa);
	double db = vlength2(xb);
	double dc = vlength2(xc);
	double d = MIN (da, db);
	return MIN (d, dc);
}

#if 0

/* NOTE: Below "distance" function is slightly cheaper,
         but yields to worse results. */

static double distance (int ef, int p)
     /* Returns FP "distance" of edfacet ef to p. */
{
  double p1 = sos_fp (p, 1);
  double p2 = sos_fp (p, 2);
  double p3 = sos_fp (p, 3);
  double d1, d2, d3;
  int a, b, c;
  tr_.triangle (ef, a, b, c);
  d1 = (sos_fp (a, 1) + sos_fp (b, 1) + sos_fp (c, 1)) / 3.0;
  d2 = (sos_fp (a, 2) + sos_fp (b, 2) + sos_fp (c, 2)) / 3.0;
  d3 = (sos_fp (a, 3) + sos_fp (b, 3) + sos_fp (c, 3)) / 3.0;
  return (square (p1 - d1) + square (p2 - d2) + square (p3 - d3));
}

#endif

/*--------------------------------------------------------------------------*/

int JWPLocate::
jwSearch_GetTests (void)
     /* Returns (accumulated) number of sEval_.positive3() tests. */
{
  return (positive3Tests_);
}

int JWPLocate::
jwSearch_GetTets (void)
     /* Returns (accumulated) number of tetrahedra visited. */
{
  return (tetsVisited_);
}


