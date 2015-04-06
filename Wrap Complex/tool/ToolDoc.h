//********************************************
// ToolDoc.h
// The document, contain a SceneGraph
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 12/12/97
// Modified
//********************************************


#if !defined(AFX_TOOLDOC_H__06A35631_72E5_11D1_A6C2_00A0242C0A32__INCLUDED_)
#define AFX_TOOLDOC_H__06A35631_72E5_11D1_A6C2_00A0242C0A32__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSeneGraph3d; 

#define NODATA 0
#define POINTPHASE 1
#define DELPHASE 2
#define SKINPHASE 3
#define ALPHAPHASE 4

#include "delone/dcbuilder.h"
#include "CompDB/compDB.h"
#include "CompDB/SkinMesh.h"
class CToolDoc : public CDocument
{
protected: // create from serialization only
	CToolDoc();
	DECLARE_DYNCREATE(CToolDoc)

// Attributes
private:
	//CSeneGraph3d m_SceneGraph;

// Functions
public:

	// OpenGL
	void RenderScene();


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	void growSkinMesh(double);
	void toggleSkinMesh();
	void flipAllEdges();
	virtual void genSkinMesh(double);
	virtual double getC() {return sm_->getC();};
	virtual double getQ() {return sm_->getQ();};
	virtual void setCQ(double c,double q) {sm_->setCQ(c,q);};
	virtual void ComputeDelTri();
	virtual void ComputeAlphaShape(double);
	virtual void DumpPoints(const char*);
	virtual void refine();
	virtual int Phase();
	virtual void BreathMotion();
	virtual void SetBreathTo(double);
	double* ComputeRankZero();
	double* ComputeRankOne();
	double* ComputeRankTwo();
	double* ComputeRankThree();
	virtual ~CToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DCompBuilder* dcb_;
	DCompBuilder* dcb_alpha;
	double absMax_;
	double alpha1_,alpha2_;
	int motion_;
	double motionIntv_;
	double BBoxMin_[3],BBoxMax_[3];
	double BBoxCenter_[3];
	Bool drawMesh_;
	int phase_;
	CompDB* cdb_;
	SkinMesh* sm_;
	double alpha_;
// Generated message map functions
protected:
	//{{AFX_MSG(CToolDoc)
	afx_msg void OnViewStatistics();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void drawAlphaShape(double alpha);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLDOC_H__06A35631_72E5_11D1_A6C2_00A0242C0A32__INCLUDED_)
