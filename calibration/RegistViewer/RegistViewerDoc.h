// RegistViewerDoc.h : interface of the CRegistViewerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTVIEWERDOC_H__FA1EBC77_4823_4C4E_AC00_B35B92791312__INCLUDED_)
#define AFX_REGISTVIEWERDOC_H__FA1EBC77_4823_4C4E_AC00_B35B92791312__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "orsImage/orsIImageService.h"
#include "orsImage/orsIImageSourceRotator.h"
#include "orsMap\orsILayerCollection.h"


class CRegistViewerView;

class CRegistViewerDoc : public CDocument
{
protected: // create from serialization only
	CRegistViewerDoc();
	DECLARE_DYNCREATE(CRegistViewerDoc)

// Attributes
public:
	ref_ptr<orsIImageSourceReader> m_imgReader;
	
	ref_ptr<orsIImageGeometry>		m_imgGeo;

//	ref_ptr<orsIImageSourceRotator>  m_rotator;
	ref_ptr<orsIImageChain> m_imageChain;
	ref_ptr<orsIImageSource> m_pImg;

	std::string m_imgPathName;

	ref_ptr <orsILayerCollection> m_pLayerCollection;
// Operations
public:
	orsILayerCollection *getLayerCollection()	{	return m_pLayerCollection.get(); }
	orsIImageSource	*getViewSpace()	
	{	
		return m_pLayerCollection->getActiveDataFrame()->getViewSpace();
		//return m_viewSpace.get();	
 	};
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegistViewerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

	void ExtractTieObjects(char *apjName);

// Implementation
public:
	virtual ~CRegistViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CRegistViewerView* GetMainView();

// Generated message map functions
protected:
	//{{AFX_MSG(CRegistViewerDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAddapj();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTVIEWERDOC_H__FA1EBC77_4823_4C4E_AC00_B35B92791312__INCLUDED_)
