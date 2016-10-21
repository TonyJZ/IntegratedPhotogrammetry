#pragma once


// CDlgLidCorrectYuan dialog

class CDlgLidCorrectYuan : public CDialog
{
	DECLARE_DYNAMIC(CDlgLidCorrectYuan)

public:
	CDlgLidCorrectYuan(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgLidCorrectYuan();

// Dialog Data
	enum { IDD = IDD_DLG_LidCorrect_Yuan_Model };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strCalibFile;
	CString m_strRawLasFile;
	CString m_strOutputDir;

	CArray<CString, CString&> m_lasNameVec;


	afx_msg void OnBnClickedOpencpfile();
	afx_msg void OnBnClickedOpenrawlasfile();
	afx_msg void OnBnClickedSetlasdir();
};
