// PhotoLocationDoc.h : interface of the CPhotoLocationDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHOTOLOCATIONDOC_H__0623256A_8610_4C7C_A740_98D4CDD7BE13__INCLUDED_)
#define AFX_PHOTOLOCATIONDOC_H__0623256A_8610_4C7C_A740_98D4CDD7BE13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "orsI3DViewDoc.h"
#include <vector>

#define NUM_CAMERA_PARAMS 9
#define POLY_INVERSE_DEGREE 6

typedef struct {
    double R[9];     /* Rotation */
    double t[3];     /* Translation */
    double f;        /* Focal length */
    double k[2];     /* Undistortion parameters */
    double k_inv[POLY_INVERSE_DEGREE]; /* Inverse undistortion parameters */
    char constrained[NUM_CAMERA_PARAMS];
    double constraints[NUM_CAMERA_PARAMS];  /* Constraints (if used) */
    double weights[NUM_CAMERA_PARAMS];      /* Weights on the constraints */
    double K_known[9];  /* Intrinsics (if known) */
    double k_known[5];  /* Distortion params (if known) */
	
    char fisheye;            /* Is this a fisheye image? */
    char known_intrinsics;   /* Are the intrinsics known? */
    double f_cx, f_cy;       /* Fisheye center */
    double f_rad, f_angle;   /* Other fisheye parameters */
    double f_focal;          /* Fisheye focal length */
	
    double f_scale, k_scale; /* Scale on focal length, distortion params */
} camera_params_t;

typedef struct 
{
    double pos[3];
    double color[3];
    
} point_t;

struct imageframe
{
	POINT3D	frame[4];		//框标
	POINT3D	optcenter;		//光心
	POINT3D princip;		//主点
};

class CPhotoLocationDoc : public CDocument, public orsI3DViewDoc
{
protected: // create from serialization only
	CPhotoLocationDoc();
	DECLARE_DYNCREATE(CPhotoLocationDoc)

// Attributes
public:
	virtual void OnDraw3D( C3DView *pView );

	void  InitViewer();

	void  GetCameraPOS();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhotoLocationDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPhotoLocationDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	int m_num_images, m_num_points;

	std::vector<camera_params_t> m_cameras;
    std::vector<point_t> m_points;
	std::vector<imageframe> m_imgframe;
// Generated message map functions
protected:
	//{{AFX_MSG(CPhotoLocationDoc)
	afx_msg void OnOpenBundle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHOTOLOCATIONDOC_H__0623256A_8610_4C7C_A740_98D4CDD7BE13__INCLUDED_)
