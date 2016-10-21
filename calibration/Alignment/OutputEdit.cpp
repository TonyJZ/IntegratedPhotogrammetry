#include "stdafx.h"
#include "OutputEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputEdit

COutputEdit::COutputEdit()
{
	m_bSelectable = TRUE;
	m_strInfo.Empty();
}

COutputEdit::~COutputEdit()
{
}

BEGIN_MESSAGE_MAP(COutputEdit, CEdit)
//{{AFX_MSG_MAP(COutputEdit)
ON_WM_SETFOCUS()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputEdit operations

void COutputEdit::AppendString(char* str, Msg_Style style, Scroll_To pos)
{
//	CString   strBuffer;    // current contents of edit control
	
	// Append string
//	GetWindowText (strBuffer);
	if(style==msg_new)
		m_strInfo.Empty();

	if (!m_strInfo.IsEmpty())
		m_strInfo += "\r\n";
	m_strInfo += str;
	SetWindowText (m_strInfo);
	
	// Scroll the edit control
	if(pos==scroll_to_beginning)
		LineScroll (0, 0);
	else if(pos==scroll_to_end)
		LineScroll (GetLineCount(), 0);
}

/////////////////////////////////////////////////////////////////////////////
// COutputEdit message handlers

void COutputEdit::OnSetFocus(CWnd* pOldWnd) 
{
	// Don't allow user to select text
	if (m_bSelectable)
		CEdit::OnSetFocus (pOldWnd);
	else
		pOldWnd->SetFocus();
}

void COutputEdit::Erase()
{
	m_strInfo.Empty();
	SetWindowText (m_strInfo);
	LineScroll(0,0);
}

