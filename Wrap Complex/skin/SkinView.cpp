// SkinView.cpp : implementation of the CSkinView class
//

#include "stdafx.h"
#include "Skin.h"
#include "math.h"
#include "basic/points.h"
#include "SkinDoc.h"
#include "SkinView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "geometry\trist.h"
#include "basic/vectmat.h"
/////////////////////////////////////////////////////////////////////////////
// CSkinView

IMPLEMENT_DYNCREATE(CSkinView, CView)

BEGIN_MESSAGE_MAP(CSkinView, CView)
//{{AFX_MSG_MAP(CSkinView)
ON_WM_SIZE()
ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_COMMAND(ID_ROTATE_START, OnRotateStart)
	ON_COMMAND(ID_ROTATE_STOP, OnRotateStop)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
// Standard printing commands
ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinView construction/destruction

HPALETTE hPalette;
void setupPalette(HDC hDC);

CSkinView::CSkinView()
{
	m_pPal = NULL ;

	m_bRotate = FALSE ;
	m_RotatingObject = Box ;

	m_angle[Box].cx = 30 ;
	m_angle[Box].cy = 70 ;
	m_angle[Pyramid].cx = 150 ;
	m_angle[Pyramid].cy = 120 ;
	m_angle[Dodec].cx = 30 ;
	m_angle[Dodec].cy = 70 ;	

	// Set up some colors
	red[0] = 0.8; red[1] = 0.0; red[2] = 0.0;
	green[0] = 0.0; green[1] = 0.75; green[2] = 0.0;
	purple[0] = 1.0; purple[2] = 0.14; purple[2] = 0.66667;
	white[0] = 1; white[1] = 1; white[2] = 1;

	lMouseDown=FALSE;
}

CSkinView::~CSkinView()
{
	if(glIsList(pointsObj))
		glDeleteLists(pointsObj,1);
	if(glIsList(delTriObj))
		glDeleteLists(delTriObj,1);
	if (m_pPal) delete m_pPal ;
}

BOOL CSkinView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN ;
	
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSkinView drawing

void CSkinView::OnDraw(CDC* pDC)
{
	CSkinDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	PrepareScene();


	// Make the HGLRC current.
	BOOL bResult = wglMakeCurrent (pDC->m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() ) ;
	}
	DrawScene() ;
/*
    // Select the palette.
    CPalette* ppalOld = pDC->SelectPalette(m_pPal, 0);
    pDC->RealizePalette();
*/
	
	// Draw.
	
	// Swap buffers.
	SwapBuffers(pDC->m_hDC) ;
	
	// Select old palette. 
//	if (ppalOld) pDC->SelectPalette(ppalOld, 0); 
	
	wglMakeCurrent(NULL, NULL) ;
}

/////////////////////////////////////////////////////////////////////////////
// CSkinView printing

BOOL CSkinView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSkinView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSkinView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSkinView diagnostics

#ifdef _DEBUG
void CSkinView::AssertValid() const
{
	CView::AssertValid();
}

void CSkinView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSkinDoc* CSkinView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSkinDoc)));
	return (CSkinDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSkinView message handlers

void CSkinView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
	CClientDC dc(this) ;


	BOOL bResult = wglMakeCurrent (dc.m_hDC, m_hrc);
	if (!bResult)
	{
		TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() ) ;
		return ;
	}
	
	WinWidth=cx;
	WinHeight=cy;
	
	GLdouble gldAspect = (GLdouble) cx/ (GLdouble) cy;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(20.0, gldAspect, 1.0, 10.0);
	glViewport(0, 0, cx, cy);
	
	wglMakeCurrent(NULL, NULL);
	
}
 
int CSkinView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	 
	CClientDC dc(this) ;
    LOGPALETTE* pPal;
    int paletteSize;

	//
	// Fill in the pixel format descriptor.
	//
