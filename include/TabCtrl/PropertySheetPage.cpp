// PropertyPageWithPropertySheet.cpp : implementation file
//

#include "stdafx.h"

#include "orsImageLayerRender.h"

#include "PropertySheetPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetPage property page

IMPLEMENT_DYNCREATE(CPropertySheetPage, CBCGPPropertyPage)

CPropertySheetPage::CPropertySheetPage() : CBCGPPropertyPage(CPropertySheetPage::IDD)
{
	//{{AFX_DATA_INIT(CPropertySheetPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_internal_sheet.SetLook( CBCGPPropertySheet::PropSheetLook_List );
}

CPropertySheetPage::~CPropertySheetPage()
{
}

void CPropertySheetPage::DoDataExchange(CDataExchange* pDX)
{
	CBCGPPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertySheetPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	// this need for check data when user change tab in parent property sheet
	if(m_internal_sheet)
		if(!m_internal_sheet.GetActivePage()->UpdateData(pDX->m_bSaveAndValidate))
			pDX->Fail();	
}


BEGIN_MESSAGE_MAP(CPropertySheetPage, CBCGPPropertyPage)
	//{{AFX_MSG_MAP(CPropertySheetPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetPage message handlers

BOOL CPropertySheetPage::OnInitDialog() 
{
	CBCGPPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_internal_sheet.EnableStackedTabs( FALSE );
	m_internal_sheet.Create(this, WS_CHILD | WS_VISIBLE , 0);
	m_internal_sheet.ModifyStyleEx (0, WS_EX_CONTROLPARENT);
	m_internal_sheet.ModifyStyle( 0, WS_TABSTOP );
	
	// move to left upper corner
	m_internal_sheet.SetWindowPos( NULL, 0, 0, 0, 0, 
                        SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropertySheetPage::AddPage(CBCGPPropertyPage *pPage)
{
	m_internal_sheet.AddPage(pPage);
}

BOOL CPropertySheetPage::UpdateData(BOOL bSaveAndValidate)
{
	if(!CBCGPPropertyPage::UpdateData(bSaveAndValidate))
		return FALSE;
	// check property sheet. Need when OK button press
	return m_internal_sheet.GetActivePage()->UpdateData(bSaveAndValidate);	
}
