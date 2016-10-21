#pragma once


// CCamSettingDlg dialog

class CCamSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CCamSettingDlg)

public:
	CCamSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCamSettingDlg();

// Dialog Data
	enum { IDD = IDD_CameraSettingDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_x0;
	double m_y0;
	double m_f;
	int m_ImgWid;
	int m_ImgHei;
	double m_PixelWid;
	double m_PixelHei;
	double m_k1;
	double m_k2;
	double m_k3;
	double m_p1;
	double m_p2;
};