/*
	static  PIXELFORMATDESCRIPTOR pfd ;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)) ;
	pfd.nSize	  = sizeof(PIXELFORMATDESCRIPTOR); 
	pfd.nVersion   = 1 ; 
	pfd.dwFlags	  = PFD_DOUBLEBUFFER |
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW; 
	pfd.iPixelType = PFD_TYPE_RGBA ;
	pfd.cColorBits = 24 ;
	pfd.cDepthBits = 32 ;
	pfd.iLayerType = PFD_MAIN_PLANE ;
*/ 
/*
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                              // version number
		PFD_DRAW_TO_WINDOW |            // support window
		  PFD_SUPPORT_OPENGL |          // support OpenGL
		  PFD_DOUBLEBUFFER,             // double buffered
		PFD_TYPE_RGBA,                  // RGBA type
		32,                             // 24-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		0,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		32,                             // 32-bit z-buffer
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};

	
	int nPixelFormat = ChoosePixelFormat(dc.m_hDC, &pfd);
	if (nPixelFormat == 0)
	{
		TRACE("ChoosePixelFormat Failed %d\r\n",GetLastError()) ;
		return -1 ;
	}
	TRACE("Pixel Format %d\r\n",nPixelFormat) ;
	
	
	
	BOOL bResult = SetPixelFormat (dc.m_hDC, nPixelFormat, &pfd);
	if (!bResult)
	{
		TRACE("SetPixelFormat Failed %d\r\n",GetLastError()) ;
		return -1 ;
	}
*/
	PIXELFORMATDESCRIPTOR pfd ;
	int pf;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cDepthBits   = 32;
    pfd.cColorBits   = 32;

    pf = ChoosePixelFormat(dc.m_hDC, &pfd);
    if (pf == 0) {
		TRACE("ChoosePixelFormat Failed %d\r\n",GetLastError()) ;
		return -1 ;
/*
        MessageBox(NULL, "ChoosePixelFormat() failed:  "
                   "Cannot find a suitable pixel format.", "Error", MB_OK); 
        return 0;*/
    } 
 
    if (SetPixelFormat(dc.m_hDC, pf, &pfd) == FALSE) {
		TRACE("SetPixelFormat Failed %d\r\n",GetLastError()) ;
		return -1 ;
/*
        MessageBox(NULL, "SetPixelFormat() failed:  "
                   "Cannot set format specified.", "Error", MB_OK);
        return 0;*/
    } 
	
	m_hrc = wglCreateContext(dc.m_hDC);
	wglMakeCurrent (dc.m_hDC, m_hrc); 
 
//    wglRealizeLayerPalette(dc.m_hDC, 1, FALSE);	
//	SelectObject (dc.m_hDC, GetStockObject (SYSTEM_FONT)); 
//	wglUseFontBitmaps(dc.m_hDC, 0, 256, 1000); 
	if (!m_hrc)
	{
		TRACE("wglCreateContext Failed %x\r\n", GetLastError()) ;
		return -1;
	}
	
/*
	float sp[]={1,0,0,1};
	float shin[]={50.0};
	float lp[]={1000000000000,1000000000000,-10000000000,0};
	float lc[]={0,1,0,1};
	glShadeModel(GL_FLAT);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
//	glMaterialfv(GL_FRONT,GL_SPECULAR,sp);
//	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shin);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,sp); 
//	glLightfv(GL_LIGHT0,GL_DIFFUSE,lc);
	glLightfv(GL_LIGHT0,GL_POSITION,lp);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
*/
/*
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
*/
	wglMakeCurrent(NULL, NULL) ;

}


BOOL CSkinView::OnEraseBkgnd(CDC* pDC) 
{
	/*
	// TODO: Add your message handler code here and/or call default
	
	return CView::OnEraseBkgnd(pDC);
	*/
	return TRUE;
}

