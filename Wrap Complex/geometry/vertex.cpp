
/*--------------------------------------------------------------------------*/

#ifdef WRAP_DEBUG
#define __DEBUG__
#endif

#include "vertex.h"
#include <basic/basic.h>
#define AssertDimension(d) Assert( (d) >= 0 && (d) <= 3)

/*--------------------------------------------------------------------------*/

#define EXCHANGE2(i,j) (tmp = v[i], v[i] = v[j], v[j] = tmp)
#define EXCHANGE3(i,j,k) (tmp = v[i], v[i] = v[j], v[j] = v[k], v[k] = tmp)
#define EXCHANGE4(i,j,k,l) \
            (tmp = v[i], v[i] = v[j], v[j] = v[k], v[k] = v[l], v[l] = tmp)

/*--------------------------------------------------------------------------*/

void
sortVertexArray (register VertexArray4 v, int d)

{
#define AssertOrder(x) 
  register Vertex tmp;
  AssertDimension (d);

  if (d == 0) return;

  if (d == 1) {
    if (v[0] > v[1]) EXCHANGE2 (1, 0);
    return;
  }

  if (v[0] > v[1]) {
    if (v[0] > v[2]) {
      if (v[1] > v[2]) { /* 2 < 1 < 0 */
	AssertOrder ((v[2] < v[1]) && (v[1] < v[0]));
	EXCHANGE2 (2, 0);
      } else { /* 1 < 2 < 0 */
	AssertOrder ((v[1] < v[2]) && (v[2] < v[0]));
	EXCHANGE3 (2, 0, 1);
      }
    } else { /* 1 < 0 < 2*/
      AssertOrder ((v[1] < v[0]) && (v[0] < v[2]));
      EXCHANGE2 (1, 0);
    }
  } else {
    if (v[1] > v[2]) {
      if (v[0] > v[2]) { /* 2 < 0 < 1*/
	AssertOrder ((v[2] < v[0]) && (v[0] < v[1]));
	EXCHANGE3 (2, 1, 0);
      } else { /* 0 < 2 < 1*/
	AssertOrder ((v[0] < v[2]) && (v[2] < v[1]));
	EXCHANGE2 (1, 2);
      }
    }
  }

  AssertOrder ((v[0] < v[1]) && (v[1] < v[2]));
  if (d == 2) return;

  if (v[3] > v[1]) {
    if (v[3] < v[2]) { /* 0 < 1 < 3 < 2 */
      AssertOrder ((v[0] < v[1]) && (v[1] < v[3]) && (v[3] < v[2]));
      EXCHANGE2 (2, 3);
    }
  } else {
    if (v[3] > v[0]) { /* 0 < 3 < 1 < 2 */
      AssertOrder ((v[0] < v[3]) && (v[3] < v[1]) && (v[1] < v[2]));
      EXCHANGE3 (3, 2, 1);
    } else { /* 3 < 0 < 1 < 2 */
      AssertOrder ((v[3] < v[0]) && (v[0] < v[1]) && (v[1] < v[2]));
      EXCHANGE4 (3, 2, 1, 0);
    }
  }

  AssertOrder ((v[0] < v[1]) && (v[1] < v[2]) && (v[2] < v[3]));
#undef AssertOrder
}

/*--------------------------------------------------------------------------*/

int
verticesEqual (register VertexArray4 v1, register VertexArray4 v2, int d)

{
  register VertexPointer v1Stop;

  AssertDimension (d);

  v1Stop = v1 + d + 1;
  do {
    if (*v1++ != *v2++) return 0;
  } while (v1 != v1Stop);

  return 1;
}

/*--------------------------------------------------------------------------*/

int
isSubsetVertices (register VertexArray4 v1, int d1, 
		  register VertexArray4 v2, int d2)

{
  register int a, b;

  AssertDimension (d1);
  AssertDimension (d2);

  if (d1 > d2) return 0;

  a = b = 0;
  while (1) {
    if (v1[a] == v2[b]) {
      a++;
      if (a > d1) return 1;
    }
    b++;
    if (b > d2) return 0;
  }
}

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

