// SkinDoc.cpp : implementation of the CSkinDoc class
//

#include "stdafx.h"
#include "Skin.h"
#include "delone/dcbuilder.h"
#include "compDB/compDB.h"
#include "SkinDoc.h"
#include "CompDB/SkinMesh.h"
#include "AlphaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinDoc

IMPLEMENT_DYNCREATE(CSkinDoc, CDocument)

BEGIN_MESSAGE_MAP(CSkinDoc, CDocument)
	//{{AFX_MSG_MAP(CSkinDoc)
	ON_COMMAND(ID_OP_DELTRI, OnOpDeltri)
	ON_UPDATE_COMMAND_UI(ID_OP_DELTRI, OnUpdateOpDeltri)
	ON_COMMAND(ID_OP_GENSKIN, OnOpGenskin)
	ON_UPDATE_COMMAND_UI(ID_OP_GENSKIN, OnUpdateOpGenskin)
	ON_UPDATE_COMMAND_UI(ID_OP_DUMPSKINPPOINTS, OnUpdateOpDumpskinppoints)
	ON_COMMAND(ID_OP_DUMPSKINPPOINTS, OnOpDumpskinppoints)
	ON_COMMAND(ID_VIEW_TETRAHEDRA, OnViewTetrahedra)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TETRAHEDRA, OnUpdateViewTetrahedra)
	ON_COMMAND(ID_VIEW_EDGES, OnViewEdges)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EDGES, OnUpdateViewEdges)
	ON_COMMAND(ID_VIEW_VERTICES, OnViewVertices)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VERTICES, OnUpdateViewVertices)
	ON_COMMAND(ID_VIEW_TRIANGLES, OnViewTriangles)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRIANGLES, OnUpdateViewTriangles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinDoc construction/destruction

CSkinDoc::CSkinDoc()
{
	// TODO: add one-time construction code here
	dcb_= NULL;
	phase = NODATA;
	changed=TRUE;
	cdb_ = NULL;
	sm_ = NULL;
	saveFileDlg_= new CFileDialog(FALSE);
}

CSkinDoc::~CSkinDoc()
{
	if(dcb_)
		delete dcb_;
	if(cdb_)
		delete cdb_;
	if(sm_)
		delete sm_;
	delete saveFileDlg_;
}

BOOL CSkinDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSkinDoc serialization

void CSkinDoc::Serialize(CArchive& ar)
{
	char s[200];
	double x[4];

	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		if(dcb_)
			delete dcb_;
		if(cdb_)
			delete cdb_;
		if(sm_)
			delete sm_;
		sm_= NULL;
		cdb_=NULL;

		dcb_= new DCompBuilder;
		while(ar.ReadString(s,200))
		{
			sscanf(s," %lf %lf %lf %lf",&(x[0]),&(x[1]),&(x[2]),&x[3]);
			dcb_->addPoint(x);
		}
//		dcb_->getPArray()->removeDuplicates();
		changed=TRUE;
//		absMax = dcb_->getPArray()->getAbsMax();
		phase = POINTPHASE;
		dcb_->getPArray()->getBBox(BBoxMin,BBoxMax);
		absMax = 0;
		for(int i=0;i<3;i++)
		{
			BBoxCenter[i] = (BBoxMin[i]+BBoxMax[i])/2;
			absMax = (absMax>fabs(BBoxMax[i]-BBoxMin[i]))?absMax:fabs(BBoxMax[i]-BBoxMin[i]);
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
// CSkinDoc diagnostics

#ifdef _DEBUG
void CSkinDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSkinDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSkinDoc commands

RealPArray* CSkinDoc::getPArray()
{
	return dcb_ ? dcb_->getPArray() : cdb_->getPArray();
}

BOOL CSkinDoc::changeQ()
{
	if(changed)
	{
		changed=FALSE;
		return TRUE;
	}
	return FALSE;
}

void CSkinDoc::computeDelaunay()
{
	time_t start,finish;
	char sec[100];
	time(&start);
	dcb_->compute();
	time(&finish);
	sprintf(sec,"Time used for computing Delaunay Triangulation: %10.2f seconds\n",
		difftime(finish,start));
//    MessageBox(NULL,sec,"",MB_OK);
	dcb_->cleanTriangulation();
	if(cdb_)
		delete cdb_;
	cdb_ = new CompDB(dcb_);
	sm_ = new SkinMesh(cdb_);
//	delete dcb_;
//	dcb_=NULL;
	changed=TRUE;
}

int CSkinDoc::Phase()
{
	return phase;
}

Trist* CSkinDoc::getTrist()
{
	return cdb_->getTrist();
}

double CSkinDoc::getAbsMax()
{
	return absMax;
}

void CSkinDoc::getBBoxCenter(double * center)
{
	center[0] = BBoxCenter[0];
	center[1] = BBoxCenter[1];
	center[2] = BBoxCenter[2];
}

Index CSkinDoc::getIndex(OrTri ef,Dim d)
{
	return cdb_->getIndex(ef,d);
}

void CSkinDoc::genMesh(double alpha)
{
	sm_->genMesh(alpha);
	phase=SKINPHASE;
}

void CSkinDoc::drawMesh()
{
	sm_->draw();
}


void CSkinDoc::OnOpDeltri() 
{
	computeDelaunay();	
	phase=DELPHASE;
}

void CSkinDoc::OnUpdateOpDeltri(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(phase==POINTPHASE);
}

void CSkinDoc::OnOpGenskin() 
{
	CAlphaDlg dlg;
	// Initialize dialog data

	// Invoke the dialog box
	if (dlg.DoModal() == IDOK)
	{
		genMesh(dlg.m_alphaValue);
	}
}

void CSkinDoc::OnUpdateOpGenskin(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((phase==DELPHASE)||(phase==SKINPHASE));
}

void CSkinDoc::OnUpdateOpDumpskinppoints(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(phase==SKINPHASE);

}

void CSkinDoc::OnOpDumpskinppoints() 
{
	if(saveFileDlg_->DoModal()==IDOK)
		sm_->dumpPoints(LPCTSTR (saveFileDlg_->GetPathName()));
}

void CSkinDoc::OnViewTetrahedra() 
{
	if(sm_)
		sm_->toggleVisible(3);
}

void CSkinDoc::OnUpdateViewTetrahedra(CCmdUI* pCmdUI) 
{
	if(sm_)
		pCmdUI->SetCheck(sm_->visible(3));
}

void CSkinDoc::OnViewEdges() 
{
	if(sm_)
		sm_->toggleVisible(1);
	
}

void CSkinDoc::OnUpdateViewEdges(CCmdUI* pCmdUI) 
{
	if(sm_)
		pCmdUI->SetCheck(sm_->visible(1));	
}

void CSkinDoc::OnViewVertices() 
{
	if(sm_)
		sm_->toggleVisible(0);
}

void CSkinDoc::OnUpdateViewVertices(CCmdUI* pCmdUI) 
{
	if(sm_)
		pCmdUI->SetCheck(sm_->visible(0));
}

void CSkinDoc::OnViewTriangles() 
{
	if(sm_)
		sm_->toggleVisible(2);
}

void CSkinDoc::OnUpdateViewTriangles(CCmdUI* pCmdUI) 
{
	if(sm_)
		pCmdUI->SetCheck(sm_->visible(2));	
}
