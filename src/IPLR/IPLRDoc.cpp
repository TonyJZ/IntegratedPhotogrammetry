// IPLRDoc.cpp : implementation of the CIPLRDoc class
//

#include "stdafx.h"
#include "IPLR.h"

#include "IPLRDoc.h"
#include "orsBase\orsUtil.h"
#include "orsImage/orsIImageService.h"
#include "..\ImageMatch\sift.h"
#include "KeyMatch.h"
#include "..\CheckKeyPoints\CheckKPDialog.h"
#include "orsGuiBase/orsIProcessMsgBar.h"
#include "lidBase.h"
#include "..\ImageFreeNet\ImgBlockNet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPLRDoc

IMPLEMENT_DYNCREATE(CIPLRDoc, CDocument)

BEGIN_MESSAGE_MAP(CIPLRDoc, CDocument)
	//{{AFX_MSG_MAP(CIPLRDoc)
	ON_COMMAND(ID_SIFT_KeyPoint, OnSIFTKeyPoint)
	ON_COMMAND(ID_Check_Keypoint, OnCheckKeypoint)
	ON_COMMAND(ID_Match_INIT, OnMatchINIT)
	ON_COMMAND(ID_ExporImgaop, OnExporImgaop)
	ON_COMMAND(ID_Exportblv, OnExportblv)
	ON_COMMAND(ID_ExportImgiop, OnExportImgiop)
	ON_COMMAND(ID_ExportMatchImage, OnExportMatchImage)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_SIFT_KeyPoint, &CIPLRDoc::OnUpdateSiftKeypoint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPLRDoc construction/destruction

CIPLRDoc::CIPLRDoc()
{
	// TODO: add one-time construction code here

}

CIPLRDoc::~CIPLRDoc()
{
}

BOOL CIPLRDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CIPLRDoc serialization

void CIPLRDoc::Serialize(CArchive& ar)
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
// CIPLRDoc diagnostics

#ifdef _DEBUG
void CIPLRDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIPLRDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIPLRDoc commands

static char pszKeyListFile[256];
static char pszInitMatchFile[256];

void CIPLRDoc::OnSIFTKeyPoint() 
{
	int i, imgNum;
	iphImgList	*ImgList;

	CWinApp	*theApp=AfxGetApp();

	CALMPrjManager	*pBlockManager;
	pBlockManager=((CIPLRApp*)theApp)->m_pBlockManager;
	
	orsIPlatform*  pPlatform = getPlatform();
	
	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService,pPlatform->getService(ORS_SERVICE_IMAGE) );

	ref_ptr<orsIImageSourceReader> imgReader;

	//创建一个orsIImageSourceReader对象
	orsIRegisterService *registerService = getPlatform()->getRegisterService();

	ref_ptr<orsIProcessMsgBar> process;
	
	imgReader = ORS_CREATE_OBJECT( orsIImageSourceReader, "ors.dataSource.image.reader.gdal");
	if( imgReader == NULL)
	{
		return;
	}

	CFileDialog	dlg(TRUE, "txt", NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("image list File (*.txt)|*.txt|All Files(*.*)|*.*||"));

	if(dlg.DoModal()!=IDOK)
		return;

	CString strImgListName=dlg.GetPathName();

	FILE *fp=NULL;
	fp=fopen(strImgListName.GetBuffer(128), "r");
	if(fp==NULL)
		return;

// 	imgNum=pBlockManager->GetImageCount();
// 	ImgList=pBlockManager->GetImgList();
// 	imgNum=ImgList->GetSize();
// 	if(imgNum<=0)
// 		return;

	CString strImgName, strKeyName;
// 	orsIImageData *imgData;
// 	orsRect_i rect;
// 	orsBandSet bandSet;
// 	int		imgWid, imgHei;
	int pos;

	CSIFT	_sift;
//	char	cmd[1024]="-glsl -v 0 -fo 4 -lm 256 -no 2 -maxd 4096";
	char	cmd[1024]="-glsl -v 0 -fo 0 -maxd 8000";
