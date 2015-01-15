//
// Copyright 1996-97. Raindrop Geomagic, Inc.
// All Rights Reserved.
//
// This is unpublished proprietary source code of Raindrop Geomagic, Inc.;
// the contents of this file may not be disclosed to third parties, copied
// or duplicated in any form, in whole or in part, without the prior written
// permission of Raindrop Geomagic, Inc.
//
// This copyright notices may not be removed from this file at any time.
//
// RESTRICTED RIGHTS LEGEND:
// Use, duplication or disclosure by the Government is subject to restrictions
// as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
// and Computer Software clause at DFARS 252.227-7013, and/or in similar or
// successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished-
// rights reserved under the Copyright Laws of the United States.
//
//                 Raindrop Geomagic, Inc
//                 206 N. Randolph Street, Suite 520
//                 Champaign IL 61820-3898
//                 Phone: 217-239-2551
//                 Fax:   217-239-2556
//                 Email: inquiry@geomagic.com
//
//


/*
 * dcomp.h -- C++ Header file for the Delaunay Complex class
 *
 * author -- Mike Facello
 *
 * date -- 10/24/96
 */

#ifndef __RGI_DCOMP_H__
#define __RGI_DCOMP_H__

#include <iostream>

#include <basic/basic.h>
#include <basic/bitvector.h>

#include <sos/size.h>
#include <sos/sosfloat.h>

#include <geometry/comp.h>
#include <geometry/trist.h>
#include <geometry/bvtag.h>
#include <geometry/ihandler.h>
#include <geometry/ksimplex.h>

class DComplex;

#include <delone/dcofaces.h>
/* Alan Comment
#include <delone/ceflow.h>
#include <delone/confident.h>
#include <delone/centered.h>
#include <delone/equivocal.h>
#include <delone/classarray.h>
*/



/************************************************************************/

/************************************************************************/

/*
 * Class DComplex
 *
 * 
 *
 */

class DComplex : public Complex
{
friend class IndexHandler;
friend class DCompBuilder;
friend class SubComplexW;
friend class WCompBuilder;
friend class DCSub;
friend class WrapNormalItrI;
friend class Centered;
friend class CEFlow;
friend class ClassArray;
public:

  /////////////////////
  // Defined in dcomp.cpp
  /////////////////////

  // Constructors
  DComplex (void);
  DComplex (Trist  * tr, RealPArray * rArray, LI_Converter& converter);

  ~DComplex (void);

  // Input/Output
  Bool binInit (BinInput& bi);        // Binary Input
  void binDump (BinOutput& bo);       // Binary Output
  static Bool isValid (int magic);    // verification
  friend ostream& operator<< (ostream& s, DComplex& comp);  

  // ASCII Output
  void printSimplex (ostream& s, Ksimplex sigma);
  void printSimplex (ostream& s, OrTri ef, Dim d);
  void printlnSimplex (ostream& s, Ksimplex sigma);
  void printlnSimplex (ostream& s, OrTri ef, Dim d);

  void printSimplex (Ksimplex sigma) { printSimplex (cout, sigma); };
  void printSimplex (OrTri ef, Dim d) { printSimplex (cout, ef, d); };
  void printlnSimplex (Ksimplex sigma) { printlnSimplex (cout, sigma); };
  void printlnSimplex (OrTri ef, Dim d) { printlnSimplex (cout, ef, d); };

  void printSimplex (ostream& s, Simplex sigma) 
                             { printSimplex (cout, createKsimplex (sigma)); };
  void printlnSimplex (ostream& s, Simplex sigma) 
                           { printlnSimplex (cout, createKsimplex (sigma)); };
  void printSimplex (Simplex sigma) { printSimplex (cout, sigma); };
  void printlnSimplex (Simplex sigma) { printlnSimplex (cout, sigma); };

  // Point accessors
  RealPArray *getNewPArray (void);   // User always gets real coords
                                     // User is responsible for deallocation

  void getCoords (VIndex ix, double *coords);
       // coords should be an array of at least length 3.  The coords
       //      will be loaded into this array.

  // Statistics
  int numTriangles (void);
  int numClosedTri (void);
  int numCompleteTri (void);
  int numEdges (void);
  int numVertices (void);

