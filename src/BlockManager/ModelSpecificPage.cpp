// ModelSpecificPage.cpp : implementation file
//

#include "stdafx.h"
#include "blockmanager.h"
#include "ModelSpecificPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelSpecificPage dialog


CModelSpecificPage::CModelSpecificPage(CWnd* pParent /*=NULL*/)
	: CDialog(CModelSpecificPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelSpecificPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CModelSpecificPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelSpecificPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelSpecificPage, CDialog)
	//{{AFX_MSG_MAP(CModelSpecificPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelSpecificPage message handlers
