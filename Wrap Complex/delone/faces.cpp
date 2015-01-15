
#include "dcomp.h"
#include "dcompiter.h"
#include "basic/rarray.h"
#include "geometry/simplex.h"
#include "boundary.h"

//-------------------------------------------------------------------------

Bool DComplex::
isFace (Ksimplex tau, Ksimplex sigma)
{
  VertArray vTau, vSigma;

  Dim dTau = tau.getDim ();
  Dim dSigma = tau.getDim ();
  if (dTau > dSigma) return FALSE;

  getSortedVertices (tau, vTau);
  getSortedVertices (sigma, vSigma);

  return vTau.isPrefix (vSigma);
}

//-------------------------------------------------------------------------

Bool DComplex::
isProperFace (Ksimplex tau, Ksimplex sigma)
{
  Dim dTau = tau.getDim ();
  Dim dSigma = tau.getDim ();
  if (dTau == dSigma) return FALSE;

  return isFace (tau, sigma);
}

//-------------------------------------------------------------------------

Bool DComplex::
isCoface (Ksimplex tau, Ksimplex sigma)
{
  return isFace (sigma, tau);
}

//-------------------------------------------------------------------------

Bool DComplex::
isPrefix (Ksimplex tau, Ksimplex sigma)
{
  VertArray vTau, vSigma;

  Dim dTau = tau.getDim ();
  Dim dSigma = tau.getDim ();
  if (dTau > dSigma) return FALSE;

  getSortedVertices (tau, vTau);
  getSortedVertices (changeDim (sigma, dTau), vSigma);

  return vTau == vSigma;
}

//-------------------------------------------------------------------------

Bool DComplex::
isFace (Simplex tau, Simplex sigma)
{
  return isFace (createKsimplex (tau), createKsimplex (sigma));
}

//-------------------------------------------------------------------------

Bool DComplex::
isProperFace (Simplex tau, Simplex sigma)
{
  return isProperFace (createKsimplex (tau), createKsimplex (sigma));
}

//-------------------------------------------------------------------------

