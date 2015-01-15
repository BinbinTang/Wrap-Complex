//********************************************
// ToolDoc.cpp
//********************************************
// The active document, contain a SceneGraph
//********************************************
// pierre.alliez@cnet.francetelecom.fr
// Created : 12/12/97
// Modified
//********************************************


#include "stdafx.h"
#include "math.h"
#include "Tool.h"

#include "ToolDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "geometry/trist.h"
#include "basic/vectmat.h"

#define MOTION_STOP 0
#define MOTION_UP 1
#define MOTION_DOWN 2

/////////////////////////////////////////////////////////////////////////////
// CToolDoc

IMPLEMENT_DYNCREATE(CToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CToolDoc, CDocument)
//{{AFX_MSG_MAP(CToolDoc)
	ON_COMMAND(ID_VIEW_STATISTICS, OnViewStatistics)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolDoc construction/destruction

//********************************************
// Constructor
//********************************************
CToolDoc::CToolDoc()
{
	dcb_ = NULL;
	cdb_ = NULL;
	sm_  = NULL;
	phase_ = NODATA;
	drawMesh_ = FALSE;	
}

//********************************************
// Destructor
//********************************************
CToolDoc::~CToolDoc()
{
	if(dcb_) 
		delete dcb_;
	if(sm_)
		delete sm_;
	if(cdb_)
		delete cdb_;
		
}

