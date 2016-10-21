#include "stdafx.h"

#include <vector>
#include <windowsx.h>

using namespace std;

#include "layerTreeControl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP( CLayerTreeCtrl, CTreeCtrl )
//{{AFX_MSG_MAP( CLayerTreeCtrl )
ON_WM_LBUTTONDOWN()
ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
ON_WM_KEYDOWN()
ON_WM_LBUTTONDBLCLK()
ON_NOTIFY_REFLECT( NM_CUSTOMDRAW, OnCustomDraw )
ON_NOTIFY_REFLECT(NM_RCLICK,OnRButtonClick)
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_NOTIFY_REFLECT(NM_CLICK, OnButtonClick)
	//}}AFX_MSG_MAP
ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
ON_NOTIFY_REFLECT( TVN_BEGINLABELEDIT, OnBeginLabelEdit )
ON_NOTIFY_REFLECT( TVN_ENDLABELEDIT, OnEndLabelEdit )
// ON_NOTIFY_REFLECT_EX( TVN_ITEMEXPANDED, OnItemExpanding )
ON_NOTIFY_REFLECT( TVN_ITEMEXPANDED, OnItemExpanding )
END_MESSAGE_MAP()



typedef LPARAM(*PFNMTICOPYDATA)(const CTreeCtrl*, HTREEITEM, LPARAM);

static LPARAM CopyData(const CTreeCtrl* tree, HTREEITEM hItem, LPARAM lParam)
{
	if (lParam == 0)
		return 0;
	CString* ps = (CString*)lParam;
	CString* psNew = new CString(*ps);
	return (LPARAM)psNew;
}


// 递归移动树节点
HTREEITEM MoveTreeItem(BOOL bExpand, CTreeCtrl* tree, HTREEITEM hItem, HTREEITEM hItemTo,HTREEITEM hItemAfter, 
					   BOOL bCopyOnly = FALSE, PFNMTICOPYDATA pfnCopyData = NULL, 
					   HTREEITEM hItemPos = TVI_LAST)
{
	if (hItem == NULL || hItemTo == NULL)
		return NULL;
	//   if (hItem == hItemTo || hItemTo == tree->GetParentItem(hItem))
	//     return hItem;
	if(hItem == hItemTo)
		return NULL;
	// check we're not trying to move to its descendant
	HTREEITEM hItemParent = hItemTo;
	while (hItemParent != TVI_ROOT && (hItemParent = tree->GetParentItem(hItemParent)) != NULL) {
		if (hItemParent == hItem)
			return NULL;
	}
	// copy items to new location, recursively, then delete old heirarchy
	// get text, and other info
	CString sText = tree->GetItemText(hItem);
	TVINSERTSTRUCT tvis;
	tvis.item.mask = TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | 
		TVIF_SELECTEDIMAGE | TVIF_STATE;
	tvis.item.hItem = hItem;
	// we don't want to copy selection/expanded state etc
	tvis.item.stateMask = (UINT)-1 & ~(TVIS_DROPHILITED | TVIS_EXPANDED | 
		TVIS_EXPANDEDONCE | TVIS_EXPANDPARTIAL | TVIS_SELECTED);
	tree->GetItem(&tvis.item);
	tvis.hParent = hItemTo;
	tvis.hInsertAfter = hItemAfter;
	// if we're only copying, then ask for new data
	if (bCopyOnly && pfnCopyData != NULL)
		tvis.item.lParam = pfnCopyData(tree, hItem, tvis.item.lParam);
	HTREEITEM hItemNew = tree->InsertItem(&tvis);
	tree->SetItemText(hItemNew, sText);
	
	// now move children to under new item
	HTREEITEM hItemChild = tree->GetChildItem(hItem);
	while (hItemChild != NULL)
	{
		HTREEITEM hItemNextChild = tree->GetNextSiblingItem(hItemChild);
		MoveTreeItem(false,tree, hItemChild, hItemNew,TVI_LAST, bCopyOnly, pfnCopyData);
		hItemChild = hItemNextChild;
	}
	
	if (! bCopyOnly)
	{
		// clear item data, so nothing tries to delete stuff based on lParam
		tree->SetItemData(hItem, 0);
		// no (more) children, so we can safely delete top item
		tree->DeleteItem(hItem);
	}
	if (bExpand)
	{
		tree->Expand(hItemNew, TVE_EXPAND);
	}
	return hItemNew;
}



void CreateCustomPopupMenu( CMenu &popMenu, const int& ciItemKind)
{
	popMenu.CreatePopupMenu();
	switch( ciItemKind )	{
	case ciRoot:
		popMenu.AppendMenu(MF_STRING|MF_POPUP, ID_MENUITEM_DATAFRAME_ACTIVATE, _T("激活"));
		popMenu.AppendMenu(MF_SEPARATOR);
	case ciGroup:
		popMenu.AppendMenu(MF_STRING|MF_POPUP, ID_MENUITEM_NEWGROUP, _T("新建图层组"));
		popMenu.AppendMenu(MF_SEPARATOR);
	case ciLayer:
		popMenu.AppendMenu(MF_STRING|MF_POPUP, ID_MENUITEM_DEL, _T("删除"));	
	}
}


//////////////////////////////////////////////////////////////////////////

CLayerTreeCtrl::CLayerTreeCtrl( void )
	: m_bQuitEdits( false ), m_xBackGroundColor( GetSysColor( COLOR_WINDOW ) ),
	m_PreInsertPointFrom(0,0), 	m_PreInsertPointTo(0,0)
{
	///these code here are used to initialize the ImageList which contains
	///the plus icon and subtract icon
	CWinApp *pApp = (CWinApp*)AfxGetApp();

	m_ImgExpandCollapse.Create(16,16,ILC_COLOR24|ILC_MASK,0,2);

	// 0
	HICON hiconplus = pApp->LoadIcon(IDI_Plus);
	m_ImgExpandCollapse.Add(hiconplus);

	// 1
	HICON hiconsub  = pApp->LoadIcon(IDI_Subtract);
	m_ImgExpandCollapse.Add(hiconsub);

	// 2
	HICON hiconlayer = pApp->LoadIcon(IDI_ICON_LAYERS);
	m_ImgExpandCollapse.Add(hiconlayer);

	///////////
	m_hPreviousCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_bDraging = false;
	m_hItemDrag=NULL;
	m_hSelect = NULL;

	m_pLayerCollection = NULL;
	
}


CLayerTreeCtrl::~CLayerTreeCtrl( void )
{
    ITEM_RECTS_ITER oIter;
    for ( oIter  = m_oItemRects.begin();
	oIter != m_oItemRects.end();
	oIter++ )
	{
		delete (*oIter).second;
	}
}


