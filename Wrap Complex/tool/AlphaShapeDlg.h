#ifndef _DIALOG_ALPHASHAPE_
#define _DIALOG_ALPHASHAPE_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "resource.h"
#include "ChartCtrl.h"
#include "ChartLineSerie.h"

// AlphaShapeDlg dialog
class CAlphaGraphClass{

public:
protected:
};

class CAlphaShapeDlg : public CDialog
{
	//DECLARE_DYNAMIC(AlphaShapeDlg)

public:
	CAlphaShapeDlg(CWnd* pParent = NULL);   // standard constructor
	

// Dialog Data
	enum { IDD = IDD_ALPHASHAPE };
	// array

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	void OnAddSeries();

	CChartAxis* GetSelectedAxis();
	CChartCtrl m_ChartCtrl;
	double* rankZero;

	DECLARE_MESSAGE_MAP()
};


#endif