BOOL CToolDoc::OnNewDocument()
{


	return FALSE; // there is no 'new' blank document

	if (!CDocument::OnNewDocument())
		return FALSE;
	
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CToolDoc serialization

void CToolDoc::Serialize(CArchive& ar)
{
	char s[1000];
	double x[4];
	
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
		if(dcb_)
			delete dcb_;
		
		dcb_= new DCompBuilder;
		while(ar.ReadString(s,200))
		{
			sscanf(s," %lf %lf %lf %lf",&(x[0]),&(x[1]),&(x[2]),&x[3]);
//			x[3] *= sqrt(2);
			dcb_->addPoint(x);
		}
		phase_ = POINTPHASE;
		dcb_->getPArray()->getBBox(BBoxMin_,BBoxMax_);
		absMax_ = 0;
		for(int i=0;i<3;i++)
		{
			BBoxCenter_[i] = (BBoxMin_[i]+BBoxMax_[i])/2;
			absMax_ = max(absMax_,fabs(BBoxMax_[i]-BBoxMin_[i]));
		}
		UpdateAllViews(NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CToolDoc diagnostics

#ifdef _DEBUG
void CToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CToolDoc commands


//////////////////////////////////////////////
//////////////////////////////////////////////
// RENDERING
//////////////////////////////////////////////
//////////////////////////////////////////////

//***********************************************
// RenderScene
//***********************************************
void CToolDoc::RenderScene()
{
	// Default rendering 
	
	int i;
	double x[4];
//	char str[255];
	double x1[4],x2[4],x3[4];
/*	
	glColor3ub(255,255,255);
	GLUquadricObj*	q = gluNewQuadric();
	gluQuadricDrawStyle(q, GLU_FILL);
	gluQuadricNormals(q, GLU_SMOOTH);
	gluSphere(q, 1.0, 16, 16);
	gluDeleteQuadric(q);
*/	
	glScaled(2/absMax_,2/absMax_,2/absMax_);
	glTranslated(-BBoxCenter_[0],-BBoxCenter_[1],-BBoxCenter_[2]);
	if(phase_==POINTPHASE)
	{
		glDisable(GL_LIGHTING);
		int npt = dcb_->getPArray()->size();
		
		glPointSize(5);
		glColor3d(0,0.4,0);
		glBegin(GL_POINTS);
		for(i=1;i<=npt;i++)
		{
			dcb_->getPArray()->getPoint(i,x);
			glVertex3dv(x);
		}
		glEnd();
/*
		glListBase(1000); 
		for(i=1;i<=npt;i++)
		{
			dcb_->getPArray()->getPoint(i,x);
			glRasterPos3d(x[0],x[1],x[2]);
			sprintf(str,"%d",i);
			glCallLists(strlen(str), GL_UNSIGNED_BYTE, str); 
		}
		glPointSize(1);
*/
		glEnable(GL_LIGHTING);
	}

	if(phase_==DELPHASE)
	{
		Trist* tt=dcb_->getTrist();
		TristHullIter thi(*tt);
//		TristIter ti(*tt);
		for(thi.first();!thi.isDone();thi.next())
//		for(ti.first();!ti.isDone();ti.next())
		{
			OrTri tri = thi.curOrTri();
//			OrTri tri(ti.curItem(),0);
//			glColor3dv(green);
			glBegin(GL_POLYGON);
				dcb_->getPArray()->getPoint(tt->org(tri),x1);
				dcb_->getPArray()->getPoint(tt->dest(tt->enext(tri)),x3);
				dcb_->getPArray()->getPoint(tt->dest(tri),x2);
				glNormal3dv(cross_prod(Vector3(x2)-Vector3(x1),Vector3(x3)-Vector3(x1)).x);
				glVertex3dv(x1);
				glVertex3dv(x2);
				glVertex3dv(x3);
			glEnd();
			
//		  glVertex3dv(x1);
		}
	}
	if(phase_==SKINPHASE)
	{
		sm_->draw();
		if(drawMesh_)
			sm_->drawMesh(absMax_/1000);
	}
	if (phase_==ALPHAPHASE)
	{
	}
}

int CToolDoc::Phase()
{
	return phase_;
}

void CToolDoc::ComputeDelTri()
{
	if(phase_==POINTPHASE)
	{
		dcb_->compute();
		dcb_->cleanTriangulation();
		phase_=DELPHASE;
		

		if(cdb_)
			delete cdb_;
		cdb_ = new CompDB(dcb_,true);
		sm_ = new SkinMesh(cdb_);
		UpdateAllViews(NULL);
	}
}

void CToolDoc::genSkinMesh(double alpha)
{
	if(phase_==POINTPHASE)
		ComputeDelTri();
	alpha_ = alpha;
	sm_->genMesh(alpha_);
	phase_=SKINPHASE;
	UpdateAllViews(NULL);

}

void CToolDoc::DumpPoints(const char* fn)
{
	sm_->dumpPoints(fn);
}

void CToolDoc::refine()
{
	if(!sm_->refineMesh())
		MessageBox(NULL,"Nothing changed anymore","",MB_OK);
	UpdateAllViews(NULL);
}

void CToolDoc::toggleSkinMesh()
{
	drawMesh_ = !drawMesh_;
}

void CToolDoc::growSkinMesh(double alpha)
{
	sm_->finalUpdate(alpha);
}

void CToolDoc::flipAllEdges()
{
	sm_->flipAllEdges();
}

void CToolDoc::SetBreathTo(double a)
{
	alpha1_ = min(sm_->getCurrentAlpha(),a);
	alpha2_ = max(sm_->getCurrentAlpha(),a);
//	Assert(alpha2_>alpha1_);
	motion_ = MOTION_UP;
	motionIntv_ = fabs(alpha2_-alpha1_)/50.0;
}

void CToolDoc::BreathMotion()
{
	if(sm_->getCurrentAlpha()>=alpha2_)
		motion_=MOTION_DOWN;
	if(sm_->getCurrentAlpha()<=alpha1_)
		motion_=MOTION_UP;

	if(motion_==MOTION_UP)
		growSkinMesh(sm_->getCurrentAlpha()+motionIntv_);

	if(motion_==MOTION_DOWN)
		growSkinMesh(sm_->getCurrentAlpha()-motionIntv_);

}

void CToolDoc::OnViewStatistics() 
{
	sm_->collectStatistics();
}