void CLayerTreeCtrl::AttachLayerCollection( orsILayerCollection *pLayerCollection )
{
	m_pLayerCollection = pLayerCollection;

	pLayerCollection->SetLayerTreeViewer( (orsILayerTreeViewer*)this );

	//////////////////////////////////////////////////////////////
	orsILayerGroup* layerTreeRoot = pLayerCollection->GetLayerTree();

	m_rootItem = InsertItem( "Layers" );

	
	treeItemDATA *itemData = m_itemDataPool.PoolAlloc();
	
	itemData->pLayerTreeItem = layerTreeRoot;

	
	SetItemData( m_rootItem, (DWORD)itemData );
	
	layerTreeRoot->m_hLayerTreeItem = m_rootItem;	
}



void CLayerTreeCtrl::MoveItem( HTREEITEM hItem, HTREEITEM hItemTo, HTREEITEM hItemAfter )
{
	if (hItem == NULL || hItemTo == NULL)
		return;
	
	//从原先的Group中移出
	HTREEITEM hItemMoveFrom = GetParentItem(hItem);
	
	treeItemDATA *pTreeItemDataTemp = (treeItemDATA *)GetItemData(hItemMoveFrom);
	
	orsILayerGroup* pFromGroup = (orsILayerGroup* )pTreeItemDataTemp->pLayerTreeItem;
	
	pTreeItemDataTemp = (treeItemDATA *)GetItemData(hItem);
	
	ref_ptr<orsILayerTreeItem> pItem = pTreeItemDataTemp->pLayerTreeItem;

	int iItem = pFromGroup->GetChildIndex( pItem.get() );
	pFromGroup->RemoveItem(iItem);

	//////////////////////////////////////////////////////////////////////////
	//加入当前的父节点下
	pTreeItemDataTemp = (treeItemDATA *)(GetItemData(hItemTo));
	
	orsILayerGroup* pToGroup = (orsILayerGroup*)(pTreeItemDataTemp->pLayerTreeItem);
	
	if( hItemAfter == TVI_FIRST)
	{
		pToGroup->InsertItem( pItem.get(), (orsILayerTreeItem *)ORS_TVI_FIRST );
	}
	else
	{
		pTreeItemDataTemp = (treeItemDATA *)(GetItemData( hItemAfter) );
		
		orsILayerTreeItem *pItemAfter = (orsILayerTreeItem*)(pTreeItemDataTemp->pLayerTreeItem);
		
		pToGroup->InsertItem( pItem.get(), pItemAfter );
	}
	
	//改变移动的Item的父节点
	pItem->SetParent( pToGroup );

}



bool CLayerTreeCtrl::InsertDataFrame( orsIDataFrame *pDataFrame )
{
	HTREEITEM hLayerTreeItem = InsertItem( pDataFrame->getDataFrameName() );

	orsILayerGroup *LayerTree = pDataFrame->getLayerTree();

	LayerTree->m_hLayerTreeItem = hLayerTreeItem;
	
	treeItemDATA *itemData = m_itemDataPool.PoolAlloc();
	
	itemData->pLayerTreeItem = LayerTree;
	
	itemData->rendFlag = 0;
	
	itemData->style = ciSelectable;	//| ciEditable
	
	SetItemData( hLayerTreeItem, (DWORD)itemData );
	
	LayerTree->m_pLayerTreeViewer = this;

	return true;
}

//
//	有两种情形
//	 (1) 插入的节点是逐项插入
//	 (2) 图层移动
//

