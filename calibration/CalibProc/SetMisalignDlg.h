#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

// CSetMisalignDlg dialog

class CSetMisalignDlg : public CDialog
{
	DECLARE_DYNCREATE(CSetMisalignDlg)

public:
	CSetMisalignDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetMisalignDlg();
// Overrides

// Dialog Data
	enum { IDD = IDD_SetMisalignDIALOG, IDH = IDR_HTML_SETMISALIGNDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
//	DECLARE_DHTML_EVENT_MAP()
public:
	double m_roll;
	double m_pitch;
	double m_heading;
	double m_tx;
	double m_ty;
	double m_tz;
	BOOL m_bMisalign6Param;
	double m_dTranslationX;
	double m_dTranslationY;
	double m_dTranslationZ;
	afx_msg void OnBnClickedRadio3param();
	afx_msg void OnBnClickedRadio6param();
	BOOL m_bTranslationParam;
	afx_msg void OnBnClickedCheckSemi3param();
	afx_msg void OnEnChangeEditRoll();
	double m_sa0;
	BOOL m_bScanAngle0;
	afx_msg void OnBnClickedCheckSa0();
	double m_sa_ceof;
	double m_drho;
};
