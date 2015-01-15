/*
 * simph.h -- C++ Header file for the Simplex Hash Table class
 *
 * author -- Mike, Dmitry, Damrong
 *
 * date -- 08/28/97
 *	-- 02/28/98
 */

#ifndef __RGI_SIMPH_H__
#define __RGI_SIMPH_H__

class   SimpHTable;
class  FSimpHTable;
class  RSimpHTable;

#include <basic/rarray.h>
#include <basic/binio.h>
#include <geometry/comp.h>
#include <geometry/trist.h>
#include <geometry/vertarray.h>
#include <geometry/geomutil.h>


/******************************************************************************/
/*
 * Note about consistency of index
 *
 *     We use integer index from Hash table as unique identifier
 * for edge and tetrahedron.  Hash Table also provides alternate mechanism
 * to iterate over all simplices(edges/tetrahedra), in addition to 
 * the main triangle-based Trist. (Which way is more efficient may
 * depend on context.)  
 *  
 *     The way we get an integer index for a simplex is nondeterministic 
 * due to the nature of universal hashing.  Anyway, we guarantee that 
 * after the simplex is inserted into Hash Table, its index is preserved
 * under resizing of Hash Table(see RSimpHTable). 
 *
 *     Continuous indices should not be assumed due to the way 
 * nondeterministic universal hash function works.
 *
 *     Usually there are two major index convention : 0-based and 1-based.
 * The unfortunate conditions come when these two conventions are mixed
 * inconsistently over many parts of project, and can change when 
 * implementation change without changing interface.   This problem
 * is more complicated when universal hash function is used.  We can't
 * deterministically determine when the '0' index is used.
 *
 *     In the current implementation, RSimpHTable basically uses FreeArray
 * to store entries represented simplex, so it will be 1-based index. 
 * Unfortunately, FSimpHTable stores entries in ordinary C array which
 * is 0-based.  I'm not sure whether this inconsistency will cause future 
 * unforeseen problems.
 *
 *     Some base classes                  Simplex Hash Tables
 *     FreeArray<T>   1-based index       RSimpHTable   1-based index
 *     BitVector      0-based index       FSimpHTable   0-based index
 *     RArray<T>      0-based index
 *
 */
/******************************************************************************/
/*
 * Note about notion of 'size'
 *
 *    FSimpHTable(Fixed Size Simplex Hash Table) assumes caller know 
 * exactly how many simplices are gonna be stored, but 
 * RSimpHTable (Resizable Simplex Hash Table) recognize size only as
 * an expected number; the table can be shrink/expand to arbitrary size
 * later.
 *    This results in a decision to make prototypes of constructors
 * different :-
 *
 *  FSimpHTable (DComplex *dcomp, Dim d, int size, BinInput& bi);
 *  RSimpHTable (DComplex *dcomp, Dim d, BinInput& bi);
 *  
 *    The former use 'size' for consistency checking against what is
 * read from BinInput.  Similar consistency checking for RSimpHTable
 * ,if needed, is expected to be performed by caller {her/him/it}self. 
 *
 */ 
/******************************************************************************/




