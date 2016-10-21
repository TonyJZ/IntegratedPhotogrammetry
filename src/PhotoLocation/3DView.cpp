// 3DView.cpp : implementation of the C3DView class
//

#include "stdafx.h"
//#include "Mesh.h"

#include "gl/gl.h"
#include "gl/glu.h"
//#include "glut.h"
//#include <GL\glaux.h>

//#include "ViewDSM.h"

//#include "ViewDSMDoc.h"
//#include "PyramidMatch.h"

#include "orsI3DViewDoc.h"
#include "3DView.h"

//#include "Texture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <math.h>

double Degree2Radian( double degrees )
{
	return degrees/180*3.145926;
}

//////////////////////////////////////////////////////////////////////
// utility func: returns matrix for rotation about y axis
//////////////////////////////////////////////////////////////////////

void xRotation( double radian, double *xR )
{
	double cosA = cos ( radian );
	double sinA = sin ( radian );

	memset( xR, 0, 9*sizeof(double) );

	xR[0] = 1;	
				xR[4] = cosA;	xR[5] = sinA;
				xR[7] =-sinA;	xR[8] = cosA;
};



//////////////////////////////////////////////////////////////////////
// utility func: returns matrix for rotation about y axis
//////////////////////////////////////////////////////////////////////
void yRotation( double radian, double *yR )
{
	double cosA = cos ( radian );
	double sinA = sin ( radian );

	memset( yR, 0, 9*sizeof(double) );

	yR[0] = cosA;				yR[2] = -sinA;
					yR[4] = 1;
	yR[6] = sinA;				yR[8] = cosA;

};


//////////////////////////////////////////////////////////////////////
// utility func: returns matrix for rotation about z axis
//////////////////////////////////////////////////////////////////////
void zRotation( double radian, double *zR )
{
	double cosA = cos ( radian );
	double sinA = sin ( radian );

	memset( zR, 0, 9*sizeof(double) );

	zR[0] =  cosA;	zR[1] = sinA;
	zR[3] = -sinA;	zR[4] = cosA;
									zR[8] = 1;
};


void MatMat(double *M10, double *M20, double *M3, int l, int m, int n )

{
	int i, j, k;
	double *M1, *M2;

	for( i=0; i<l; i++)
	{
		for( j=0; j<n; j++)
		{
			*M3 = 0;
			
			M1 = M10 + i*m;
			M2 = M20 + j;

			for( k=0; k<m; k++)
			{
				*M3 += *M1 * *M2;
				M1++;	M2 += n;
			}

			M3++;
		}
	}
}


//////////////////////////////////////////////////////////////////////
// utility func: return mat for rotation about _axis_ by _parameter_
//////////////////////////////////////////////////////////////////////
void RotationMatrix( VECTOR &axis, double degrees, double *R )
{
	double Rt[9], Ry[9], Rx[9], Rz[9];
	// if the axis is already the z axis then we don't need to do all this
	if( axis.X != 0 || axis.Y != 0 || axis.Z != 0  )  {
		double len;
//		double len   = sqrt ( axis.X * axis.X + axis.Y * axis.Y + axis.Z * axis.Z ) ;

//		axis.X /= len;	axis.Y /= len;	axis.Z /= len;
	
		double theta = atan2 ( axis.X, axis.Z );
		len   = sqrt ( axis.X * axis.X + axis.Z * axis.Z ) ;
		double phi = atan2 ( axis.Y, len );

		//
		// R = Ry(-theta)*Rx(phi)*Rz(degrees)*Rx(-phi)*Ry( theta )
		//
		yRotation( -theta, Ry );
		xRotation ( phi, Rx );
		zRotation ( Degree2Radian( degrees ), Rz );

		MatMat( Ry, Rx, Rt, 3, 3, 3 );
		MatMat( Rt, Rz, R, 3, 3, 3 );
	
		xRotation ( -phi, Rx );
		yRotation ( theta, Ry );

		MatMat( R, Rx, Rt, 3, 3, 3 );
		MatMat( Rt, Ry, R, 3, 3, 3  );
	}
	else {
		memset( R, 0, 9*sizeof(double) );
		R[0] = R[4] = R[8] = 1;
	}	
}


void CrossProduct( const VECTOR &v0, const VECTOR &v1, VECTOR *v2 )
{
	v2->X =   v0.Y*v1.Z - v1.Y*v0.Z;
	v2->Y = -(v0.X*v1.Z - v1.X*v0.Z);
	v2->Z =   v0.X*v1.Y - v1.X*v0.Y;

//	double len = sqrt( v2->X*v2->X + v2->Y*v2->Y +  v2->Z*v2->Z );

//	v2->X /= len;
//	v2->Y /= len;
//	v2->Z /= len;
}


