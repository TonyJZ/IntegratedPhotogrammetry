// DlgTrackPt.cpp : implementation file
//

#include "stdafx.h"
#include "Alignment.h"
#include "DlgTrackPt.h"


// CDlgTrackPt dialog

IMPLEMENT_DYNAMIC(CDlgTrackPt, CDialog)

CDlgTrackPt::CDlgTrackPt(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTrackPt::IDD, pParent)
	, m_strMatchFile(_T(""))
	, m_strTrackFile(_T(""))
	, m_strImgList(_T(""))
	, m_strKeyList(_T(""))
{

}

CDlgTrackPt::~CDlgTrackPt()
{
}

void CDlgTrackPt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Match, m_strMatchFile);
	DDX_Text(pDX, IDC_EDIT_Tracks, m_strTrackFile);
	DDX_Text(pDX, IDC_EDIT_ImgList, m_strImgList);
	DDX_Text(pDX, IDC_EDIT_KeyList, m_strKeyList);
}


BEGIN_MESSAGE_MAP(CDlgTrackPt, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_Match, &CDlgTrackPt::OnBnClickedButtonMatch)
	ON_BN_CLICKED(IDC_BUTTON_Track, &CDlgTrackPt::OnBnClickedButtonTrack)
	ON_BN_CLICKED(IDC_BUTTON_ImgList, &CDlgTrackPt::OnBnClickedButtonImglist)
	ON_BN_CLICKED(IDC_BUTTON_KeyList, &CDlgTrackPt::OnBnClickedButtonKeylist)
END_MESSAGE_MAP()


// CDlgTrackPt message handlers

void CDlgTrackPt::OnBnClickedButtonMatch()
{
	CFileDialog  dlg(TRUE,"匹配点",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.txt)|*.txt|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	m_strMatchFile=dlg.GetPathName();

	UpdateData(FALSE);
}

void CDlgTrackPt::OnBnClickedButtonTrack()
{
	CFileDialog  dlg(FALSE,"连接点",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"(*.txt)|*.txt||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	m_strTrackFile=dlg.GetPathName();

	UpdateData(FALSE);
}

void CDlgTrackPt::OnBnClickedButtonImglist()
{
	CFileDialog  dlg(TRUE,"影像列表",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.txt)|*.txt|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	m_strImgList=dlg.GetPathName();

	UpdateData(FALSE);
}

void CDlgTrackPt::OnBnClickedButtonKeylist()
{
	CFileDialog  dlg(TRUE,"特征点列表",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.txt)|*.txt|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	m_strKeyList=dlg.GetPathName();

	UpdateData(FALSE);
}