  // Simplex testing functions
  Bool inComplex (Simplex sigma);
  Bool inComplex (FIndex f) { return Fvalid_(f); }

  Bool isBoundarySimplex (Simplex sigma);
  OrTri boundaryFacet (void);

  // Member Accessors
  Trist *cloneTrist (void) { return tr_; }
      //- Returns a pointer to the Trist, NOT A COPY!!!
  RealPArray * getPArray (void) { return rArray_; }
      //- Access PArray directly (note that it may change over time.
  IndexHandler * getIndexHandler (void) {return ix_; }
      //- Converts Ksimplices (oriented simplices) to Simplices
  SizeEvaluator& getSizeEvaluator (void) { return *wse_; }
      //- Computes sizes of simplices
  //ClassArray * getClassArray (void) { return ca_; }

    // External Iterators

  // Iterate over all faces of sigma
  CompIter *newIter (Simplex sigma);

  // Iterate over all d-dimensional faces (or cofaces) of sigma
  CompIter *newIter (Simplex sigma, Dim d);

  // Iterate over all simplices of the convex as indicated by the
  //     initialization variables
  CompIter *newIter (Bool v, Bool e, Bool f, Bool t,
		     Bool bd, Bool interior);

  enum {DT, DT_WEIGHTED};
  void useWeightedGeometry (int type);

  /////////////////////
  // Defined in faces.cpp
  /////////////////////

  Bool isFace (Ksimplex tau, Ksimplex sigma);
      //- dim(tau) <= dim(sigma), and tau is a face of sigma
  Bool isProperFace (Ksimplex tau, Ksimplex sigma);
      //- dim(tau) < dim(sigma), and tau is a face of sigma
  Bool isCoface (Ksimplex tau, Ksimplex sigma);
      //- dim(tau) > dim(sigma), and sigma is a face of tau
  Bool isPrefix (Ksimplex tau, Ksimplex sigma);
      //- dim(tau) <= dim(sigma), and tau is a prefix of sigma
 
  Bool isFace (Simplex tau, Simplex sigma);
      //- dim(tau) <= dim(sigma), and tau is a face of sigma
  Bool isProperFace (Simplex tau, Simplex sigma);
      //- dim(tau) < dim(sigma), and tau is a face of sigma
  Bool isCoface (Simplex tau, Simplex sigma);
      //- dim(tau) > dim(sigma), and sigma is a face of tau
  
	Bool cofaces (Ksimplex sigma, Dim k, 
               KsimplexCB, RgiObject * obj);
  Bool cofaces (Simplex sigma, Dim k, 
               SimplexCB, RgiObject * obj);
  Bool enumSimplicies (Dim k, 
               SimplexCB, RgiObject * obj);
  Bool enumKsimplicies (Dim k, 
               KsimplexCB, RgiObject * obj);

  Bool facesEnum (Simplex sigma, SimplexCB cbf, RgiObject * obj);
  Bool properFacesEnum (Simplex sigma, SimplexCB cbf, RgiObject * obj);
  Bool kFacesEnum (Simplex sigma, Dim k, SimplexCB cbf, RgiObject *obj);
  int bdEnum (SimplexCB cbf, RgiObject * obj);
  int kBdEnum (Dim k, SimplexCB cbf, RgiObject * obj);

  void properCofaceFaces (Simplex sigma, Simplex tau, 
			  SimplexArray * fStar, SimplexArray * fLink);
  void properCofaceFaces (Ksimplex sigma, Ksimplex tau, 
			  KsimplexArray * fStar, KsimplexArray * fLink);

  void cofaceFaces (Simplex sigma, Simplex tau,
		    SimplexArray * fStar, SimplexArray * fLink);
  void cofaceFaces (Ksimplex sigma, Ksimplex tau,
		    KsimplexArray * fStar, KsimplexArray * fLink);

  void properFaces (Simplex sigma, SimplexArray * f);
  void properFaces (Ksimplex sigma, KsimplexArray * f);

  void faces (Simplex sigma, SimplexArray * f);
  void faces (Ksimplex sigma, KsimplexArray * f);