void RotateVector( double *R, VECTOR &v )
{
	double x = v.X;	
	double y = v.Y;

	v.X = R[0]*x + R[1]*y + R[2]*v.Z;
	v.Y = R[3]*x + R[4]*y + R[5]*v.Z;
	v.Z = R[6]*x + R[7]*y + R[8]*v.Z;
}


void Rotate2D( double cosA, double sinA, double *x, double *y )
{
	double xt = *x;
	
	*x = cosA * xt - sinA* *y;
	*y = sinA * xt + cosA* *y;
}


/////////////////////////////////////////////////////////////////////////////
// C3DView

IMPLEMENT_DYNCREATE(C3DView, CView)

BEGIN_MESSAGE_MAP(C3DView, CView)
	//{{AFX_MSG_MAP(C3DView)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DView construction/destruction

C3DView::C3DView()
{
	// OpenGL
	m_hGLContext = NULL;
	m_glPixelIndex = 0;
	
	// Mouse
	m_LeftButtonDown = FALSE;
	m_RightButtonDown = FALSE;

	// Colors
	m_ClearColorRed   = 0.0f;
	m_ClearColorGreen = 0.0f;
	m_ClearColorBlue  = 0.2f;

	InitGeometry();

	m_bInitialized = false;
	m_bListCreated = false;

	m_listName = 0;

	m_pDoc = NULL;

	Initialize(0,0,10,0,0,0,-1,1);
	
}

C3DView::~C3DView()
{
	//m_pDocument = NULL;

	if( m_bListCreated )
		glDeleteLists( m_listName, 1 );
}


void C3DView::AttachDocument( orsI3DViewDoc *pDoc )	
{	
	m_pDoc = pDoc;

	//if( NULL != pDoc->m_imageMatch )
	//	pDoc->m_imageMatch->Initialize3DView();

};



// 设置投影中心和场景中心， 和最大最小高程
bool C3DView::Initialize( double Xs, double Ys, double Zs, double Xc, double Yc, double Zc, double zMin, double zMax )
{
// 	if( m_bInitialized )
// 		return false;

	// 投影中心
	m_eye.X = Xs;		m_eye.Y = Ys;		m_eye.Z = Zs;
	
	// 注视点
	m_lookAt.X = Xc;	m_lookAt.Y = Yc;	m_lookAt.Z = Zc;	

	// 注视方向
	m_look.X = Xc - Xs ; m_look.Y = Yc - Ys;	m_look.Z = Zc - Zs;
	m_distance = sqrt( m_look.X*m_look.X + m_look.Y*m_look.Y + m_look.Z*m_look.Z );
	m_look.X /= m_distance;	m_look.Y /= m_distance;	m_look.Z /= m_distance;

	m_testBoxSize = m_distance/3;
	// 像片旋转角 kapa, 假设开始指向北（Y）
	m_up.X = 0;	m_up.Y = 1;	m_up.Z = 0;

	// 景深
	m_near = Zs - zMax;
	m_far =  Zs - zMin;

	m_zMin = zMin;
	m_zMax = zMax;

	// 
    GLfloat light_position[] = { Xs, Ys, Zs, 0.f}; /* w=0 ==> directional */

	glLightfv( GL_LIGHT0, GL_POSITION, light_position);

	if( m_listName == 0 )	{
		m_listName = glGenLists( 1 );
	}

	m_bInitialized = true;

	if( m_bListCreated )
		glDeleteLists( m_listName, 1 );

	m_bListCreated = false;

	return true;
}


// 设置新的场景中心，同时修改投影中心
void C3DView::PanTo( double Xc, double Yc, double Zc )
{
	double dX, dY, dZ;

	dX = Xc - m_lookAt.X;	dY = Yc - m_lookAt.Y;	dZ = Zc - m_lookAt.Z;

	m_lookAt.X = Xc;	m_lookAt.Y = Yc;	m_lookAt.Z = Zc;
	m_eye.X += dX;	m_eye.Y += dY;	m_eye.Z += dZ;	

//	InvalidateRect(NULL,FALSE);
}



//********************************************
// InitGeometry
//********************************************
void C3DView::InitGeometry(void)
{
	// 视点，投影中心	eye point
	m_eye.X = m_eye.Y = 0;	m_eye.Z = 5;
	
	// 注视点， 场景中心
	m_lookAt.X = m_lookAt.Y = m_lookAt.Z = 0;

	m_distance = 5;

	// up direction
	m_look.X = 0;	m_look.Y = 1;	m_look.Z = 0;

	m_up.X = 0;		m_up.Y = 1;		m_up.Z = 0;

	m_fovy = 45;
	m_xyAspect = 1;
	m_near = 1;
	m_far = 1000;			// clipping planes

    GLfloat light_position[] = { 1, 1, 5, 0.f}; /* w=0 ==> directional */

	glLightfv( GL_LIGHT0, GL_POSITION, light_position);
//	glScalef(1,1,10);
}


void C3DView::BuildTextImage_planarSeparate( BYTE *pImgBits, int wid, int hei, int nBands )
{
	// 纹理影像大小
	m_textureWid = wid;
	m_textureHei = hei;

	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);

	wglMakeCurrent(hDC,m_hGLContext);
	glDrawBuffer(GL_BACK);

	///////////////////////////////////////////////
	// Create MipMapped Texture

 	glGenTextures(1, &m_textures[0] );
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glBindTexture(GL_TEXTURE_2D, m_textures[0] );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
	if( nBands == 3 )	{
		BYTE *pImgTemp0, *pImgTemp, *band0, *band1, *band2;
	
		pImgTemp = pImgTemp0 = new BYTE[3*wid*hei];
		
		band0 = pImgBits;	band1 = band0 + wid*hei; band2 = band1 + wid*hei; 
		for( int i=0; i<wid*hei; i++ )
		{
			*pImgTemp++ = *band2++;
			*pImgTemp++ = *band1++;
			*pImgTemp++ = *band0++;
		}

		int err = gluBuild2DMipmaps(GL_TEXTURE_2D, 3, wid, hei, GL_RGB, GL_UNSIGNED_BYTE, pImgTemp0 ); 
		glTexImage2D(GL_TEXTURE_2D,0, 3, wid, hei,0, GL_RGB, GL_UNSIGNED_BYTE, pImgTemp0 );
		delete pImgTemp0;
	}
	else
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, 1, wid, hei, GL_LUMINANCE, GL_UNSIGNED_BYTE, pImgBits ); 
		glTexImage2D(GL_TEXTURE_2D, 0,1, wid, hei,0, GL_RGB, GL_UNSIGNED_BYTE, pImgBits );
	}


// 	int wid1, hei1;
//  	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &wid );
//  	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &hei );

	////////////////////////////////
	::ReleaseDC(hWnd,hDC);

	return ;
	

}





BOOL C3DView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// C3DView drawing


void C3DView::DrawLineStrip( POINT3D *pts, int n, COLORREF &color, int wid )
{
	//glColor3bv( (GLbyte *) &color );

	glLineWidth( wid );

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LINE_SMOOTH);

	// 设置当前颜色

	glColor3f(1.0f,0.0f,0.0f);

	glBegin( GL_LINE_STRIP );

	for( int i=0; i<n; i++)
	{
		glVertex3dv( (GLdouble *) pts );
		pts++;
	}
	
	glEnd();
}




void C3DView::DrawTriangle( POINT3D_IMG *pts )
{
	glPolygonMode(GL_BACK,GL_FILL );
	
	glBegin( GL_TRIANGLES );
//	glScalef(1,1,100);
	glClearDepth(1.0f);

//	glColor3f(0.0f, 0.0f, 0.7f);

	
//  	double dx0, dy0, dz0, dx1, dy1, dz1;
// 
//  	dx0 = pts[1].X - pts->X;	dy0 = pts[1].Y - pts->Y;	dz0 = pts[1].Z - pts->Z;	
//  	dx1 = pts[2].X - pts->X;	dy1 = pts[2].Y - pts->Y;	dz1 = pts[2].Z - pts->Z;
//  
// 	glNormal3d( dy0*dz1 - dy1*dz0, -(dx0*dz1 - dx1*dz0), (dx0*dy1-dx1*dy0) );
// 
 	for( int i=0; i<3; i++)
 	{
 //		glTexCoord2i( (int)(pts->ix/2),  (int)(pts->iy/2) );	//m_textureWid m_textureHei
 
   		glTexCoord2f( pts->ix/m_textureWid,  pts->iy/m_textureHei );	
 //		glVertex3dv( (GLdouble *)pts );
		glVertex3d( pts->X, pts->Y, pts->Z );
 		pts++;
 	}

	
 	glEnd();
}


static int _textureWid,  _textureHei;

static void glBeginCallback(GLenum type)
{
	glBegin( type );
}

// 设置纹理坐标
static void glVertexCallback(void * vertex_data)
{
	if( (long)vertex_data == 0xcdcdcdcd || vertex_data == NULL )
		return ;

	POINT3D_IMG *pt = (POINT3D_IMG *)vertex_data;

	glTexCoord2f( pt->ix / _textureWid,  pt->iy / _textureHei );
	glVertex3dv( (GLdouble *)pt );
}



static void glEndCallback(GLenum type)
{
	glEnd();
}


static void glCombineCallBack( GLdouble coords[3], void *nbVertex_data[4], GLfloat w[4], void **outData)
{
    POINT3D_IMG *newVt = new POINT3D_IMG; 
	POINT3D_IMG **nbVertex = (POINT3D_IMG **)nbVertex_data;
	
    newVt->X = coords[0]; 
    newVt->Y = coords[1]; 
    newVt->Z = coords[2]; 
	
	newVt->ix = newVt->iy = 0;
	for( int i=0; i<4; i++ )
	{
		newVt->ix += w[i]* nbVertex[i]->ix; 
		newVt->iy += w[i]* nbVertex[i]->iy; 
	}
	newVt->ix /= 4; 
	newVt->iy /= 4; 

	*outData = newVt; 
}



void C3DView::DrawPolygon( POINT3D_IMG *pts, int n, double A, double B, double C )
{
	int i;
	GLUtesselator *tess = gluNewTess();
	
	gluTessNormal( tess, A, B, C);

	_textureWid = m_textureWid;
	_textureHei = m_textureHei;

	gluTessCallback( tess, GLU_TESS_BEGIN ,  (void (__stdcall *)() ) glBeginCallback );
	gluTessCallback( tess, GLU_TESS_VERTEX,  (void (__stdcall *)() ) glVertexCallback );
	gluTessCallback( tess, GLU_TESS_END,  (void (__stdcall *)() ) glEndCallback );
	gluTessCallback( tess, GLU_TESS_COMBINE ,  (void (__stdcall *)() ) glCombineCallBack );

	gluTessBeginPolygon(tess, NULL); 
	{
		gluTessBeginContour(tess); 
		for( i=0; i < n; i++ )
		{
			gluTessVertex( tess, (double *)(pts+i), pts+i );
		}
		gluTessEndContour(tess);  
	}
	gluTessEndPolygon(tess); 
}


void C3DView::DrawTriangle( POINT3D *pts, COLORREF &color )
{
//	glPolygonMode(GL_BACK,GL_POINT );
	glPolygonMode(GL_BACK,GL_LINE );

//     glPolygonMode(GL_BACK,GL_POINT);
//	glScalef(1,1,10);
	glBegin( GL_TRIANGLES );

//	glColor3f(0.7f, 0.0f,0.0f);
	BYTE r, g, b;
	r=GetRValue(color);
	g=GetGValue(color);
	b=GetBValue(color);
	glColor3f(float(r)/255,float(g)/255,float(b)/255);
//	glColor3f(1.0,0,0);

 

// 	double dx0, dy0, dz0, dx1, dy1, dz1;
// 
// 	dx0 = pts[1].X - pts->X;	dy0 = pts[1].Y - pts->Y;	dz0 = pts[1].Z - pts->Z;	
// 	dx1 = pts[2].X - pts->X;	dy1 = pts[2].Y - pts->Y;	dz1 = pts[2].Z - pts->Z;
// 
// 	glNormal3d( dy0*dz1 - dy1*dz0, -(dx0*dz1 - dx1*dz0), (dx0*dy1-dx1*dy0) );

	for( int i=0; i<3; i++)
	{
		glVertex3d( pts->X, pts->Y, pts->Z );
		pts++;
	}

	glEnd();
}


void C3DView::DrawTriangle( POINT3D *pts, float *color )
{
	glPolygonMode(GL_FRONT, GL_LINE );
	glPolygonMode(GL_BACK, GL_LINE );
	
	glColor3fv( color );
	
	glBegin( GL_TRIANGLES );
	
	for( int i=0; i<3; i++)
	{
		glVertex3d( pts->X, pts->Y, pts->Z );
		pts++;
	}
	
	glEnd();
}

void C3DView::DrawVertex(POINT3D *pts, float *color)
{
	glPolygonMode(GL_FRONT, GL_POINT );
	glPolygonMode(GL_BACK, GL_POINT );
	glPointSize(2);
	
	glColor3f( 1,0,0 );
	
	glBegin( GL_POINTS );
	
//	for( int i=0; i<3; i++)
//	{
		glVertex3d( pts->X, pts->Y, pts->Z );
//		pts++;
//	}
	
	glEnd();
}

void C3DView::DrawNormal(POINT3D *pts, float *color)
{
	glPolygonMode(GL_FRONT, GL_LINE );
	glPolygonMode(GL_BACK, GL_LINE );
	
	glColor3fv( color );
	
	glBegin( GL_TRIANGLES );
	
//	for( int i=0; i<3; i++)
//	{
		glNormal3d( pts->X, pts->Y, pts->Z );
//		pts++;
//	}
	
	glEnd();
}

void C3DView::OnDraw(CDC* pDC)
{

	wglMakeCurrent( pDC->GetSafeHdc(), m_hGLContext);
	
	glClearColor( 1.0, 1.0, 1.0, 1.0 );
	glClearColor( 0, 0, 0, 1.0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glScalef(1,1,10);

	// Lights properties
//	float	ambientProperties[]  = {1.0f, 0.0f, 0.0f, 1.0f};
//	float	diffuseProperties[]  = {1.0f, 0.0f, 0.0f, 1.0f};
//	float	specularProperties[] = {1.0f, 0.0f, 0.0f, 1.0f};
	
//	glLightfv( GL_LIGHT0, GL_AMBIENT, ambientProperties);
//	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseProperties);
//	glLightfv( GL_LIGHT0, GL_SPECULAR, specularProperties);
//	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);

//	GLfloat light_position[] = { m_eye.X, m_eye.Y, m_eye.Z + 100 , 0.f}; /* w=0 ==> directional */
//
//	glLightfv( GL_LIGHT0, GL_POSITION, light_position);


	// Default : material
//	float	MatAmbient[]  = {0.0f, 0.33f, 0.50f, 1.0f};
//	float	MatDiffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
//	float	MatSpecular[]  = {0.5f, 0.0f, 0.0f, 1.0f};
//	float	MatShininess[]  = { 84 };
//	float	MatEmission[]  = {0.0f, 0.0f, 0.0f, 1.0f};
	
	// Back : green
//	float MatAmbientBack[]  = {0.0f, 0.5f, 0.0f, 1.0f};
//
//	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
//	glMaterialfv(GL_FRONT, GL_SPECULAR, MatSpecular );
//	glMateriali(GL_FRONT,GL_SHININESS, 100);
//
	///////////////////////////////////////////////////////////

 	glEnable(GL_TEXTURE_2D);						// Enable Texture Mapping
 	glShadeModel(GL_SMOOTH);						// Enable Smooth Shading

	/////////////////////////////
	glMatrixMode(GL_MODELVIEW );
	glLoadIdentity();

	gluLookAt( m_eye.X, m_eye.Y, m_eye.Z, m_lookAt.X, m_lookAt.Y, m_lookAt.Z, m_up.X, m_up.Y, m_up.Z );
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective( m_fovy, m_xyAspect, m_distance/2, 3*m_distance );// m_near, m_far );

	//////////////////////////////////////////////////////
	// 设置当前颜色
//	glColor3f(0.0f,0.0f,0.7f);
	
	//////////////////
//	glEnable(  GL_NORMALIZE );
	glDisable(GL_NORMALIZE );
	glDisable(GL_LIGHTING );

	glPolygonMode(GL_FRONT, GL_LINE );
	glPolygonMode(GL_BACK, GL_LINE );

 	glBindTexture(GL_TEXTURE_2D, m_textures[0]);

	if( m_bInitialized && NULL != m_pDoc )
	{
		glPolygonMode(GL_BACK, GL_LINE );
		glPolygonMode(GL_FRONT, GL_FILL );

		if( !m_bListCreated )	{
			TRACE("glNewList");
			glNewList( m_listName, GL_COMPILE_AND_EXECUTE );
	
			m_pDoc->OnDraw3D( this );

			glEndList();
			m_bListCreated = true;
		}
		else
			glCallList( m_listName );

		//m_pDoc->OnDraw3D( this );
	}

//	Test();		//测试用函数，在窗口绘制锥体	
	
// 	POINT3D	pt;
// 	pt.X=0;
// 	pt.Y=1;
// 	pt.Z=0;
// 	float color[3] = { 1.0, 0, 0};
// 	DrawVertex(&pt, color);

	////////////////////////////////////////
	HDC hDC=  pDC->GetSafeHdc();
	SwapBuffers(hDC );
	
	// Double buffer
	glFlush();
}

void C3DView::Test()
{
	glDisable(GL_LIGHTING );
	
	// 多边形风格
	glPolygonMode( GL_FRONT, GL_LINE );
	glPolygonMode( GL_BACK, GL_LINE );
	glBegin( GL_TRIANGLE_FAN );
	
	glColor3f(0.5f,0.5f,0.5f);
	
	float dxy = m_testBoxSize;
	glVertex3f( m_lookAt.X, m_lookAt.Y, m_lookAt.Z-dxy/2);
	glVertex3f( m_lookAt.X - dxy, m_lookAt.Y - dxy, m_lookAt.Z-dxy/3 );
	glVertex3f( m_lookAt.X + dxy, m_lookAt.Y - dxy, m_lookAt.Z-dxy/3 );
	glVertex3f( m_lookAt.X + dxy, m_lookAt.Y + dxy, m_lookAt.Z-dxy/3 );
	glVertex3f( m_lookAt.X - dxy, m_lookAt.Y + dxy, m_lookAt.Z-dxy/3 );
	glVertex3f( m_lookAt.X - dxy, m_lookAt.Y - dxy, m_lookAt.Z-dxy/3 );
	
	glEnd();
}


/////////////////////////////////////////////////////////////////////////////
// C3DView printing

BOOL C3DView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void C3DView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void C3DView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// C3DView diagnostics

#ifdef _DEBUG
void C3DView::AssertValid() const
{
	CView::AssertValid();
}

void C3DView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

//CViewDSMDoc* C3DView::GetDocument() // non-debug version is inline
//{
//	if( m_pDocument )	{
//		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CViewDSMDoc)));
//		return (CViewDSMDoc*)m_pDocument;
//	}
//	else
//		return NULL;
//}
#endif //_DEBUG



//////////////////////////////////////////////
//////////////////////////////////////////////
// OPENGL
//////////////////////////////////////////////
//////////////////////////////////////////////


//********************************************
// OnCreate
// Create OpenGL rendering context 
//********************************************
int C3DView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	
	if(SetWindowPixelFormat(hDC)==FALSE)
		return 0;
	
	if(CreateViewGLContext(hDC)==FALSE)
		return 0;
	
	//::ReleaseDC(hWnd,hDC);
	
	// Default mode
//	glPolygonMode(GL_FRONT,GL_FILL);
//	glPolygonMode(GL_BACK,GL_FILL);
	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	
	// Lights properties
	float	ambientProperties[]  = {1.0f, 0.0f, 0.0f, 1.0f};
	float	diffuseProperties[]  = {1.0f, 0.0f, 0.0f, 1.0f};
	float	specularProperties[] = {1.0f, 0.0f, 0.0f, 1.0f};
	
	glLightfv( GL_LIGHT0, GL_AMBIENT, ambientProperties);
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseProperties);
	glLightfv( GL_LIGHT0, GL_SPECULAR, specularProperties);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
	
	glClearColor(m_ClearColorRed,m_ClearColorGreen,m_ClearColorBlue,1.0f);
	glClearDepth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	
	// Perspective
	CRect rect;
	GetClientRect(&rect);

	double aspect = (rect.Height() == 0) ? rect.Width() : (double)rect.Width()/(double)rect.Height();

	// 定义透视矩阵
	// (y方向视场角, 宽高比，最近视距、最远视距 
	gluPerspective(45, aspect, 0.1, 1000.0);
	
	//glPolygonMode(GL_FRONT,GL_FILL);
	//glPolygonMode(GL_BACK,GL_POINT);
	
	// Default : lighting
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	
	// Default : blending
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);
	
	// Default : material
	float	MatAmbient[]  = {0.0f, 0.33f, 0.50f, 1.0f};
	float	MatDiffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
	float	MatSpecular[]  = {0.5f, 0.0f, 0.5f, 1.0f};
	float	MatShininess[]  = { 84 };
	float	MatEmission[]  = {0.0f, 0.0f, 0.0f, 1.0f};
	
	// Back : green
	float MatAmbientBack[]  = {0.0f, 0.5f, 0.0f, 1.0f};

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	glMaterialfv(GL_FRONT, GL_SPECULAR, MatSpecular );
	glMateriali(GL_FRONT,GL_SHININESS, 100);

//  	glEnable( GL_CULL_FACE );
//  	glCullFace( GL_BACK  );
//  	glFrontFace( GL_CCW );
//  
//  	glEnable( GL_AUTO_NORMAL );

	glEnable( GL_DEPTH_TEST);
	//glDepthFunc(Gl_LESS);
	
	return 1;
}



//********************************************
// SetWindowPixelFormat
//********************************************
BOOL C3DView::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	
	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE;
	
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 32;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 64;
	pixelDesc.cAccumRedBits = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 8;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;
	
	m_glPixelIndex = ChoosePixelFormat(hDC,&pixelDesc);
	if(m_glPixelIndex == 0) // Choose default
	{
		m_glPixelIndex = 1;
		if(DescribePixelFormat(hDC,m_glPixelIndex,
			sizeof(PIXELFORMATDESCRIPTOR),&pixelDesc)==0)
			return FALSE;
	}
	
	if(!SetPixelFormat(hDC,m_glPixelIndex,&pixelDesc))
		return FALSE;
	

	// CListBox

	return TRUE;
}



//********************************************
// CreateViewGLContext
// Create an OpenGL rendering context
//********************************************
BOOL C3DView::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);
	
	if(m_hGLContext==NULL)
		return FALSE;
	
	if(wglMakeCurrent(hDC,m_hGLContext)==FALSE)
		return FALSE;
	
	return TRUE;
}




void C3DView::OnDestroy() 
{
	if(wglGetCurrentContext() != NULL)
		wglMakeCurrent(NULL,NULL);
	
	if(m_hGLContext != NULL)
	{
		wglDeleteContext(m_hGLContext);
		m_hGLContext = NULL;
	}

	CView::OnDestroy();
}

void C3DView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	//TRACE("Activate view, set active OpenGL rendering context...\n");
	wglMakeCurrent(hDC,m_hGLContext);

	// Set OpenGL perspective, viewport and mode
	m_xyAspect = (cy == 0) ? cx : (double)cx/(double)cy;
	
	glViewport(0, 0, cx, cy );
	glDrawBuffer(GL_BACK);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);


	// Release
	::ReleaseDC(hWnd,hDC);
}

void C3DView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_LeftButtonDown = TRUE;
	m_LeftDownPos = point;
	SetCapture();
	
	CView::OnLButtonDown(nFlags, point);
}

void C3DView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_LeftButtonDown = FALSE;
	ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
}

void C3DView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_RightButtonDown = TRUE;
	m_RightDownPos = point;
	SetCapture();

	CView::OnRButtonDown(nFlags, point);
}

void C3DView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_RightButtonDown = FALSE;
	ReleaseCapture();
	CView::OnRButtonUp(nFlags, point);
}




void C3DView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetClientRect( &rect );

//	CViewDSMDoc * pDoc = GetDocument();


	double R[9];
	if( m_LeftButtonDown )
	{
		if( nFlags & MK_CONTROL )	// PAN
		{
			double ratioX = -(double)(point.x - m_LeftDownPos.x ) / rect.Width();
			double ratioY =  (double)(point.y - m_LeftDownPos.y ) / rect.Height();

			VECTOR	right;
			
			CrossProduct( m_look, m_up, &right );

			double dX, dY, dZ;

			dX = m_distance*( right.X*ratioX - right.Y*ratioY ) / 2;
			dY = m_distance*( right.Y*ratioX + right.X*ratioY ) / 2;
//			dZ = m_distance*( right.Z*ratioX + m_look.Z*ratioY );

			m_eye.X += dX;		m_eye.Y += dY;		//	m_eye.Z += dZ;
			m_lookAt.X += dX;	m_lookAt.Y += dY;	//	m_lookAt.Z += dZ;

		}
		else if( nFlags & MK_SHIFT  )	{	// 绕Z轴
			double sinA = -(double)( point.x - m_LeftDownPos.x ) / rect.Width();
			double cosA = sqrt( 1 - sinA*sinA );

			Rotate2D( cosA, sinA, &m_look.X, &m_look.Y );
			Rotate2D( cosA, sinA, &m_up.X, &m_up.Y );

			m_eye.X = m_lookAt.X - m_look.X *m_distance;
			m_eye.Y = m_lookAt.Y - m_look.Y *m_distance;
		}

		// roll, pitch
		else	{
			double roll = -(double)( point.x - m_LeftDownPos.x ) / rect.Width();
			double pitch = (double)( point.y - m_LeftDownPos.y ) / rect.Height();

			// 绕光轴的投影滚动
			RotationMatrix( m_look, roll*180, R );
			RotateVector( R, m_up );
			
			// 绕右方向的投影俯仰
			VECTOR	right;
			CrossProduct( m_look, m_up, &right );

			RotationMatrix( right, pitch*180, R );
			RotateVector( R, m_look );
			RotateVector( R, m_up );

//			m_up.X = 0;	m_up.Y = 1;	m_up.Z= 0 ;

			// 更新投影中心
			m_eye.X = m_lookAt.X - m_look.X *m_distance;
			m_eye.Y = m_lookAt.Y - m_look.Y *m_distance;
			m_eye.Z = m_lookAt.Z - m_look.Z *m_distance;
		}

		m_LeftDownPos = point;

		InvalidateRect(NULL,FALSE);
	}
	// 缩放: 改变投影中心，同时修改裁剪平面
	else if( m_RightButtonDown )
	{
		if( nFlags & MK_CONTROL )	// PAN, Z
		{
			double ratioZ = (double)(point.y - m_LeftDownPos.y ) / rect.Height();

			double	dZ = 5*ratioZ;

			if( m_lookAt.Z + dZ > m_zMin && m_lookAt.Z + dZ < m_zMax )	{
				m_eye.Z += dZ;
				m_lookAt.Z += dZ;
			}
		}
		else	{
			double ratio = 1 - (double)(point.y  - m_RightDownPos.y ) / rect.Height();
			
			m_distance *= ratio;
			
			// 更新投影中心
			m_eye.X = m_lookAt.X - m_look.X *m_distance;
			m_eye.Y = m_lookAt.Y - m_look.Y *m_distance;
			m_eye.Z = m_lookAt.Z - m_look.Z *m_distance;
		}

		m_RightDownPos = point;
		InvalidateRect(NULL,FALSE);
	}

	
	CView::OnMouseMove(nFlags, point);
}

