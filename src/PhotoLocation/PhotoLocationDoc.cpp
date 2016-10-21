// PhotoLocationDoc.cpp : implementation of the CPhotoLocationDoc class
//

#include "stdafx.h"
#include "PhotoLocation.h"

#include "PhotoLocationDoc.h"
#include "PhotoLocationView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhotoLocationDoc

IMPLEMENT_DYNCREATE(CPhotoLocationDoc, CDocument)

BEGIN_MESSAGE_MAP(CPhotoLocationDoc, CDocument)
	//{{AFX_MSG_MAP(CPhotoLocationDoc)
	ON_COMMAND(ID_OpenBundle, OnOpenBundle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhotoLocationDoc construction/destruction

CPhotoLocationDoc::CPhotoLocationDoc()
{
	// TODO: add one-time construction code here

}

CPhotoLocationDoc::~CPhotoLocationDoc()
{
}

BOOL CPhotoLocationDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CPhotoLocationDoc serialization

void CPhotoLocationDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPhotoLocationDoc diagnostics

#ifdef _DEBUG
void CPhotoLocationDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPhotoLocationDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPhotoLocationDoc commands
void CPhotoLocationDoc::OnDraw3D(C3DView *pView)
{
// 	int	i, j;
// 	long index=0;
// 	POINT3D		pt;
// 	//POINT3D_IMG	pt_IMG[3];
// 	POINT3D		*pTemp=NULL;
// 	float color[3] = { 1.0, 0, 0};
// 	//	float		color[3]={1,0,0};
// 	
// 	pt.X = 1;
// 	pt.Y = 1;
// 	pt.Z = 1;
// 
// 	pView->DrawVertex(&pt, color);


	
}

void CPhotoLocationDoc::OnOpenBundle() 
{
	InitViewer();
	CFileDialog	dlg(TRUE, "out", NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("bundle File (*.out)|*.out|All Files(*.*)|*.*||"));

	if(dlg.DoModal()!=IDOK)
		return;

	CString	strPath;
	strPath=dlg.GetPathName();
	
	FILE *f = fopen(strPath.GetBuffer(128), "r");
    if (f == NULL) 
	{
		printf("Error opening file %s for reading\n", strPath.GetBuffer(128));
		return;
    }
	
    //int num_images, num_points;
	double bundle_version;
	
	int i;
    char first_line[256];
    fgets(first_line, 256, f);
    if (first_line[0] == '#') 
	{
        double version;
        sscanf(first_line, "# Bundle file v%lf", &version);
		
        bundle_version = version;
        printf("[ReadBundleFile] Bundle version: %0.3f\n", version);
		
        fscanf(f, "%d %d\n", &m_num_images, &m_num_points);
    }
	else if (first_line[0] == 'v') 
	{
        double version;
        sscanf(first_line, "v%lf", &version);
        bundle_version = version;
        printf("[ReadBundleFile] Bundle version: %0.3f\n", version);
		
        fscanf(f, "%d %d\n", &m_num_images, &m_num_points);
    } 
	else
	{
        bundle_version = 0.1;
        sscanf(first_line, "%d %d\n", &m_num_images, &m_num_points);
    }
	
    printf("[ReadBundleFile] Reading %d images and %d points...\n",
		m_num_images, m_num_points);
	
	m_cameras.clear();
    /* Read cameras */
    for (i = 0; i < m_num_images; i++) 
	{
		double focal_length, k0, k1;
		double R[9];
		double t[3];
        
        if (bundle_version < 0.2) 
		{
            /* Focal length */
            fscanf(f, "%lf\n", &focal_length);
        } 
		else 
		{
            fscanf(f, "%lf %lf %lf\n", &focal_length, &k0, &k1);
        }
		
		/* Rotation */
		fscanf(f, "%lf %lf %lf\n%lf %lf %lf\n%lf %lf %lf\n", 
			R+0, R+1, R+2, R+3, R+4, R+5, R+6, R+7, R+8);
		/* Translation */
		fscanf(f, "%lf %lf %lf\n", t+0, t+1, t+2);
		
        // if (focal_length == 0.0)
        //     continue;
		
        camera_params_t cam;
		
        cam.f = focal_length;
        memcpy(cam.R, R, sizeof(double) * 9);
        memcpy(cam.t, t, sizeof(double) * 3);
		
        m_cameras.push_back(cam);
    }
    
	m_points.clear();
    /* Read points */
    for (i = 0; i < m_num_points; i++) 
	{
		point_t pt;
		
		/* Position */
		fscanf(f, "%lf %lf %lf\n", 
			pt.pos + 0, pt.pos + 1, pt.pos + 2);
		
		/* Color */
		fscanf(f, "%lf %lf %lf\n", 
			pt.color + 0, pt.color + 1, pt.color + 2);
		
		int num_visible;
		fscanf(f, "%d", &num_visible);
		
		for (int j = 0; j < num_visible; j++) 
		{
			int view, key;
			fscanf(f, "%d %d", &view, &key);
			
            double x, y;
            if (bundle_version >= 0.3)
                fscanf(f, "%lf %lf", &x, &y);
		}
		
        if (num_visible > 0) 
		{
            m_points.push_back(pt);
        }
    }
	
    fclose(f);


	CView *pView=((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();
	//OnDraw3D((C3DView *)pView);
	GetCameraPOS();

}


void CPhotoLocationDoc::InitViewer()
{
	CView *pView=((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->GetActiveView();

	if(pView->IsKindOf(RUNTIME_CLASS(CPhotoLocationView)))
		((CPhotoLocationView*)pView)->AttachDocument(this);
	
}

void CPhotoLocationDoc::GetCameraPOS()
{
	int	i;
	camera_params_t cam;
	imageframe	frame;		//假设初始光心位置(0,0,0),变换到目前的位置

	int imgWid=640;
	int imgHei=480;
	float pixelWid=0.035;  //(mm)
	
	double	wid=imgWid*pixelWid;
	double  hei=imgHei*pixelWid;

	for(i=0; i<m_cameras.size(); i++)
	{
		cam=m_cameras[i];

		frame.optcenter.X=0;
		frame.optcenter.Y=0;
		frame.optcenter.Z=0;

		frame.princip.X=0;
		frame.princip.Y=0;
		frame.princip.Z=-cam.f_focal;

		frame.frame[0].X=-wid/2;
		frame.frame[0].Y=hei/2;
		frame.frame[0].Z=-cam.f_focal;
		frame.frame[1].X=wid/2;
		frame.frame[1].Y=hei/2;
		frame.frame[1].Z=-cam.f_focal;
		frame.frame[2].X=wid/2;
		frame.frame[2].Y=-hei/2;
		frame.frame[2].Z=-cam.f_focal;
		frame.frame[3].X=-wid/2;
		frame.frame[3].Y=-hei/2;
		frame.frame[3].Z=-cam.f_focal;

		
	}
}




