//	char	cmd[1024]="-glsl -v 0 -fo 0";

	_sift.Startup(cmd);

	process = ORS_CREATE_OBJECT( orsIProcessMsgBar, "ors.process.bar.mfc");
	process->InitProgressBar("提取Sift特征...", "wait...", 10);
	i=0;
	char pLine[256];
	while(feof(fp)==0)
	{
		fscanf(fp, "%s", pLine);

		imgReader->open(pLine);
//		_sift.RunSift_Integrated(imgReader.get());
		_sift.RunSift(imgReader.get(), 2);

		strImgName=pLine;
		pos=strImgName.ReverseFind('.');
		strKeyName=strImgName.Left(pos)+_T(".sift.txt");	//特征文件与影像同名，后缀.sift.txt
		_sift.SaveSift_txt(strKeyName.GetBuffer(128));

//		sprintf(keyName, "%s\n", strKeyName.GetBuffer(128));
		//		fprintf(fp, keyName);

		imgReader->close();
		process->SetPos(++i);
	}
	
	process->SetPos(10);
	fclose(fp);
	return;

	//测试siftGPU

// 	imgReader->open("G:/D_experiment/像片与强度影像匹配试验/080424_062105_2m.img");
// 
// 	_sift.SetBlock(1920, 3456);
// 
// 	_sift.RunSift_Integrated(imgReader.get());
// 
// 	_sift.SaveSift_txt("G:/D_experiment/像片与强度影像匹配试验/080424_062105_2m.sift.txt");
// 	
// 	return;

//	FILE	*fp;
//	CString	strKeyListFile;
	char	keyName[256];
	
//	strKeyListFile=_ExtractFileDir(ImgList->GetAt(0).strImgName);
//	strKeyListFile+=_T("\\listKeys.txt");

//	sprintf(pszKeyListFile, strKeyListFile.GetBuffer(256));

//	fp=fopen(strKeyListFile.GetBuffer(128), "w");

	process = ORS_CREATE_OBJECT( orsIProcessMsgBar, "ors.process.bar.mfc");
	process->InitProgressBar("提取Sift特征...", "wait...", imgNum);
	for(i=0; i<imgNum; i++)
	{
		strImgName=ImgList->GetAt(i).strImgName;
		imgReader->open(strImgName.GetBuffer(128));

		_sift.RunSift(imgReader.get());
//		_sift.RunSift_Integrated(imgReader.get());

		pos=strImgName.ReverseFind('.');
		strKeyName=strImgName.Left(pos)+_T(".sift.txt");	//特征文件与影像同名，后缀.sift.txt
		_sift.SaveSift_txt(strKeyName.GetBuffer(128));
		
		sprintf(keyName, "%s\n", strKeyName.GetBuffer(128));
//		fprintf(fp, keyName);

		imgReader->close();
		process->SetPos(i+1);
	}
	
//	fclose(fp);
}

void CIPLRDoc::OnCheckKeypoint() 
{
	CheckKeyPoints(getPlatform());
	
}

//近似最近邻匹配
void CIPLRDoc::OnMatchINIT() 
{
	CFileDialog	dlg(TRUE, "txt", NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("Key File (*.txt)|*.txt|All Files(*.*)|*.*||"));
	dlg.m_ofn.lpstrTitle="打开特征点列表文件";

	CString		strKeyListFile;
	CString		strMatchInit;

	if(dlg.DoModal()!=IDOK)
		return;
	
	strKeyListFile=dlg.GetPathName();

//	sprintf(pszKeyListFile, "I:\\experiment\\sift\\RawImages\\listKeys.txt");

//	strKeyListFile=pszKeyListFile;
	if(!_IsFileExist(strKeyListFile.GetBuffer(128)))
	{
		return;
	}
	
	CFileDialog	outputdlg( FALSE, " ", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("match init (*.txt)|*.txt|All Files(*.*)|*.*||"));
	outputdlg.m_ofn.lpstrTitle="初始匹配文件";	
	
	if(outputdlg.DoModal()!=IDOK)
		return;

	strMatchInit=outputdlg.GetPathName();
	

	ref_ptr<orsIProcessMsgBar> process;
	
	process = ORS_CREATE_OBJECT( orsIProcessMsgBar, "ors.process.bar.mfc");
	if( process == NULL)
	{
		AfxMessageBox("不能创建进度条!");
		return;
	}
	CKeyMatch	KeyMatch;
	KeyMatch.ANNMatch_pairwise(strKeyListFile.GetBuffer(128), strMatchInit.GetBuffer(128), process.get());
//	KeyMatch.flann_pairwise(strKeyListFile.GetBuffer(128), pszInitMatchFile, process.get());

	
	
}


