// ModelSpecificPage.cpp : implementation file
//

#include "stdafx.h"
//#include "BlockSetting.h"
#include "ModelSpecificPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelSpecificPage dialog


CModelSpecificPage::CModelSpecificPage(CWnd* pParent /*=NULL*/)
	: CPropertiesTabPage(CModelSpecificPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModelSpecificPage)
	m_dMinHeight = 0.0;
	m_dMaxHeight = 0.0;
	m_flyoverlap = 65;
	m_stripoverlap = 30;
	//}}AFX_DATA_INIT
	m_pBlockInfo=0;
}


void CModelSpecificPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModelSpecificPage)
	DDX_Text(pDX, IDC_MinHeight, m_dMinHeight);
	DDX_Text(pDX, IDC_MaxHeight, m_dMaxHeight);
	DDX_Text(pDX, IDC_FlyOverlap, m_flyoverlap);
	DDX_Text(pDX, IDC_StripOverlap, m_stripoverlap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModelSpecificPage, CDialog)
	//{{AFX_MSG_MAP(CModelSpecificPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelSpecificPage message handlers
void CModelSpecificPage::OnActivate()
{
	
}

BOOL CModelSpecificPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_pBlockInfo)
	{
		m_dMaxHeight=m_pBlockInfo->GetMaxGrdElev();
		m_dMinHeight=m_pBlockInfo->GetMinGrdElev();
		
		m_flyoverlap=m_pBlockInfo->GetFlyOverlap();
		m_stripoverlap=m_pBlockInfo->GetStripOverlap();
		//to add 
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

