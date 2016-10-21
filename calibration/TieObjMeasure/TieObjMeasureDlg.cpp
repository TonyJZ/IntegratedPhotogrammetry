// TieObjMeasureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TieObjMeasure.h"
#include "TieObjMeasureDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTieObjMeasureDlg dialog

CTieObjMeasureDlg::CTieObjMeasureDlg(char *pAlgName, HWND LinkWindow, CWnd* pParent /*=NULL*/)
	: CDialog(CTieObjMeasureDlg::IDD, pParent)
	, m_bGCP(FALSE)
	, m_dXGcp(0)
	, m_dYGcp(0)
	, m_dZGcp(0)
	, m_TiePointType(0)
{
	//{{AFX_DATA_INIT(CTieObjMeasureDlg)
	m_ObjType = 0;
	m_TiePointType = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_SelObjRow=-1;

	if(pAlgName==NULL)
		m_pAlgName=NULL;
	else
	{
		m_pAlgName=new char[strlen(pAlgName)+1];
		strncpy(m_pAlgName, pAlgName, strlen(pAlgName));
		*(m_pAlgName+strlen(pAlgName))=0;
	}

	m_LinkWindow = LinkWindow;

	m_pCurChain=NULL;
}

static linkMSG s_msg;
CTieObjMeasureDlg::~CTieObjMeasureDlg()
{
// 	for(int i=0; i<m_pTieList.GetSize(); i++)
// 	{
// 		TieObjChain *pObjChain;
// 		pObjChain=m_pTieList[i];
// 		delete pObjChain;
// 		pObjChain=NULL;
// 	}
//	m_pTieList.RemoveAll();	

	if(m_pAlgName)	delete m_pAlgName;	m_pAlgName=NULL;

	//发送退出消息
	COPYDATASTRUCT cpd;
	cpd.dwData = 0;
	cpd.cbData = sizeof( linkMSG );
	cpd.lpData = &s_msg;
	
	s_msg.id = ORS_LM_TOMeasure_Exit;
	
	if(m_LinkWindow)
		::SendMessage( m_LinkWindow, WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
}

void CTieObjMeasureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTieObjMeasureDlg)
	DDX_Control(pDX, IDC_GRID1, m_TieObjGrid);
	DDX_Control(pDX, IDC_GRID2, m_MeasuredGrid);
	DDX_Radio(pDX, IDC_RADIO1, m_ObjType);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_IsGCP, m_bGCP);
	DDX_Text(pDX, IDC_GCP_X, m_dXGcp);
	DDX_Text(pDX, IDC_GCP_Y, m_dYGcp);
	DDX_Text(pDX, IDC_GCP_Z, m_dZGcp);
	DDX_Radio(pDX, IDC_RADIO_Undefine, m_TiePointType);
}

BEGIN_MESSAGE_MAP(CTieObjMeasureDlg, CDialog)
	//{{AFX_MSG_MAP(CTieObjMeasureDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_NewObj, OnBUTTONNewObj)
	ON_BN_CLICKED(IDC_BUTTON_DeleteObj, OnBUTTONDeleteObj)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_BUTTON_Save, OnBUTTONSave)
	ON_BN_CLICKED(IDC_BUTTON_DeleteItem, OnBUTTONDeleteItem)
	ON_COMMAND(ID_SaveAllObjs, OnSaveAllObjs)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CLICK, IDC_GRID1, OnTieObjGrid)
	ON_NOTIFY(GVN_SELCHANGING, IDC_GRID1, OnTieObjGridStartSelChange)
	ON_BN_CLICKED(IDC_IsGCP, &CTieObjMeasureDlg::OnBnClickedIsgcp)
	ON_EN_CHANGE(IDC_GCP_X, &CTieObjMeasureDlg::OnEnChangeGcpX)
	ON_EN_CHANGE(IDC_GCP_Y, &CTieObjMeasureDlg::OnEnChangeGcpY)
	ON_EN_CHANGE(IDC_GCP_Z, &CTieObjMeasureDlg::OnEnChangeGcpZ)
	ON_BN_CLICKED(IDC_RADIO_Undefine, &CTieObjMeasureDlg::OnBnClickedRadioUndefine)
	ON_BN_CLICKED(IDC_RADIO_HorVer, &CTieObjMeasureDlg::OnBnClickedRadioHorver)
	ON_BN_CLICKED(IDC_RADIO_Hor, &CTieObjMeasureDlg::OnBnClickedRadioHor)
	ON_BN_CLICKED(IDC_RADIO_Ver, &CTieObjMeasureDlg::OnBnClickedRadioVer)
	ON_COMMAND(ID_LIST_OPENALIGNPROJECT, &CTieObjMeasureDlg::OnListOpenalignproject)
	ON_COMMAND(ID_LIST_LOADTIEPOINTS, &CTieObjMeasureDlg::OnListLoadtiepoints)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTieObjMeasureDlg message handlers

BOOL CTieObjMeasureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	InitTieObjList();
	InitMeasuredValueList();

	m_AlgPrj.Open(m_pAlgName);
	
	m_pTieList=m_AlgPrj.GetTieObjList();

	LayoutTieObjID();	
	
	m_SelObjRow=1;

	SetSelectTieObj(m_SelObjRow);

	if(m_ObjType==0)
		EnableGCPCheck(TRUE);