/******************************************************************************/
/*
 * Class SimpHTable
 *
 *       Abstract class for Simplex Hash Table using universal
 * hashing scheme and open addressing with double hashing
 * to solve collision.  The actual container of data elements
 * must be defined in the derived class.  This absctract class
 * only provide hashing function.
 * (Cormen,Leiserson,& Rivest. Introduction to Algorithms,
 * pages 226-239, MIT Press, 1990.)
 *
 *       This Hash Table maps Vertex Array to 'index' of the 
 * corresponding simplex.  This 'index' will be used in iteration over 
 * all edges or tetrahedra.
 *       The plan is to have two derived classes : FSimpHTable and
 * RSimpHTable.  The former is for fixed-size hash table similar
 * to the previous version of SimpHTable.  The latter is resizable
 * which will support 'dynamic' nature of new version of wrap.
 *
 * 
 * NOTE
 *
 * 1. search by either Vertex Array or Edge Facet.
 *    insert by Edge Facet only.
 *
 * 2. OrTri = Oriented Triangle, EF = Edge Facet, Trist = Triangle
 *    Edge structure.  Essentially, they refer to the same thing.
 *
 * 3. Generally speaking, search key is essentially Vertex Array, 
 *    but what gets stored is an OrTri index of the corresponding 
 *    simplex. Using this OrTri, the corresponding Vertex Array is 
 *    acquired via Trist structure of Delaunay Complex.  To guarantee 
 *    consistency, order of change in Delaunay Complex and Hash Table 
 *    is important.
 *
 *
 * PURE VIRTUAL FUNCTIONS (to be defined in derived classes)
 *
 * searchKey    return index of simplex, -1(NIL) on unsuccessful search.
 *
 * insertKey    Insert the simplex, return index of the new simplex.
 *	        Simply return index if the simplex was already there before.
 *
 * isValidIndex	Return TRUE  for valid simplex index,
 *              FALSE for non-existing simplex index or
 *              out of bound, e.g. negative index.
 *
 * op[index]    OrTri of indexed simplex. 
 *
 *
 *
 * MEMBER FUNCTIONS
 *
 * size      	Our hash function needs prime size. Table size
 *           	is set to be the prime successor of the requested size
 *           	at construction time.
 *
 * numElts
 *
 * binInit    	read hash table parameters from BinInput
 *
 * binDump    	save hash function into BinOutput.
 *            	Only hash function parameter is saved.
 *            	The data elements are not saved because this 
 *            	abstract class has no container for data.
 *
 * <<         	friend print operator
 *
 *
 *
 * NESTED CLASS
 *
 * SimpKey, EF_SimpKey, VA_SimpKey
 * 		Basically, they're wrappers of an external vertex array.
 */

class SimpHTable
{
public:
  class    SimpKey;
  class EF_SimpKey;
  class VA_SimpKey;

  enum  { NIL= -1 };          // return value for unsuccessful search
                              // also, internal NULL value for empty entry

  SimpHTable (Trist     *tr, Dim d, int size, Bool isRandom= TRUE);
  virtual ~SimpHTable (void);

  virtual int    searchKey    (VA_SimpKey& vaKey)=0;  
  virtual int    insertKey    (EF_SimpKey& efKey)=0;
  virtual Bool   isValidIndex	      (int index)=0;
  virtual OrTri& operator []  	      (int index)=0;

  inline  int 	 searchKey     (EF_SimpKey & efKey);
  inline  int	 search                  (OrTri ef);
  inline  int	 insert                  (OrTri ef);
  inline  int	 size                        (void);
  inline  int	 numElts                     (void);

  	  Bool   binInit	    (BinInput & bi);
  	  void   binDump	    (BinOutput& bo);

  friend  
  ostream& operator << (ostream& s, SimpHTable& simpHTable);


  // Nested classes

  class SimpKey
  {
  public:
    inline             SimpKey   (VertArray * vArray = NULL);
    inline VertArray*  getVertices                    (void);

  protected:
    VertArray * vArray_;
  };

  class EF_SimpKey : public SimpKey
  {
  public:
    inline       EF_SimpKey (OrTri ef , Dim d , VertArray * vArray, Trist * tr);
    inline OrTri getEF      (void);

  private:
    OrTri ef_;
    Dim    d_;
  };

  class VA_SimpKey : public SimpKey
  {
  public:
    inline  VA_SimpKey (VertArray *vArray);
  };


protected:

  Magic     magic_; 
  int        size_; 
  int     numElts_; 
  int	keyLength_; 
  Trist      * tr_;    
  Dim           d_;
  int        *  a_;	// Hashing parameter
  Bool   isRandom_;
  VertArray  vRef_;	// temporary vertex array used in search(), insert()

  inline  Bool    compare_         (SimpKey& key1, SimpKey& key2);
  	  void   uhashNew_ 		                   (void);
  	  int       uhash_		     (VA_SimpKey&  vaKey);
  	  void hashWriteA_ (BinOutput& bo, int *a, int keyLength);
  	  void  hashReadA_ (BinInput&  bi, int *a, int keyLength);
};


