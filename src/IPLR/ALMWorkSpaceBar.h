#if !defined(ALM_WORKSPACEBAR_H__9D33B45D_4D98_423D_B395_9E250593DE9E__INCLUDED_)
#define ALM_WORKSPACEBAR_H__9D33B45D_4D98_423D_B395_9E250593DE9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BCGPDockingControlBar.h" 
//#include "ALMPrjManager.h"

class CALMWorkspaceBar : public CBCGPDockingControlBar
{
public:
	CALMWorkspaceBar();
	
//	void SetALMInfo(CALMPrjManager *pALM);


private:
	void	InitTree();
	void	CleanTree();

	// Attributes
protected:
//	CALMPrjManager	*m_pALM;
	CTreeCtrl	m_wndTree;

	CImageList	m_workSpaceImages;

//	HTREEITEM	m_hImgRoot;		//像片列表
//	HTREEITEM	m_hLiDRoot;		//点云列表
	HTREEITEM	m_hBlockRoot;		//产品列表

	HTREEITEM	m_hImgRoot;
	HTREEITEM	m_hOrthoRoot;
	HTREEITEM	m_hDTMRoot;
	

private:
	void	CreateWorkSpaceImages();
//	void	LayoutImage(CArray<IMGLIST,IMGLIST> *pImgList);
//	void	LayoutLiDAR(CArray<LASLIST,LASLIST>	*pLidList);
	void	LayoutProduct(CArray<CString,CString> *pDEMList,
						  CArray<CString,CString> *pDOMList,
						  CArray<CString,CString> *pTMList);

	void	OutputImageInfo(HTREEITEM hItem);
	void	OutputLiDARInfo(HTREEITEM hItem);
	void	OutputProductInfo(HTREEITEM hItem);

	void	DisplayImage(HTREEITEM hItem);

	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceBar)
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	virtual ~CALMWorkspaceBar();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CWorkspaceBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSPACEBAR_H__9D33B45D_4D98_423D_B395_9E250593DE9E__INCLUDED_)