//	GetDlgItem(IDC_IsGCP)->EnableWindow(FALSE);
//	GetDlgItem(IDC_GCP_X)->EnableWindow(FALSE);
//	GetDlgItem(IDC_GCP_Y)->EnableWindow(FALSE);
//	GetDlgItem(IDC_GCP_Z)->EnableWindow(FALSE);

	m_TiePointType = TP_type_undefine;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTieObjMeasureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTieObjMeasureDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTieObjMeasureDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTieObjMeasureDlg::InitTieObjList()
{
	m_TieObjGrid.SetColumnCount(2);	//ObjectID, Type
//	m_TieObjGrid.SetRowCount(ClassGradation+1);
	m_TieObjGrid.SetFixedRowCount(1);
//	m_TieObjGrid.SetFixedColumnCount(1);
	
//	m_TieObjGrid.SetDefCellHeight(32);
	
	m_TieObjGrid.SetColumnWidth(0, 120);
	m_TieObjGrid.SetColumnWidth(1,120);


// 	int hei = m_TieObjGrid.GetRowHeight(0);
// 
 	m_TieObjGrid.SetRowHeight(0, 32);

	GV_ITEM Item;
	CString str;
	

	str="Object ID";
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 0;
	Item.strText = str;
	m_TieObjGrid.SetItem(&Item);
	
	str="Type";
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 1;
	Item.strText = str;
	m_TieObjGrid.SetItem(&Item);

	m_TieObjGrid.SetListMode();
	m_TieObjGrid.SetSingleRowSelection();
//    m_TieObjGrid.EnableSelection();

	
// 	for (int i=0; i<10; i++)
// 	{
// //		m_TieObjGrid.SetRowCount(i+1);
// 		int j=i+1;	//包括表头的编号
// 		str.Format("%d", j);
// 		Item.mask = GVIF_TEXT;
// 		Item.row = j;
// 		Item.col = 0;
// 		Item.strText = str;
// //		m_TieObjGrid.SetItem(&Item);
// 		m_TieObjGrid.InsertRow(str);
// 		m_TieObjGrid.SetItemState(j,0, m_TieObjGrid.GetItemState(j,0) | GVIS_READONLY);
// 		
// 		str="POINT";		//POINT, LINE, PATCH
// 		Item.mask = GVIF_TEXT;
// 		Item.row = j;
// 		Item.col = 1;
// 		Item.strText = str;
// 		m_TieObjGrid.SetItem(&Item);
// 		m_TieObjGrid.SetItemState(j,1, m_TieObjGrid.GetItemState(j,1) | GVIS_READONLY);
// 	}
	
}

void CTieObjMeasureDlg::InitMeasuredValueList()
{
	m_MeasuredGrid.SetColumnCount(6);	//ObjectID, Type
	//	m_TieObjGrid.SetRowCount(ClassGradation+1);
	m_MeasuredGrid.SetFixedRowCount(1);
	//	m_TieObjGrid.SetFixedColumnCount(1);

//	m_MeasuredGrid.SetRowResize(TRUE);
	
	
	m_MeasuredGrid.SetColumnWidth(0, 70);
	m_MeasuredGrid.SetColumnWidth(1,60);
	m_MeasuredGrid.SetColumnWidth(2,60);
	m_MeasuredGrid.SetColumnWidth(3,60);
	m_MeasuredGrid.SetColumnWidth(4,60);
	m_MeasuredGrid.SetColumnWidth(5,60);

	m_MeasuredGrid.SetRowHeight(0, 32);
	
	GV_ITEM Item;
	CString str;
	
	
	str="Data ID";
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 0;
	Item.strText = str;
	m_MeasuredGrid.SetItem(&Item);
	
	str="Col";		//x
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 1;
	Item.strText = str;
	m_MeasuredGrid.SetItem(&Item);

	str="Row";		//y
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 2;
	Item.strText = str;
	m_MeasuredGrid.SetItem(&Item);

	str="X";		
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 3;
	Item.strText = str;
	m_MeasuredGrid.SetItem(&Item);

	str="Y";		
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 4;
	Item.strText = str;
	m_MeasuredGrid.SetItem(&Item);

	str="Z";		
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 5;
	Item.strText = str;
	m_MeasuredGrid.SetItem(&Item);

	m_TieObjGrid.SetListMode();
	m_TieObjGrid.SetSingleRowSelection();
}


void CTieObjMeasureDlg::OnTieObjGrid(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	CGridCellBase* pCell=NULL;
//	BOOL	bCheck;
	
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if(pItem->iRow<1 || pItem->iRow>m_TieObjGrid.GetRowCount())
		return;
	
	int tieID=GetSelectTieID(m_SelObjRow);

}

void CTieObjMeasureDlg::OnBUTTONNewObj() 
{
	int i=0;
	int row;
	CString str;
	GV_ITEM Item;

	i=m_pTieList->GetSize();
	if(i==0)
		i++;
	else
		i=m_pTieList->GetAt(m_pTieList->GetSize()-1)->TieID+1;


	UpdateData(TRUE);

	row=m_TieObjGrid.GetRowCount();
	str.Format("%d", i);
	Item.mask = GVIF_TEXT;
	Item.row = row;
	Item.col = 0;
	Item.strText = str;
	//		m_TieObjGrid.SetItem(&Item);
	m_TieObjGrid.InsertRow(str);
	
	m_TieObjGrid.SetItemState(row,0, m_TieObjGrid.GetItemState(row,0) | GVIS_READONLY| GVIS_SELECTED);
	
	//POINT, LINE, PATCH
	TieObj_Type type;
	if(m_ObjType==0)
	{
		str="POINT";	
		type=TO_POINT;
	}
	else if(m_ObjType==1)
	{
		str="LINE";
		type=TO_LINE;
	}
	else if(m_ObjType==2)
	{
		str="PATCH";
		type=TO_PATCH;
	}

	if(m_ObjType==0)
	{
		EnableGCPCheck(TRUE);
	}
	else
	{
		EnableGCPCheck(FALSE);
	}

	Item.mask = GVIF_TEXT;
	Item.row = row;
	Item.col = 1;
	Item.strText = str;
	m_TieObjGrid.SetItem(&Item);
	m_TieObjGrid.SetItemState(row,1, m_TieObjGrid.GetItemState(row,1) | GVIS_READONLY| GVIS_SELECTED);

	//TieObjChain  *pObjChain;
	m_pCurChain = new TieObjChain;

	m_pCurChain->TieID=i;
	m_pCurChain->type=type;
	m_pCurChain->bDel=false;
	m_pCurChain->pChain=new CArray<TieObject*, TieObject*>; //无连接点
	m_pCurChain->objNum=0;

	m_pTieList->Add(m_pCurChain);
	
	m_SelObjRow=row;
	
	BroadcastCreateNewTieObj();
	
	m_TieObjGrid.Invalidate();
	RemoveMeasureItems();
	
}

