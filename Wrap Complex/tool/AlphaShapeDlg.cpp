// AlphaShapeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "AlphaShapeDlg.h"
//#include "afxdialogex.h"


// AlphaShapeDlg dialog

CAlphaShapeDlg::CAlphaShapeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlphaShapeDlg::IDD)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDD_ALPHASHAPE);	
}


void CAlphaShapeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CChartDemoDlg)
    // Add this line with the appropriate ID and variable name
	
	DDX_Control(pDX, IDC_CHARTCTRL, m_ChartCtrl);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAlphaShapeDlg, CDialog)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	//ON_WM_QUERYDRAGICON()
	//{{AFX_MSG_MAP(CAlphaShapeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAlphaShapeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CChartStandardAxis* pBottomAxis = 
		m_ChartCtrl.CreateStandardAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetMinMax(0, 100000);
	CChartStandardAxis* pLeftAxis =
		m_ChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	pLeftAxis->SetMinMax(0, 100);
	CChartStandardAxis* pTopAxis = 
		m_ChartCtrl.CreateStandardAxis(CChartCtrl::TopAxis);
	pTopAxis->SetMinMax(0, 100000);
	CChartStandardAxis* pRightAxis =
		m_ChartCtrl.CreateStandardAxis(CChartCtrl::RightAxis);
	pRightAxis->SetMinMax(0, 100);

	
	return TRUE;
}

void CAlphaShapeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
		CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAlphaShapeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CAlphaShapeDlg::OnAddSeries()
{

}