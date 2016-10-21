// EnvDialog.cpp : implementation file
//

#include "stdafx.h"
#include "lidBasLib.h"
#include "EnvDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnvDialog dialog


CEnvDialog::CEnvDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CEnvDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnvDialog)
	m_strCurDir = _T("");
	m_strSysDir = _T("");
	m_strTempDir = _T("");
	//}}AFX_DATA_INIT
}


void CEnvDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnvDialog)
	DDX_Text(pDX, IDC_Cur_Dir, m_strCurDir);
	DDX_Text(pDX, IDC_Sys_Dir, m_strSysDir);
	DDX_Text(pDX, IDC_Temp_Dir, m_strTempDir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEnvDialog, CDialog)
	//{{AFX_MSG_MAP(CEnvDialog)
	ON_BN_CLICKED(IDC_Cur_Button, OnCurButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnvDialog message handlers

void CEnvDialog::OnCurButton() 
{
	
	
}

void CEnvDialog::SetSysDir(char *pSysDir)
{
	m_strSysDir = pSysDir;
}

void CEnvDialog::SetCurDir(char *pCurDir)
{
	m_strCurDir = pCurDir;
}

void CEnvDialog::SetTempDir(char *pTempDir)
{
	m_strTempDir = pTempDir;
}
