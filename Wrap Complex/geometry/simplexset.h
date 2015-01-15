/*
 * simplexset.h -- SimplexSet
 *
 * A simple class for set of simplices(vertices/edges/triangles/tetrahedra)
 * in OrTri representation.
 *
 * -- Damrong  Feb 6, 1998
 *
 */

#ifndef __RGI_SIMPLEXSET_H__
#define __RGI_SIMPLEXSET_H__

#include "basic/index.h"

class OrTriStack;

class SimplexSet 
{
public:
        SimplexSet (void);
       ~SimplexSet (void);

	void pushSimplex (OrTri ot, Dim dim);
        void clear (void);
        IStackIter<OrTri> *getIter (Dim dim);

private:
  OrTriStack *stacks[4];
};

#endif 