void CSkinView::OnInitialUpdate() 
{

	TRACE0("OnInitialUpdate\r\n") ;	
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			OldViewingMatrix[i*4+j]= i==j?1:0;
	PrepareScene() ;		


	CView::OnInitialUpdate();	
}

void CSkinView::DrawScene()
{
	GLfloat mat_spec[] = {1,1,1,1};
	GLfloat mat_shin[] = {50};
	GLfloat light_pos[] = {1.0,1.0,1.0,0.0};

	glMaterialfv(GL_FRONT,GL_SPECULAR, mat_spec);
	glMaterialfv(GL_FRONT,GL_SHININESS, mat_shin);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

/*
	GLfloat no_mat[] = {0.0,0.0,1.0,1.0};
	GLfloat mat_ambient[] = {0.7,0.7,0.7,1.0};
	GLfloat mat_ambient_color[] = {0.8,0.8,0.2,1.0};
	GLfloat mat_diffuse[] = {0.1,0.5,0.8,1.0};
	GLfloat mat_specular[] = {1.0,1.0,1.0,1.0};
	GLfloat no_shininess[] = {0.0};
	GLfloat low_shininess[] = {5.0};
	GLfloat high_shininess[] = {100.0};
	GLfloat light_position[] = {1.0,1.0,1.0,0.0};

//	glShadeModel(GL_FLAT);
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,low_shininess);
//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE) ;
//	glColor3f(1.0,1.0,1.0);

	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,TRUE);

	// Enable lighting calculations
	glEnable(GL_LIGHTING) ; 		OutputGlError("glEnable (GL_LIGHTING) ;") ;
	glEnable(GL_LIGHT0) ;			OutputGlError("glEnable (GL_LIGHT0) ;") ;
	glEnable(GL_COLOR_MATERIAL) ;   OutputGlError("glEnable (GL_COLOR_MATERIAL) ;") ;
*/	
	// Enable depth calculations
    glEnable(GL_DEPTH_TEST);  		OutputGlError("glEnable (GL_DEPTH_TEST);") ;

	// Clear the color and depth buffers
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f) ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the material color to follow the current color
//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE) ;
//	glEnable(GL_COLOR_MATERIAL) ;
//	glColor3f(1.0,1.0,1.0);
    //
    // Draw the box.
    //		
    glMatrixMode(GL_MODELVIEW); 	OutputGlError("MatrixMode") ;
	
	// Translate and rotate the axis.
	glLoadIdentity();

	glPushMatrix();
	if(lMouseDown)
	{
		glRotated((180*(mousePosx-mouseDownPosx))/WinWidth,0,1,0);
		glRotated((180*(mousePosy-mouseDownPosy))/WinWidth,1,0,0);
	}
	glMultMatrixd(OldViewingMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX,NewViewingMatrix);
	glColor3dv(white);

	glPopMatrix();
	glTranslated(0,0,-3.5);
	glMultMatrixd(NewViewingMatrix);

//	glPointSize(5);

	if(GetDocument()->Phase()!=NODATA)
	{
		double center[3];
		GetDocument()->getBBoxCenter(center);
		double am = GetDocument()->getAbsMax();
		glScaled(1/am,1/am,1/am);
		glTranslated(-center[0],-center[1],-center[2]);

		if(glIsList(pointsObj)&&GetDocument()->Phase()==POINTPHASE)
			glCallList(pointsObj);

		if(glIsList(delTriObj)&&GetDocument()->Phase()==DELPHASE)
		{
			glCallList(delTriObj);
			glColor3dv(white);
		}
		if(glIsList(delTriObj)&&GetDocument()->Phase()==SKINPHASE)
		{
			GetDocument()->drawMesh();
		}

	}
	glTranslated(0,0,3.5);

    //
    // Flush the drawing pipeline.
    //

    glFlush();	

}

