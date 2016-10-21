#ifndef CUSTOM_TREE_CONTROL_H_INCLUDED
#define CUSTOM_TREE_CONTROL_H_INCLUDED


#include <afxext.h>
#include <afxcview.h>
#include <vector>

#include "resource.h"

#include "orsMap\orsILayerTreeItem.h"
#include "orsMap\orsILayerTreeViewer.h"

#include "Global.h"
///////////////////////////////////////////////////////////////////////////////
//
// Define the tree control for use in show/hide prefs page dialog
//
///////////////////////////////////////////////////////////////////////////////

// item state flag
//
const int ciExpanded    = 1;
const int ciChecked     = 2;
const int ciDisabled    = 4;


// item styles
//
const int ciEditable    = 1;
const int ciCheckedNode = 2;
const int ciRadioNode   = 4;
const int ciSelectable  = 8;

//kind of item data
const int ciRoot = 1;
const int ciGroup = 2;
const int ciLayer = 4;
const int ciSymbol = 8;

#include <map>
using namespace std;

typedef struct _tagItemRects
{
	CRect m_oLeftRect;
	CRect m_oExpandRect;
	CRect m_oCheckRect;
	CRect m_oTextRect;
}
ITEM_RECTS, *P_ITEM_RECTS;


typedef map< HTREEITEM, P_ITEM_RECTS > ITEM_RECT_MAP;
typedef ITEM_RECT_MAP::iterator        ITEM_RECTS_ITER;

//////////////////////////////////////////////////////////////////////////

enum ItemRendType{
	COULORIMG_R,
		COULORIMG_G,
		COULORIMG_B,
		GRAYIMG_ONE,
		GRAYIMG_TWO,
		GRAYIMG_THREE,
		TXT_VALUE,
		TXT_RGB,
		ITEM_DEF
};//add by liubin


typedef struct DrawParam {
	
	double m_StartX;       //当前坐标原点横坐标
	double m_StartY;       //当前坐标原点纵坐标 
	int m_ScreenWidth;  //当前屏幕宽度
	int m_ScreenHeigh;  //当前屏幕高度
	double m_Top;
	double m_left;
	float m_Scale;      //当前绘图比例尺     
} DRAWPARAM;



struct treeItemDATA {
public:
	orsILayerTreeItem *pLayerTreeItem;
	int rendFlag;

	WORD  style;
	WORD  state;

public:
	treeItemDATA() : pLayerTreeItem(NULL), rendFlag(0), style(0), state(0)	{};

	bool checkableStyle()	{ return ( 0 != (style & ciCheckedNode) );	}

	bool isChecked()	{	return pLayerTreeItem->IsVisible(); }

	void toggleCheckState( bool bSetChildren )	{	
		pLayerTreeItem->SetVisible(	!pLayerTreeItem->IsVisible(), bSetChildren );	
	}

	bool isDisabled()	{	return (0 != (state & ciDisabled) ); }
	
	void disable()	{ 
		if( isDisabled() ) 
			state = ( state & ~ciDisabled );
		else
			state = ( state | ciDisabled );
	}
};



#include "orsBase/orsMemoryPool.h"

class CLayerTreeCtrl : public CTreeCtrl, public orsILayerTreeViewer
{
protected:
	CBCGPMenuBar m_wndMenuBar;

	orsMemoryPool <treeItemDATA> m_itemDataPool;

	orsILayerCollection *m_pLayerCollection;

public:
	CLayerTreeCtrl();
	virtual ~CLayerTreeCtrl();

	void AttachLayerCollection( orsILayerCollection *pLayerCollection );

	bool InsertDataFrame( orsIDataFrame *pDataFrame );

	bool InsertLayerTreeItem( orsILayerGroup *pLayerGroup, orsILayerTreeItem *pInsertLayer, orsILayerTreeItem* pInsertBefore );

	void NotifyUpdataRenderItem(orsILayerTreeItem *pLayer);

	void DeleteLayerTreeItem( orsILayerTreeItem *pDeleteLayer );

	void AttachLayerTree( orsILayerGroup *RootGroup );

	void MoveItem( HTREEITEM hItem, HTREEITEM hItemTo, HTREEITEM hItemAfter );

	void SetCurrentLayer( orsILayerTreeItem *pLayer );

public:
	COLORREF SetBackGroundColor( COLORREF xNewColor )
	{
		COLORREF xReturn   = m_xBackGroundColor;
		m_xBackGroundColor = xNewColor;
		SetBkColor( xNewColor );
		return xReturn;
	}
	
	HTREEITEM FindThePlace(CPoint point, HTREEITEM hItemDrop, HTREEITEM* phItemAfter);
	

	//{{AFX_VIRTUAL( CLayerTreeCtrl )
	virtual BOOL PreTranslateMessage( MSG* pxMsg );
	//}}AFX_VIRTUAL

protected:
	void InvalidateItem( HTREEITEM hItem, BOOL bInvalidateSelf );
	void ChangeItemState( HTREEITEM hItem, bool bSetChildren  );

	BOOL CanEdit( HTREEITEM hTreeItem );
	int  GetIndentLevel( HTREEITEM hTreeItem );
	BOOL IsItemChecked( HTREEITEM hItem );
	void EnableParentTree( HTREEITEM hTreeItem, bool bEnable );
	void DisableChildrenItem(HTREEITEM hTreeItem);
	void OnEndDrag(UINT nFlags, CPoint point);
	void FinishDragging();
	int ItemKind(HTREEITEM hItem);
	
	//{{AFX_MSG( CLayerTreeCtrl )
	afx_msg void OnLButtonDown( UINT iFlags, CPoint oPoint );
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown( UINT iChar, UINT iRepeatCount, UINT iFlags );
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCustomDraw( NMHDR* pxNMHDR, LRESULT* plResult );
	afx_msg void OnRButtonClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnButtonClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnBeginDrag(NMHDR* pxNMHDR, LRESULT* plResult);
	afx_msg void OnBeginLabelEdit( NMHDR* pxNMHDR, LRESULT* plResult );
	afx_msg void OnEndLabelEdit( NMHDR* pxNMHDR, LRESULT* plResult );
	
//#if _MSC_VER >= 1400
	afx_msg void OnItemExpanding( NMHDR* pxNMHDR, LRESULT* plResult ); 
//#else
//	afx_msg BOOL OnItemExpanding( NMHDR* pxNMHDR, LRESULT* plResult ); 
//#endif
	
	DECLARE_MESSAGE_MAP()
		
private:
	HTREEITEM      m_hEditNode;
	HTREEITEM      m_hItemDrag;
	HTREEITEM      m_hSelect;
	ITEM_RECT_MAP  m_oItemRects;
	bool           m_bQuitEdits;
	bool           m_bDraging;
	COLORREF       m_xBackGroundColor;
	CImageList     m_ImgExpandCollapse;
	HCURSOR        m_hDropMoveCursor;
	HCURSOR        m_hNoDropCursor;
	HCURSOR		 m_hPreviousCursor;
	HCURSOR		 m_hCurrentCursor;
	
	CPoint         m_PreInsertPointFrom;
	CPoint         m_PreInsertPointTo;
		
public:
	HTREEITEM      m_rootItem;
	HTREEITEM      m_lastItem;

};


#endif
