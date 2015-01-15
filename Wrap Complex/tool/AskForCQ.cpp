// AskForCQ.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "AskForCQ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AskForCQ dialog


AskForCQ::AskForCQ(CWnd* pParent /*=NULL*/)
	: CDialog(AskForCQ::IDD, pParent)
{
	//{{AFX_DATA_INIT(AskForCQ)
	m_C = 0.0;
	m_Q = 0.0;
	//}}AFX_DATA_INIT
}


void AskForCQ::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AskForCQ)
	DDX_Text(pDX, IDC_ASKFORC, m_C);
	DDX_Text(pDX, IDC_ASKFORQ, m_Q);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AskForCQ, CDialog)
	//{{AFX_MSG_MAP(AskForCQ)
	ON_EN_CHANGE(IDC_ASKFORC, OnChangeAskforc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AskForCQ message handlers

void AskForCQ::OnChangeAskforc() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void AskForCQ::setCQ(double c, double q)
{
	m_C = c;
	m_Q = q;
}
