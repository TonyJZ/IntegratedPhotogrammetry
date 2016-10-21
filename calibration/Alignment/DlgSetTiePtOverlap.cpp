// DlgSetTiePtOverlap.cpp : implementation file
//

#include "stdafx.h"
#include "Alignment.h"
#include "DlgSetTiePtOverlap.h"
#include "afxdialogex.h"


// CDlgSetTiePtOverlap dialog

IMPLEMENT_DYNAMIC(CDlgSetTiePtOverlap, CDialog)

CDlgSetTiePtOverlap::CDlgSetTiePtOverlap(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetTiePtOverlap::IDD, pParent)
{

	m_minTiePtOverlap = 2;
}

CDlgSetTiePtOverlap::~CDlgSetTiePtOverlap()
{
}

void CDlgSetTiePtOverlap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Overlap, m_minTiePtOverlap);
}


BEGIN_MESSAGE_MAP(CDlgSetTiePtOverlap, CDialog)
END_MESSAGE_MAP()


// CDlgSetTiePtOverlap message handlers
