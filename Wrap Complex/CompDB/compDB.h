// compDB.h: interface for the compDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPDB_H__AE78C6D9_D4E6_11D3_9C0A_00500415A6F5__INCLUDED_)
#define AFX_COMPDB_H__AE78C6D9_D4E6_11D3_9C0A_00500415A6F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "geometry/packedihandler.h"
#include "basic/vectmat.h"
 
class Complex;
class DCompBuilder;
class DComplex;
class RealPArray;
class IndexHandler;
class OrTri;
class Trist;

#define NOTASIMPLEX 0
//----------------------------
//add rank type
//----------------------------
#define MINUSINFI	-6.27743e+065
#define MAXUSINFI	6.27743e+065
//-----end of rank type-------

typedef struct {
    double center[3];
    double size2; // size square
	Vector3 n; // normal, from lower to upper
	Matrix33 m; // alignment of the n to Y-axis
	Matrix33 mi; // inverse of m
    OrTri repTri;
} SimpRecord;

typedef struct {
	int faceIdx;
	int faceType;
	int rankType;
	double rankValue;
} MasterEntry;


#include "basic/iterstack.h"

class CompDB  
{
public:
    CompDB(DCompBuilder*,bool computeAlphaFilter=FALSE);
    Index locateMC(double x[3],Index initalMC);
    Index getIndex(OrTri,Dim);
    
	//modified by xinwei 2003/06/30
	inline DComplex *getComplex() {return dcomp_;};

	//add by xinwei, 2003/08/25
	int getSxIndex(VIndex a, VIndex b);
    int getSxIndex(VIndex a, VIndex b, VIndex c, VIndex d);

	//DComplex *getComplex();

    inline OrTri repTri(Index i,Dim d) {return simpRec_[d][i].repTri;};
    inline OrTri repTri(Index i)       {return simpRec_[i&3][i>>2].repTri;};
    double getSize2(Index);
    inline Vector3 getCenter(OrTri,Dim);
    inline Vector3 getCenter(Index);
	inline bool isOnHull(OrTri ef){return tr_->isOnHull(ef);}; // test if a triangle is on Hull
	Vector3 getNormal(Index mc) {return simpRec_[mc&3][mc>>2].n;};
	Vector3 planeNormal(Index mc1,Index mc2); 
	// plane normal n of the mixed cell wall between mc1 and mc2
	// p in mixex cell mc1 if p dot n < o dot n (o is point on the wall)
	Matrix33 getM(Index mc)  {return simpRec_[mc&3][mc>>2].m;};
	Matrix33 getMI(Index mc)  {return simpRec_[mc&3][mc>>2].mi;};
    inline bool redundent(int i) {return redundent_->test(i);}// vertex redundent?

    inline bool hidden(Index i,Dim d) {return hiddenBV_[d]->test(i);};
    inline bool hidden(Index i) {return hiddenBV_[i&3]->test(i>>2);};
    inline bool hide(Index i,Dim d) {return hideBV_[d]->test(i);};
    inline bool hide(Index i) {return hideBV_[i&3]->test(i>>2);};

    inline bool topoChg(Index i) {return topoChg(i>>2,i&3);};
    bool topoChg(Index i, Dim d);

	//------------------------------------------
	inline double getTopoValue(int idx) {return topoChangeTime_[idx]; };

    Vector3 getVertex(int);
    void    getVertex(EIndex,Vector3&,Vector3&);
    void    getVertex(FIndex,Vector3&,Vector3&,Vector3&);
    void    getVertex(TIndex,Vector3&,Vector3&,Vector3&,Vector3&);
    double  getW(int);
	double MWD(Index mc) {return wallDist_[mc&3][mc>>2];}; // min wall dist
	double MWD2(Index mc) {return wallDist2_[mc&3][mc>>2];};
	double MWD(Index ix,Dim d) {return wallDist_[d][ix];}; // min wall dist
	double MWD2(Index ix,Dim d) {return wallDist2_[d][ix];};
    void computeEdgeCenter(VIndex,VIndex,double [3]);
    void computeFaceCenter(VIndex,VIndex,VIndex,double [3]);
    void computeTetrahedronCenter(VIndex,VIndex,VIndex,VIndex,double [3]);
    bool inMixedCell(double[3],OrTri,Dim,Index excludeMC=0);
    bool inUpperMixedCell(double[3],Index); 
    // for Dim 1: upper means the part of mc nearer to the 'smaller index' vertex
    // for Dim 2: upper means the part on the +ve side of the OrTri with version 0
    int numSimplex(Dim d) {return nSimp_[d];};
    void computeMinWallDistSqr(); // min squared distance from every center to wall of *Mixed Cell* 
    // not V cell or D cell 
    virtual ~CompDB();
    
    inline OrTri enext(OrTri ef) {return tr_->enext(ef);};
    inline OrTri fnext(OrTri ef) {return tr_->fnext(ef);};
    inline OrTri sym(OrTri ef) {return tr_->sym(ef);};
    inline VIndex org(OrTri ef) {return tr_->org(ef);};
    inline VIndex dest(OrTri ef) {return tr_->dest(ef);};
    
    void edgeStar(VIndex,IterStack<OrTri>*);
    
    Trist* getTrist();
    RealPArray* getPArray();
	double minAlpha() {return minAlpha_;};

	void getMCHalfSpaces(Index mc,IterStack<Vector3>*,IterStack<double>*);
	// get the bounding halfspaces of a mixed cell
	void clipPolygon(Index mc,RArray<Vector3>* va,int& vn);

