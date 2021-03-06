// FormCommandView.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "FormCommandView.h"
#include "ToolDoc.h"
#include "RenderView.h"
#include "AlphaDlg.h"
#include "AskForCQ.h"
#include "AlphaShapeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFormCommandView

IMPLEMENT_DYNCREATE(CFormCommandView, CFormView)

CFormCommandView::CFormCommandView()
: CFormView(CFormCommandView::IDD)
{
	//{{AFX_DATA_INIT(CFormCommandView)
	m_Smooth = FALSE;
	m_Antialias = FALSE;
	m_VRotate = FALSE;
	m_Breath = FALSE;
	//}}AFX_DATA_INIT
}

CFormCommandView::~CFormCommandView()
{
}

void CFormCommandView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFormCommandView)
	DDX_Control(pDX, IDC_SLIDER_SCALE, m_Scale);
	DDX_Control(pDX, IDC_FRAME_COLOR_LIGHT_AMBIENT, m_ControlColorLightAmbient);
	DDX_Control(pDX, IDC_FRAME_COLOR_BACK, m_ControlBackColor);
	DDX_Check(pDX, IDC_CHECK_SMOOTH, m_Smooth);
	DDX_Check(pDX, IDC_CHECK_ANTIALIAS, m_Antialias);
	DDX_Check(pDX, IDC_CHECK_VROTATION, m_VRotate);
	DDX_Check(pDX, IDC_BREATH, m_Breath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFormCommandView, CFormView)
	//{{AFX_MSG_MAP(CFormCommandView)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_CHECK_SMOOTH, OnCheckSmooth)
	ON_BN_CLICKED(IDC_CHECK_VROTATION, OnCheckVrotation)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_ANTIALIAS, OnCheckAntialias)
	ON_BN_CLICKED(IDC_DELTRI, OnDelunay)
	ON_BN_CLICKED(IDC_GENSKIN, OnGenskin)
	ON_BN_CLICKED(IDC_SAVESTL, OnDumpPts)
	ON_BN_CLICKED(IDC_REFINEMESH, OnRefineMesh)
	ON_BN_CLICKED(IDC_CHANGECQ, OnChangecq)
	ON_BN_CLICKED(IDC_CHECK_MESH, OnCheckMesh)
	ON_BN_CLICKED(IDC_GROW_MESH, OnGrowMesh)
	ON_BN_CLICKED(IDC_FLIPALLEDGES, OnFlipalledges)
	ON_BN_CLICKED(IDC_BREATH, OnBreath)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ALPHASHAPE, OnAlphashape)
	ON_BN_CLICKED(IDC_CHANGALPHA, OnChangeAlpha)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFormCommandView diagnostics

#ifdef _DEBUG
void CFormCommandView::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormCommandView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CToolDoc* CFormCommandView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}

#endif //_DEBUG

//********************************************
// OnPaint
//********************************************
void CFormCommandView::OnPaint() 
{
	// Device context for painting
	CPaintDC dc(this); 
	
	// Options are stored in Application
	CToolApp *pApp = (CToolApp *)AfxGetApp();
	CRect rect;

	// Refresh Light0
	float r = (float)GetRValue(pApp->m_OptionColorGlLightAmbient) / 255.0f;
	float g = (float)GetGValue(pApp->m_OptionColorGlLightAmbient) / 255.0f;
	float b = (float)GetBValue(pApp->m_OptionColorGlLightAmbient) / 255.0f;
	float	ambientProperties[]  = {r,g,b,1.0f};
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambientProperties);

	CRenderView *pView = (CRenderView *)GetRenderView();
	pView->m_ClearColorRed   = (float)GetRValue(pApp->m_OptionColorGlBack) / 255.0f;
	pView->m_ClearColorGreen = (float)GetGValue(pApp->m_OptionColorGlBack) / 255.0f;
	pView->m_ClearColorBlue  = (float)GetBValue(pApp->m_OptionColorGlBack) / 255.0f;

	// Color back
	m_ControlBackColor.GetWindowRect(&rect);	
	ScreenToClient(&rect);
	CBrush BrushBack(pApp->m_OptionColorGlBack);
	dc.FillRect(&rect,&BrushBack);
	
	// Color light ambient
	m_ControlColorLightAmbient.GetWindowRect(&rect);	
	ScreenToClient(&rect);
	CBrush BrushLightAmbient(pApp->m_OptionColorGlLightAmbient);
	dc.FillRect(&rect,&BrushLightAmbient);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	this->GetRenderView()->InvalidateRect(NULL,FALSE); 
}

