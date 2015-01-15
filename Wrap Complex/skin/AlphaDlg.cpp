// AlphaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Skin.h"
#include "AlphaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlphaDlg dialog


CAlphaDlg::CAlphaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlphaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlphaDlg)
	m_alphaValue = 0.0;
	//}}AFX_DATA_INIT
}


void CAlphaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlphaDlg)
	DDX_Text(pDX, IDC_ALPHAVALUE, m_alphaValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlphaDlg, CDialog)
	//{{AFX_MSG_MAP(CAlphaDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlphaDlg message handlers