Bool DComplex::
isCoface (Simplex tau, Simplex sigma)
{
  return isCoface (createKsimplex (tau), createKsimplex (sigma));
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

Bool DComplex::
cofaces (Ksimplex sigma, Dim k, KsimplexCB cbFunc, RgiObject * obj)
{
  DCofaces cf(this);
	return cf.cofaces(sigma, k, cbFunc, obj);
}

Bool DComplex::
cofaces (Simplex sigma, Dim k, SimplexCB  cbFunc, RgiObject * obj)
{
  DCofaces cf(this);
	return cf.cofaces(sigma, k, cbFunc, obj);
}

Bool DComplex::
enumSimplicies (Dim k, SimplexCB  cbFunc, RgiObject * obj)
{
  DCofaces cf(this);
	return cf.enumSimplicies(k, cbFunc, obj);
}

Bool DComplex::
enumKsimplicies (Dim k, KsimplexCB  cbFunc, RgiObject * obj)
{
  DCofaces cf(this);
	return cf.enumKsimplicies(k, cbFunc, obj);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

Bool DComplex::
facesEnum (Simplex sigma, SimplexCB cbf, RgiObject * obj)
{
  SimplexArray f;
  faces (sigma, &f);
  while (!f.empty())
  {
    sigma = f.pop();
    if (!(obj->*cbf)(sigma))
      return FALSE;
  }
  return TRUE;
}

/*--------------------------------------------------------------------------*/

Bool DComplex::
properFacesEnum (Simplex sigma, SimplexCB cbf, RgiObject * obj) 
{
  SimplexArray f;

  properFaces (sigma, &f);
  while (!f.empty())
  {
    sigma = f.pop();
    if (!(obj->*cbf)(sigma))
      return FALSE;
  }
  return TRUE;

}

//-------------------------------------------------------------------------

Bool DComplex::
kFacesEnum (Simplex sigma, Dim k, SimplexCB cbf, RgiObject * obj) 
{
  SimplexArray f;
  kFaces (sigma, k, &f);
  while (!f.empty())
  {
    sigma = f.pop();
    if (!(obj->*cbf)(sigma))
      return FALSE;
  }
  return TRUE;
}

//-------------------------------------------------------------------------

int DComplex::
bdEnum (SimplexCB cbf, RgiObject * obj)
{
  DBoundary bd(this);
  return bd.bdEnum (cbf, obj);
}

//-------------------------------------------------------------------------

int DComplex::
kBdEnum (Dim k, SimplexCB cbf, RgiObject * obj)
{
  DBoundary bd(this);
  return bd.kBdEnum (k, cbf, obj);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

void DComplex::
properCofaceFaces (Simplex sigma, Simplex tau, 
		   SimplexArray * fStar, SimplexArray * fLink)
{
  fStar->reset ();
  fLink->reset ();
  properCofaceFaces_ (createKsimplex (sigma), createKsimplex (tau),
		     &DComplex::fStar_, &DComplex::fLink_, 
		     (void *) fStar, (void *) fLink);
}

//-------------------------------------------------------------------------

void DComplex::
properCofaceFaces (Ksimplex sigma, Ksimplex tau, 
		   KsimplexArray * fStar, KsimplexArray * fLink)
{
  fStar->reset ();
  fLink->reset ();
  properCofaceFaces_ (sigma, tau, 
		     &DComplex::fStarK_, &DComplex::fLinkK_, 
		     (void *) fStar, (void *) fLink);
}

//-------------------------------------------------------------------------

void DComplex::
cofaceFaces (Simplex sigma, Simplex tau,
	     SimplexArray * fStar, SimplexArray * fLink)
{
  fStar->reset ();
  fLink->reset ();
  
  ((SimplexArray *) fStar)->push (sigma);
  cofaceFaces_ (createKsimplex (sigma), createKsimplex (tau),
	       &DComplex::fStar_, &DComplex::fLink_, 
	       (void *) fStar, (void *) fLink);
}

//-------------------------------------------------------------------------

void DComplex::
cofaceFaces (Ksimplex sigma, Ksimplex tau,
	     KsimplexArray * fStar, KsimplexArray * fLink)
{
  fStar->reset ();
  fLink->reset ();

  ((KsimplexArray *) fStar)->push (sigma);
  cofaceFaces_ (sigma, tau, &DComplex::fStarK_, &DComplex::fLinkK_, 
	       (void *) fStar, (void *) fLink);
}

//-------------------------------------------------------------------------

void DComplex::
properFaces (Simplex sigma, SimplexArray * f)
{
  f->reset ();
  properFaces_ (createKsimplex (sigma), &DComplex::f_, f);
}

//-------------------------------------------------------------------------

void DComplex::
properFaces (Ksimplex sigma, KsimplexArray * f)
{
  f->reset ();
  properFaces_ (sigma, &DComplex::fK_, f);
}

//-------------------------------------------------------------------------

void DComplex::
faces (Simplex sigma, SimplexArray * f)
{
  f->reset ();

  ((SimplexArray *) f)->push (sigma);
  faces_ (createKsimplex (sigma), &DComplex::f_, f);
}

//-------------------------------------------------------------------------

void DComplex::
faces (Ksimplex sigma, KsimplexArray * f)
{
  f->reset ();
  
  ((KsimplexArray *) f)->push (sigma);
  faces_ (sigma, &DComplex::fK_, f);
}

//-------------------------------------------------------------------------

// gives all faces of sigma with dimension d - 1
void DComplex::
kFaces (Simplex sigma, Dim k, SimplexArray * sf)
{
  sf->reset();
  KsimplexArray f;
  kFaces (createKsimplex(sigma), k, &f);
  while (!f.empty())
    sf->push(sigma = createSimplex(f.pop()));
}

void DComplex::
kFaces (Ksimplex sigma, Dim k, KsimplexArray * f)
{
  f->reset();
  if (k < 0)
    return;
  Ksimplex tau;
  OrTri ef = sigma.getFace();
  switch (sigma.getDim())
  {
    case 0:
      break;
    case 1:
      Assert (k == 0);
      f->push(tau = Ksimplex(ef, k));
      f->push(tau = Ksimplex(Enext_(ef), k));
      break;
    case 2:
      Assert (k == 0 || k == 1);
      f->push(tau = Ksimplex(ef, k));
      f->push(tau = Ksimplex(Enext_(ef), k));
      f->push(tau = Ksimplex(Enext2_(ef), k));
      break;
    case 3:
      Assert (k < 3); 
      if (k == 1)
      {
        f->push(tau = Ksimplex(ef, k));  // abc
        f->push(tau = Ksimplex(Enext_ (ef), k)); // bca
        f->push(tau = Ksimplex(Enext2_ (ef), k)); // cab
        f->push(tau = Ksimplex(Enext_ (Sym_ (Fnext_ (ef))), k)); // adb
        f->push(tau = Ksimplex(Enext_ (Sym_ (Fnext_ (Enext_(ef)))), k)); // bdc
        f->push(tau = Ksimplex(Enext_ (Sym_ (Fnext_ (Enext2_(ef)))), k)); // cda
      }
      else
      {
        f->push(tau = Ksimplex(ef, k));  // abc
        f->push(tau = Ksimplex(Sym_ (Fnext_ (ef)), k)); // bad
        f->push(tau = Ksimplex(Sym_ (Fnext_ (Enext_(ef))), k)); // cbd
        f->push(tau = Ksimplex(Enext2_ (Sym_ (Fnext_ (Enext2_(ef)))), k)); // dac
      }
      break;
    default:
      Assert_always(0);
  }
}

//-------------------------------------------------------------------------

void DComplex::
fStar_ (int index, OrTri facet, Dim k, void * fStar)
{
  Simplex sigma = createSimplex (facet, k);
  ((SimplexArray *) fStar)->push (sigma);
}

//-------------------------------------------------------------------------

void DComplex::
fLink_ (int index, OrTri facet, Dim k, void * fLink)
{
  Simplex sigma = createSimplex (facet, k);
  ((SimplexArray *) fLink)->push (sigma);
}

//-------------------------------------------------------------------------

void DComplex::
fStarK_ (int index, OrTri facet, Dim k, void * fStar)
{
  Ksimplex sigma (facet, k);
  ((KsimplexArray *) fStar)->push (sigma);
}

//-------------------------------------------------------------------------

void DComplex::
fLinkK_ (int index, OrTri facet, Dim k, void * fLink)
{
  Ksimplex sigma (facet, k);
  ((KsimplexArray *) fLink)->push (sigma);
}

//-------------------------------------------------------------------------

void DComplex::
f_ (int index, OrTri facet, Dim k, void * f)
{
  Simplex sigma = createSimplex (facet, k);
  ((SimplexArray *) f)->push (sigma);
}

//-------------------------------------------------------------------------

void DComplex::
fK_ (int index, OrTri facet, Dim k, void * f)
{
  Ksimplex sigma (facet, k);
  ((KsimplexArray *) f)->push (sigma);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

#define FSTARFUN (this->*fStarFun)
#define FLINKFUN (this->*fLinkFun)

//-------------------------------------------------------------------------

void DComplex::
properCofaceFaces_ (Ksimplex sigma, Ksimplex tau, 
		   void (DComplex::*fStarFun) (int, OrTri, Dim, void *),
		   void (DComplex::*fLinkFun) (int, OrTri, Dim, void *),
		   void * fStar, void * fLink)
{
#define ISPREFIX_TAU_SIGMA \
  (getSortedVertices (fSigma, dTau, vSigma), vTau == vSigma)

  OrTri fSigma;
  VertArray vSigma, vTau;

  Dim dSigma = sigma.getDim ();
  Dim dTau = tau.getDim ();

  getSortedVertices (tau, vTau);

  Assert (dTau < dSigma);
  Assert (isFace (tau, sigma));

  if (dSigma == 0) return;

  fSigma = sigma.getFace ();

  if (dSigma == 3) {
    if (dTau == 0) {
      if (! ISPREFIX_TAU_SIGMA) {
	fSigma = Enext_ (fSigma);
	if (! ISPREFIX_TAU_SIGMA) {
	  fSigma = Enext_ (fSigma);
	  if (! ISPREFIX_TAU_SIGMA) 
	    fSigma = Enext2_ (Sym_ (Fnext_ (fSigma)));
	}
      }
	
      /*printSimplex (tau); 
      printSimplex (sigma); printlnSimplex (makeSimplex (fSigma, dSigma));
      */
      Assert (ISPREFIX_TAU_SIGMA);

      FSTARFUN (0, fSigma, 2, fStar);
      FSTARFUN (1, Sym_ (Fnext_ (fSigma)), 2, fStar);
      FLINKFUN (0, Sym_ (Fnext_ (fSigma = Enext_ (fSigma))), 2, fLink);
      FSTARFUN (2, Sym_ (Fnext_ (Enext_ (fSigma))), 2, fStar);
      FLINKFUN (1, fSigma, 1, fLink);
      FSTARFUN (3, fSigma = Enext_ (fSigma), 1, fStar);
      FSTARFUN (4, Enext_ (fSigma), 1, fStar);
      FSTARFUN (5, Enext_ (Sym_ (Fnext_ (fSigma))), 1, fStar);
      FLINKFUN (2, Enext_ (Sym_ (Fnext_ (fSigma = Enext_ (fSigma)))),
	      1, fLink);
      FLINKFUN (3, Enext_ (Sym_ (Fnext_ (Enext_ (fSigma)))), 1, fLink);
      FSTARFUN (6, fSigma, 0, fStar);
      FLINKFUN (4, fSigma = Enext_ (fSigma), 0, fLink);
      FLINKFUN (5, Enext_ (fSigma), 0, fLink);
      FLINKFUN (6, Sym_ (Enext_ (Fnext_ (fSigma))), 0, fLink);

      return;
    }

    if (dTau == 1) {
      if (! ISPREFIX_TAU_SIGMA) {
	fSigma = Enext_ (fSigma);
	if (! ISPREFIX_TAU_SIGMA) {
	  fSigma = Enext_ (fSigma);
	  if (! ISPREFIX_TAU_SIGMA) {
	    fSigma = Enext_ (Sym_ (Fnext_ (fSigma)));
	    if (! ISPREFIX_TAU_SIGMA) {
	      fSigma = Enext_ (fSigma);
	      if (! ISPREFIX_TAU_SIGMA) fSigma = Enext_ (Sym_ (Fnext_ (fSigma)));
	    }
	  }
	}
      }

      /*printSimplex (tau); 
      printSimplex (sigma); printlnSimplex (makeSimplex (fSigma, dSigma));
	*/
      Assert (ISPREFIX_TAU_SIGMA);

      FSTARFUN (0, fSigma, 2, fStar);
      FSTARFUN (1, Sym_ (Fnext_ (fSigma)), 2, fStar);
      FLINKFUN (0, Sym_ (Fnext_ (fSigma = Enext_ (fSigma))), 2, fLink);
      FLINKFUN (1, Sym_ (Fnext_ (Enext_ (fSigma))), 2, fLink);
      FLINKFUN (2, fSigma, 1, fLink);
      FLINKFUN (3, fSigma = Enext_ (fSigma), 1, fLink);
      FSTARFUN (2, Enext_ (fSigma), 1, fStar);
      FLINKFUN (4, Enext_ (Sym_ (Fnext_ (fSigma))), 1, fLink);
      FLINKFUN (5, Enext_ (Sym_ (Fnext_ (fSigma = Enext_ (fSigma)))),
		1, fLink);
      FLINKFUN (6, Enext_ (Sym_ (Fnext_ (Enext_ (fSigma)))), 1, fLink);
      FLINKFUN (7, fSigma, 0, fLink);
      FLINKFUN (8, fSigma = Enext_ (fSigma), 0, fLink);
      FLINKFUN (9, Enext_ (fSigma), 0, fLink);
      FLINKFUN (10, Sym_ (Enext_ (Fnext_ (fSigma))), 0, fLink);

      return;
    }

    if (! ISPREFIX_TAU_SIGMA) {
      fSigma = Sym_ (Fnext_ (fSigma));
      if (! ISPREFIX_TAU_SIGMA) {
	fSigma = Sym_ (Fnext_ (Enext_ (fSigma)));
	if (! ISPREFIX_TAU_SIGMA) {
	  fSigma = Sym_ (Fnext_ (Enext2_ (fSigma)));
	  if (! ISPREFIX_TAU_SIGMA) 
	    fSigma = Sym_ (Fnext_ (Enext2_ (fSigma)));
	}
      }
    }

    /*printSimplex (tau); 
    printSimplex (sigma); printlnSimplex (makeSimplex (fSigma, dSigma));
    */
    Assert (ISPREFIX_TAU_SIGMA);

    FSTARFUN (0, fSigma, 2, fStar);
    FLINKFUN (0, Sym_ (Fnext_ (fSigma)), 2, fLink);
    FLINKFUN (1, Sym_ (Fnext_ (fSigma = Enext_ (fSigma))), 2, fLink);
    FLINKFUN (2, Sym_ (Fnext_ (Enext_ (fSigma))), 2, fLink);
    FLINKFUN (3, fSigma, 1, fLink);
    FLINKFUN (4, fSigma = Enext_ (fSigma), 1, fLink);
    FLINKFUN (5, Enext_ (fSigma), 1, fLink);
    FLINKFUN (6, Enext_ (Sym_ (Fnext_ (fSigma))), 1, fLink);
    FLINKFUN (7, Enext_ (Sym_ (Fnext_ (fSigma = Enext_ (fSigma)))), 
	    1, fLink);
    FLINKFUN (8, Enext_ (Sym_ (Fnext_ (Enext_ (fSigma)))), 1, fLink);
    FLINKFUN (9, fSigma, 0, fLink);
    FLINKFUN (10, fSigma = Enext_ (fSigma), 0, fLink);
    FLINKFUN (11, Enext_ (fSigma), 0, fLink);
    FLINKFUN (12, Sym_ (Enext_ (Fnext_ (fSigma))), 0, fLink);

    return;
  }	  
  
  if (dSigma == 2) {
    if (! ISPREFIX_TAU_SIGMA) {
      fSigma = Enext_ (fSigma);
      if (! ISPREFIX_TAU_SIGMA) fSigma = Enext_ (fSigma);
    }
    Assert (ISPREFIX_TAU_SIGMA);

    if (dTau == 0) {
      FSTARFUN (0, fSigma, 1, fStar);
      FLINKFUN (0, fSigma = Enext_ (fSigma), 1, fLink);
      FSTARFUN (1, Enext_ (fSigma), 1, fStar);
      FLINKFUN (1, fSigma, 0, fLink);
      FLINKFUN (2, fSigma = Enext_ (fSigma), 0, fLink);
      FSTARFUN (2, Enext_ (fSigma), 0, fStar);

      return;
    }

    FSTARFUN (0, fSigma, 1, fStar);
    FLINKFUN (0, fSigma = Enext_ (fSigma), 1, fLink);
    FLINKFUN (1, Enext_ (fSigma), 1, fLink);
    FLINKFUN (2, fSigma, 0, fLink);
    FLINKFUN (3, fSigma = Enext_ (fSigma), 0, fLink);
    FLINKFUN (4, Enext_ (fSigma), 0, fLink);

    return;
  }

  if (dSigma == 1) {
    if (! ISPREFIX_TAU_SIGMA) {
      FLINKFUN (0, fSigma, 0, fLink);
      FSTARFUN (0, Enext_ (fSigma), 0, fStar);
    } else {
      FSTARFUN (0, fSigma, 0, fStar);
      FLINKFUN (0, Enext_ (fSigma), 0, fLink);
    }

    return;
  }

  Assert_always (0);

#undef ISPREFIX_TAU_SIGMA
}

//-------------------------------------------------------------------------

void DComplex::
cofaceFaces_ (Ksimplex sigma, Ksimplex tau, 
	     void (DComplex::*fStarFun) (int, OrTri, Dim, void *),
	     void (DComplex::*fLinkFun) (int, OrTri, Dim, void *),
	     void * fStar, void * fLink)
{
  properCofaceFaces_ (sigma, tau, fStarFun, fLinkFun, fStar, fLink);
}

//-------------------------------------------------------------------------

#define FFUN (this->*fFun)

//-------------------------------------------------------------------------
// returns in f all proper faces (encoded in Ksimplex format) of sigma;

void DComplex::
properFaces_ (Ksimplex sigma, void (DComplex::*fFun) (int, OrTri, Dim, void *),
	     void * f)
{
  OrTri fSigma;

  Dim dSigma = sigma.getDim ();
  if (dSigma == 0) return;

  fSigma = sigma.getFace ();

  if (dSigma == 3) {
    FFUN (0, fSigma, 2, f);
    FFUN (1, Sym_ (Fnext_ (fSigma)), 2, f);
    FFUN (2, Sym_ (Fnext_ (fSigma = Enext_ (fSigma))), 2, f);
    FFUN (3, Sym_ (Fnext_ (Enext_ (fSigma))), 2, f);
    FFUN (4, fSigma, 1, f);
    FFUN (5, fSigma = Enext_ (fSigma), 1, f);
    FFUN (6, Enext_ (fSigma), 1, f);
    FFUN (7, Enext_ (Sym_ (Fnext_ (fSigma))), 1, f);
    FFUN (8, Enext_ (Sym_ (Fnext_ (fSigma = Enext_ (fSigma)))), 1, f);
    FFUN (9, Enext_ (Sym_ (Fnext_ (Enext_ (fSigma)))), 1, f);
    FFUN (10, fSigma, 0, f);
    FFUN (11, fSigma = Enext_ (fSigma), 0, f);
    FFUN (12, Enext_ (fSigma), 0, f);
    FFUN (13, Sym_ (Enext_ (Fnext_ (fSigma))), 0, f);

    return;
  }
  
  if (dSigma == 2) {
    FFUN (0, fSigma, 1, f);
    FFUN (1, fSigma = Enext_ (fSigma), 1, f);
    FFUN (2, Enext_ (fSigma), 1, f);
    FFUN (3, fSigma, 0, f);
    FFUN (4, fSigma = Enext_ (fSigma), 0, f);
    FFUN (5, Enext_ (fSigma), 0, f);

    return;
  }

  if (dSigma == 1) {
    FFUN (0, fSigma, 0, f);
    FFUN (1, Enext_ (fSigma), 0, f);

    return;
  }

  Assert_always (0);
}

//-------------------------------------------------------------------------
// returns in f all faces (encoded in Ksimplex format) of sigma;

void DComplex::
faces_ (Ksimplex sigma, void (DComplex::*fFun) (int, OrTri, Dim, void *), 
       void * f)
{
  properFaces_ (sigma, fFun, f);
}

/*--------------------------------------------------------------------------*/

/* returns in f the k-faces (encoded in Simplex format) of sigma; returns
   the number of such faces. 
*/

void DComplex::
kFaces_ (Ksimplex sigma, Dim k, 
	void (DComplex::*fFun) (int, OrTri, Dim, void *), void * f)
{
  OrTri fSigma;

  fSigma = sigma.getFace ();
  Dim dSigma = sigma.getDim ();

  FFUN (0, fSigma, k, f);
  if (k == dSigma) return;

  if (dSigma == 3) {
    if (k == 2) {
      FFUN (1, Sym_ (Fnext_ (fSigma)), k, f);
      FFUN (2, Sym_ (Fnext_ (fSigma = Enext_ (fSigma))), k, f);
      FFUN (3, Sym_ (Fnext_ (Enext_ (fSigma))), k, f);
      return;
    }

    FFUN (1, fSigma = Enext_ (fSigma), k, f);
    FFUN (2, Enext_ (fSigma), k, f);

    if (k == 1) {
      FFUN (3, Enext_ (Sym_ (Fnext_ (fSigma))), k, f);
      FFUN (4, Enext_ (Sym_ (Fnext_ (fSigma = Enext_ (fSigma)))), k, f);
      FFUN (5, Enext_ (Sym_ (Fnext_ (Enext_ (fSigma)))), k, f);
      return;
    }

    FFUN (3, Sym_ (Enext_ (Fnext_ (fSigma))), k, f);
    return;
  }
  
  if (dSigma == 2) {
    FFUN (1, fSigma = Enext_ (fSigma), k, f);
    FFUN (2, Enext_ (fSigma), k, f);
    return;
  }

  if (dSigma == 1) {
    FFUN (1, Enext_ (fSigma), k, f);
    return;
  }

  Assert_always (0);
}

