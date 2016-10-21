// AlignmentDoc.h : interface of the CAlignmentDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALIGNMENTDOC_H__4D4DEE14_296B_4F15_AA28_91220A30A302__INCLUDED_)
#define AFX_ALIGNMENTDOC_H__4D4DEE14_296B_4F15_AA28_91220A30A302__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAlignmentDoc : public CDocument
{
protected: // create from serialization only
	CAlignmentDoc();
	DECLARE_DYNCREATE(CAlignmentDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlignmentDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAlignmentDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAlignmentDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALIGNMENTDOC_H__4D4DEE14_296B_4F15_AA28_91220A30A302__INCLUDED_)
