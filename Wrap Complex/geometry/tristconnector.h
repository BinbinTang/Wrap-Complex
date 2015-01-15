/*
 * tristconnector.h - class for connecting the trist structure;
 *
 * author -- Ho-lun Cheng
 *
 * function : 	add the triangles and points into the TristConnector
 *		and then call the function GlueTheTriangles().
 *
 * output :	trist pointer, long interger array of points,
 *		principal and shadow triangle bitvector, 
 *
 * usage  : 	case 1:
 *		input the point position of the triangles by 
 *		AddPointWithTri(), when every three points
 *		are inputed, a triangle will automatically made
 *
 *		case 2:
 *		first input all the points by AddPoint(), and
 *		then input the triangles with the index of the 
 *		vertex with AddTri();
 *
 *		After input all the triangles, you need to call
 *		the GlueTheTriangles() to construct the linkage.
 *
 * date -- 6/1/97
 */

#ifndef __RGI_TRISTCONNECTOR_H__
#define __RGI_TRISTCONNECTOR_H__

#include <geometry/tristmodifier.h>

#include <basic/kdtree.h>
#include <li/li.h>

class DoublePArray;
class LI_PArray;
class SOS_PrimEvaluator;
class SOS_FloatEvaluator;

// for addVertex return value
#define NO_TRI_CREATED -999

class TristConnector : public TristModifier
{
public:
  TristConnector(Bool useKdTree = TRUE, Bool componentShareVertex = FALSE,
		double epsilon = 0.0);
  ~TristConnector (void);
  void setPrecision(int lMax,int pMax);
  int setPrecision(const char * s);
  void getPrecision(int& lMax, int& pMax);
  void glueTheTriangles(void);
  
  int addPointWithTri(char *,char *,char *); // return the tri index or NO_TRI_CREATED
  int addPointWithTri(double x[3]);
  int addTri(VIndex,VIndex,VIndex);
  
  int addPoint(char *,char *,char *);
  int addPoint(double x[3]);
  
  void newObject();
  
  DoublePArray* getDoublePArray(void);
  BitVector& getShadow(void);
  BitVector& getPrincipal(void);
  Trist * getTrist (void); // override parent's getTrist - change ownership
  void setEpsilon(double epsilon);
  double getEpsilon (void);
  const char * getErrorMsg (void) { return errorMsg_; }
  LI_Converter * getConverter (void) { return &converter_; }

public:
  class EdgeGlueList : public RArray<OrTri> 
  {
  public:
    EdgeGlueList (void);
    ~EdgeGlueList (void);
    Bool existEdge(VIndex v2, Trist * tr);
    Bool existOrTri(OrTri tri);
    OrTri getOneTri(VIndex v2, Trist * tr);
    void addNewEdge(OrTri tri);
    unsigned short nUsed;
  };
  
protected:
  Bool componentShareVertex_;
  DoublePArray   *ra_;
  LI_Converter converter_;
  BitVector shadow_;
  BitVector principal_;
  SOS_FloatEvaluator *spe_;
  KdTree *tree_;
  RObjArray<EdgeGlueList>  * edgelist_;
  int numPtAdded_;
  int actualNumPtAdded_;
  int prevObjsActualNumPtAdded_;
  VIndex triVertIdx_[3];
  IMap * vmap_;
  char errorMsg_[128];
  int noTriConnectedToThisEdge_(OrTri);
  void fittingTri_(OrTri newtri,VIndex v1,VIndex v2);
  int inOrder_(OrTri,OrTri,OrTri);
  int below_(VIndex v1,VIndex v2,VIndex v3,VIndex p);
  void addShadowTri_(void);
  OrTri turnEdge_(OrTri tri,VIndex v1,VIndex v2);
  OrTri flip_(OrTri);
  void  correctOrient_(void);
  void  CODFS_(OrTri tri,BitVector* flipped,
    Bool isprincipal,int version);
  //  CODFS() -> Correct Orientation DFS
  
  void printVIdx(OrTri);
  void removeDuplicateTri_(OrTri,OrTri);
};

void sort3 (VIndex &v1,VIndex &v2,VIndex &v3);

#endif // __TRISTCONNECTOR_H__
