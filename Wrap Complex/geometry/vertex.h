
#ifndef __RGI_VERTEX_H__
#define __RGI_VERTEX_H__

#ifdef __cplusplus
extern "C" {
#endif
/*--------------------------------------------------------------------------*/

typedef int VertexArray4[4];
typedef double CoordArray[3];
typedef int Vertex;
typedef int * VertexPointer;

/*--------------------------------------------------------------------------*/

int verticesEqual (VertexArray4 v1, VertexArray4 v2, int d);
int isSubsetVertices (VertexArray4 v1, int d1, 
		      VertexArray4 v2, int d2);
void getCoordsVertex (Vertex v, CoordArray c);
void getScaledCoordsVertex (Vertex v, CoordArray c);
void computeLimits (double minCoord[3], double maxCoord[3]);

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif


