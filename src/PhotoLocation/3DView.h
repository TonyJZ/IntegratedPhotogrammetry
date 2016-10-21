// C3DView.h : interface of the C3DView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_C3DVIEW_H__50ED6A32_C0C1_11D2_B4C7_006067306B48__INCLUDED_)
#define AFX_C3DVIEW_H__50ED6A32_C0C1_11D2_B4C7_006067306B48__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <gl\glu.h>
//#include "../include/BaseDataType.h"

//class CViewDSMDoc;

struct POINT3D_IMG	{
	double X, Y, Z;
	double ix, iy;			// texture coordinate
};


struct VECTOR	{
	double X, Y, Z;
};

struct POINT3D	{
	double X, Y, Z;
};

interface orsI3DViewDoc;

class C3DView : public CView
{
public: // create from serialization only
	C3DView();
	DECLARE_DYNCREATE(C3DView)

// Attributes
private:
	orsI3DViewDoc	*m_pDoc;

public:
	void AttachDocument( orsI3DViewDoc *pDoc );

//	void DetachDocument()	{	m_pDocument = NULL;	m_bInitialized = FALSE;	};

private:
	// OpenGL specific
	BOOL SetWindowPixelFormat(HDC hDC);
	BOOL CreateViewGLContext(HDC hDC);

	void SetClearColor(void);
	
	unsigned char *SnapClient(CSize *pSize);

private:
	HGLRC m_hGLContext;
	int m_glPixelIndex;

	GLuint	m_listName;
	bool	m_bListCreated;

	// Mouse 
	BOOL m_LeftButtonDown;
	BOOL m_RightButtonDown;

	CPoint m_LeftDownPos;
	CPoint m_RightDownPos;

	HCURSOR m_CursorRotation;

	// Position, rotation ,scaling
	void InitGeometry(void);

	// Colors
	float m_ClearColorRed;
	float m_ClearColorGreen;
	float m_ClearColorBlue;

private:

	int		m_textureWid;
	int		m_textureHei;
	GLuint	m_textures[1];	// Storage for 1 textures

private:
//	bool	m_bInitialized;
	//
	// for gluLookAt, 通过视点的移动决定模型的相对运动（平移、旋转、缩放）
	//
	POINT3D m_eye;
	POINT3D m_lookAt;
	double m_distance;	// 物距

	VECTOR	m_look;		// 光轴单位向量（像平面法向量）
	VECTOR	m_up;		// 像平面y方向

	//
	// for gluPerspective
	double m_fovy;
	double m_xyAspect;
	double m_near, m_far;			// clipping planes

	double m_zMin, m_zMax;			// 高程范围

	float  m_testBoxSize;

protected:
	bool	m_bInitialized;
// Operations
public:

	// 设置投影中心和场景中心， 和最大最小高程
	bool Initialize( double Xs, double Ys, double Zs, double Xc, double Yc, double Zc, double zMin, double zMax );

	void BuildTextImage_planarSeparate( BYTE *pImgBits, int wid, int hei, int nBands );

	// 设置新的场景中心，同时修改投影中心
	void PanTo( double Xc, double Yc, double Zc );
	
	//
	void DrawLineStrip( POINT3D *pts, int n, COLORREF &color, int wid );
	void DrawTriangle( POINT3D *pts, float *color );
	void DrawTriangle( POINT3D *pts, COLORREF &color );
	void DrawTriangle( POINT3D_IMG *pts );

	void DrawPolygon( POINT3D_IMG *pts, int n, double a, double b, double c );

	void DrawVertex(POINT3D *pts, float *color);
	void DrawNormal(POINT3D *pts, float *color);

	//更新3D视图，重新初始化显示列表
	void Update3DView();

	void Test();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~C3DView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(C3DView)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditCopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_C3DVIEW_H__50ED6A32_C0C1_11D2_B4C7_006067306B48__INCLUDED_)
