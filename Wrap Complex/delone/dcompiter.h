/*
 * dcompiter.h -- C++ Header file for the Delaunay Complex Iterators
 *
 * author -- Mike Facello
 *
 * date -- 10/24/96
 */

#include "delone/dcomp.h"

/************************************************************************/

/*
 * Class DCompAllFacesIter
 *
 * 
 *
 */

class DCompAllFacesIter : public CompIter
{
public:
  DCompAllFacesIter (DComplex& dcomp, Simplex sigma);

  // Iteration operations -- these just call the FArrayIter methods

  void first(void);
  void next(void);
  Bool isDone(void);     // This is true when the current element 
                            //     is not valid
  Simplex curItem(void);

private:
  DComplex& dcomp_;
  Simplex sigma_;
  // Add other variables that are necessary for this iteration
};




/************************************************************************/

/*
 * Class DCompFacesIter
 *
 * 
 *
 */

class DCompFacesIter : public CompIter
{
public:
  DCompFacesIter (DComplex& dcomp, Simplex sigma, Dim d) ;

  // Iteration operations -- these just call the FArrayIter methods

  void first(void);
  void next(void);
  Bool isDone(void);     // This is true when the current element 
                            //     is not valid
  Simplex curItem(void);

private:
  DComplex& dcomp_;
  Simplex sigma_;
  Dim d_;
  // Add other variables that are necessary for this iteration
};




/************************************************************************/

/*
 * Class DCompCoFacesIter
 *
 * 
 *
 */

#ifndef __DCOMPITER_H__
#define __DCOMPITER_H__

class DCompCoFacesIter : public CompIter
{
public:
  DCompCoFacesIter (DComplex& dcomp, Simplex sigma, Dim d) ;

  // Iteration operations -- these just call the FArrayIter methods

  void first(void);
  void next(void);
  Bool isDone(void);     // This is true when the current element 
                        //     is not valid
  Simplex curItem(void);

private:
  DComplex& dcomp_;
  Simplex sigma_;
  Dim d_;
  // Add other variables that are necessary for this iteration
};




/************************************************************************/

/*
 * Class DCompUnivIter
 *
 * 
 *
 */

class DCompUnivIter : public CompIter
{
public:
  DCompUnivIter (DComplex& dcomp, 
		 Bool v, Bool e, Bool f, Bool t, 
		 Bool bd, Bool interior);

  // Iteration operations -- these just call the FArrayIter methods

  void first(void);
  void next(void);
  Bool isDone(void);     // This is true when the current element 
                            //     is not valid
  Simplex curItem(void);

private:
  DComplex& dcomp_;
  Bool v_, e_, f_, t_, bd_, interior_;
  // Add other variables that are necessary for this iteration
};




#endif // __DCOMPITER_H__