//********************************************
// OnLButtonUp
//********************************************
void CFormCommandView::OnLButtonUp(UINT nFlags, 
																	 CPoint point) 
{
	CRect rect;
	CToolApp *pApp = (CToolApp *)AfxGetApp();
	
	// Option back color
	m_ControlBackColor.GetWindowRect(&rect);	
	ScreenToClient(&rect);
	if(rect.PtInRect(point))
	{
		CColorDialog dlg(pApp->m_OptionColorGlBack);
		if(dlg.DoModal()==IDOK)
		{
			pApp->m_OptionColorGlBack = dlg.GetColor();	
			CRenderView *pView = (CRenderView *)GetRenderView();
			this->InvalidateRect(&rect,FALSE);
			pView->InvalidateRect(NULL,FALSE); 
		}
	}
	
	// Option ambient light color
	m_ControlColorLightAmbient.GetWindowRect(&rect);	
	ScreenToClient(&rect);
	if(rect.PtInRect(point))
	{
		CColorDialog dlg(pApp->m_OptionColorGlLightAmbient);
		if(dlg.DoModal()==IDOK)
		{
			pApp->m_OptionColorGlLightAmbient = dlg.GetColor();	
			CRenderView *pView = (CRenderView *)GetRenderView();

			// Refresh views
			this->InvalidateRect(&rect,FALSE);
			pView->InvalidateRect(NULL,FALSE); 

		}
	}
	
	
		
	CFormView::OnLButtonUp(nFlags, point);
}


//********************************************
// GetRenderView
//********************************************
CView *CFormCommandView::GetRenderView() 
{
	CToolApp *pApp = (CToolApp *)AfxGetApp();
	CMainFrame *pMainFrame = (CMainFrame *)pApp->m_pMainWnd;
	CChildFrame *pFrame = (CChildFrame *)pMainFrame->GetActiveFrame();
	CView *pView = (CView *)pFrame->m_wndSplitter.GetPane(0,1);
	return pView;
}


//********************************************
// OnCheckSmooth
//********************************************
void CFormCommandView::OnCheckSmooth() 
{
	m_Smooth = !m_Smooth;
	if(m_Smooth)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);
	this->GetRenderView()->InvalidateRect(NULL,FALSE); 
}

void CFormCommandView::OnCheckVrotation() 
{
	m_VRotate = !m_VRotate;
	CRenderView *pView = (CRenderView *)GetRenderView();
	if(m_VRotate)
		pView->SetTimer(1,10,NULL);
	else
		pView->KillTimer(1);
}

//********************************************
// OnInitialUpdate
//********************************************
void CFormCommandView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// Slider
	TRACE("Sliders : updating...\n");

	m_Scale.SetRange(1,100,TRUE);
	m_Scale.SetPos(50);
	
}

//********************************************
// OnHScroll
//********************************************
void CFormCommandView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch( nSBCode )
	{
	case TB_BOTTOM:
		UpdateScale();
		break;
	case TB_ENDTRACK:
		UpdateScale();
		break;
	case TB_LINEDOWN:
		UpdateScale();
		break;
	case TB_LINEUP:
		UpdateScale();
		break;
	case TB_PAGEDOWN:
		UpdateScale();
		break;
	case TB_PAGEUP:
		UpdateScale();
		break;
	case TB_THUMBPOSITION:
		UpdateScale();
		break;
	case TB_THUMBTRACK:
		UpdateScale();
		break;
	case TB_TOP:
		UpdateScale();
		break;
	default:
		{}
	}

	// Update scaling
	GetRenderView()->InvalidateRect(NULL,FALSE); 
	
	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}


BOOL CFormCommandView::UpdateScale() 
{
	CRenderView *pView = (CRenderView *)GetRenderView();
	pView->m_Scaling = (float)m_Scale.GetPos()/50.0f;
	return TRUE;
}

//********************************************
// OnCheckAntialias
// Toggle antialiased lines
//********************************************
void CFormCommandView::OnCheckAntialias() 
{
	m_Antialias = !m_Antialias;

	if(m_Antialias)
	{
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		glLineWidth(1.5);
	}
	else
	{
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_BLEND);
		glDisable(GL_POLYGON_SMOOTH);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
		glLineWidth(1.0);
	}
	GetRenderView()->InvalidateRect(NULL,FALSE); 
}