void CTieObjMeasureDlg::OnBUTTONDeleteObj() 
{
	TieObjChain  *pObjChain;

	if(m_SelObjRow<1)
	{
		CString str;
		str.Format("不能删除第%d条记录!", m_SelObjRow);
		AfxMessageBox(str.GetBuffer(128));
		return;
	}

	int tieID=GetSelectTieID(m_SelObjRow);
	pObjChain=GetTieChain(tieID);
	pObjChain->bDel=true;
	
	
	int nRow = m_SelObjRow;
    if (nRow > 1)
    {//取上一条
		m_TieObjGrid.DeleteRow(nRow);	
// 		m_SelObjRow--;
// 		m_TieObjGrid.SetItemState(m_SelObjRow,1, m_TieObjGrid.GetItemState(m_SelObjRow,1) | GVIS_SELECTED);
// 		m_TieObjGrid.Invalidate();
    }
	else
	{//取下一条
		m_TieObjGrid.DeleteRow(nRow);	
// 		m_SelObjRow++;
// 		m_TieObjGrid.SetItemState(m_SelObjRow,1, m_TieObjGrid.GetItemState(m_SelObjRow,1) | GVIS_SELECTED);
// 		m_TieObjGrid.Invalidate();
    }

	m_TieObjGrid.Invalidate();

	RemoveMeasureItems();
	m_SelObjRow--;
	SetSelectTieObj(m_SelObjRow);

}

// GVN_SELCHANGING
void CTieObjMeasureDlg::OnTieObjGridStartSelChange(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	
//	if(m_TieObjGrid.GetSelectedCount()!=1
	TRACE("Selection is %d\n", m_TieObjGrid.GetSelectedCount());

	m_SelObjRow=pItem->iRow;
	
	if(m_SelObjRow==0)	
		return;
	
	BroadcastCreateNewTieObj();
	//切换m_MeasuredGrid中的记录
//	RemoveMeasureItems();
	LayoutMeasureItems(GetSelectTieID(m_SelObjRow));
}

void CTieObjMeasureDlg::RemoveMeasureItems()
{
	int i=m_MeasuredGrid.GetRowCount();
	while(i>1)
	{
		m_MeasuredGrid.DeleteRow(i-1);
		i--;
	}
	m_MeasuredGrid.Invalidate();
}

void CTieObjMeasureDlg::RemoveTieObjID()
{
	int i=m_TieObjGrid.GetRowCount();
	while(i>1)
	{
		m_TieObjGrid.DeleteRow(i-1);
		i--;
	}
	m_TieObjGrid.Invalidate();
}

void CTieObjMeasureDlg::LayoutMeasureItems(int tieID)
{
	int objNum;
	POINT2D *pt2D=NULL;
	POINT3D *pt3D=NULL;
	CString strName, str_x, str_y, str_X, str_Y, str_Z;
	GV_ITEM Item;
	TieObject *pObj;
	int row;
	int i;

	RemoveMeasureItems();

	m_pCurChain=NULL;
	m_pCurChain=GetTieChain(tieID);

	if(m_pCurChain==NULL)	return;
//	m_pCurChain=m_pTieList->GetAt(tieID);

	if(m_pCurChain->type==TO_POINT)
		m_ObjType=0;
	else if(m_pCurChain->type==TO_LINE)
		m_ObjType=1;
	else if(m_pCurChain->type==TO_PATCH)
		m_ObjType=2;

	if(m_ObjType==0)
		EnableGCPCheck(TRUE);
	else
		EnableGCPCheck(FALSE);

	if(m_ObjType==0)
	{
		pObj=m_pCurChain->pChain->GetAt(0);
		if(pObj->objType==TO_POINT)
		{
			m_TiePointType = ((TiePoint*)pObj)->tpType;
		}
		EnableTiepointType(TRUE);
	}
	else
	{
		m_TiePointType = 0;
		EnableTiepointType(FALSE);
	}


	if(m_pCurChain->bGCP)
	{
		m_bGCP=TRUE;
		m_dXGcp=m_pCurChain->gcp.X;
		m_dYGcp=m_pCurChain->gcp.Y;
		m_dZGcp=m_pCurChain->gcp.Z;
//		EnableGCPCheck(TRUE);
	}
	else
	{
		m_bGCP=FALSE;	
	}
	EnableGCPEdit(m_bGCP);

//	UpdateData(FALSE);
	
	objNum=m_pCurChain->pChain->GetSize();

	for(i=0; i<objNum; i++)
	{
		pObj=m_pCurChain->pChain->GetAt(i);
		
		if(pObj->sourceType==ST_LiDAR)
		{
			strName.Format("Line_%d", pObj->sourceID);
		}
		else if(pObj->sourceType==ST_Image)
		{
			strName.Format("Image_%d", pObj->sourceID);
		}

		if(pObj->objType==TO_POINT)
		{
			pt2D=&(((TiePoint*)pObj)->pt2D);
			pt3D=&(((TiePoint*)pObj)->pt3D);
		}
		else if(pObj->objType==TO_LINE)
		{
			pt2D=((TieLine*)pObj)->pt2D;
			pt3D=((TieLine*)pObj)->pt3D;
		}
		else if(pObj->objType==TO_PATCH)
		{
			pt2D=((TiePatch*)pObj)->pt2D;
			pt3D=((TiePatch*)pObj)->pt3D;
		}

		//在list中只显示第一个点
		str_x.Format("%.3f", pt2D->x);
		str_y.Format("%.3f", pt2D->y);

		str_X.Format("%.3f", pt3D->X);
		str_Y.Format("%.3f", pt3D->Y);
		str_Z.Format("%.3f", pt3D->Z);

		row=i+1;

		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 0;
		Item.strText = strName;
		m_MeasuredGrid.InsertRow(strName);
//		m_MeasuredGrid.SetItem(&Item);
		m_MeasuredGrid.SetItemState(row, 0, m_MeasuredGrid.GetItemState(row,0) | GVIS_READONLY);

		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 1;
		Item.strText = str_x;
		m_MeasuredGrid.SetItem(&Item);
		m_MeasuredGrid.SetItemState(row, 1, m_MeasuredGrid.GetItemState(row,1) | GVIS_READONLY);
		
		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 2;
		Item.strText = str_y;
		m_MeasuredGrid.SetItem(&Item);
		m_MeasuredGrid.SetItemState(row, 2, m_MeasuredGrid.GetItemState(row,2) | GVIS_READONLY);

		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 3;
		Item.strText = str_X;
		m_MeasuredGrid.SetItem(&Item);
		m_MeasuredGrid.SetItemState(row, 3, m_MeasuredGrid.GetItemState(row,3) | GVIS_READONLY);

		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 4;
		Item.strText = str_Y;
		m_MeasuredGrid.SetItem(&Item);
		m_MeasuredGrid.SetItemState(row, 4, m_MeasuredGrid.GetItemState(row,4) | GVIS_READONLY);

		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 5;
		Item.strText = str_Z;
		m_MeasuredGrid.SetItem(&Item);
		m_MeasuredGrid.SetItemState(row, 5, m_MeasuredGrid.GetItemState(row,5) | GVIS_READONLY);
	}

// 	if(objNum>0)
// 	{
// 		m_MeasuredGrid.SetItemState(1, 0, m_MeasuredGrid.GetItemState(1,0) | GVIS_SELECTED);
// 	}

	m_MeasuredGrid.Invalidate();
	UpdateData(FALSE);
}