bool CLayerTreeCtrl::InsertLayerTreeItem( orsILayerGroup *pParentLayerGroup, orsILayerTreeItem *pInsertLayer, orsILayerTreeItem* pInsertAfter )
{
	assert( pParentLayerGroup );
	assert( pInsertLayer );
	
	HTREEITEM hParentTreeItem;
	HTREEITEM hInsertAfter;
	
	hParentTreeItem = (HTREEITEM)pParentLayerGroup->m_hLayerTreeItem;
	
	switch( (unsigned long)pInsertAfter )	{
	case NULL:
	case ORS_TVI_FIRST:
		hInsertAfter = TVI_FIRST;
		break;
	case ORS_TVI_LAST:
		hInsertAfter = TVI_LAST;
		break;
	default:
		hInsertAfter = (HTREEITEM)pInsertAfter->m_hLayerTreeItem;
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	HTREEITEM hLayerTreeItem = InsertItem( pInsertLayer->GetLayerName(), hParentTreeItem, hInsertAfter );
	pInsertLayer->m_hLayerTreeItem = hLayerTreeItem;
	
	treeItemDATA *itemData = m_itemDataPool.PoolAlloc();
	
	itemData->pLayerTreeItem = pInsertLayer;
	
	itemData->rendFlag = 0;
	
	itemData->style = ciCheckedNode  | ciSelectable;	//| ciEditable
	
	SetItemData( hLayerTreeItem, (DWORD)itemData );
	
	pInsertLayer->m_pLayerTreeViewer = this;

	//////////////////////////////////////////////////////////////////////////
	// 图层组
	if( pInsertLayer->layerTreeItemType() != ORS_LTT_LAYER ) {
		pParentLayerGroup = (orsILayerGroup *)pInsertLayer;
		
		// 递归插入子节点
		for( int i=0; i< pParentLayerGroup->GetNumOfChildren(); i++ )
		{
			InsertLayerTreeItem( pParentLayerGroup, pParentLayerGroup->GetChildren(i),  (orsILayerTreeItem *)ORS_TVI_LAST );
		}
	}
	else	{	// 图层	
		int nItems;
		orsLayerStyleITEM *pStyleItems = pInsertLayer->GetStyleItemsOnTreeCtrl( nItems );
		
		for( int i=0; i<nItems; i++)
		{
			HTREEITEM hStyleItem= InsertItem( pStyleItems[i].tagStr, hLayerTreeItem );
			
			pStyleItems[i].hTreeItem = hStyleItem;
			
			treeItemDATA *itemData = m_itemDataPool.PoolAlloc();
			
			itemData->pLayerTreeItem = NULL;
			itemData->rendFlag = i;
			itemData->style =  ciSelectable;	// |ciEditable
			
			SetItemData( hStyleItem,  (DWORD)itemData );
		}
	}

	Expand( hParentTreeItem, TVE_EXPAND );

	//RedrawWindow();

	return true;
}

//跟其Render子节点
void CLayerTreeCtrl::NotifyUpdataRenderItem(orsILayerTreeItem *pLayer)
{
	assert( pLayer );
	
	// 	//////////////////////////////////////////////////////////////////////////
	// 图层组
	if( pLayer->layerTreeItemType() != ORS_LTT_LAYER ) {
		return;
	}
	else	{	// 图层	
		int nItems;
		orsLayerStyleITEM *pStyleItems = pLayer->GetStyleItemsOnTreeCtrl( nItems );
		
		int nSetedItem;
		
		nSetedItem = 0;
		
		HTREEITEM hParentItem;
		HTREEITEM hChildItem;
		HTREEITEM hNextItem;
		
		hParentItem = (HTREEITEM)pLayer->m_hLayerTreeItem;
		
		if (ItemHasChildren(hParentItem))
		{
			hChildItem = GetChildItem(hParentItem);	
			
			while (hChildItem != NULL)
			{
				hNextItem = GetNextItem(hChildItem, TVGN_NEXT);
				if(nSetedItem>=nItems)
					DeleteItem(hChildItem);
				else
				{
					SetItemText( hChildItem, pStyleItems[nSetedItem].tagStr );
					nSetedItem++;
				}
				
				hChildItem = hNextItem;
			}
			
			if(nSetedItem<nItems)
			{
				for (;nSetedItem<nItems;nSetedItem++)
				{
					HTREEITEM hStyleItem= InsertItem( pStyleItems[nSetedItem].tagStr, hParentItem );
					
					pStyleItems[nSetedItem].hTreeItem = hStyleItem;
					
					treeItemDATA *styleItemData = m_itemDataPool.PoolAlloc();
					
					styleItemData->pLayerTreeItem = NULL;
					styleItemData->rendFlag = nSetedItem;
					styleItemData->style = ciEditable | ciSelectable;
					
					SetItemData( hStyleItem,  (DWORD)styleItemData );
				}
			}
		}
		else
		{	
			for( int i=0; i<nItems; i++)
			{
				HTREEITEM hStyleItem= InsertItem( pStyleItems[i].tagStr, hParentItem );
				
				pStyleItems[i].hTreeItem = hStyleItem;
				
				treeItemDATA *styleItemData = m_itemDataPool.PoolAlloc();
				
				styleItemData->pLayerTreeItem = NULL;
				styleItemData->rendFlag = i;
				styleItemData->style = ciEditable | ciSelectable;
				
				SetItemData( hStyleItem,  (DWORD)styleItemData );
			}
		}
	}
}

//用于删除视图中的节点
void CLayerTreeCtrl::DeleteLayerTreeItem( orsILayerTreeItem* pDeleteLayer)
{	
	HTREEITEM delitem = (HTREEITEM)pDeleteLayer->m_hLayerTreeItem;

	// clear item data, so nothing tries to delete stuff based on lParam
    SetItemData(delitem, 0);

    // no (more) children, so we can safely delete top item
    DeleteItem( delitem );

	m_hSelect = NULL;
}



void CLayerTreeCtrl::OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	NMTVCUSTOMDRAW*     pxCustomDraw = (NMTVCUSTOMDRAW*)pNMHDR;
	
	switch ( pxCustomDraw->nmcd.dwDrawStage )
	{
	case CDDS_PREPAINT: 
		{
			CRect oRect( pxCustomDraw->nmcd.rc );
			
			CDC oDC;
			oDC.Attach( pxCustomDraw->nmcd.hdc );		
			oDC.FillRect( oRect, &CBrush( m_xBackGroundColor ) );
			oDC.Detach();
			
			*pResult = CDRF_NOTIFYITEMDRAW; 
		}
		break;
	case CDDS_ITEMPREPAINT: 
		{
			HTREEITEM       hTreeItem = (HTREEITEM)pxCustomDraw->nmcd.dwItemSpec;
			
			// get/create the object that holds each items rectangles
			//
			ITEM_RECTS_ITER oIter     = m_oItemRects.find( hTreeItem );
			P_ITEM_RECTS    pxRects;
			
			if ( oIter == m_oItemRects.end() )
			{
				pxRects = new ITEM_RECTS;
				m_oItemRects.insert( ITEM_RECT_MAP::value_type( hTreeItem, pxRects ) );
			}
			else
			{
				pxRects = (*oIter).second;
			}
			
			// get some basic facts to help with the render that is about to happen
			//
			int   iIndent   = GetIndentLevel( hTreeItem );
			UINT  iStyle    = 0;
			UINT  iState    = 0;
			
			treeItemDATA *treeItemData = (treeItemDATA *)GetItemData(hTreeItem);

			if( NULL != treeItemData ) {
				iStyle    = treeItemData->style;
				iState    = treeItemData->state;
			}

			bool  bExpanded = iState & ciExpanded;
			HDC   hDC       = pxCustomDraw->nmcd.hdc;
			RECT  xRect     = pxCustomDraw->nmcd.rc;	// 绘制范围
			CDC*  poDC      = CDC::FromHandle( hDC );	// DC
			CRect oRect( xRect );
			
			COLORREF xBackColor;
			COLORREF xTextColor;
			
			// colors of this row are based on select state of the item
			//
			bool bSelect = false;
			if( NULL != treeItemData && m_pLayerCollection->GetCurrentLayer() == treeItemData->pLayerTreeItem )
				bSelect = true;
			
			if( bSelect && iStyle & ciSelectable )	// GetSelectedItem() == hTreeItem
			{
				xTextColor = GetSysColor( COLOR_HIGHLIGHTTEXT );
				xBackColor = GetSysColor( COLOR_HIGHLIGHT );
			}
			else
			{
				xTextColor = GetSysColor( COLOR_WINDOWTEXT );
				xBackColor = m_xBackGroundColor;
			}
			
			CBrush  oBackBrush;
			CBrush  oBrush;
			CBrush* poOldBrush;
			
			oBrush.CreateSolidBrush( xBackColor );
			oBackBrush.CreateSolidBrush( m_xBackGroundColor );
			
			// fill the whole rectangle with the background color
			//
			poOldBrush = poDC->SelectObject( &oBackBrush );
			poDC->FillSolidRect( oRect, m_xBackGroundColor );
			poDC->SelectObject( poOldBrush );
			oBackBrush.DeleteObject();
			
			// start rendering with the left side based on the 
			// indent level of this node
			//
			oRect.left += GetIndent() * iIndent;
			
			// tree control likes it better if kid node text is inset from parent text
			//
			HTREEITEM hParent = GetParentItem( hTreeItem );
			if (  NULL != hParent&& hParent != GetRootItem())
			{
				ITEM_RECTS_ITER oParentIter = m_oItemRects.find( hParent );
				P_ITEM_RECTS    pxParentRects;
				
				if ( oParentIter != m_oItemRects.end() )
				{
					pxParentRects = (*oParentIter).second;
					if ( pxParentRects->m_oTextRect.left > oRect.left )
					{
						//        oRect.left = pxParentRects->m_oTextRect.left + 4;// add a indent based on the text-rect of parent item
					}
				}
			}
			int iOriginalLeft = oRect.left;
			
			CRect oButton( oRect );
			oButton.right = oButton.left + ( oButton.bottom - oButton.top );
			
			// if this node has kids, then must put up an indicator for that
			//
			if( ItemHasChildren( hTreeItem ))
			{
				HICON hicon;
				if ( !bExpanded )
				{
				/*////yao080807
				poDC->DrawFrameControl( oButton, 
				DFC_SCROLL,
					DFCS_SCROLLRIGHT ); *///yao080807
					////these code are used to draw plus icon instead of drawing button which the codes above did
					hicon = m_ImgExpandCollapse.ExtractIcon(0);
					HDC hdc = poDC->GetSafeHdc();
					::DrawIconEx(hdc,oButton.left,oButton.top, hicon,16,16,0,NULL,DI_NORMAL);
				}
				else
				{
				/*////yao080807
				poDC->DrawFrameControl( oButton, 
				DFC_SCROLL,
					DFCS_SCROLLDOWN | DFCS_PUSHED ); *///yao080807
					////these code are used to draw substract icon instead of drawing button which the codes above did
					hicon = m_ImgExpandCollapse.ExtractIcon(1);
					HDC hdc = poDC->GetSafeHdc();
					::DrawIconEx(hdc,oButton.left,oButton.top, hicon,16,16,0,NULL,DI_NORMAL);
				}
			}
			
			// save rect for hit testing later
			//
			oRect.left += oButton.Width() + 4;
			pxRects->m_oExpandRect = oButton;
			
			// now put in any check boxes and or radio buttons that may be needed
			//
			bool bNoStyle = false;

			// 根节点
			if( NULL == hParent )
			{
				CRect oCtrlRect( oRect.left, oRect.top, oRect.left + oRect.Height(), oRect.bottom );
				HICON hicon;

				hicon = m_ImgExpandCollapse.ExtractIcon(2);

				HDC hdc = poDC->GetSafeHdc();
				::DrawIconEx(hdc,oCtrlRect.left,oCtrlRect.top, hicon,16,16,0,NULL,DI_NORMAL);

				pxRects->m_oCheckRect = oCtrlRect;

				oRect.left += oCtrlRect.Width() + 4;
			}

			if( !ItemHasChildren( hTreeItem ) && NULL != hParent && ItemKind(GetParentItem(hTreeItem))&ciLayer)
			{
				//add by liubin 
				//获取hTreeItem 的parent
				HTREEITEM hParent = GetParentItem( hTreeItem );
				
				treeItemDATA *itemDataP = (treeItemDATA *)GetItemData(hParent);
				treeItemDATA *itemData = (treeItemDATA *)GetItemData(hTreeItem);
				
				orsRect_i CtrlRect;
				CtrlRect.m_xmax = oRect.right;
				CtrlRect.m_xmin = oRect.left;
				CtrlRect.m_ymax = oRect.bottom;
				CtrlRect.m_ymin = oRect.top;
				
				orsILayer* pparentlayer = (orsILayer*)(itemDataP->pLayerTreeItem);
				pparentlayer->OnDrawStyle(pxCustomDraw->nmcd.hdc,CtrlRect,itemData->rendFlag);
				
				CRect oCtrlRect( CtrlRect.m_xmin, CtrlRect.m_ymin, CtrlRect.m_xmax, CtrlRect.m_ymax );
				pxRects->m_oCheckRect = oCtrlRect;
				
				oRect.left += oCtrlRect.Width() + 4;
				
			}
			if( iStyle & ciCheckedNode || iStyle & ciRadioNode )
			{
				CRect oCtrlRect( oRect.left, oRect.top, oRect.left + oRect.Height(), oRect.bottom );
				bool  bChecked  = treeItemData->isChecked();	// ( 0 != ( iState & ciChecked ) );
				
				DWORD dwButton   = ( iStyle & ciCheckedNode ) ? DFCS_BUTTONCHECK : DFCS_BUTTONRADIO;
				DWORD dwChecked  = ( bChecked )               ? DFCS_CHECKED     : 0;
				DWORD dwInactive = ( iState & ciDisabled )    ? DFCS_INACTIVE    : 0;
				
				poDC->DrawFrameControl( oCtrlRect, 
					DFC_BUTTON, dwButton | dwChecked | dwInactive ); 

				pxRects->m_oCheckRect = oCtrlRect;

				oRect.left += oCtrlRect.Width() + 4;
			}
			else
			{
				bNoStyle = true;
			}        
			
			// get the text
			//
			CString oText;
			oText += GetItemText( hTreeItem );
			
			// measure it and set the text rect accordinly
			// 
			CSize oSize = poDC->GetTextExtent( oText ); 
			oRect.right = oRect.left + oSize.cx;
			
			// save rect for hit testing later
			//
			pxRects->m_oTextRect = oRect;
			
			// fill the text rectangle background color
			//
			poOldBrush = poDC->SelectObject( &oBrush );
			poDC->FillSolidRect( oRect, xBackColor );
			
			poDC->SelectObject( poOldBrush );
			oBrush.DeleteObject();
			
			// display the text
			//
			poDC->SetBkColor(xBackColor);
			poDC->SetTextColor( xTextColor );
			poDC->TextOut( oRect.left, oRect.top, oText );
			//the code following notify windows that the application drew the item manually,
			//the control will not draw the item 
			*pResult = CDRF_SKIPDEFAULT;

			poDC->DeleteTempMap();
		}
		break;	
    default:
		*pResult = CDRF_DODEFAULT;
	}
}