void CSkinView::PrepareScene()
{
	int i;
	double x[4],x1[4],x2[4],x3[4];
	char str[100];
	int v1,v2;

	// Faster than using glScale.
	GLdouble size = 0.5 ;
	GLdouble dsize = 0.35 ; // Size for Dodecadedron

	if(GetDocument()->changeQ())
	{
		CClientDC dc(this) ;

		BOOL bResult = wglMakeCurrent(dc.m_hDC, m_hrc);	 

		if (!bResult)
		{
			TRACE("wglMakeCurrent Failed %x\r\n", GetLastError() ) ;
		}

		pointsObj=4;
		if(GetDocument()->Phase()==POINTPHASE)
		{

			int npt = GetDocument()->getPArray()->size();
			if(glIsList(pointsObj))
				glDeleteLists(pointsObj,1);
			if(GetDocument()->Phase()==POINTPHASE)
			{
				glNewList(pointsObj, GL_COMPILE) ; 
				  glPointSize(5);
				  glColor3dv(white);
				  glBegin(GL_POINTS);
				  for(i=1;i<=npt;i++)
				  {
					  GetDocument()->getPArray()->getPoint(i,x);
					  glVertex3dv(x);

				  }
				  glEnd();
				  glListBase(1000); 
				  for(i=1;i<=npt;i++)
				  {
					  GetDocument()->getPArray()->getPoint(i,x);
					  glRasterPos3d(x[0],x[1],x[2]);
					  sprintf(str,"%d",i);
					  glCallLists(strlen(str), GL_UNSIGNED_BYTE, str); 
				  }
				  glPointSize(1);
				glEndList();
			}
		}

		delTriObj=5;

		if(glIsList(delTriObj))
			glDeleteLists(delTriObj,1);
		if(GetDocument()->Phase()==DELPHASE)
		{
			Trist* tt=GetDocument()->getTrist();
			TristHullIter thi(*tt);
//			TristIter ti(*tt);
			glNewList(delTriObj, GL_COMPILE);
			for(thi.first();!thi.isDone();thi.next())
//			for(ti.first();!ti.isDone();ti.next())
			{
				OrTri tri = thi.curOrTri();
//				OrTri tri(ti.curItem(),0);
//				glColor3dv(green);
//				glBegin(GL_LINE_STRIP);
				glBegin(GL_POLYGON);
				  GetDocument()->getPArray()->getPoint(v1=tt->org(tri),x1);
				  GetDocument()->getPArray()->getPoint(tt->dest(tt->enext(tri)),x3);
				  GetDocument()->getPArray()->getPoint(v2=tt->dest(tri),x2);
				  glNormal3dv(cross_prod(Vector3(x2)-Vector3(x1),Vector3(x3)-Vector3(x1)).x);
				  glVertex3dv(x1);
				  glVertex3dv(x2);
				  glVertex3dv(x3);
//				  glVertex3dv(x1);
				glEnd();
			}
/*
			for(ti.first();!ti.isDone();ti.next())
			{
//				OrTri tri = thi.curOrTri();
				OrTri tri(ti.curItem(),0);
				VIndex a,b,c,d;
				double xt[3];
				glColor3dv(green);
				GetDocument()->getPArray()->getPoint(v1=tt->org(tri),x1);
				GetDocument()->getPArray()->getPoint(v2=tt->dest(tri),x2);
				glRasterPos3d((x1[0]+x2[0])/2,(x1[1]+x2[1])/2,(x1[2]+x2[2])/2);

				xt[0]=x1[0]+x2[0];
				xt[1]=x1[1]+x2[1];
				xt[2]=x1[2]+x2[2];

				sprintf(str,"%d",GetDocument()->getIndex(tri,1));
				glCallLists(strlen(str), GL_UNSIGNED_BYTE, str); 
				GetDocument()->getPArray()->getPoint(v1=tt->dest(tt->enext(tri)),x1);
				glRasterPos3d((x1[0]+x2[0])/2,(x1[1]+x2[1])/2,(x1[2]+x2[2])/2);
				sprintf(str,"%d",GetDocument()->getIndex(tt->enext(tri),1));
				glCallLists(strlen(str), GL_UNSIGNED_BYTE, str); 

				xt[0] += x1[0];
				xt[1] += x1[1];
				xt[2] += x1[2];

				GetDocument()->getPArray()->getPoint(v2=tt->org(tri),x2);
				glRasterPos3d((x1[0]+x2[0])/2,(x1[1]+x2[1])/2,(x1[2]+x2[2])/2);
				sprintf(str,"%d",GetDocument()->getIndex(tt->enext2(tri),1));
				glCallLists(strlen(str), GL_UNSIGNED_BYTE, str); 

				glColor3dv(purple);

				tt->tetra(tri,a,b,c,d);
				if(d)
				{
					GetDocument()->getPArray()->getPoint(d,x1);
					glRasterPos3d((xt[0]+x1[0])/4,(xt[1]+x1[1])/4,(xt[2]+x1[2])/4);
					sprintf(str,"%d",GetDocument()->getIndex(tri,3));
					glCallLists(strlen(str), GL_UNSIGNED_BYTE, str); 
				}
				tt->tetra(tt->sym(tri),a,b,c,d);
				if(d)
				{
					GetDocument()->getPArray()->getPoint(d,x1);
					glRasterPos3d((xt[0]+x1[0])/4,(xt[1]+x1[1])/4,(xt[2]+x1[2])/4);
					sprintf(str,"%d",GetDocument()->getIndex(tt->sym(tri),3));
					glCallLists(strlen(str), GL_UNSIGNED_BYTE, str); 
				}


				glEnd();
			}
*/
			glEndList();
		}
		

		glFinish();
		wglMakeCurrent(NULL, NULL) ;

	}


}