// inline functions 

inline SimpHTable::SimpKey::
SimpKey (VertArray *vArray)
:vArray_(vArray)
{}

inline VertArray* SimpHTable::SimpKey::
getVertices (void) 
{ 
  return vArray_; 
}

inline SimpHTable::EF_SimpKey::
EF_SimpKey (OrTri ef , Dim d , VertArray * vArray, Trist * tr)
:            SimpKey (vArray), ef_ (ef), d_ (d)
{ 
  tr->getSortedVertices(ef, d, *vArray); 
}

inline OrTri SimpHTable::EF_SimpKey::       
getEF (void) 
{ 
  return ef_; 
}

inline SimpHTable::VA_SimpKey::
VA_SimpKey (VertArray *vArray)
:             SimpKey (vArray)
{}

inline int SimpHTable::
searchKey (EF_SimpKey & efKey)
{
  VA_SimpKey k = VA_SimpKey (efKey.getVertices ());
  return searchKey (k);
}

inline int SimpHTable::
search (OrTri ef)
{
  EF_SimpKey k = EF_SimpKey (ef, d_, &vRef_, tr_);
  return searchKey (k);
}

inline int SimpHTable::
insert (OrTri ef)
{ 
  EF_SimpKey k = EF_SimpKey (ef, d_, &vRef_, tr_);
  return insertKey (k);
}

inline int SimpHTable:: 
size (void) 
{ 
  return size_;    
}

inline int SimpHTable:: 
numElts (void) 
{ 
  return numElts_; 
}  

inline Bool SimpHTable:: 
compare_ (SimpKey& key1, SimpKey& key2) 
{ 
  return *key1.getVertices ()== *key2.getVertices ();
}


/******************************************************************************/

/*
 * Class FSimpHTable
 *       Fixed size Simplex Hash Table derived from abstract 
 * class SimpHTable.  This is similar to the previous version
 * of SimpHTable for static point set.
 *
 *
 * NOTE
 *
 *       To alleviate collision from hash function, the hash
 * table size is made 'double' of what requested.  
 *       There is no default size in this version.  Caller 
 * must know in advance how many items will be stored in the
 * table.  Otherwise, use RSimpHTable instead.  
 *       Because data/key is kept directly in C array (arr_),
 * this table is 0-based indexed.
 *       There is no 'save' operation for data/keys because the
 * order of simplices in Delaunay complex is assumed to be known.
 * As a result, when calling binInit (), programmer is assumed to insert 
 * simplices in the correct order.
 *
 *
 * PUBLIC MEMBER FUNCTIONS
 *	
 * 	    Realized all purely virtual functions of SimpHTable and ...
 *
 * del      mark entry specified by simplex index as `deleted.'
 *
 *          WARNING :  Since we did not store keys into disk,
 *          -------    binDump/binInit FSimpHTable with some deleted entries
 *                     might cause inconsistency.
 *
 * getBV    give reference to BitVector representing all valid simplex 
 *	    indices. 
 * 
 * binInit  just call SimpHTable::binInit and check size_ in BinInput
 *          is the same as constructor.
 *
 * 
 */

class FSimpHTable : public SimpHTable
{
public:

  FSimpHTable (Trist *tr, Dim d, int size, Bool isRandom= TRUE);
 ~FSimpHTable (void);

  	 int         searchKey   (VA_SimpKey& vaKey);
  	 int         insertKey   (EF_SimpKey& efKey);
  inline Bool        isValidIndex        (int index);
  inline OrTri&      operator []         (int index);

  	 Bool        del          	 (int index);
  inline BitVector&  getBV         	      (void);  
         Bool	     binInit	     (BinInput & bi);

protected:

  BitVector      used_;  //  used valid
  BitVector	valid_;  //    0    0      empty
                         //    1    1   occupied
                         //    1    0    deleted
  OrTri*          arr_;  
  VertArray     vTest_;  // vertex array for comparison
};

// inline functions

