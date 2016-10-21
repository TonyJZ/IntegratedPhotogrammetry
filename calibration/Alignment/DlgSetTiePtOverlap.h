#pragma once


// CDlgSetTiePtOverlap dialog

class CDlgSetTiePtOverlap : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetTiePtOverlap)

public:
	CDlgSetTiePtOverlap(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSetTiePtOverlap();

// Dialog Data
	enum { IDD = IDD_DIALOG_SetTiePtOverlap };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_minTiePtOverlap;
};