void CLayerTreeCtrl::OnLButtonDown( UINT nFlags, CPoint oPoint ) 
{
	bool      bnotstyleitem = true; 
	UINT      iItemFlags = 0;
	HTREEITEM hItem      = HitTest( oPoint, &iItemFlags );
	
	if ( NULL == hItem )
		return;

	UINT  iStyle = 0;
	UINT  iState = 0;
	
	treeItemDATA *treeItemData = (treeItemDATA *)GetItemData(hItem); 

	if( NULL == treeItemData )
		return ;
	
	if( NULL != treeItemData )  {
		iStyle = treeItemData->style;
		iState = treeItemData->state;
	}
	
	// DWORD dwData = GetItemData( hItem );
	
	if ( treeItemData->checkableStyle() )
	{
		ITEM_RECTS_ITER oIter = m_oItemRects.find( hItem );
		if ( oIter != m_oItemRects.end() )
		{
			P_ITEM_RECTS pxRects = (*oIter).second;
			
			if ( pxRects->m_oCheckRect.PtInRect( oPoint ) ) 
			{
				ChangeItemState( hItem, ( nFlags & MK_CONTROL ) );
				return;
			}
		}
	}
	
	if(!ItemHasChildren(hItem))
	{
		ITEM_RECTS_ITER oIter = m_oItemRects.find( hItem );
		if ( oIter != m_oItemRects.end() )
		{
			P_ITEM_RECTS pxRects = (*oIter).second;
			
			if ( pxRects->m_oCheckRect.PtInRect( oPoint ) ) 
			{
				//add by LiuBin
				
				HTREEITEM itemparent = GetParentItem(hItem);
				
				if( NULL != itemparent )	{
					treeItemDATA *treeItemDataParent = (treeItemDATA *)GetItemData(itemparent);	
					treeItemDATA *itemData = (treeItemDATA *)GetItemData(hItem);
					
					orsILayer* pparentlayer = (orsILayer*)(treeItemDataParent->pLayerTreeItem);
					pparentlayer->SetRender(itemData->rendFlag);
					bnotstyleitem = false;
					
					RedrawWindow();				
				}
			}
		}
	}
	
	if( bnotstyleitem)
	{
		CTreeCtrl::OnLButtonDown( nFlags, oPoint );
	}
	
}

void CLayerTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnRButtonDown( nFlags, point );
	// TODO: Add your message handler code here and/or call default
}

void CLayerTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp( nFlags, point );
}

void CLayerTreeCtrl::OnKeyDown( UINT iChar, UINT iRepeatCnt, UINT nFlags ) 
{
	if ( iChar == VK_SPACE )
	{
		HTREEITEM hItem = GetSelectedItem();
		
		if ( hItem )
		{
			CRect oRect;
			
			if ( GetItemRect( hItem, &oRect, TRUE ) )
			{
				UINT   iItemFlags = 0;
				CPoint oPoint( oRect.top, oRect.left );
				
				HTREEITEM hTest = HitTest( oPoint, &iItemFlags );
				if ( hTest == hItem )
				{
					if ( iItemFlags & TVHT_ONITEMSTATEICON )
					{
						ChangeItemState( hItem, (nFlags & MK_CONTROL) );
						return;
					}
				}
			}
		}
	}
	CTreeCtrl::OnKeyDown( iChar, iRepeatCnt, nFlags );
}


void CLayerTreeCtrl::ChangeItemState( HTREEITEM hItem, bool bSetChildren )
{
	if ( hItem != NULL )
	{
		treeItemDATA *treeItemData = (treeItemDATA *)GetItemData(hItem);

		if( !treeItemData->isDisabled() && treeItemData->checkableStyle() ) {
			treeItemData->toggleCheckState( bSetChildren );
		}

		RedrawWindow();
	}
}

void CLayerTreeCtrl::InvalidateItem( HTREEITEM hItem, BOOL bInvalidateSelf )
{
	CRect oItemRect;
	
	if ( bInvalidateSelf )
	{
		GetItemRect( hItem, &oItemRect, false );
		InvalidateRect( &oItemRect, true );
	}
	
	for ( hItem = GetChildItem( hItem ) ; hItem != NULL ; )
	{
		if ( ItemHasChildren( hItem ) 
			&& GetItemState( hItem, TVIS_EXPANDED ) )
		{
			InvalidateItem( hItem, true );
		}
		else
		{
			GetItemRect( hItem, &oItemRect, false );
			InvalidateRect( &oItemRect, true );
		}
		hItem = GetNextSiblingItem( hItem );
	}
}


BOOL CLayerTreeCtrl::CanEdit( HTREEITEM hTreeItem )
{
	if( NULL == hTreeItem ) 
		return FALSE;
	
	int iStyle = 0;
	treeItemDATA *treeItemData = (treeItemDATA *)GetItemData(hTreeItem); 
	
	if( treeItemData )
		iStyle = treeItemData->style;
	
	if( iStyle & ciEditable )
	{
		m_hEditNode = hTreeItem;
		return TRUE;
	}
	
	return FALSE;
}

BOOL CLayerTreeCtrl::IsItemChecked(HTREEITEM hItem)
{
	if ( hItem != NULL )
	{
		treeItemDATA *treeItemData = (treeItemDATA *)GetItemData(hItem); 

		treeItemData->isChecked();
	}
	return FALSE;
}


void CLayerTreeCtrl::OnBeginLabelEdit( NMHDR* pxNMHDR, LRESULT* plResult ) 
{
	TV_DISPINFO* pxTVDispInfo = (TV_DISPINFO*)pxNMHDR;
	//get the position of the mouse
	DWORD dwPos = GetMessagePos();
	
	// turn the co-ords into the CPoint structure
	CPoint point(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos)), sPoint;
	sPoint = point;
	
	ScreenToClient(&sPoint);
	UINT flags;
	HTREEITEM htItem = HitTest(sPoint, &flags); 
	// check to see if edits are allowed at this node
	//
	if ( CanEdit( pxTVDispInfo->item.hItem ) && (flags & TVHT_ONITEM) )
	{
		SetFocus();
		
		// Limit text to 6 characters
		//
		GetEditControl()->LimitText( 128 );
		
		// allow edit to proceed
		//
		*plResult = 0;  
	}
	else
	{
		// deny edit
		//
		*plResult = 1;
	}
}

