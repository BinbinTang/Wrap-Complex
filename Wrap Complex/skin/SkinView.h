// SkinView.h : interface of the CSkinView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINVIEW_H__D5F06188_92FA_11D3_9BE3_00500415A6F5__INCLUDED_)
#define AFX_SKINVIEW_H__D5F06188_92FA_11D3_9BE3_00500415A6F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSkinView : public CView
{
protected: // create from serialization only
	CSkinView();
	DECLARE_DYNCREATE(CSkinView)

// Attributes
protected:
	int phase;
	HGLRC m_hrc;
	CPalette* m_pPal ;		
	enum enum_OBJECTS {Box=1, Pyramid=2, Dodec=3} ;		
	enum_OBJECTS m_RotatingObject ;
	GLuint pointsObj; 
	GLuint delTriObj;

	// Set up some colors
	GLdouble red[3];
	GLdouble green[3];
	GLdouble purple[3];
	GLdouble white[3]; 

public:
	CSkinDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void Tick();
	virtual ~CSkinView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CSize m_angle[4];
	BOOL m_bRotate ;
	int mouseDownPosx,mouseDownPosy;
	int mousePosx,mousePosy;
	BOOL lMouseDown;
	GLdouble OldViewingMatrix[16];
	GLdouble NewViewingMatrix[16];
	int WinWidth;
	int WinHeight;
// Generated message map functions
protected:
	void MouseMoveRotate();
	void Rotate(BOOL bRotate);
	void OutputGlError(char* label);
	void PrepareScene();
	void DrawScene();
	//{{AFX_MSG(CSkinView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnRotateStart();
	afx_msg void OnRotateStop();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SkinView.cpp
inline CSkinDoc* CSkinView::GetDocument()
   { return (CSkinDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINVIEW_H__D5F06188_92FA_11D3_9BE3_00500415A6F5__INCLUDED_)
