/*
 * simplexset.cpp -- SimplexSet
 *
 * A simple class for set of simplices(vertices/edges/triangles/tetrahedra)
 * in OrTri representation.
 *
 * -- Damrong  Feb 6, 1998
 *
 */

#include <geometry/trist.h>
#include <geometry/simplexset.h>

SimplexSet::
SimplexSet (void)
{	int i;
	for(i=0; i<4; i++) stacks[i] = new OrTriStack;
}

SimplexSet::
~SimplexSet (void)
{	int i;
	for(i=0; i<4; i++) delete stacks[i];
}

void SimplexSet::
pushSimplex (OrTri ot, Dim dim)
{
	stacks[dim]->push(ot);
}

void SimplexSet::
clear (void)
{	int i;
	for(i=0; i<4; i++) stacks[i]->reset();
}

IStackIter<OrTri> * SimplexSet::
getIter (Dim dim)
{	
        return new IStackIter<OrTri>(*stacks[dim]);
}