inline Bool FSimpHTable::
isValidIndex (int index)  
{ 
  return (index>=0)? used_.test(index) : FALSE; 
}

inline OrTri & FSimpHTable::
operator []  (int index)
{ 
  Assert_always (used_.test (index)); 
  return arr_[index]; 
}

inline BitVector & FSimpHTable::
getBV (void)
{
  return valid_;
}




/******************************************************************************/

/*
 * Class RSimpHTable (Resizable Simplex Hash Table)
 *
 *       Derived from SimpHTable, this hash table can dynamically 
 * insert or delete elements without prior knowledge of maximum number 
 * of elements.  The current strategy is to automatically resize/rehash
 * the table at appropriate time.
 *       When resize is performed, indices of all current elements are
 * preserved.  This is needed in order to guarantee consistency
 * with other parts of project i.e. wrap.
 *
 * NOTE
 *       Preservation of indices of elements at resize time
 * is achieved by indirection table for index of simplex, ix_[].  
 * The hash function will index into entry of ix_, which in turn index 
 * into table of OrTri, namely arr_, represented simplex.
 *       To alleviate collision, ix_ size is made 'double' of the
 * expected size.  This is not needed for OrTri table arr_.
 * This issue will probably reflect the way iterator(to be implemented)
 * for simplices (edges/tetrahedra) works. 
 *       Due to the requirement to save/load hash table, I derive
 * FreeArray<OrTri> to a new class PFOrTriA(Persistent Free OrTri Array).
 * I want to make PFOrTriA local to implementation simph.cpp,
 * but don't know how.
 *
 *
 *
 * PUBLIC MEMBER FUNCTIONS
 *
 *          Realized all purely virtual functions of SimpHTable and ...
 *
 *
 * del      Delete simplex specified by simplex index.  
 *          Return TRUE(1) on successful deletion, FALSE(0) if 
 *          item not exists.
 * binInit
 *
 * binDump  In addition to hash function parameters, save data/keys into 
 *          BinOutput.  The latter is not needed in FSimpHTable but is 
 *          crucial here in RSimpHTable in order to preserve simplex 
 *          indices.
 *
 * getBV    give reference to BitVector representing all valid simplex 
 *	    indices. 
 *
 * size     return size of arr_, which is also the maximum simplex index.
 *
 * numDel
 *
 * resizeCount1  number of resize from insert
 * resizeCount2  number of resize from search  (see RESIZE CRITERIA)
 * avgProbeLen   average probe length
 * numSearch    
 *
 *
 * NETSTED CLASS PFOrTriA (Persistent Free OrTri Array)
 *
 *      This tiny class, derived from FreeArray<OrTri>, provides additional
 * functionality that allow load/save from/to BinInput/BinOutput.
 * It let us load/save OrTri table arr_[].
 *
 *      
 * RESIZE CRITERIA
 *   
 *  insert:
 *     (numEmpty_ < minEmpty_) ==> resize(EXPAND*numElts_)
 *  delete:
 *     (  numDel_ >  maxDel_ ) ==> resize(REHASH*numElts_)
 *
 *  enableResize_ is not used for now. --Damrong April 23, 1998.
 *     The reason we have additional state variable 'enableResize_' is that
 *     resize() function also call search() to implicitly rehash ix_.
 *     In the current implementation, we can't afford possible re-entrance 
 *     of resize() called by search() called by resize() itself.
 *     (resize->search->resize can possibly cause consistency problem)
 */

class RSimpHTable : public SimpHTable 
{
public:
  RSimpHTable (Trist * tr, Dim d, 
               int   size         =  50, Bool  IsRandom	     = TRUE,
               int   maxProbeLen  =   2, float minEmptyRatio =  0.3, 
               float maxDelRatio  = 0.3
              );

 ~RSimpHTable (void) { delete ix_; }

         int         searchKey     (VA_SimpKey& vaKey);
         int         insertKey     (EF_SimpKey& efKey);
  inline Bool        isValidIndex          (int index);
  inline OrTri&      operator []           (int index);