void CTieObjMeasureDlg::LayoutTieObjID()
{
	CString str;
	GV_ITEM Item;
	int row;

	RemoveTieObjID();

	row=1;
	for(int i=0; i<m_pTieList->GetSize(); i++)
	{
	
		m_pCurChain=m_pTieList->GetAt(i);
		if(m_pCurChain->bDel)
			continue;

		
		str.Format("%d", m_pCurChain->TieID);
		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 0;
		Item.strText = str;
		m_TieObjGrid.InsertRow(str);
		
		m_TieObjGrid.SetItemState(row,0, m_TieObjGrid.GetItemState(row,0) | GVIS_READONLY);
		
		if(m_pCurChain->type==TO_POINT)
		{
			str="POINT";	
		}
		else if(m_pCurChain->type==TO_LINE)
		{
			str="LINE";
		}
		else if(m_pCurChain->type==TO_PATCH)
		{
			str="PATCH";
		}
		
		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 1;
		Item.strText = str;
		m_TieObjGrid.SetItem(&Item);
		m_TieObjGrid.SetItemState(row,1, m_TieObjGrid.GetItemState(row,1) | GVIS_READONLY);

		row++;
	}

	m_SelObjRow=1;

	m_TieObjGrid.Invalidate();

	BroadcastCreateNewTieObj();
//	RemoveMeasureItems();
	LayoutMeasureItems(GetSelectTieID(m_SelObjRow));
}

int CTieObjMeasureDlg::GetSelectTieID(int gridRow)
{
	if(gridRow<=0)
		return -1;

	CString	str=m_TieObjGrid.GetItemText(gridRow, 0);
	int id=atoi(str.GetBuffer(128));

	return id;	//id从1开始
}

TieObjChain* CTieObjMeasureDlg::GetTieChain(int tieID)
{
	TieObjChain *pChain=NULL;

	for(int i=0; i<m_pTieList->GetSize(); i++)
	{
		if(m_pTieList->GetAt(i)->TieID==tieID)
		{
			pChain=m_pTieList->GetAt(i);
			break;
		}
	}

	return pChain;
}


TieObject* CTieObjMeasureDlg::GetTieObject(int tieRow, int itemRow)
{
	int id=GetSelectTieID(tieRow);
	TieObjChain* pChain=NULL;
	TieObject *pObj=NULL, *pTmpObj;

	ASSERT(id>0 && id<=m_pTieList->GetSize());

	pChain=GetTieChain(id);
	ASSERT(pChain);

	CString	str=m_MeasuredGrid.GetItemText(itemRow, 0);
	CString strTmp;
	int pos;

	int srcID;
	Source_Type srcType;
	
	if(str.Find("Line_")>=0)
	{
		srcType=ST_LiDAR;
	}
	else if(str.Find("Image_")>=0)
	{
		srcType=ST_Image;
	}

	pos=str.ReverseFind('_');
	if(pos!=-1)
		strTmp=str.Right(str.GetLength()-pos-1);

	srcID=atoi(strTmp.GetBuffer(128));

	for(int i=0; i<pChain->pChain->GetSize(); i++)
	{
		pTmpObj=pChain->pChain->GetAt(i);
		if(pTmpObj->sourceType==srcType && pTmpObj->sourceID==srcID)
		{
			pObj=pTmpObj;
			break;
		}
	}
	
	return pObj;
}

void CTieObjMeasureDlg::OnRadio1() 
{
	m_ObjType=0;

	//////////////////////////////////////////////////
	COPYDATASTRUCT cpd;
	cpd.dwData = 0;
	cpd.cbData = sizeof( linkMSG );
	cpd.lpData = &s_msg;
	
	s_msg.id = ORS_LM_LINK;
	s_msg.objType=TO_POINT;
	

//	int i=0;
	// 通知其他窗口
//	for( int i=0; i< s_msg.nWindows; i++ ) 
//	{	
		// 只能用SendMessage
		//::PostMessage( m_linkWindows[i], WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
//		if( m_hWnd != s_msg.hLinkWindows[i] && s_msg.bWindowLinkOn[i] )
	if(m_LinkWindow)
		::SendMessage( m_LinkWindow, WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
//	}

	EnableGCPCheck(TRUE);
}

void CTieObjMeasureDlg::OnRadio2() 
{
	m_ObjType=1;

//////////////////////////////////////////////////
	COPYDATASTRUCT cpd;
	cpd.dwData = 0;
	cpd.cbData = sizeof( linkMSG );
	cpd.lpData = &s_msg;
	
	s_msg.id = ORS_LM_LINK;
	s_msg.objType=TO_LINE;
	
	if(m_LinkWindow)
		::SendMessage( m_LinkWindow, WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );

	EnableGCPCheck(FALSE);
}

void CTieObjMeasureDlg::OnRadio3() 
{
	m_ObjType=2;

	//////////////////////////////////////////////////
	COPYDATASTRUCT cpd;
	cpd.dwData = 0;
	cpd.cbData = sizeof( linkMSG );
	cpd.lpData = &s_msg;
	
	s_msg.id = ORS_LM_LINK;
	s_msg.objType=TO_PATCH;
	
	if(m_LinkWindow)
		::SendMessage( m_LinkWindow, WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );

	EnableGCPCheck(FALSE);
}

BOOL CTieObjMeasureDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	linkMSG *msg = (linkMSG *)pCopyDataStruct->lpData;

	if(msg->id==ORS_LM_Measure_Info)
	{
		ProcessMeasureInfo(msg);
	}
	
	return S_OK;
}

