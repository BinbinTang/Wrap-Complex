/*
 * index.h -- Classes for simplex indices and dimensions.
 *
 * author -- Mike Facello
 *
 * date -- 10/25/96
 */

/************************************************************************/

/*
 * Class Index
 *
 * This class represents a simplex index.  It is synonymous with an integer.
 *
 */



#ifndef __RGI_INDEX_H__
#define __RGI_INDEX_H__

#include <basic/basic.h>

// Indices will begin with 1 (not 0)
#define NOTANINDEX 0

typedef int Index;
typedef int VIndex;
typedef int EIndex;
typedef int FIndex;
typedef int TIndex;
typedef int Dim;

#endif  // __INDEX_H__