void CSkinView::OutputGlError(char *label)
{
	GLenum errorno = glGetError() ;
	if (errorno != GL_NO_ERROR)
		TRACE("%s had error: #(%d) %s\r\n", label, errorno, gluErrorString(errorno)) ;

}


void CSkinView::OnDestroy() 
{
	CView::OnDestroy();
	glDeleteLists(1000, 256) ; 	
	wglMakeCurrent(NULL, NULL) ;
	if (m_hrc)
	{
		wglDeleteContext(m_hrc) ;
		m_hrc = NULL ;
	}

	
}

void CSkinView::Tick()
{
	m_angle[m_RotatingObject].cx += 10 ;
	m_angle[m_RotatingObject].cy += 10 ;
	if (m_angle[m_RotatingObject].cx >= 360) m_angle[m_RotatingObject].cx = 0 ;
	if (m_angle[m_RotatingObject].cy >= 360) m_angle[m_RotatingObject].cy = 0 ;
	Invalidate(FALSE) ;
	UpdateWindow() ;
}


void CSkinView::OnRotateStart() 
{
	Rotate(TRUE) ;	
}

void CSkinView::OnRotateStop() 
{
	Rotate(FALSE) ;	
}

void CSkinView::Rotate(BOOL bRotate)
{
	m_bRotate = bRotate ;
 	// Stop idle loop mode
// 	CSkinApp* pApp = (CSkinApp*) AfxGetApp();
// 	pApp->SetIdleEvent(bRotate ? this : NULL);
}

void CSkinView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	mouseDownPosx=mousePosx=point.x;
	mouseDownPosy=mousePosy=point.y;
	lMouseDown=TRUE;
	CView::OnLButtonDown(nFlags, point);
}

void CSkinView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	mousePosx=point.x;
	mousePosy=point.y;
	Invalidate(FALSE) ;
	UpdateWindow() ;

	CView::OnMouseMove(nFlags, point);
}

void CSkinView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	lMouseDown=FALSE;
	for(int i=0;i<16;i++)
		OldViewingMatrix[i]=NewViewingMatrix[i];
	CView::OnLButtonUp(nFlags, point);
}