  void kFaces (Simplex sigma, Dim k, SimplexArray * f);
  void kFaces (Ksimplex sigma, Dim k, KsimplexArray * f);

  /////////////////////
  // Defined in classify.cpp
  /////////////////////

  enum ClassificationMethod 
      {CLASSIFY_ENUM = 0,
        CLASSIFY_ARRAY = 1,
        CLASSIFY_DYNA = 2,
        CLASSIFY_VERIFY = 3,
        CLASSIFY_ARRAY_DYNA = 4};

  void setClassificationMethod (DComplex::ClassificationMethod method);
  DComplex::ClassificationMethod  getClassificationMethod (void);
  int  classifyInit (void);
  Bool classifyBinInit  (BinInput& bi);
  void classifyBinDump (BinOutput& bo);

  int verifyEquivocal (Simplex sigma);
  int verifyCentered (Simplex sigma);
  int verifyConfident (Simplex sigma);
  Simplex verifyUpred (Simplex sigma);
  int verifyCeFlow (Simplex sigma, Simplex tau);
  int verifyEcFlow (Simplex sigma, Simplex tau);
  int verifyLdSucc (Simplex sigma, Simplex tau);
  double verifySizeOfCenteredSimplex (Simplex sigma);

  int arrdynaEquivocal (Simplex sigma);
  int arrdynaCentered (Simplex sigma);
  int arrdynaConfident (Simplex sigma);
  Simplex arrdynaUpred (Simplex sigma);
  int arrdynaCeFlow (Simplex sigma, Simplex tau);
  int arrdynaEcFlow (Simplex sigma, Simplex tau);
  int arrdynaLdSucc (Simplex sigma, Simplex tau);
  double arrdynaSizeOfCenteredSimplex (Simplex sigma);

  /////////////////////
  // Defined in dutil.cpp
  /////////////////////

  int enumEquivocal (Ksimplex sigma);
  int enumConfident (Ksimplex sigma);
  int enumCentered (Ksimplex sigma);
  int equivocalToo (Ksimplex tau);
  Ksimplex enumGetUpred (Ksimplex sigma);
  int enumCeFlow (Ksimplex sigma, Dim d);
  int enumCeFlow (Simplex sigma, Simplex tau);
  int enumLdSucc (Simplex sigma, Simplex tau);
  Simplex enumGetLdSucc (Simplex sigma);
  int enumEcFlow (Dim dSigma, Ksimplex tau);
  int enumEcFlow (Simplex sigma, Simplex tau);
  double enumSizeOfCenteredSimplex (Simplex sigma);
/*
  int arrEquivocal (Simplex sigma);
  int arrConfident (Simplex sigma);
  int arrCentered (Simplex sigma);
*/
  Simplex arrGetUpred (Simplex sigma);
  int arrUpredEqualLdSucc (Simplex upsilon, Dim minDimUPred);
/*
  int arrCeFlow (Simplex sigma, Simplex tau);
  int arrLdSucc (Simplex sigma, Simplex tau);
  int arrEcFlow (Simplex sigma, Simplex tau);
  double arrSizeOfCenteredSimplex (Simplex sigma);
*/
  void arrInitClassify (void);

  int dynaEquivocal (Simplex sigma);
  int dynaConfident (Simplex sigma);
  int dynaCentered (Simplex sigma);
  Simplex dynaGetUpred (Simplex sigma);
  int dynaUpredEqualLdSucc (Simplex upsilon, Dim minDimUPred);
  int dynaCeFlow (Simplex sigma, Simplex tau);
  int dynaLdSucc (Simplex sigma, Simplex tau);
  int dynaEcFlow (Simplex sigma, Simplex tau);
  double dynaSizeOfCenteredSimplex (Simplex sigma);
  void dynaInitClassify (void);


  // Flow relations

  int equivocal (Simplex sigma);
  int confident (Simplex sigma);
  int centered (Simplex sigma);
  Simplex uPred (Simplex sigma);
  int ceFlow (Simplex sigma, Simplex tau);
  int ldSucc (Simplex sigma, Simplex tau);
  int ecFlow (Simplex sigma, Simplex tau);
  double sizeOfCenteredSimplex (Simplex sigma);