void CTieObjMeasureDlg::ProcessMeasureInfo(linkMSG *msg)
{
	ASSERT(msg);
	int nFind=-1;
	int i;
	POINT2D *pt2D;
	POINT3D *pt3D;
	int ptNum;
	bool bInsertItem=false;

	for(i=0; i<m_pTieList->GetSize(); i++)
	{
		m_pCurChain=m_pTieList->GetAt(i);
		if(m_pCurChain->TieID==msg->tieID)
		{
			nFind=i;
			break;
		}
	}
	
	if(nFind==-1)
	{//在现有连接点中未找到
		m_pCurChain=new TieObjChain;
		m_pTieList->Add(m_pCurChain);

		TRACE("logical error!\n");	//必须先创建，再接收量测消息
	}


	if(m_pCurChain->pChain==NULL)
	{
		m_pCurChain->pChain=new CArray<TieObject*, TieObject*>;
	}

	nFind=-1;
	TieObject *pTmpObj, *pObj;
	for(i=0; i<m_pCurChain->pChain->GetSize(); i++)
	{//遍历每个连接点
		pTmpObj=m_pCurChain->pChain->GetAt(i);
		
		if(msg->sourceID == pTmpObj->sourceID 
			&& msg->sourceType == pTmpObj->sourceType 
			&& msg->objType == pTmpObj->objType)
		{
			nFind=i;
			break;
		}
	}

	if(nFind>=0)
	{//更新连接点
		if(pTmpObj->objType==TO_POINT)
		{
			((TiePoint*)pTmpObj)->pt2D=msg->pt2D;
			((TiePoint*)pTmpObj)->pt3D=msg->pt3D;
		}
		else if(pTmpObj->objType==TO_LINE)
		{
			pt2D=((TieLine*)pTmpObj)->pt2D;
			pt3D=((TieLine*)pTmpObj)->pt3D;
			ptNum=((TieLine*)pTmpObj)->ptNum;
		}
		else if(pTmpObj->objType==TO_PATCH)
		{
			pt2D=((TiePatch*)pTmpObj)->pt2D;
			pt3D=((TiePatch*)pTmpObj)->pt3D;
			ptNum=((TiePatch*)pTmpObj)->ptNum;
		}
		
		if(pTmpObj->objType!=TO_POINT)
		{
			for(i=0; i<ptNum; i++)
			{
				if(msg->ptID==i)
				{
					pt2D[i]=msg->pt2D;
					pt3D[i]=msg->pt3D;
					break;
				}
			}
		}
	}
	else
	{//加入新记录
		bInsertItem=true;

		switch(msg->objType)
		{
		case TO_POINT:
			pTmpObj=new TiePoint;
			pTmpObj->tieID=msg->tieID;
			pTmpObj->objType=msg->objType;
			pTmpObj->sourceID=msg->sourceID;
			pTmpObj->sourceType=msg->sourceType;
			((TiePoint*)pTmpObj)->pt2D=msg->pt2D;
			((TiePoint*)pTmpObj)->pt3D=msg->pt3D;
			((TiePoint*)pTmpObj)->tpType = m_TiePointType;
			m_pCurChain->pChain->Add(pTmpObj);
			break;
			
		case TO_LINE:
			pTmpObj=new TieLine;
			pTmpObj->tieID=msg->tieID;
			pTmpObj->objType=msg->objType;
			pTmpObj->sourceID=msg->sourceID;
			pTmpObj->sourceType=msg->sourceType;
			((TieLine*)pTmpObj)->ptNum=msg->ptNum;
			((TieLine*)pTmpObj)->pt2D=new POINT2D[msg->ptNum];
			((TieLine*)pTmpObj)->pt3D=new POINT3D[msg->ptNum];
			((TieLine*)pTmpObj)->pt2D[msg->ptID]=msg->pt2D;
			((TieLine*)pTmpObj)->pt3D[msg->ptID]=msg->pt3D;
			m_pCurChain->pChain->Add(pTmpObj);
			break;
			
		case TO_PATCH:
			pTmpObj=new TiePatch;
			pTmpObj->tieID=msg->tieID;
			pTmpObj->objType=msg->objType;
			pTmpObj->sourceID=msg->sourceID;
			pTmpObj->sourceType=msg->sourceType;
			((TiePatch*)pTmpObj)->ptNum=msg->ptNum;
			((TiePatch*)pTmpObj)->pt2D=new POINT2D[msg->ptNum];
			((TiePatch*)pTmpObj)->pt3D=new POINT3D[msg->ptNum];
			((TiePatch*)pTmpObj)->pt2D[msg->ptID]=msg->pt2D;
			((TiePatch*)pTmpObj)->pt3D[msg->ptID]=msg->pt3D;
			m_pCurChain->pChain->Add(pTmpObj);
			break;
		}
		m_pCurChain->objNum++;
	}

	//////////////////////////////////////////////////////////////////////////
	//更新grid中的信息
	ASSERT(pTmpObj->objType==msg->objType);
	
	if(bInsertItem)
	{//插入新的Item
		if(pTmpObj->objType==TO_POINT)
			InsertMeasureItem(pTmpObj);
		else 
		{
			if(msg->ptID==0)
			{
				InsertMeasureItem(pTmpObj);
			}
		}
	}
	else
	{//更新
		int rowNum;
		rowNum=m_MeasuredGrid.GetRowCount();
		for(i=1; i<rowNum; i++)
		{
			pObj=GetTieObject(m_SelObjRow, i);

			if(pObj->sourceID==pTmpObj->sourceID && pObj->sourceType==pTmpObj->sourceType)
			{
				CString str_x, str_y, str_X, str_Y, str_Z;
				GV_ITEM Item;

				if(pTmpObj->objType==TO_POINT)
				{
					str_x.Format("%.3f", msg->pt2D.x);
					str_y.Format("%.3f", msg->pt2D.y);
					str_X.Format("%.3f", msg->pt3D.X);
					str_Y.Format("%.3f", msg->pt3D.Y);
					str_Z.Format("%.3f", msg->pt3D.Z);

					Item.mask = GVIF_TEXT;
					Item.row = i;
					Item.col = 1;
					Item.strText = str_x;
					m_MeasuredGrid.SetItem(&Item);
					m_MeasuredGrid.SetItemState(i, 1, m_MeasuredGrid.GetItemState(i,1) | GVIS_READONLY);
					
					Item.mask = GVIF_TEXT;
					Item.row = i;
					Item.col = 2;
					Item.strText = str_y;
					m_MeasuredGrid.SetItem(&Item);
					m_MeasuredGrid.SetItemState(i, 2, m_MeasuredGrid.GetItemState(i,2) | GVIS_READONLY);

					Item.mask = GVIF_TEXT;
					Item.row = i;
					Item.col = 3;
					Item.strText = str_X;
					m_MeasuredGrid.SetItem(&Item);
					m_MeasuredGrid.SetItemState(i, 3, m_MeasuredGrid.GetItemState(i,3) | GVIS_READONLY);

					Item.mask = GVIF_TEXT;
					Item.row = i;
					Item.col = 4;
					Item.strText = str_Y;
					m_MeasuredGrid.SetItem(&Item);
					m_MeasuredGrid.SetItemState(i, 4, m_MeasuredGrid.GetItemState(i,4) | GVIS_READONLY);

					Item.mask = GVIF_TEXT;
					Item.row = i;
					Item.col = 5;
					Item.strText = str_Z;
					m_MeasuredGrid.SetItem(&Item);
					m_MeasuredGrid.SetItemState(i, 5, m_MeasuredGrid.GetItemState(i,5) | GVIS_READONLY);
				}
				else
				{
					if(msg->ptID==0)
					{
						str_x.Format("%.3f", msg->pt2D.x);
						str_y.Format("%.3f", msg->pt2D.y);
						str_X.Format("%.3f", msg->pt3D.X);
						str_Y.Format("%.3f", msg->pt3D.Y);
						str_Z.Format("%.3f", msg->pt3D.Z);
						
						Item.mask = GVIF_TEXT;
						Item.row = i;
						Item.col = 1;
						Item.strText = str_x;
						m_MeasuredGrid.SetItem(&Item);
						m_MeasuredGrid.SetItemState(i, 1, m_MeasuredGrid.GetItemState(i,1) | GVIS_READONLY);
						
						Item.mask = GVIF_TEXT;
						Item.row = i;
						Item.col = 2;
						Item.strText = str_y;
						m_MeasuredGrid.SetItem(&Item);
						m_MeasuredGrid.SetItemState(i, 2, m_MeasuredGrid.GetItemState(i,2) | GVIS_READONLY);

						Item.mask = GVIF_TEXT;
						Item.row = i;
						Item.col = 3;
						Item.strText = str_X;
						m_MeasuredGrid.SetItem(&Item);
						m_MeasuredGrid.SetItemState(i, 3, m_MeasuredGrid.GetItemState(i,3) | GVIS_READONLY);
						
						Item.mask = GVIF_TEXT;
						Item.row = i;
						Item.col = 4;
						Item.strText = str_Y;
						m_MeasuredGrid.SetItem(&Item);
						m_MeasuredGrid.SetItemState(i, 4, m_MeasuredGrid.GetItemState(i,4) | GVIS_READONLY);
						
						Item.mask = GVIF_TEXT;
						Item.row = i;
						Item.col = 5;
						Item.strText = str_Z;
						m_MeasuredGrid.SetItem(&Item);
						m_MeasuredGrid.SetItemState(i, 5, m_MeasuredGrid.GetItemState(i,5) | GVIS_READONLY);
					}
				}

				break;
			}
		}
	}

	m_MeasuredGrid.Invalidate();
}

