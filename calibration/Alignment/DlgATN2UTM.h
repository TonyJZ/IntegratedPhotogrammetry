#pragma once


// CDlgATN2UTM dialog

class CDlgATN2UTM : public CDialog
{
	DECLARE_DYNAMIC(CDlgATN2UTM)

public:
	CDlgATN2UTM(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgATN2UTM();

// Dialog Data
	enum { IDD = IDD_DlgATN2UTM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void AddImageTable(int startPos);

	DECLARE_MESSAGE_MAP()
public:
	CString m_strOutDir;
//	CEdit m_centralMeridian;
//	double m_centralMeridian;
	CListCtrl m_wndAtnList;
	orsString  m_targetWkt;
	CArray<CString, CString&>		m_atnNameVec;

	afx_msg void OnBnClickedButtonAddatn();
	afx_msg void OnBnClickedButtonDstproj();
	afx_msg void OnBnClickedButtonOutputdir();
	virtual BOOL OnInitDialog();
//	afx_msg void OnChangeEditCentralmeridian();
};