void CFormCommandView::OnDelunay() 
{
	// TODO: Add your control notification handler code here
	CToolApp *pApp = (CToolApp *)AfxGetApp();
	pApp->DoWaitCursor(1);
	GetDocument()->ComputeDelTri();
	GetRenderView()->InvalidateRect(NULL,FALSE); 
}

void CFormCommandView::OnGenskin() 
{
	CAlphaDlg dlg;
	// Initialize dialog data

	// Invoke the dialog box
	if (dlg.DoModal() == IDOK)
	{
		CToolApp *pApp = (CToolApp *)AfxGetApp();
		pApp->DoWaitCursor(1);
		GetDocument()->genSkinMesh(dlg.m_alphaValue);
		GetRenderView()->InvalidateRect(NULL,FALSE); 
	}
}

void CFormCommandView::OnDumpPts() 
{
	CFileDialog saveFileDlg_(FALSE);
	CToolApp *pApp = (CToolApp *)AfxGetApp();
	pApp->DoWaitCursor(1);
	if(saveFileDlg_.DoModal()==IDOK)
		GetDocument()->DumpPoints(LPCTSTR (saveFileDlg_.GetPathName()));	
}

void CFormCommandView::OnRefineMesh() 
{
	CToolApp *pApp = (CToolApp *)AfxGetApp();
	pApp->DoWaitCursor(1);
	GetDocument()->refine();	
	GetRenderView()->InvalidateRect(NULL,FALSE); 
}


void CFormCommandView::OnChangecq() 
{
	AskForCQ dlg;
	// Initialize dialog data 
 
	dlg.setCQ(GetDocument()->getC(),GetDocument()->getQ());

	// Invoke the dialog box
	if (dlg.DoModal() == IDOK)
	{
		CToolApp *pApp = (CToolApp *)AfxGetApp();
		pApp->DoWaitCursor(1);
		GetDocument()->setCQ(dlg.m_C,dlg.m_Q);
		GetDocument()->refine();
	}
	GetRenderView()->InvalidateRect(NULL,FALSE); 
}

void CFormCommandView::OnCheckMesh() 
{
	GetDocument()->toggleSkinMesh();
	GetRenderView()->InvalidateRect(NULL,FALSE); 

}

void CFormCommandView::OnGrowMesh() 
{
	CAlphaDlg dlg;
	// Initialize dialog data

	// Invoke the dialog box
	if (dlg.DoModal() == IDOK)
	{
		CToolApp *pApp = (CToolApp *)AfxGetApp();
		pApp->DoWaitCursor(1);
		GetDocument()->growSkinMesh(dlg.m_alphaValue);
		GetRenderView()->InvalidateRect(NULL,FALSE); 
	}
	
}

void CFormCommandView::OnFlipalledges() 
{
	GetDocument()->flipAllEdges();
	GetRenderView()->InvalidateRect(NULL,FALSE); 
}

void CFormCommandView::OnBreath() 
{
	m_Breath = !m_Breath;
	CRenderView *pView = (CRenderView *)GetRenderView();
	if(m_Breath)
	{
		CAlphaDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			CToolApp *pApp = (CToolApp *)AfxGetApp();
			pApp->DoWaitCursor(1);
			GetDocument()->SetBreathTo(dlg.m_alphaValue);
			pView->SetTimer(2,100,NULL);
		}
	}
	else
		pView->KillTimer(2);
}




void CFormCommandView::OnAlphashape()
{

	CAlphaDlg dlg;
	// Initialize dialog data

	// Invoke the dialog box
	if (dlg.DoModal() == IDOK)
	{
		CToolApp *pApp = (CToolApp *)AfxGetApp();
		pApp->DoWaitCursor(1);
		GetDocument()->ComputeAlphaShape(dlg.m_alphaValue);
		GetRenderView()->InvalidateRect(NULL,FALSE); 
	}
}


void CFormCommandView::OnChangeAlpha()
{
	// TODO: Add your control notification handler code here
	
	AfxEnableControlContainer();
	InitCommonControls();

	CAlphaShapeDlg dlg;

	INITCOMMONCONTROLSEX InitCtrlEx;

	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrlEx.dwICC  = ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&InitCtrlEx); 
	
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		//GetDocument()->ComputeRankZero();
		//GetDocument()->ComputeRankOne();
		//GetDocument()->ComputeRankTwo();
		//GetDocument()->ComputeRankThree();
	}
	else if (nResponse == IDCANCEL)
	{

	}
}