         Bool        del                   (int index);
         Bool        binInit          (BinInput &  bi);
         void        binDump          (BinOutput&  bo);
  inline BitVector&  getBV			(void);
  
  inline int         size                       (void);
  inline int         numDel                     (void);
  inline unsigned    resizeCount1               (void); 
  inline unsigned    resizeCount2               (void);
  inline double      avgProbeLen                (void);
  inline double      numSearch                  (void);

protected:
  class PFOrTriA : public FreeArray <OrTri>
  {
  public:
    PFOrTriA    (int size=20);
    Bool        binInit  (BinInput & bi);
    void        binDump  (BinOutput& bo);
    BitVector&  getBV             (void)    { return exists_; }
  };


protected:
  
  Magic     magic_; 	   // another magic_ since we're gonna store
			   // something into BinInput

  enum{    DEL=-2, NIL      }; 
  enum{ REHASH= 2, EXPAND=3 };  // values should come from experiments

  // class PFOrTriA;

  PFOrTriA           arr_; // store OrTri at (simplex index)th entry.
  RArray<int>        *ix_; // Array of 'index' unique to each simplex.
                           // This indirection helps preserve simplex 'index'
                           // when Hash Table is resized.

  int         probeIndex_; // search() tells delete() the corresponding
  int          freeIndex_; // entry in ix_ through probeIndex_ and tells
                           // insert() where to insert new element through
                           // freeIndex_.
                           // They're the Hashing Index into ix_, not the
                           // simplex index for arr_.
                           // Certainly, ix_[probeIndex_] is a simplex index.
                           // ix_[freeIndex_] is NIL or DEL.
  // For resizing

  int        maxProbeLen_;
  double   minEmptyRatio_; // minimum fraction of empty entries in ix_
                           // prevent non-terminate Probe sequence
                           // and help alleviate collision.
  double     maxDelRatio_; // maximum fraction of ix_ DEL entries.
                           // reduce unused memory
  int             maxDel_; // maximum number of DEL   entries in ix_
  int           minEmpty_; // minimum number of empty entries in ix_
  int             numDel_; // number of DEL   entries in ix_
  int           numEmpty_; // number of empty entries in ix_
  double     avgProbeLen_; // average probe lenght since last resize
  double       numSearch_; // number of times searching is called
                           // since last resize
  Bool      enableResize_; // State variable to be set to FALSE in resize().
                           // Usually it's always TRUE.
                           // See NOTE about RESIZE CRITERIA above.
                           // This variable also prevent resize()
                           // being called indirectly when loading
                           // hash table from disk via binInit().
  unsigned  resizeCount1_, // resize counter for benchmarking
            resizeCount2_; // it count up everytime resize() is called.
                           // It's never reset.
                           // resizeCount1_  resize from insert()
                           // resizeCount2_  resize from delete()
  void  resize_(int size); // rehash ix_ with new hashing function,
                           // preserve indices  of simplices in arr_.
                           // DEL are cleared from ix_. 
};


// inline functions

inline Bool  RSimpHTable::
isValidIndex (int index)   
{ 
  return  (index>0)? arr_.isValidIndex(index) : FALSE; 
}

inline OrTri & RSimpHTable::
operator []  (int index)
{ 
  Assert_always (arr_.isValidIndex(index)); 
  return arr_[index]; 
}

inline BitVector & RSimpHTable::
getBV (void)
{
  return arr_.getBV ();
}

inline int RSimpHTable::
size (void)
{
  return arr_.maxSize ();
}

inline int RSimpHTable::
numDel (void)  
{ 
  return numDel_; 
}

inline unsigned RSimpHTable::
resizeCount1 (void)  
{ 
  return resizeCount1_; 
}

inline unsigned RSimpHTable::
resizeCount2 (void)  
{ 
  return resizeCount2_; 
}

inline double RSimpHTable::
avgProbeLen (void)
{
  return avgProbeLen_;
}

inline double RSimpHTable::
numSearch (void)
{
  return numSearch_;
}

//------------------------------------------------------------------------------

#endif // __SIMPH_H__
