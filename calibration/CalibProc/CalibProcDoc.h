// CalibProcDoc.h : interface of the CCalibProcDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALIBPROCDOC_H__2BA57F26_4E12_42E5_9555_38485FEA265C__INCLUDED_)
#define AFX_CALIBPROCDOC_H__2BA57F26_4E12_42E5_9555_38485FEA265C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCalibProcDoc : public CDocument
{
protected: // create from serialization only
	CCalibProcDoc();
	DECLARE_DYNCREATE(CCalibProcDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalibProcDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCalibProcDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCalibProcDoc)
	afx_msg void OnReadTrj();
	afx_msg void OnReadAtnfile();
	afx_msg void OnReadTSBin();
	afx_msg void OnGPSInterpolation();
	afx_msg void OnCalLaserPosition();
	afx_msg void OnStatPointError();
	afx_msg void OncheckProjection();
	afx_msg void OnDetectCircleTarget();
	afx_msg void OnSegmentCircle();
	afx_msg void OnImageTypeTrans();
	afx_msg void OnGetOverlap();
	afx_msg void OnGetBoundary();
	afx_msg void OnGetOverlapData();
	afx_msg void OnQCOverlap();
	afx_msg void OnPointInterpolation();
	afx_msg void OnLidarPointClip();
	afx_msg void OnGeodeticToGeocentric();
	afx_msg void OnTrajectoryClipping();
	afx_msg void OnTransTPlane();
	afx_msg void OnCalPosTPlane();
	afx_msg void OnATNToLAS11();
	afx_msg void OnATN2TPlane();
	afx_msg void OnTiePoint();
	afx_msg void OnCreateATNIdx();
	afx_msg void OnTESTlevmar();
	afx_msg void OnCoplanarAdjustment();
	afx_msg void OnSimulateRectangle();
	afx_msg void OnTp2utm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGcpWgs84ToTplane();
	afx_msg void OnLidarcalibrationTruepoint();
	afx_msg void OnCoplanaradjustment3param();
	afx_msg void OnCoplanaradj6pZconstraint();
	afx_msg void OnMountingerror();
	afx_msg void OnAdjustmentevaluation();
	afx_msg void OnImginorrectify();
	afx_msg void OnGeodetic2utm();
	afx_msg void OnTest();
//	afx_msg void On32824();
	afx_msg void OnImgcalib();
	afx_msg void OnPosinterpolateformlidar();
	afx_msg void OnRotate();
	afx_msg void OnAtnCrop();
	afx_msg void OnExorrigidtranslate();
	afx_msg void OnResectionMisalign();
	afx_msg void OnLastranslate();
	afx_msg void OnQcPlane();
	afx_msg void OnQcVp();
	afx_msg void OnHonvStat();
	afx_msg void OnSelecttieptbyplane();
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALIBPROCDOC_H__2BA57F26_4E12_42E5_9555_38485FEA265C__INCLUDED_)