void CLayerTreeCtrl::OnEndLabelEdit( NMHDR* pxNMHDR, LRESULT* plResult ) 
{
	TV_DISPINFO* pxDispInfo = (TV_DISPINFO*)pxNMHDR;
	
	// Get new label text and item number from struct 
	// passed to this handler
	//
	TVITEM pxItem = pxDispInfo->item;
	
	if ( pxItem.pszText == 0 )
	{
		// editing was cancelled
		//
		return;
	}
	else
	{
		// capture / process the new value for the node
		//
		CString oNewLabel = pxItem.pszText;
		
		//     TV_INSERTSTRUCT xAddNode;
		//     HTREEITEM       hNode;
		// 
		//     memset( &xAddNode, 0, sizeof( xAddNode ) );
		// 
		//     xAddNode.hParent         = GetParentItem( m_hEditNode );
		//     xAddNode.hInsertAfter    = m_hEditNode;
		//     xAddNode.item.mask       = TVIF_STATE | TVIF_TEXT;
		//     xAddNode.item.state      = INDEXTOSTATEIMAGEMASK( 0 );
		//     xAddNode.item.stateMask  = TVIS_STATEIMAGEMASK;
		//     xAddNode.item.pszText    = (char*)(LPCTSTR)oNewLabel;
		//     xAddNode.item.cchTextMax = oNewLabel.GetLength();
		//     
		//     hNode = InsertItem( &xAddNode );
		// 
		// //     SetItemData( hNode, MAKEWPARAM( 0, 0 ) );
		// 
		//     if ( !m_bQuitEdits )
		//     {
		//       ::SendMessage( this->m_hWnd, TVM_EDITLABEL, 0, (LPARAM) m_hEditNode );
		//     }
	}
	// accept the change
	//
	*plResult = TRUE; //;
}

void CLayerTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	UINT flags;
	HTREEITEM hTreeItem = HitTest(point, &flags);
	
	if ( CanEdit( hTreeItem ) )
	{
		m_bQuitEdits = false;
		::SendMessage( this->m_hWnd, TVM_EDITLABEL, 0, (LPARAM) hTreeItem );
	}
	else
	{
		CTreeCtrl::OnLButtonDblClk( nFlags, point );
	}
}


int CLayerTreeCtrl::GetIndentLevel( HTREEITEM hTreeItem )
{
	int iIndent = 0;
	while ( ( hTreeItem = GetParentItem( hTreeItem ) ) != NULL )
	{
		iIndent++;
	}
	return iIndent;
}


//#if _MSC_VER >= 1400
void CLayerTreeCtrl::OnItemExpanding( NMHDR* pxNMHDR, LRESULT* plResult )
//#else
//BOOL CLayerTreeCtrl::OnItemExpanding( NMHDR* pxNMHDR, LRESULT* plResult )
//#endif
{
	NM_TREEVIEW* pxNMTV = (NM_TREEVIEW*)pxNMHDR;
	*plResult = 0;
	
	HTREEITEM hItem  = pxNMTV->itemNew.hItem;
	treeItemDATA *treeItemData = (treeItemDATA *)GetItemData(hItem); 

	if ( TVE_COLLAPSE == pxNMTV->action ) 
		treeItemData->state &= ~ciExpanded;
	
	else  if ( TVE_EXPAND == pxNMTV->action ) 
		treeItemData->state |= ciExpanded;
	
	RedrawWindow();
	
//#if _MSC_VER >= 1400
//	return TRUE;
//#endif
	// SetItemData( hItem, MAKEWPARAM( iData, iStyle ) );
}


BOOL CLayerTreeCtrl::PreTranslateMessage( MSG* pxMsg ) 
{
	if ( pxMsg->message == WM_KEYDOWN )
	{
		// When an item is being edited make sure the edit control
		// receives certain important key strokes
		//
		if ( GetEditControl() 
			&& ( pxMsg->wParam == VK_RETURN 
			|| pxMsg->wParam == VK_DELETE 
			|| pxMsg->wParam == VK_ESCAPE
			|| GetKeyState( VK_CONTROL ) ) )
		{
			::TranslateMessage( pxMsg );
			::DispatchMessage( pxMsg );
			
			if ( pxMsg->wParam == VK_ESCAPE )
			{
				m_bQuitEdits = true;
			}
			
			return TRUE; // DO NOT process further
		}
	}
	return CTreeCtrl::PreTranslateMessage( pxMsg );
}

void CLayerTreeCtrl::OnRButtonClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	//get the position of the mouse
	DWORD dwPos = GetMessagePos();
	
	// turn the co-ords into the CPoint structure
	CPoint point(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos)), sPoint;
	sPoint = point;
	
	ScreenToClient(&sPoint);
	UINT flags;
	
	HTREEITEM htItem = HitTest(sPoint, &flags);
	
	if( htItem )	{
		
		HTREEITEM parantItem = GetParentItem( htItem );
		
		
		//if the item is used for style, then return
		treeItemDATA *treeItemData = (treeItemDATA *)GetItemData(htItem); 
		
		if( !treeItemData->checkableStyle() && NULL != parantItem )
			return;
		
		//do the click occured on an item
		if ((htItem != NULL)&&(flags & TVHT_ONITEM))
		{
			SetItemState(htItem, TVIS_SELECTED, TVIS_SELECTED);
			
			m_hSelect = htItem;
			m_pLayerCollection->SetCurrentLayer( treeItemData->pLayerTreeItem );
			
			Invalidate();
			
			CMenu menuPopup;
			CreateCustomPopupMenu( menuPopup, ItemKind(htItem) );
			
			UINT uCmd = menuPopup.TrackPopupMenu(TPM_RETURNCMD|TPM_LEFTALIGN|TPM_LEFTBUTTON, point.x, point.y, this, NULL);
			
			orsILayerTreeItem *pLayer = treeItemData->pLayerTreeItem;

			switch(uCmd) {
			case ID_MENUITEM_DATAFRAME_ACTIVATE:
				m_pLayerCollection->ActivateDataFrame( pLayer->GetLayerName() );
				break;

			case ID_MENUITEM_DEL:
				{
					if( NULL == parantItem )	{
						if( AfxMessageBox( "删除数据框?", MB_YESNO ) == IDYES ) 
						{
							m_pLayerCollection->RemoveDataFrame( pLayer->GetLayerName() );
						}
					}
					else if( m_hSelect != NULL )
					{
						if( AfxMessageBox( "删除图层?", MB_YESNO ) == IDYES ) 
						{
							//判断图层是否更改
						    orsILayer *pLyr = ORS_PTR_CAST( orsILayer, pLayer );
							if (pLyr && _tcsicmp(pLyr->GetLayerType(),ORS_LAYER_SF) == 0)
							{
								orsISFLayer *pSFLayer = (orsISFLayer *)pLyr;

								if (pSFLayer && pSFLayer->IsModified())
								{
									if( AfxMessageBox( "Save changes to layer?", MB_YESNO ) == IDYES )
										pSFLayer->SaveChanges();
								}
							}
							// 从图层管理器删除
							m_pLayerCollection->RemoveLayer( pLayer );
						}
					}
					break;
				}
			case ID_MENUITEM_NEWGROUP:
				{
					if( getMapService() )	{
						treeItemDATA *pSelectitemData = (treeItemDATA *)GetItemData(htItem);
						orsILayerGroup* pSelectItemGroup = (orsILayerGroup*)(pSelectitemData->pLayerTreeItem);
						
						orsILayerGroup* pGroupNew = getMapService()->CreateLayerGroup();
						
						pGroupNew->SetLayerName("New Group");
						pGroupNew->SetParent(pSelectItemGroup);
						
						InsertLayerTreeItem( pSelectItemGroup, pGroupNew,  (orsILayerTreeItem *)ORS_TVI_LAST );
					}
				}
			default:
				break;
			}
			menuPopup.DestroyMenu();
		}	
	}

	*pResult = 0;
	
}

