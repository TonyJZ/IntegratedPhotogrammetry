#pragma once


// CATN2LASDlg dialog

class CATN2LASDlg : public CDialog
{
	DECLARE_DYNAMIC(CATN2LASDlg)

public:
	CATN2LASDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CATN2LASDlg();

// Dialog Data
	enum { IDD = IDD_GenLAS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bCheck_dp;
	BOOL m_bCheck_sa0;
	BOOL m_bCheck_SemiXYZ;
	double m_dp;
	double m_semi_dx;
	double m_semi_dy;
	double m_semi_dz;
	double m_misalign_heading;
	double m_misalign_pitch;
	double m_misalign_roll;
	double m_sac;
	double m_sa0;
	double m_misalign_tx;
	double m_misalign_ty;
	double m_misalign_tz;
	CString m_strATNName;
	CArray<CString, CString&> m_atnNameVec;
	CString m_strLasDir;
	afx_msg void OnBnClickedSetsrcproj();
	afx_msg void OnBnClickedSetdstproj();
	afx_msg void OnBnClickedOpenatnfile();
	afx_msg void OnBnClickedSetlasdir();
	afx_msg void OnBnClickedRadio3param();
//	BOOL m_bMisalign_3Param;
	afx_msg void OnBnClickedRadio6param();
	afx_msg void OnBnClickedCheckSemi3param();
	afx_msg void OnBnClickedCheckSa0();
	afx_msg void OnBnClickedCheckdp();
	virtual BOOL OnInitDialog();

	orsString  m_srcWkt;
	orsString  m_targetWkt;
	BOOL m_bTP_ATN;
	afx_msg void OnBnClickedRadioTplaneatn();
	BOOL m_bMisalign_6Param;
	afx_msg void OnChangeEditRoll();
	afx_msg void OnChangeEditPitch();
	afx_msg void OnChangeEditHeading();
	afx_msg void OnChangeEditTx();
	afx_msg void OnChangeEditTy();
	afx_msg void OnChangeEditTz();
	afx_msg void OnChangeEditDx();
	afx_msg void OnChangeEditDz();
	afx_msg void OnChangeEditDy();
	afx_msg void OnChangeEditDp();
	afx_msg void OnChangeEditSa0();
	afx_msg void OnChangeEditSaCeof();
};
