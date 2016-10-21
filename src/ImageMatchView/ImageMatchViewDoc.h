// ImageMatchViewDoc.h : interface of the CImageMatchViewDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEMATCHVIEWDOC_H__AB832A44_8CA1_4F44_BDB8_7DBA38B90726__INCLUDED_)
#define AFX_IMAGEMATCHVIEWDOC_H__AB832A44_8CA1_4F44_BDB8_7DBA38B90726__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CImageMatchViewDoc : public CDocument
{
protected: // create from serialization only
	CImageMatchViewDoc();
	DECLARE_DYNCREATE(CImageMatchViewDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageMatchViewDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImageMatchViewDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CImageMatchViewDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEMATCHVIEWDOC_H__AB832A44_8CA1_4F44_BDB8_7DBA38B90726__INCLUDED_)