void CLayerTreeCtrl::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW* pNMTreeView = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	ASSERT(hItem);
	
	if (!hItem)
	{
		return;
	}
	m_hNoDropCursor = AfxGetApp()->LoadCursor(IDC_NODROP);
	m_hDropMoveCursor = AfxGetApp()->LoadCursor(IDC_DROPMOVE);
	
	COLORREF rgb = GetSysColor(COLOR_HIGHLIGHT);
	::SetCursor(m_hDropMoveCursor);
	CTreeCtrl::SetInsertMarkColor(rgb);
	
	SetCapture();
	m_hItemDrag = hItem;
	m_bDraging = true;
}

void CLayerTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	UINT flags = 0;
	HTREEITEM hItem = HitTest(point, &flags);
	HTREEITEM hItemAfter = NULL;
	if (hItem && m_bDraging)
	{
		HTREEITEM hParentItem = FindThePlace(point, hItem, &hItemAfter);
		if (hParentItem == NULL)
		{
			SetInsertMark(0,0);
			if (m_hNoDropCursor)
			{
				SetCursor(m_hNoDropCursor);
			}
			else
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		}
		else
		{
			SetCursor(m_hDropMoveCursor);
			ITEM_RECTS_ITER oIter     = m_oItemRects.find( hParentItem );
			P_ITEM_RECTS    pxRects = (*oIter).second;
			int leftX = pxRects->m_oCheckRect.right;
			int leftY;
			if (hItemAfter == NULL)
			{
				leftY = pxRects->m_oCheckRect.bottom;
			}
			else
			{
				oIter  = m_oItemRects.find( hItemAfter );
				pxRects = (*oIter).second;
				leftY = pxRects->m_oCheckRect.bottom;
			}
			
			CRect rec_TreeCtrl;
			GetClientRect(rec_TreeCtrl);
			int RightX = rec_TreeCtrl.right - 5;
			CDC* poDC = GetDC();
			CPen EdgePen;
			CPen PrePen;
			CPen *poOldPen;
			
			PrePen.CreatePen(PS_SOLID,2,RGB(255,255,255));
			poOldPen = poDC->SelectObject(&PrePen);
			poDC->MoveTo(m_PreInsertPointFrom);
			poDC->LineTo(m_PreInsertPointTo);
			poDC->SelectObject(poOldPen);
			
			EdgePen.CreatePen(PS_SOLID,2,RGB(0,0,255));
			poOldPen = poDC->SelectObject(&EdgePen);
			poDC->MoveTo(leftX, leftY);
			poDC->LineTo(RightX, leftY);
			poDC->SelectObject(poOldPen);
			EdgePen.DeleteObject();
			
			m_PreInsertPointFrom.x = leftX;
			m_PreInsertPointFrom.y = leftY;
			m_PreInsertPointTo.x = RightX;
			m_PreInsertPointTo.y = leftY;
			
			SelectDropTarget(NULL);
		}
		
	}
	else if(m_bDraging)
	{
		SetInsertMark(0,0);
		if (m_hNoDropCursor)
		{
			SetCursor(m_hNoDropCursor);
		}
		else
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
	}
	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CLayerTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_hItemDrag != NULL )
	{
		int x = point.x;
		int y = point.y;
		// 		CString str;
		// 		str.Format("point: %d, %d" ,x,y);
		// 		MessageBox(str);
		OnEndDrag(nFlags, point);	
	}		
}

void CLayerTreeCtrl::OnEndDrag(UINT nFlags, CPoint point)
{
	if (m_hItemDrag == NULL)
	{
		return;
	}
	CPoint pt;
	pt = point;
	ClientToScreen(&pt);
	BOOL bCopy = false;//(GetKeyState(VK_CONTROL) & 0x10000000);
	//do drop
	UINT flags = 0;
	HTREEITEM hItemDrop = HitTest(point, &flags);

	if (hItemDrop != NULL)
	{
		//////////////////////////////////////////////////////////////////////////
		HTREEITEM hItemAfterTemp = NULL;
		HTREEITEM hroot = FindThePlace(point, hItemDrop, &hItemAfterTemp);
		if (hroot != NULL)
		{
			
			
			//////////////////////////////////////////////////////////////////////////
			// 	HTREEITEM hroot1 = GetRootItem();
			// 	HTREEITEM hroot = hItemDrop;
			// 	HTREEITEM hroot = GetParentItem(m_hItemDrag);
			// 	if (hroot1 == hroot)
			// 	{
			// 		MessageBox("the two root are the same!");
			// 	}
			CString str = GetItemText(hroot);
			// MessageBox(str);

			treeItemDATA *treeItemData = (treeItemDATA *)(GetItemData( m_hItemDrag ));
						
			bool bExpanded = treeItemData->state & ciExpanded;
			
			// MoveTreeItem(bExpanded, this,m_hItemDrag, hroot, hItemDrop == NULL ? TVI_LAST : hItemDrop, bCopy, CopyData);
			MoveItem( m_hItemDrag, hroot, hItemAfterTemp==NULL? TVI_FIRST : hItemAfterTemp);

			MoveTreeItem(bExpanded, this, m_hItemDrag, hroot, hItemAfterTemp==NULL? TVI_FIRST : hItemAfterTemp, bCopy, CopyData);
		}
	}
	FinishDragging();
	RedrawWindow();
}


void CLayerTreeCtrl::FinishDragging()
{
	if (m_hItemDrag != NULL)
	{
		ReleaseCapture();
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		SetInsertMark(0, 0);
		m_hItemDrag = NULL;
		m_bDraging = false;
		SelectDropTarget(NULL);
	}
}