  int flowAcrossBoundary (Ksimplex sigma);


  /////////////////////
  // Inline Simplex Operations defined below
  /////////////////////

  void getVertices (Simplex sigma, VertArray & v);
  void getVertices (Ksimplex sigma, VertArray & v);
  void getVertices (OrTri ef, Dim d, VertArray & v); 
  void getSortedVertices (Simplex sigma, VertArray & v); 
  void getSortedVertices (Ksimplex sigma, VertArray & v);
  void getSortedVertices (OrTri ef, Dim d, VertArray & v);

  Index getIndex (OrTri ef, Dim d);

  Index getIndexV (VIndex v);
  Index getIndexV (VIndex v0, VIndex v1);
  Index getIndexV (VIndex v0, VIndex v1, VIndex v2);
  Index getIndexV (VIndex v0, VIndex v1, VIndex v2, VIndex v3);

  Simplex createSimplex (Ksimplex sigma);
  Simplex createSimplex (OrTri ef, Dim d);
  Ksimplex createKsimplex (Simplex sigma);

  Simplex createSimplexV (VIndex v);
  Simplex createSimplexV (VIndex v0, VIndex v1);
  Simplex createSimplexV (VIndex v0, VIndex v1, VIndex v2);
  Simplex createSimplexV (VIndex v0, VIndex v1, VIndex v2, VIndex v3);

  Ksimplex changeDim (Ksimplex sigma, Dim k);
  Ksimplex increaseDim (Ksimplex sigma);
  Ksimplex decreaseDim (Ksimplex sigma);

  OrTri getFace (Simplex sigma);

private:

  Magic magic_; 
      //- for binary IO check
	RealPArray *rArray_;
      //- Point in the complex
  IndexHandler * ix_;
      //- Converts an Index into an OrTri and
      //-   allows iteration over the simplices 
      //-   of this complex
  WSizeEvaluator * wse_;
      //- Size evaluators for computing simplex sizes

  ClassificationMethod classMethod_;
  //ClassArray * ca_;

// Alan Comment
//  Centered      centered_;
//  CEFlow        ceflow_;
//  Confident     confident_;
//  Equivocal     equivocal_;
      //- Simplex classification structures


  /////////////////////
  // Defined in faces.cpp
  /////////////////////
  void fStar_ (int index, OrTri facet, Dim k, void * fStar);
  void fLink_ (int index, OrTri facet, Dim k, void * fLink);
  void fStarK_ (int index, OrTri facet, Dim k, void * fStar);
  void fLinkK_ (int index, OrTri facet, Dim k, void * fLink);
  void f_ (int index, OrTri facet, Dim k, void * f);
  void fK_ (int index, OrTri facet, Dim k, void * f);

  void properCofaceFaces_ (Ksimplex sigma, Ksimplex tau, 
		          void (DComplex::*fLinkFun) (int, OrTri, Dim, void *),
		          void (DComplex::*fStarFun) (int, OrTri, Dim, void *),
		          void * fStar, void * fLink);
  void cofaceFaces_ (Ksimplex sigma, Ksimplex tau, 
              void (DComplex::*fLinkFun) (int, OrTri, Dim, void *),
	            void (DComplex::*fStarFun) (int, OrTri, Dim, void *),
	            void * fStar, void * fLink);
  void properFaces_ (Ksimplex sigma, 
              void (DComplex::*fFun) (int, OrTri, Dim, void *),
	            void * f);
  void faces_ (Ksimplex sigma, void (DComplex::*fFun) (int, OrTri, Dim, void *),
	            void * f);
  void kFaces_ (Ksimplex sigma, Dim k, 
              void (DComplex::*fFun) (int, OrTri, Dim, void *), void * f);



  Bool isFiniteSimplexFacet_ (OrTri f);

  int testBoundary_ (Simplex sigma);
  int testBoundaryB_ (Simplex sigma);
  Bool isBoundary_;

