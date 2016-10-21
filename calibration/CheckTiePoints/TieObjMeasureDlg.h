// TieObjMeasureDlg.h : header file
//

#if !defined(AFX_TIEOBJMEASUREDLG_H__A34D9CF5_C2EE_4C0C_803D_B1EEC3F6D944__INCLUDED_)
#define AFX_TIEOBJMEASUREDLG_H__A34D9CF5_C2EE_4C0C_803D_B1EEC3F6D944__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GridCtrl_src/GridCtrl.h"
#include "ThumbnailControl.h"
#include "Align_def.h"
#include "AlignPrj.h"
#include "afxwin.h"

#include "ImgViewerDlg.h"
#include "CombineAdjustment\CATTStruct_def.h"
/////////////////////////////////////////////////////////////////////////////
// CTieObjMeasureDlg dialog


class CTieObjMeasureDlg : public CDialog
{
// Construction
public:
	CTieObjMeasureDlg(char *pAlgName, HWND LinkWindow, CWnd* pParent = NULL);	// standard constructor
	~CTieObjMeasureDlg();

	void InitTieObjList();
	void InitMeasuredValueList();

	void RemoveMeasureItems();
	void RemoveTieObjID();
	void LayoutMeasureItems(int tieID);	//将指定连接点的量测信息展开在grid框中
	void LayoutTieObjID();				//将连接点链表中的有效连接点展开到grid框中

	void SetSelectTieObj(int row);
	
	//根据grid行号取tieID(从1开始编号)
	int  GetSelectTieID(int gridRow); 
	//根据tieID取TieChain
	TieObjChain* GetTieChain(int tieID);
	//根据连接行号和测量目标行号，提取对象
	TieObject *GetTieObject(int tieRow, int itemRow);

	void ProcessMeasureInfo(linkMSG *msg);

	void BroadcastCreateNewTieObj();

	void InsertMeasureItem(TieObject *pObj);

// Dialog Data
	//{{AFX_DATA(CTieObjMeasureDlg)
	enum { IDD = IDD_TIEOBJMEASURE_DIALOG };
	int		m_ObjType;
	//}}AFX_DATA

	CGridCtrl	m_TieObjGrid;
	CGridCtrl	m_MeasuredGrid;

	char        *m_pAlgName;
	CAlignPrj   m_AlgPrj;

	std::vector<ATT_tiePoint>	m_tiePts;	//连接点
	ATT_tiePoint                m_curTie;

	CArray<TieObjChain*, TieObjChain*>  *m_pTieList;
	TieObjChain  *m_pCurChain;
	int  m_SelObjRow;	//在TieObjGrid选中的行号

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTieObjMeasureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

private:
	bool readTiepoints(char *fileName);
	int getMaxOverlap();

	void ClearViewerDlg();

	void DisplayLiDARPoint( ATT_LaserPoint lasPt );

	void DisplayImagePoint( ATT_ImagePoint imgPt);

	void DrawThumbnail(CString filename, orsPOINT2D pt2D, orsIImageSource *pImg);

// Implementation
protected:
	HICON m_hIcon;

	HWND  m_LinkWindow;

	int m_maxDlgNum;
//	std::vector<CImgViewerDlg*>	m_pImgViewerBuf;
//	std::vector<orsIImageSource*> m_pImgSourceBuf;
//	std::vector<int> m_sIDBuf;

	CThumbNailControl	m_cThumbFrame;

	// Generated message map functions
	//{{AFX_MSG(CTieObjMeasureDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBUTTONNewObj();
	afx_msg void OnBUTTONDeleteObj();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnBUTTONSave();
	afx_msg void OnBUTTONDeleteItem();
	afx_msg void OnSaveAllObjs();
	//}}AFX_MSG
	afx_msg void OnTieObjGrid(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTieObjGridStartSelChange(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridLButtonDbClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedIsgcp();
	void EnableGCPCheck(BOOL bEnable);
	void EnableGCPEdit(BOOL bEnable);
	void EnableTiepointType(BOOL bEnable);
	
private:
	BOOL m_bGCP;
	double m_dXGcp;
	double m_dYGcp;
	double m_dZGcp;

	int m_rectWid;
	int m_rectHei;

public:
	afx_msg void OnEnChangeGcpX();
	afx_msg void OnEnChangeGcpY();
	afx_msg void OnEnChangeGcpZ();
	int m_TiePointType;
	afx_msg void OnBnClickedRadioUndefine();
	afx_msg void OnBnClickedRadioHorver();
	afx_msg void OnBnClickedRadioHor();
	afx_msg void OnBnClickedRadioVer();
	afx_msg void OnListOpenalignproject();
	afx_msg void OnListLoadtiepoints();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIEOBJMEASUREDLG_H__A34D9CF5_C2EE_4C0C_803D_B1EEC3F6D944__INCLUDED_)