void CLayerTreeCtrl::OnButtonClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	//get the position of the mouse
	DWORD dwPos = GetMessagePos();
	// turn the co-ords into the CPoint structure
	CPoint point(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos)), sPoint;
	sPoint = point;
	
	ScreenToClient(&sPoint);
	UINT flags;
	HTREEITEM htItem = HitTest(sPoint, &flags);
	//do the click occured on an item
	if ((htItem != NULL)&&(flags & TVHT_ONITEM))
	{
		if (m_hSelect != NULL && m_hSelect != htItem)
		{
			SetItemState(m_hSelect, 0, TVIS_SELECTED);
		}

		m_hSelect = htItem;

		//////////////////////////////////////////////////////////////////////////

		treeItemDATA *treeItemData = (treeItemDATA *)GetItemData( htItem );		
		m_pLayerCollection->SetCurrentLayer( treeItemData->pLayerTreeItem  );

		Invalidate();
	}
}


void CLayerTreeCtrl::SetCurrentLayer(  orsILayerTreeItem *pLayer )
{
	if( NULL != pLayer->m_hLayerTreeItem )
		Select( (HTREEITEM)pLayer->m_hLayerTreeItem, TVGN_CARET );
}


//////////////////////////////////////////////////////////////////////////
//修改：函数
//输入：当前HitTest获取的HTREEITEM类型hItemDrop；鼠标点击point
//返回：移入位置的父节点,如果不能移动则返回NULL
//输出：移入位置前一项的HTREEITEM
//////////////////////////////////////////////////////////////////////////
HTREEITEM CLayerTreeCtrl::FindThePlace(CPoint point, HTREEITEM hItemDrop, HTREEITEM* phItemAfter)
{
	//判断当前项是否是根节点
	if (hItemDrop == GetRootItem())
	{
		return hItemDrop;
	}
	HTREEITEM hItemParent = hItemDrop;
	if (hItemParent == m_hItemDrag)
	{
		return NULL;
	}
	while (hItemParent != TVI_ROOT && (hItemParent = GetParentItem(hItemParent)) != NULL)
		if (hItemParent == m_hItemDrag)
			return NULL;
		//判断是否是symbol item，如果是则将处理其父节点的Layer item
		HTREEITEM hParentDrop = GetParentItem(hItemDrop);
		treeItemDATA *pTreeItemDataTemp = (treeItemDATA *)(GetItemData(hParentDrop));
		orsILayerTreeItem* pParentDroptemp = (orsILayerTreeItem*)(pTreeItemDataTemp->pLayerTreeItem);
		
		if (!pParentDroptemp->layerTreeItemType() != ORS_LTT_LAYER )
		{
			hItemDrop = hParentDrop;
			hParentDrop = GetParentItem(hParentDrop);
		}
		
		//获取相关数据
		pTreeItemDataTemp = (treeItemDATA *)(GetItemData(hItemDrop));
		orsILayerTreeItem* pDropItem = (orsILayerTreeItem*)(pTreeItemDataTemp->pLayerTreeItem);
		
		pTreeItemDataTemp = (treeItemDATA *)(GetItemData(hParentDrop));
		orsILayerGroup* pParentDrop = (orsILayerGroup*)(pTreeItemDataTemp->pLayerTreeItem);
		
		////判断是Group或是Layer item
		if( pDropItem->layerTreeItemType() != ORS_LTT_LAYER )
		{
			//如果当前的Drop item不为其父节点下最后一项，则有两种情况
			//1、加入当前Group下的第一项
			//2、加入当前Group的父节点下当前Drop item后
			if (pParentDrop->GetChildIndex( pDropItem ) != (pParentDrop->GetNumOfChildren()-1))
			{
				ITEM_RECTS_ITER oIter = m_oItemRects.find( hItemDrop );
				P_ITEM_RECTS    pxRects = (*oIter).second;
				if ( point.x >= pxRects->m_oTextRect.right)
				{
					return hItemDrop;
				} 
				else
				{
					if (point.x > pxRects->m_oCheckRect.left)
					{
						*phItemAfter = hItemDrop;
						return hParentDrop;
					}
					else
						return NULL;
				}
			}
			//如果当前的Drop item为其父节点下最后一项，则有3种情况
			//1、加入当前Group下的第一项
			//2、加入当前Group的父节点下当前Drop item后
			//3、递归加入
			else
			{
				ITEM_RECTS_ITER oIter = m_oItemRects.find( hItemDrop );
				P_ITEM_RECTS    pxRects = (*oIter).second;
				if ( point.x >= pxRects->m_oTextRect.left)//1
				{
					return hItemDrop;
				} 
				else if(point.x >= pxRects->m_oCheckRect.left)//2
				{
					*phItemAfter = hItemDrop;
					return hParentDrop;
				}
				else//3
				{
					while (point.x < pxRects->m_oCheckRect.left && hParentDrop != GetRootItem())
					{
						hItemDrop = hParentDrop;
						hParentDrop = GetParentItem(hParentDrop);
						oIter = m_oItemRects.find(hItemDrop);
						pxRects = (*oIter).second;
					}
					*phItemAfter = hItemDrop;
					return hParentDrop;
				}
				
			}				
		}
		else
		{
			//如果父项为根节点，则插入当前Layer Item之后，为根节点的一个子项
			if (GetParentItem(hItemDrop) == GetRootItem())
			{
				*phItemAfter = hItemDrop;
				return GetRootItem();
			}
			//如果有非根节点的父项，并且Drop Item非此父项下的最后一项
			else if (pParentDrop->GetChildIndex(pDropItem) != (pParentDrop->GetNumOfChildren()-1))
			{
				*phItemAfter = hItemDrop;
				return hParentDrop;
			}
			//有非根的节点Group，且Drop Item是此Group下的最后一项，则有2种情况需处理
			//1、加入到此Group下，此Layer后
			//2、加入到某一上层的非根节点Group下，某一上层的Group后
			else
			{
				ITEM_RECTS_ITER oIter     = m_oItemRects.find( hItemDrop );
				P_ITEM_RECTS    pxRects = (*oIter).second;
				while (point.x < pxRects->m_oCheckRect.right && hParentDrop != GetRootItem())
				{
					hItemDrop = hParentDrop;
					hParentDrop = GetParentItem(hParentDrop);
					oIter = m_oItemRects.find(hItemDrop);
					pxRects = (*oIter).second;
				}
				*phItemAfter = hItemDrop;
				return hParentDrop;
			}
		}
		
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
int CLayerTreeCtrl::ItemKind( HTREEITEM hItem )
{
	//if( hItem == GetRootItem() )
	if( NULL == GetParentItem( hItem ) )
		return ciRoot;
	
	treeItemDATA *treeItemData = (treeItemDATA *)GetItemData( hItem );
	
	if( treeItemData->pLayerTreeItem->layerTreeItemType() != ORS_LTT_LAYER )
		return ciGroup;
	else
		return ciLayer;
}
