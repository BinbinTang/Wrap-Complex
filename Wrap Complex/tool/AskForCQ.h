#if !defined(AFX_ASKFORCQ_H__9541C14F_17CC_11D4_9C41_00500415A6F5__INCLUDED_)
#define AFX_ASKFORCQ_H__9541C14F_17CC_11D4_9C41_00500415A6F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AskForCQ.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AskForCQ dialog

class AskForCQ : public CDialog
{
// Construction
public:
	void setCQ(double c,double q);
	AskForCQ(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AskForCQ)
	enum { IDD = IDD_ASKFORCQ };
	double	m_C;
	double	m_Q;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AskForCQ)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AskForCQ)
	afx_msg void OnChangeAskforc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASKFORCQ_H__9541C14F_17CC_11D4_9C41_00500415A6F5__INCLUDED_)
