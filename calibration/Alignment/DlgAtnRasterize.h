#pragma once
#include "afxcmn.h"


// CDlgAtnRasterize dialog

class CDlgAtnRasterize : public CDialog
{
	DECLARE_DYNAMIC(CDlgAtnRasterize)

public:
	CDlgAtnRasterize(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAtnRasterize();

// Dialog Data
	enum { IDD = IDD_DIALOG_SetRaseterize };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void AddImageTable(int startPos);

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_selAttType;
	virtual BOOL OnInitDialog();
	double m_gridSize;
	afx_msg void OnBnClickedButtonAddatn();	
	CListCtrl m_wndAtnList;

	CArray<CString, CString&>		m_atnNameVec;
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEditGridsize();
	afx_msg void OnBnClickedRadioE();
	afx_msg void OnBnClickedRadioI();
};