void CIPLRDoc::OnExporImgaop() 
{
	CWinApp	*theApp=AfxGetApp();
	
	CALMPrjManager	*pBlockManager;
	pBlockManager=((CIPLRApp*)theApp)->m_pBlockManager;
	ASSERT(pBlockManager);

	int i, imgNum;
	iphImgList	*ImgList;
	int pos;
	CString	strImgName, aopName;
	
	ImgList=pBlockManager->GetImgList();
	imgNum=ImgList->GetSize();

	iphCamera	camera;
	double	R[9];
	double f, w, k;
	double	ang2rad=PI/180;
	for(i=0; i<imgNum; i++)
	{//测试旋转角与旋转矩阵的相互转换
		camera=ImgList->GetAt(i).camera;
		
		TRACE("m_phi=%f, m_omega=%f, kappa=%f\n", camera.m_phi*ang2rad, camera.m_omega*ang2rad, camera.m_kappa*ang2rad);
		_CalRotateMatrix(RotateSys_YXZ, camera.m_phi*ang2rad, camera.m_omega*ang2rad, camera.m_kappa*ang2rad, R);
		_CalRotateAngle(RotateSys_YXZ, R, &f, &w, &k); 
		
		TRACE("f=%f, w=%f, k=%f\n", f, w, k);

	}

	for(i=0; i<imgNum; i++)
	{
		strImgName=ImgList->GetAt(i).strImgName;

		pos = strImgName.ReverseFind( '.' );
		
		aopName = strImgName.Left( pos );
		aopName += ".aop.txt";

	//	ImgList->GetAt(i).camera.CalRotateMatrix();
		ImgList->GetAt(i).camera.WriteExtOrFile_aop(aopName.GetBuffer(256));
	}
	
}

void CIPLRDoc::OnExportblv() 
{
// 	CImgBlockNet	imgBlock;
// 	imgBlock.ExtimateFMatrix();


	CWinApp	*theApp=AfxGetApp();
	
	CALMPrjManager	*pBlockManager;
	pBlockManager=((CIPLRApp*)theApp)->m_pBlockManager;
	ASSERT(pBlockManager);
	
	int i, imgNum;
	iphImgList	*ImgList;
	int pos;
	CString	strImgName, aopName;
	
	ImgList=pBlockManager->GetImgList();
	imgNum=ImgList->GetSize();

	FILE *fp=0;

	CFileDialog	dlg( FALSE, "blv", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("Block Viewer File (*.blv)|*.blv|All Files(*.*)|*.*||"));
	
	if(dlg.DoModal()!=IDOK)
		return;

	CString	strPath=dlg.GetPathName();
//	int pos;
	int length;

	fp = fopen( strPath.GetBuffer(128), "wt" );
	if(fp==0)
		return;

	pos=strPath.ReverseFind('\\');
	length=strPath.GetLength();
	strPath=strPath.Right(length-pos-1);
	
	fprintf( fp, "dps survey block data file: V1.0\n" );
	fprintf( fp, "[BLOCK NAME]\n%s\n", strPath.GetBuffer(128) );
	fprintf( fp, "[BLOCK DIRECTORY]\n%s\n", pBlockManager->GetAlmFileDir().GetBuffer(256) );
	
	int	flyOverlap=65;
	int stripOverlap=30;

	fprintf( fp, "[FLY OVERLAY]\n%d\n", flyOverlap);
	fprintf( fp, "[STRIP OVERLAY]\n%d\n", stripOverlap );
	
	if( imgNum > 0 )
	{
		fprintf( fp, "[NUM OF IMAGES]\n%d\n", imgNum );
		
		for (i=0; i<imgNum; i++)
		{
			fprintf(fp, "%s\n", ImgList->GetAt(i).strImgName.GetBuffer(256) );
		}
	}
	
	fclose( fp );


}

