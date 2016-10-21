
// PhotoOrientationView.cpp : CPhotoOrientationView 类的实现
//

#include "stdafx.h"
#include "PhotoOrientation.h"

#include "PhotoOrientationDoc.h"
#include "PhotoOrientationView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPhotoOrientationView

IMPLEMENT_DYNCREATE(CPhotoOrientationView, CView)

BEGIN_MESSAGE_MAP(CPhotoOrientationView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CPhotoOrientationView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CPhotoOrientationView 构造/析构

CPhotoOrientationView::CPhotoOrientationView()
{
	// TODO: 在此处添加构造代码

}

CPhotoOrientationView::~CPhotoOrientationView()
{
}

BOOL CPhotoOrientationView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CPhotoOrientationView 绘制

void CPhotoOrientationView::OnDraw(CDC* /*pDC*/)
{
	CPhotoOrientationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CPhotoOrientationView 打印


void CPhotoOrientationView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CPhotoOrientationView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CPhotoOrientationView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CPhotoOrientationView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CPhotoOrientationView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CPhotoOrientationView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CPhotoOrientationView 诊断

#ifdef _DEBUG
void CPhotoOrientationView::AssertValid() const
{
	CView::AssertValid();
}

void CPhotoOrientationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPhotoOrientationDoc* CPhotoOrientationView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPhotoOrientationDoc)));
	return (CPhotoOrientationDoc*)m_pDocument;
}
#endif //_DEBUG


// CPhotoOrientationView 消息处理程序