  // function pointers
  int (DComplex::*equivocalP_) (Simplex);
  int (DComplex::*confidentP_) (Simplex);
  int (DComplex::*centeredP_) (Simplex);
  Simplex (DComplex::*uPredP_) (Simplex);
  int (DComplex::*ceFlowP_) (Simplex, Simplex);
  int (DComplex::*ldSuccP_) (Simplex, Simplex);
  int (DComplex::*ecFlowP_) (Simplex, Simplex);
  double (DComplex::*sizeOfCenteredSimplexP_) (Simplex);
};

//------------------------------------------------------------------------
//  inline members

inline OrTri DComplex::
getFace (Simplex sigma)
{ return (ix_->ref ((int) sigma.getIndex (), (int) sigma.getDim ())); }

inline void DComplex::
getVertices (OrTri ef, Dim d, VertArray & v)
{ tr_->getVertices (ef, d, v); }

inline void DComplex::
getVertices (Simplex sigma, VertArray & v)
{ getVertices (getFace (sigma), sigma.getDim (), v); }

inline void DComplex::
getVertices (Ksimplex sigma, VertArray & v)
{ getVertices (sigma.getFace (), sigma.getDim (), v); }

inline void DComplex::
getSortedVertices (OrTri ef, Dim d, VertArray & v)
{ tr_->getSortedVertices (ef, d, v); }

inline void DComplex::
getSortedVertices (Simplex sigma, VertArray & v)
{ getSortedVertices (getFace (sigma), sigma.getDim (), v); }

inline void DComplex::
getSortedVertices (Ksimplex sigma, VertArray & v)
{ getSortedVertices (sigma.getFace (), sigma.getDim (), v); }

inline Index DComplex::
getIndex (OrTri ef, Dim d)
{ return ix_->ix (ef, d); }

inline Index DComplex::
getIndexV (VIndex v)
{ return ix_->ix_v (v); }

inline Index DComplex::
getIndexV (VIndex v0, VIndex v1)
{ return ix_->ix_v (v0, v1); }

inline Index DComplex::
getIndexV (VIndex v0, VIndex v1, VIndex v2)
{ return ix_->ix_v (v0, v1, v2); }

inline Index DComplex::
getIndexV (VIndex v0, VIndex v1, VIndex v2, VIndex v3)
{ return ix_->ix_v (v0, v1, v2, v3); }

inline Simplex DComplex::
createSimplex (OrTri ef, Dim d)
{ return Simplex (getIndex (ef, d), d); }

inline Simplex DComplex::
createSimplex (Ksimplex sigma)
{ return createSimplex (sigma.getFace (), sigma.getDim ()); }

#ifdef WIN32
inline Ksimplex DComplex::createKsimplex (Simplex sigma)
{ return Ksimplex (getFace (sigma), sigma.getDim ()); }
#else
/*
 * On SGI, you cant declare an inline after you call it. In this case,
 * it's called by passing it's result in one of the methods above. I had
 * to move this to the main cpp file. -swp
 */
#endif

inline Simplex DComplex::
createSimplexV (VIndex v)
{ return Simplex (ix_->ix_v (v), 0); }

inline Simplex DComplex::
createSimplexV (VIndex v0, VIndex v1)
{ return Simplex (ix_->ix_v (v0, v1), 1); }

inline Simplex DComplex::
createSimplexV (VIndex v0, VIndex v1, VIndex v2)
{ return Simplex (ix_->ix_v (v0, v1, v2), 2); }

inline Simplex DComplex::
createSimplexV (VIndex v0, VIndex v1, VIndex v2, VIndex v3)
{ return Simplex (ix_->ix_v (v0, v1, v2, v3), 3); }

inline Ksimplex DComplex::
changeDim (Ksimplex sigma, Dim k)
{ return Ksimplex (sigma.getFace (), k); }

inline Ksimplex DComplex::
increaseDim (Ksimplex sigma)
{ return Ksimplex (sigma.getFace (), sigma.getDim () + 1); }

inline Ksimplex DComplex::
decreaseDim (Ksimplex sigma)
{ return Ksimplex (sigma.getFace (), sigma.getDim () - 1); }

inline Bool DComplex::
isFiniteSimplexFacet_ (OrTri f) 
{ return ! tr_->isOnHull (f); }

//------------------------------------------------------------------------

#endif // __DCOMP_H__