void CIPLRDoc::OnExportImgiop() 
{
	CWinApp	*theApp=AfxGetApp();
	
	CALMPrjManager	*pBlockManager;
	pBlockManager=((CIPLRApp*)theApp)->m_pBlockManager;
	ASSERT(pBlockManager);
	
	int i, imgNum;
	iphImgList	*ImgList;
	int pos;
	CString	strImgName, iopName;
	
	ImgList=pBlockManager->GetImgList();
	imgNum=ImgList->GetSize();
	

	for(i=0; i<imgNum; i++)
	{
		strImgName=ImgList->GetAt(i).strImgName;
		
		pos = strImgName.ReverseFind( '.' );
		
		iopName = strImgName.Left( pos );
		iopName += ".iop.txt";
		
		//	ImgList->GetAt(i).camera.CalRotateMatrix();
		ImgList->GetAt(i).camera.WriteIntOrFile_iop(iopName.GetBuffer(256));
	}
	
}

void CIPLRDoc::OnExportMatchImage() 
{
	CFileDialog	imgdlg(TRUE, NULL, NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("(*.txt)|*.txt|All Files(*.*)|*.*||"));
	CFileDialog	matchdlg(TRUE, NULL, NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("(*.txt)|*.txt|All Files(*.*)|*.*||"));
	FILE *fp=0, *fout=0;
	CString	strImgName;
	CString	strMatchName;
	CString strPairName;
	int	pairNum;
	int i;

	
	m_imgNameList.RemoveAll();
	if(imgdlg.DoModal()!=IDOK)
		return;

	fp=fopen(imgdlg.GetPathName().GetBuffer(128), "r");
	if(fp==0)
		return;

	char buf[512];
    while (fgets(buf, 512, fp)) 
	{
        /* Remove trailing newline */
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;
        
        strImgName=buf;
		m_imgNameList.Add(strImgName);
    }

	fclose(fp); fp=0;

	if(matchdlg.DoModal()!=IDOK)
		return;
	
	strMatchName=matchdlg.GetPathName();
	fp=fopen(strMatchName.GetBuffer(128), "r");
	if(fp==0)
		return;

	strMatchName=strMatchName.Left(strMatchName.ReverseFind('\\'));
	strMatchName+="\\";
	pairNum=1;
    while (fgets(buf, 512, fp)) 
	{
		/* Read the images */
        int i1, i2;
        sscanf(buf, "%d %d\n", &i1, &i2);

		int nMatches;
        fscanf(fp, "%d\n", &nMatches);
		
		if(nMatches==0)
			continue;
		
		strPairName.Format("%s%d.rpr", strMatchName, pairNum);
		pairNum++;

		fout=fopen(strPairName.GetBuffer(128), "w");

		fprintf( fout, "[Project Directory]\n");
		fprintf( fout, "%s\n", "directory" );
		
		fprintf( fout, "\n[Image Count]\n"  );
		fprintf( fout, "%d\n", 2 );

		fprintf( fout, "%s\n", m_imgNameList[i1] );
		fprintf( fout, "%s\n", m_imgNameList[i2] );
		
		

		fprintf( fout, "\n[Matches]\n");
		fprintf( fout, "%d\n", nMatches );
	
		for (i = 0; i < nMatches; i++)
		{
            int k1, k2;
			double x1, y1, x2, y2;

            fscanf(fp, "%d %lf %lf %d %lf %lf\n", &k1, &x1, &y1, &k2, &x2, &y2);
			fprintf(fout, "%lf %lf %lf %lf\n", x1, y1, x2, y2);
			
		}

		fclose(fout);
		fout=0;
	}
}

//DEL void CIPLRDoc::OnRemoveCongruityErr() 
//DEL {
//DEL 	
//DEL 	
//DEL }

void CIPLRDoc::OnUpdateSiftKeypoint(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
