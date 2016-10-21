#pragma once


#include "AlignPrj.h"
// CSelectPointDlg dialog

class CSelectPointDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectPointDlg)

public:
	CSelectPointDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectPointDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SelectTiept };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void ExtractTiePointLut(CAlignPrj &algPrj);
	void ExtractTiePlaneLut(CAlignPrj &algPrj);

	bool CheckPtInPolygon(POINT3D *pt, POINT3D *polygon, int ptNum);

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPlaneapj;
	CString m_strSIFTapj;
	CString m_strSelPtapj;
	double m_BufSize;
	afx_msg void OnBnClickedButtonplane();
	afx_msg void OnBnClickedButtonTiepoint();
	afx_msg void OnBnClickedButtonselpoint();
	afx_msg void OnBnClickedOk();

private:
// 	LidMC_VP *m_pLidVP;
// 	LidMC_Plane *m_pLidPlanes;

	long *m_pVPTieLut;		//连接点的查找表矩阵
	long *m_pPlaneTieLut;	//连接平面的查找表矩阵
};
