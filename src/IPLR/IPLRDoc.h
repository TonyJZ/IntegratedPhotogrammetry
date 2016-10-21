// IPLRDoc.h : interface of the CIPLRDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPLRDOC_H__748842BF_BDE7_49B7_BEDA_054FA5EEF881__INCLUDED_)
#define AFX_IPLRDOC_H__748842BF_BDE7_49B7_BEDA_054FA5EEF881__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CIPLRDoc : public CDocument
{
protected: // create from serialization only
	CIPLRDoc();
	DECLARE_DYNCREATE(CIPLRDoc)

// Attributes
public:

	CArray<CString, CString>	m_imgNameList;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPLRDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIPLRDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CIPLRDoc)
	afx_msg void OnSIFTKeyPoint();
	afx_msg void OnCheckKeypoint();
	afx_msg void OnMatchINIT();
	afx_msg void OnExporImgaop();
	afx_msg void OnExportblv();
	afx_msg void OnExportImgiop();
	afx_msg void OnExportMatchImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUpdateSiftKeypoint(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPLRDOC_H__748842BF_BDE7_49B7_BEDA_054FA5EEF881__INCLUDED_)
