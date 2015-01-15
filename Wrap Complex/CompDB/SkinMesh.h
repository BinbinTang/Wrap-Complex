
// SkinMesh.h: interface for the SkinMesh class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <basic/iterstack.h>
#include "basic/vectmat.h"
#include "delone/dcbuilder.h"
#include "CompDB/compDB.h"
#include "basic/queue.h"

#define MIN 0
#define MAX 1
#define AVG 2

class SkinMesh  
{
protected:
	virtual void genVertexPts_();
	virtual void genEdgePts_();
	virtual void genFacePts_();
	virtual void genTetraPts_();
	Bool visible_[4];
	CompDB *cdb_;
	double alpha_;
	double c; // number of points per unit weighted area.
	Trist* mesh_;
    FreeArrayDumpable<Vector3> parray_;
	RArray<Vector3> narray_;
	RArray<Vector3> fnarray_;
	RArray<int> vmc_;
	Bool inBody_(double[3],int);
	void sculpt_(); 
	double C_,Q_;
	Bool contractEdges_(); // return TRUE if something changed
	Bool insertCCenters_();
	// rebellious function
	Bool testInsertCCenters_();
	Bool solve_(Index,Vector3 v,Vector3 n,Vector3& p1,Vector3& p2);
	void flipEdges_(IterStack<OrTri>*,IterStack<OrTri>* touchedTri=NULL);
	void orientMesh_();
	void computeNormals_();
	IterStack<Vector3> debugPts_;
	IterStack<Vector3> debugPts2_;
	IterStack<VIndex>  debugPi_;
	Bool growPt_(int,double);
	Index growPt0_(int,double);
	Index growPt1_(int,double);
	Index growPt2_(int,double);
	Index growPt3_(int,double);
	Bool shrinkPt_(int,double);
	Index shrinkPt0_(int,double);
	Index shrinkPt1_(int,double);
	Index shrinkPt2_(int,double);
	Index shrinkPt3_(int,double);
	double currentAlpha_(VIndex);
	Vector3 pointNormal_(int);

	// statistics
	int inBodyTotal_,inBodyGeo_,inBodyExp_;
	double edgel_[3];
	double cirR_[3];
	double vDeg_[3];
	double angle_[3];
	double angleDefect_[3];
	double dangle_[3];

public:
	SkinMesh(CompDB*);
	virtual void dumpPoints(const char* filename);
	virtual void genMesh(double);
	double getCurrentAlpha() {return alpha_;};
	void finalUpdate(double alpha);
	void flipAllEdges();
	Bool refineMesh(); //return TRUE if something changed
	void draw();
	void drawMesh(double);
	double squaredSize(Dim d,int idx); // VIndex, EIndex, FIndex or TIndex (not Index)
	double squaredSize(Dim d,int idx,double alpha); // with different alpha
	virtual void toggleVisible(int i) {visible_[i]=!visible_[i];};
	Bool visible(int i) {return visible_[i];};
	virtual ~SkinMesh();
	double getC() {return C_;};
	double getQ() {return Q_;};
	void setCQ(double c,double q) {C_=c;Q_=q;};
	void collectStatistics();
	double rhoSqr(int);
	inline double rho(int vi) {return sqrt(rhoSqr(vi));};
};