	int getNSimp() {return totalNSimp_;};

	//---------compute alpha value-------------------
	void computeAlpha();
	double getAlpha(int Idx, Dim d);
	double* getRank();
	/*double* getRankZero();
	double* getRankOne();
	double* getRankTwo();
	double* getRankThree();*/
	//void computePocket(double alpha, double belta, TIndex* tetraset);
	TIndex* computePocket(double alpha, double belta);
	TIndex findset(TIndex a);
	//-------end of define computeAlpha()------------
    
protected:
    DComplex* dcomp_;
    RealPArray* rpArray_;
    BitVector* redundent_;
    PackedIndexHandler* ih_;
    BitVector* XTop_[4];
    BitVector* hiddenBV_[3];
    BitVector* hideBV_[4];
	double* wallDist2_[4];
	double* wallDist_[4];
    SimpRecord* simpRec_[4];
    int nSimp_[4];
    Trist* tr_;
    bool computeAlphaFilter_;
    int totalNSimp_;
    double *topoChangeTime_;
	double minAlpha_;
	//-------add rank table-----------
	double* rankTable_[3];
	double *rou_;
	MasterEntry *master_;
	int alphaN;
	int *dp_;
	TIndex *tetraset_;
	int *masterIdx_;
	int *tetraIdx_;
	//------end of add rank table-----
    
    LI wspace_[15];
    
    void computeEdgeCenter_(VIndex,VIndex,double [3],double,double,double); 
    // the true function, assuming the first vertex is not hidden
    bool inVertexMixedCell_(double x[3],OrTri,Index excludeMC=0);
    bool inEdgeMixedCell_(double x[3],OrTri,Index excludeMC=0);
    bool inFaceMixedCell_(double x[3],OrTri,Index excludeMC=0);
    bool inTetraMixedCell_(double x[3],OrTri,Index excludeMC=0);
    void computeSize_();
	void computeNormal_();
    
    void createRecordAndIndices_();
    void computeAlphaShapeFilter_();
    void computeHiddenBV_();
	

    Vector3 planeNormal0to1_(VIndex,EIndex);
    Vector3 planeNormal1to2_(EIndex,FIndex);
    Vector3 planeNormal2to3_(FIndex,TIndex);

	void getMCHalfSpaces0_(OrTri,IterStack<Vector3>*,IterStack<double>*);
	void getMCHalfSpaces1_(OrTri,IterStack<Vector3>*,IterStack<double>*);
	void getMCHalfSpaces2_(OrTri,IterStack<Vector3>*,IterStack<double>*);
	void getMCHalfSpaces3_(OrTri,IterStack<Vector3>*,IterStack<double>*);

	//------------------------------------
	//pockets
	//------------------------------------
	void makeMasterList();
	void getNeighbour_(TIndex idx, Index r[8]);
	void getDp();
	void unionset(TIndex a, TIndex b);
	double det(double *M, int N);
	void sortTetra();
	int findI(double avalue);
	//-------------------------------------

};

inline TIndex CompDB::findset(TIndex a)
{
	if(a>=0 && a<=nSimp_[3])
	{
		while(tetraset_[a]>0 && tetraset_[a]<=numSimplex(3))
			a=tetraset_[a];
		return a;
	}
	else
		return -1;
}

inline void CompDB::unionset(TIndex a, TIndex b)
{
	if(a>=0 && b>=0 && a<=nSimp_[3] && b<=nSimp_[3] && findset(a)!=findset(b))
	{
		if(tetraset_[a]>tetraset_[b])
		{
			tetraset_[b]+=tetraset_[a];
			tetraset_[a]=b;
		}
		else {
			tetraset_[a]+=tetraset_[b];
			tetraset_[b]=a;
		}
	}
}

inline Vector3 CompDB::
getVertex(int i)
{
    Vector3 res;
    double x[4];
    rpArray_->getPoint(i,x);
    res = x;
    return res;
};

inline void CompDB::
getVertex(EIndex i,Vector3& v1,Vector3& v2)
{
    OrTri ef = repTri(i,1);
    v1 = getVertex(org(ef));
    v2 = getVertex(dest(ef));
}

inline void CompDB::
getVertex(FIndex i,Vector3& v1,Vector3& v2,Vector3& v3)
{
    OrTri ef = repTri(i,2);
    v1 = getVertex(org(ef));
    v2 = getVertex(dest(ef));
    v3 = getVertex(dest(enext(ef)));
}

inline void CompDB::
getVertex(TIndex i,Vector3& v1,Vector3& v2,Vector3& v3,Vector3& v4)
{
    OrTri ef = repTri(i,3);
    Assert(!tr_->isOnHull(ef));
    v1 = getVertex(org(ef));
    v2 = getVertex(dest(ef));
    v3 = getVertex(dest(enext(ef)));
    v4 = getVertex(dest(enext(tr_->fnext(ef))));
}

inline double CompDB::
getW(int i)
{
    double x[4];
    rpArray_->getPoint(i,x);
    return x[3];
};


inline Vector3 CompDB::getCenter(Index idx)
{
    OrTri ef = repTri(idx>>2,idx&3);
    return getCenter(ef,idx&3);
}

inline Vector3 CompDB::getCenter(OrTri ef,Dim d)
{
    Vector3 v;
    if(d)
        v = simpRec_[d][getIndex(ef,d)].center;
    else
        v = getVertex(getIndex(ef,d));
    
    return v;
};


#endif // !defined(AFX_COMPDB_H__AE78C6D9_D4E6_11D3_9C0A_00500415A6F5__INCLUDED_)
