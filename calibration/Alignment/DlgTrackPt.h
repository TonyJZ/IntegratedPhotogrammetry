#pragma once


// CDlgTrackPt dialog

class CDlgTrackPt : public CDialog
{
	DECLARE_DYNAMIC(CDlgTrackPt)

public:
	CDlgTrackPt(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTrackPt();

// Dialog Data
	enum { IDD = IDD_DlgTackTiePt };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strMatchFile;
	CString m_strTrackFile;
	CString m_strImgList;
	CString m_strKeyList;
	afx_msg void OnBnClickedButtonMatch();
	afx_msg void OnBnClickedButtonTrack();
	afx_msg void OnBnClickedButtonImglist();
	afx_msg void OnBnClickedButtonKeylist();
};
