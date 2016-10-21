#ifndef _OUTPUT_Edit_h_
#define _OUTPUT_Edit_h_

/////////////////////////////////////////////////////////////////////////////
// COutputEdit window

enum Scroll_To
{
	scroll_to_beginning=0,
	scroll_to_end=1,
};

enum Msg_Style	
{
	msg_new,
	msg_append,
};

typedef enum alm_guiScrollPos{
	scroll_Begin = 0,		//鼠标滚动到输出框头
		scroll_End = 1,		//鼠标滚动到输出框尾
		
}alm_guiScrollPos;	

typedef enum alm_OutputStyle{
	New_Msg = 0,				//清空消息框内容后输出新消息
		Append_Msg = 1,		//在现有消息框内容后添加消息
		
}alm_OutputStyle;

class COutputEdit : public CEdit
{
	// Construction
public:
	COutputEdit();
	
	// Attributes
public:
	
	// Operations
public:
	void  AppendString (char* str, Msg_Style style, Scroll_To pos);
	BOOL  IsSelectable() { return m_bSelectable; }
	void  AllowSelection (BOOL bAllowSelect) { m_bSelectable = bAllowSelect; }
	void  Erase();
//	void  ScrollToBeging();
//	void 
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputEdit)
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	virtual ~COutputEdit();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(COutputEdit)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
protected:
	BOOL  m_bSelectable;                          // flag: user can select text in control
	CString m_strInfo;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif

// End OutputEdit.h