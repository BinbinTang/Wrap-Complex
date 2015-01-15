/*
 * ihandler.h -- C++ Header file for the Index Handler class
 *
 * author -- Mike Facello, Damrong
 *
 * NOTE the use of '#ifdef _DYNAMIC_' for dynamic hash tables.
 *
 * date -- 10/24/96
 *	--  2/27/98
 */

#ifndef __RGI_IHANDLER_H__
#define __RGI_IHANDLER_H__

#include <basic/bitvector.h>
#include <basic/binio.h>
#include <geometry/tristmodifier.h>
#include <geometry/comp.h>
#include <geometry/vertarray.h>
#include <geometry/simph.h>
#include <geometry/simplexset.h>

class FSimpHTable;				       // geometry/simph.h
class RSimpHTable;				       // geometry/simph.h

/************************************************************************/

/*
 * Class IndexHandler
 *
 * The purpose of this class is to allow convenient representation of
 * edges and tetrahedra for a Trist structure used to represent some
 * other Complex.
 *
 */

class IndexHandler : public TristModifier
{
public:
  enum { INDEX_V = 1,
         INDEX_E = 2,
         INDEX_F = 4,
         INDEX_T = 8 };

  IndexHandler (Complex *comp, BitVector * triangles = NULL,
        unsigned int dimFlag = INDEX_V | INDEX_E | INDEX_F | INDEX_T,
        Bool closed = FALSE, Bool isRandom = TRUE);
  ~IndexHandler (void);
  // Conversion operators

  inline VIndex     toVIX   (OrTri ef);
  inline EIndex     toEIX   (OrTri ef);
  inline FIndex     toFIX   (OrTri ef);
  inline TIndex     toTIX   (OrTri ef);

  inline OrTri      fromVIX  (VIndex v);
  inline OrTri      fromEIX  (EIndex e);
  inline OrTri      fromFIX  (FIndex f);
  inline OrTri      fromTIX  (TIndex t);

         int        ix (OrTri ef, Dim d);
  inline int        ix  (Ksimplex sigma);

  inline OrTri      ref   (Simplex sigma);
         OrTri      ref   (Index index, Dim d);
  inline OrTri      ref_v (VIndex a);
  inline OrTri      ref_v (VIndex a, VIndex b);
  inline OrTri      ref_v (VIndex a, VIndex b, VIndex c);
  inline OrTri      ref_v (VIndex a, VIndex b, VIndex c, VIndex d);

  inline VIndex     ix_v (VIndex a);
         EIndex     ix_v (VIndex a, VIndex b);
         FIndex     ix_v (VIndex a, VIndex b, VIndex c);
         TIndex     ix_v (VIndex a, VIndex b, VIndex c, VIndex d);

  // set new representative for current index
  // old representative should be a valid index
         int        insert     (OrTri ef,         Dim d);
         int        substitute (OrTri ef, int ix, Dim d);
         Bool       del        (          int ix, Dim d);
         Bool       del        (OrTri ef,         Dim d);

         int        ixMaxIndex       (           Dim d);
         Bool       ixIsSimplexIndex (int index, Dim d);
  inline Bool       ixIsInitialized  (void);

         BitVector& getBitVector (Dim d);
         int numUsed (Dim d);

         Bool       binInit (BinInput&  bi);
         void       binDump (BinOutput& bo);
         int        init (void); 

         void       addSimplices (SimplexSet * s);
         void       delSimplices (SimplexSet * s);
         void       updSimplices (SimplexSet * s);
         void       reMap             (IMap  * m);

  friend ostream& operator<< (ostream& s, IndexHandler& iHandler);
  
private:
  
  Complex*              comp_;
  unsigned int       dimFlag_;
  Bool                closed_;
  Bool         isInitialized_;
  BitVector *      triangles_;
  int                   numV_;
  int                   numE_;
  int                   numF_;
  int                   numT_;
  BitVector         usedVert_;

#ifdef _DYNAMIC_
	RArray <OrTri>   vertTable_;
  RSimpHTable *    edgeTable_;
  RSimpHTable *   tetraTable_;
#else
	OrTri       *    vertTable_;
  FSimpHTable *    edgeTable_;
  FSimpHTable *   tetraTable_;
#endif
};


// inline functions 

VIndex IndexHandler::
toVIX (OrTri ef) { return tr_->org(ef); }

EIndex IndexHandler::
toEIX   (OrTri ef) { return edgeTable_->search (ef); }

FIndex IndexHandler::
toFIX   (OrTri ef) { return ef.index (); }

TIndex IndexHandler::
toTIX   (OrTri ef) { return tetraTable_->search (ef); }

OrTri IndexHandler::
fromVIX (VIndex v) { return vertTable_[v]; }

OrTri IndexHandler::
fromEIX (EIndex ix) { Assert (edgeTable_->isValidIndex (ix));
  		      return ((*edgeTable_) [ix]); }
OrTri IndexHandler::
fromFIX (FIndex ix) { return OrTri (ix, 0); }

OrTri IndexHandler::
fromTIX (TIndex ix) { Assert (tetraTable_->isValidIndex (ix));
                      return ((*tetraTable_) [ix]); }
int IndexHandler::
ix (Ksimplex sigma) { return ix(sigma.getFace(), sigma.getDim()); }

OrTri IndexHandler::
ref (Simplex sigma) { return ref(sigma.getIndex(), sigma.getDim()); }

OrTri IndexHandler::
ref_v (VIndex a) { return ref(ix_v (a), 2); }

OrTri IndexHandler::
ref_v (VIndex a, VIndex b) { return ref(ix_v (a, b), 2); }

OrTri IndexHandler::
ref_v (VIndex a, VIndex b, VIndex c) { return ref(ix_v (a, b, c), 2); }

OrTri IndexHandler::
ref_v (VIndex a, VIndex b, VIndex c, VIndex d) { 
       return ref(ix_v (a, b, c, d), 2); }
VIndex IndexHandler::
ix_v (VIndex a) { return usedVert_.test(a)? a : SimpHTable::NIL; }

inline Bool IndexHandler::
ixIsInitialized (void) { return isInitialized_; }

inline int IndexHandler::
numUsed (Dim d) { return getBitVector(d).getNumSet(); }


/************************************************************************/

/*
 * Class IndexIter
 *
 * This class will iterate over the simplices in an IndexHandler,
 * returning the index or the oriented triangle for each simplex, as
 * desired.
 *
 */

class IndexIter
{
public:
  IndexIter (IndexHandler& iHandler, Dim d);

  // Iteration operations -- these just call the FArrayIter methods

  void     first (void);
  void      next (void);
  Bool    isDone (void);
  int   curItem  (void);
  int   curIndex (void);
  OrTri curOrTri (void);

private:
  IndexHandler& iHandler_;
  Dim                  d_;
  BitVectorIter     iter_;
};


// inline functions 

inline void IndexIter::
first (void) { iter_.first(); }

inline void IndexIter::
next (void) { iter_.next(); }

inline Bool IndexIter::
isDone (void) { return iter_.isDone(); }

inline int IndexIter::
curItem (void) { return iter_.curItem(); }

inline int IndexIter::
curIndex (void) { return curItem(); }

inline OrTri IndexIter::
curOrTri (void) { return iHandler_.ref(curItem(), d_); }


//-----------------------------------------------------------------------


#endif // __IHANDLER_H__