void CTieObjMeasureDlg::InsertMeasureItem(TieObject *pObj)
{
	GV_ITEM Item;
	CString str, str_x, str_y, str_X, str_Y, str_Z;
	int row;
	POINT2D *pt2D;
	POINT3D *pt3D;
	
	if(pObj->sourceType==ST_LiDAR)
		str.Format("Line_%d", pObj->sourceID);
	else if(pObj->sourceType==ST_Image)
		str.Format("Image_%d", pObj->sourceID);

	row=m_MeasuredGrid.GetRowCount();
	Item.mask = GVIF_TEXT;
	Item.row = row;
	Item.col = 0;
	Item.strText = str;
	//		m_TieObjGrid.SetItem(&Item);
	m_MeasuredGrid.InsertRow(str);
	m_MeasuredGrid.SetItemState(row,0, m_MeasuredGrid.GetItemState(row,0) | GVIS_READONLY| GVIS_SELECTED);

	if(pObj->objType==TO_POINT)
	{
		str_x.Format("%.3f", ((TiePoint*)pObj)->pt2D.x);
		str_y.Format("%.3f", ((TiePoint*)pObj)->pt2D.y);
		str_X.Format("%.3f", ((TiePoint*)pObj)->pt3D.X);
		str_Y.Format("%.3f", ((TiePoint*)pObj)->pt3D.Y);
		str_Z.Format("%.3f", ((TiePoint*)pObj)->pt3D.Z);
	}
	else if(pObj->objType==TO_LINE)
	{
		pt2D=((TieLine*)pObj)->pt2D;
		pt3D=((TieLine*)pObj)->pt3D;

		str_x.Format("%.3f", pt2D[0].x);
		str_y.Format("%.3f", pt2D[0].y);
		str_X.Format("%.3f", pt3D[0].X);
		str_Y.Format("%.3f", pt3D[0].Y);
		str_Z.Format("%.3f", pt3D[0].Z);
	}
	else if(pObj->objType==TO_PATCH)
	{
		pt2D=((TiePatch*)pObj)->pt2D;
		pt3D=((TiePatch*)pObj)->pt3D;

		str_x.Format("%.3f", pt2D[0].x);
		str_y.Format("%.3f", pt2D[0].y);
		str_X.Format("%.3f", pt3D[0].X);
		str_Y.Format("%.3f", pt3D[0].Y);
		str_Z.Format("%.3f", pt3D[0].Z);
	}
	
	Item.mask = GVIF_TEXT;
	Item.row = row;
	Item.col = 1;
	Item.strText = str_x;
	m_MeasuredGrid.SetItem(&Item);
	m_MeasuredGrid.SetItemState(row, 1, m_MeasuredGrid.GetItemState(row,1) | GVIS_READONLY| GVIS_SELECTED);
	
	Item.mask = GVIF_TEXT;
	Item.row = row;
	Item.col = 2;
	Item.strText = str_y;
	m_MeasuredGrid.SetItem(&Item);
	m_MeasuredGrid.SetItemState(row, 2, m_MeasuredGrid.GetItemState(row,2) | GVIS_READONLY| GVIS_SELECTED);

	Item.mask = GVIF_TEXT;
	Item.row = row;
	Item.col = 3;
	Item.strText = str_X;
	m_MeasuredGrid.SetItem(&Item);
	m_MeasuredGrid.SetItemState(row, 3, m_MeasuredGrid.GetItemState(row,3) | GVIS_READONLY| GVIS_SELECTED);

	Item.mask = GVIF_TEXT;
	Item.row = row;
	Item.col = 4;
	Item.strText = str_Y;
	m_MeasuredGrid.SetItem(&Item);
	m_MeasuredGrid.SetItemState(row, 4, m_MeasuredGrid.GetItemState(row,4) | GVIS_READONLY| GVIS_SELECTED);

	Item.mask = GVIF_TEXT;
	Item.row = row;
	Item.col = 5;
	Item.strText = str_Z;
	m_MeasuredGrid.SetItem(&Item);
	m_MeasuredGrid.SetItemState(row, 5, m_MeasuredGrid.GetItemState(row,5) | GVIS_READONLY| GVIS_SELECTED);	
}

