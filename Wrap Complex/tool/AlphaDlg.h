#if !defined(AFX_ALPHADLG_H__19966D66_DF58_11D3_9C10_00500415A6F5__INCLUDED_)
#define AFX_ALPHADLG_H__19966D66_DF58_11D3_9C10_00500415A6F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlphaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlphaDlg dialog

class CAlphaDlg : public CDialog
{
// Construction
public:
	CAlphaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlphaDlg)
	enum { IDD = IDD_ASKFORALPHA };
	double	m_alphaValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlphaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlphaDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALPHADLG_H__19966D66_DF58_11D3_9C10_00500415A6F5__INCLUDED_)
