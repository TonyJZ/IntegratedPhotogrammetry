// orsI3DViewDoc.h: interface for the orsI3DViewDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORSI3DVIEWDOC_H__9DFB4210_0CF8_40A6_AE13_81AE6E504157__INCLUDED_)
#define AFX_ORSI3DVIEWDOC_H__9DFB4210_0CF8_40A6_AE13_81AE6E504157__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "3dView.h"

interface orsI3DViewDoc
{
	virtual void OnDraw3D( C3DView *pView ) = 0;
};

#endif // !defined(AFX_ORSI3DVIEWDOC_H__9DFB4210_0CF8_40A6_AE13_81AE6E504157__INCLUDED_)
