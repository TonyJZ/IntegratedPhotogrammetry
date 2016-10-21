#if !defined(AFX_PROPERTYPAGEWITHPROPERTYSHEET_H__9D9751EF_5D6B_11D3_B3B5_DADCF6E30516__INCLUDED_)
#define AFX_PROPERTYPAGEWITHPROPERTYSHEET_H__9D9751EF_5D6B_11D3_B3B5_DADCF6E30516__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CPropertySheetPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetPage dialog

class CPropertySheetPage : public CBCGPPropertyPage
{
	DECLARE_DYNCREATE(CPropertySheetPage)

// Construction
public:
	CPropertySheetPage();
	~CPropertySheetPage();

	void SetActivePage( int nPage )	{	m_internal_sheet.SetActivePage(nPage); }
	int GetActivePage()	{	return	m_internal_sheet.GetActiveIndex(); }

	virtual BOOL UpdateData(BOOL bSaveAndValidate);
	virtual void AddPage(CBCGPPropertyPage *pPage);

// Dialog Data
	//{{AFX_DATA(CPropertySheetPage)
	enum { IDD = IDD_PAGE_SHEET };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropertySheetPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBCGPPropertySheet m_internal_sheet;
	// Generated message map functions
	//{{AFX_MSG(CPropertySheetPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGEWITHPROPERTYSHEET_H__9D9751EF_5D6B_11D3_B3B5_DADCF6E30516__INCLUDED_)
