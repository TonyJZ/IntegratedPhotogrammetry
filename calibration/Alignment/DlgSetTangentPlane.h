#pragma once
#include "afxcmn.h"


// CDlgSetTangentPlane dialog

class CDlgSetTangentPlane : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetTangentPlane)

public:
	CDlgSetTangentPlane(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSetTangentPlane();

// Dialog Data
	enum { IDD = IDD_DIALOG_SetAnchor };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void AddImageTable(int startPos);

	DECLARE_MESSAGE_MAP()
public:
	double m_longitude;
	double m_latitude;
	virtual BOOL OnInitDialog();
	CListCtrl m_wndAtnList;
	afx_msg void OnEnChangeEditlat();
	afx_msg void OnEnChangeEditlon();
	afx_msg void OnBnClickedButtonAddatn();

	CArray<CString, CString&>		m_atnNameVec;
	CString m_strOutputDir;
	afx_msg void OnBnClickedButtonOutputdir();
};