BOOL C3DView::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	
	GetClientRect( &rect );
	
	pDC->FillSolidRect( 0, 0, rect.Width(), rect.Height(), (COLORREF)0x00ff0000 );
	
	return CView::OnEraseBkgnd(pDC);
}

void C3DView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	wglMakeCurrent(hDC,m_hGLContext);
	::ReleaseDC(hWnd,hDC);
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void C3DView::OnEditCopy() 
{
/*
  // Clean clipboard of contents, and copy the DIB.
  if(OpenClipboard())
   {

    BeginWaitCursor();

		// Snap
		CSize size;
		unsigned char *pixel = SnapClient(&size);

		// Image
		CTexture image;

		// Link image - buffer
		int success = 0;
		VERIFY(image.ReadBuffer(pixel,size.cx,size.cy,24));

		// Cleanup memory
		delete [] pixel;

    EmptyClipboard();

    SetClipboardData(CF_DIB,image.ExportHandle());
    CloseClipboard();
    EndWaitCursor();
   }
*/
}

// Hand-made client snapping
unsigned char *C3DView::SnapClient(CSize *pSize)
{
/*
	BeginWaitCursor();

	// Client zone
	CRect rect;
	GetClientRect(&rect);
	//CSize size(CTexture::LowerPowerOfTwo(rect.Width()),rect.Height());
	CSize size(rect.Width(),rect.Height());
	*pSize = size;

	ASSERT(size.cx > 0);
	ASSERT(size.cy > 0);

	// Alloc
	unsigned char *pixel = new unsigned char[3*size.cx*size.cy];
	ASSERT(pixel != NULL);
	
	// Capture frame buffer
	TRACE("Start reading client...\n");
	TRACE("Client : (%d,%d)\n",size.cx,size.cy);

	CRect ClientRect,MainRect;
	this->GetWindowRect(&ClientRect);
	CWnd *pMain = AfxGetApp()->m_pMainWnd;
	CWindowDC dc(pMain);
	pMain->GetWindowRect(&MainRect);
	int xOffset = ClientRect.left - MainRect.left;
	int yOffset = ClientRect.top - MainRect.top;
	for(int j=0;j<size.cy;j++)
		for(int i=0;i<size.cx;i++)
		{
			COLORREF color = dc.GetPixel(i+xOffset,j+yOffset);
			pixel[3*(size.cx*(size.cy-1-j)+i)] = (BYTE)GetBValue(color);
			pixel[3*(size.cx*(size.cy-1-j)+i)+1] = (BYTE)GetGValue(color);
			pixel[3*(size.cx*(size.cy-1-j)+i)+2] = (BYTE)GetRValue(color);
		}
	EndWaitCursor();
	return pixel;
*/

	return NULL;
}


void C3DView::SetClearColor(void) 
{	
	glClearColor(m_ClearColorRed,m_ClearColorGreen,m_ClearColorBlue,1.0f); 
}

void C3DView::Update3DView()
{
	if( m_listName == 0 )	{
		m_listName = glGenLists( 1 );
	}
	
	m_bInitialized = true;
	
	if( m_bListCreated )
		glDeleteLists( m_listName, 1 );
	
	m_bListCreated = false;

	TRACE("Update3DView");
}