void CTieObjMeasureDlg::OnBUTTONSave() 
{
//	int i;
//	CString str;
//	double temp;

//	m_pTieList.Add(m_pCurChain);

//	m_pCurChain=NULL;
}

static linkMSG broadcast_msg;
#define RegistViewerWINDOW_TEXT "RegistViewer"
BOOL CALLBACK EnumWinsTB(
						 HWND hwnd,      // handle to parent window
						 LPARAM lParam  )
{
	char windowText[256];
	
	GetWindowText( hwnd, windowText, 255 );
	
	if( NULL != strstr( windowText, RegistViewerWINDOW_TEXT ) )
	{
		broadcast_msg.hLinkWindows[broadcast_msg.nWindows] = hwnd;
		//s_msg.bWindowLinkOn[s_msg.nWindows] = 1;
		broadcast_msg.nWindows++;
		broadcast_msg.nWindows%=32;
	}
	
	return TRUE;
}
void CTieObjMeasureDlg::BroadcastCreateNewTieObj()
{
	broadcast_msg.nWindows=0;

	EnumWindows( (WNDENUMPROC)EnumWinsTB, (LPARAM)this );
	
	if(m_SelObjRow<=0)
		return;
	int tieID;

	tieID=GetSelectTieID(m_SelObjRow);
	//////////////////////////////////////////////////
	COPYDATASTRUCT cpd;
	cpd.dwData = 0;
	cpd.cbData = sizeof( linkMSG );
	cpd.lpData = &broadcast_msg;
	
	if(m_ObjType==0)
	{
		broadcast_msg.objType=TO_POINT;
	}
	else if(m_ObjType==1)
	{
		broadcast_msg.objType=TO_LINE;
	}
	else if(m_ObjType==2)
	{
		broadcast_msg.objType=TO_PATCH;
	}
	broadcast_msg.id = ORS_LM_Measure_Info;
	broadcast_msg.tieID=tieID;
	
	// 通知其他窗口
	for( int i=0; i<broadcast_msg.nWindows; i++ ) 
	{	
		// 只能用SendMessage
		//::PostMessage( m_linkWindows[i], WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
		if( m_hWnd != broadcast_msg.hLinkWindows[i])
			::SendMessage( broadcast_msg.hLinkWindows[i], WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
	}
}

void CTieObjMeasureDlg::OnBUTTONDeleteItem() 
{
	// TODO: Add your control notification handler code here
	
}

void CTieObjMeasureDlg::OnSaveAllObjs() 
{
	m_AlgPrj.Save();
	
}

void CTieObjMeasureDlg::SetSelectTieObj(int row)
{
	if(row<1 || row>= m_TieObjGrid.GetRowCount())
		return;

	m_TieObjGrid.SetItemState(row, 0, m_TieObjGrid.GetItemState(row,0) | GVIS_READONLY| GVIS_SELECTED);
	m_TieObjGrid.SetItemState(row, 1, m_TieObjGrid.GetItemState(row,1) | GVIS_READONLY| GVIS_SELECTED);

	LayoutMeasureItems(GetSelectTieID(row));
}
void CTieObjMeasureDlg::OnBnClickedIsgcp()
{
	m_bGCP=!m_bGCP;

	
	if(m_bGCP)
	{
// 		GetDlgItem(IDC_GCP_X)->EnableWindow(TRUE);
// 		GetDlgItem(IDC_GCP_Y)->EnableWindow(TRUE);
// 		GetDlgItem(IDC_GCP_Z)->EnableWindow(TRUE);
		EnableGCPEdit(TRUE);
		if(m_pCurChain)
		{
			m_pCurChain->bGCP=true;
		}
	}
	else
	{
// 		GetDlgItem(IDC_GCP_X)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_GCP_Y)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_GCP_Z)->EnableWindow(FALSE);
		EnableGCPEdit(FALSE);
		if(m_pCurChain)
		{
			m_pCurChain->bGCP=false;
		}
	}
}

void CTieObjMeasureDlg::EnableGCPCheck(BOOL bEnable)
{
	m_bGCP=FALSE;
	GetDlgItem(IDC_IsGCP)->EnableWindow(bEnable);
// 	GetDlgItem(IDC_GCP_X)->EnableWindow(FALSE);
// 	GetDlgItem(IDC_GCP_Y)->EnableWindow(FALSE);
// 	GetDlgItem(IDC_GCP_Z)->EnableWindow(FALSE);
	
	EnableGCPEdit(FALSE);
//	m_dXGcp=m_dYGcp=m_dZGcp=0;

	UpdateData(FALSE);
}

void CTieObjMeasureDlg::EnableTiepointType(BOOL bEnable)
{
	GetDlgItem(IDC_RADIO_Undefine)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_HorVer)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_Hor)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_Ver)->EnableWindow(bEnable);

	UpdateData(FALSE);
}

