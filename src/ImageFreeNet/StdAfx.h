// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E4E7C864_CBCA_4711_9F28_6E1D6D08A5E8__INCLUDED_)
#define AFX_STDAFX_H__E4E7C864_CBCA_4711_9F28_6E1D6D08A5E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#ifdef _IMAGE_FREENET_
#define  _Image_FreeNet_  __declspec(dllexport)
#else
#define  _Image_FreeNet_  __declspec(dllimport)	
#endif

#ifndef _IMAGE_FREENET_
#ifdef _DEBUG
#pragma comment(lib,"ImageFreeNet.lib")
#else
#pragma comment(lib,"ImageFreeNet.lib")
#endif
#endif

#include "orsBase\orsIPlatform.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E4E7C864_CBCA_4711_9F28_6E1D6D08A5E8__INCLUDED_)
