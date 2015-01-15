// SkinDoc.h : interface of the CSkinDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINDOC_H__D5F06186_92FA_11D3_9BE3_00500415A6F5__INCLUDED_)
#define AFX_SKINDOC_H__D5F06186_92FA_11D3_9BE3_00500415A6F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DCompBuilder;
class RealPArray;
class Trist;
class CompDB;
class OrTri;
class SkinMesh;
class CFileDialog;

#define NODATA 0
#define POINTPHASE 1
#define DELPHASE 2
#define SKINPHASE 3
#include "basic/index.h"

class CSkinDoc : public CDocument
{
protected: // create from serialization only
	CSkinDoc();
	DECLARE_DYNCREATE(CSkinDoc)
	DCompBuilder *dcb_;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	Index getIndex(OrTri,Dim);
	void getBBoxCenter(double*);
	double getAbsMax();
	Trist* getTrist();
	int Phase();
	void computeDelaunay();
	BOOL changeQ();
	RealPArray* getPArray();
	void genMesh(double);
	void drawMesh();
	virtual ~CSkinDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CFileDialog *saveFileDlg_;
	double absMax;
	BOOL changed;
	int phase;
	double BBoxMin[3],BBoxMax[3];
	double BBoxCenter[3];
	CompDB* cdb_;
	SkinMesh* sm_;
// Generated message map functions
protected:
	//{{AFX_MSG(CSkinDoc)
	afx_msg void OnOpDeltri();
	afx_msg void OnUpdateOpDeltri(CCmdUI* pCmdUI);
	afx_msg void OnOpGenskin();
	afx_msg void OnUpdateOpGenskin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOpDumpskinppoints(CCmdUI* pCmdUI);
	afx_msg void OnOpDumpskinppoints();
	afx_msg void OnViewTetrahedra();
	afx_msg void OnUpdateViewTetrahedra(CCmdUI* pCmdUI);
	afx_msg void OnViewEdges();
	afx_msg void OnUpdateViewEdges(CCmdUI* pCmdUI);
	afx_msg void OnViewVertices();
	afx_msg void OnUpdateViewVertices(CCmdUI* pCmdUI);
	afx_msg void OnViewTriangles();
	afx_msg void OnUpdateViewTriangles(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINDOC_H__D5F06186_92FA_11D3_9BE3_00500415A6F5__INCLUDED_)