void CTieObjMeasureDlg::EnableGCPEdit(BOOL bEnable)
{
	if(m_bGCP)
		((CButton*)GetDlgItem(IDC_IsGCP))->SetCheck(BST_CHECKED);
	else
		((CButton*)GetDlgItem(IDC_IsGCP))->SetCheck(BST_UNCHECKED);

	GetDlgItem(IDC_GCP_X)->EnableWindow(bEnable);
	GetDlgItem(IDC_GCP_Y)->EnableWindow(bEnable);
	GetDlgItem(IDC_GCP_Z)->EnableWindow(bEnable);
}
void CTieObjMeasureDlg::OnEnChangeGcpX()
{
	UpdateData(TRUE);
	if(m_pCurChain)
	{
		if(m_pCurChain->bGCP)
		{
			m_pCurChain->gcp.X=m_dXGcp;
			TRACE("%f, %f, %f\n", m_pCurChain->gcp.X, m_pCurChain->gcp.Y, m_pCurChain->gcp.Z);
		}
	}
}

void CTieObjMeasureDlg::OnEnChangeGcpY()
{
	UpdateData(TRUE);
	if(m_pCurChain)
	{
		if(m_pCurChain->bGCP)
		{
			m_pCurChain->gcp.Y=m_dYGcp;
			TRACE("%f, %f, %f\n", m_pCurChain->gcp.X, m_pCurChain->gcp.Y, m_pCurChain->gcp.Z);
		}
	}
}

void CTieObjMeasureDlg::OnEnChangeGcpZ()
{
	UpdateData(TRUE);
	if(m_pCurChain)
	{
		if(m_pCurChain->bGCP)
		{
			m_pCurChain->gcp.Z=m_dZGcp;
			TRACE("%f, %f, %f\n", m_pCurChain->gcp.X, m_pCurChain->gcp.Y, m_pCurChain->gcp.Z);
		}
	}
}

void CTieObjMeasureDlg::OnBnClickedRadioUndefine()
{
	m_TiePointType = TP_type_undefine;
}


void CTieObjMeasureDlg::OnBnClickedRadioHorver()
{
	m_TiePointType = TP_type_hor_ver;
}

void CTieObjMeasureDlg::OnBnClickedRadioHor()
{
	m_TiePointType = TP_type_horizon;
}

void CTieObjMeasureDlg::OnBnClickedRadioVer()
{
	m_TiePointType = TP_type_vertical;
}


void CTieObjMeasureDlg::OnListOpenalignproject()
{
	CFileDialog  dlg(TRUE,"apj",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"Align工程文件(*.apj)|*.apj|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString strPath=dlg.GetPathName();

	m_AlgPrj.Open(strPath.GetBuffer(0));


	InitTieObjList();
	InitMeasuredValueList();

	LayoutTieObjID();	
}

bool CTieObjMeasureDlg::readTiepoints(char *fileName)
{
	FILE *fp=NULL;
	fp=fopen(fileName, "r");
	if(fp==NULL)	
		return false;

	char pLine[1024];
	int MAX_STRING=1024;
	int n3Dpts, imgNum, projN;
	int imgId;
	double xp, yp;
	int i, j;

	m_tiePts.clear();
	ATT_tiePoint tiePt;
	while(!feof(fp)) 
	{
		fgets(pLine, MAX_STRING, fp);
		if(strstr(pLine, "[tracks]"))
		{
			fscanf( fp, "%d\n", &n3Dpts );

			fgets(pLine, MAX_STRING, fp);
			if(strstr(pLine, "[imageNum]"))
			{
				fscanf( fp, "%d\n", &imgNum );
			}

			for(i=0; i<n3Dpts; i++)
			{
				tiePt.projs.clear();

				int tieID;
				fscanf(fp, "%ld%d ", &tieID, &projN);
				tiePt.tieId = tieID;

				if( projN< 8 )
				{//至少三度重叠
					for(j=0; j<projN; j++)
					{
						fscanf(fp, "%d %lf %lf ", &imgId, &xp, &yp);
					}				
					continue;
				}

				for(j=0; j<projN; j++)
				{
					fscanf(fp, "%d %lf %lf ", &imgId, &xp, &yp);

					ATT_ImagePoint  imgPt;
					imgPt.imgID = imgId;
					imgPt.xi = xp;
					imgPt.yi = yp;

					tiePt.projs.push_back(imgPt);

				}
				fscanf(fp, "\n");

				m_tiePts.push_back(tiePt);
			}
		}
	}
	fclose(fp);

	return true;
}

void CTieObjMeasureDlg::OnListLoadtiepoints()
{
	CFileDialog  dlg(true, "连接点", NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.txt)|*.txt|(*.*)|*.*||",NULL);	 

	if(dlg.DoModal()!=IDOK)
		return;

	CString	strPathName=dlg.GetPathName();

	if(!readTiepoints(strPathName.GetBuffer(0)))
		return;

}
