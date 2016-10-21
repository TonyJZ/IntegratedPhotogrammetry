
// PhotoOrientationDoc.cpp : CPhotoOrientationDoc 类的实现
//

#include "stdafx.h"
#include "PhotoOrientation.h"

#include "PhotoOrientationDoc.h"
#include "lidBase.h"

#include "..\..\src\ImageMatch\sift.h"
#include "..\..\src\ImageMatch\KeyMatch.h"
#include "orsGuiBase/orsIProcessMsgBar.h"
#include "..\ImageGeoProcess\ImageGeometry.h"
#include "Geometry.h"

#define __BUNDLER__
#include "bundler\Bundler_zj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPhotoOrientationDoc

IMPLEMENT_DYNCREATE(CPhotoOrientationDoc, CDocument)

BEGIN_MESSAGE_MAP(CPhotoOrientationDoc, CDocument)
	ON_COMMAND(ID_ImportRawPOS, &CPhotoOrientationDoc::OnImportrawpos)
	ON_COMMAND(ID_Export_iop, &CPhotoOrientationDoc::OnExportiop)
	ON_COMMAND(ID_Export_aop, &CPhotoOrientationDoc::OnExportaop)
	ON_UPDATE_COMMAND_UI(ID_Export_iop, &CPhotoOrientationDoc::OnUpdateExportiop)
	ON_UPDATE_COMMAND_UI(ID_Export_aop, &CPhotoOrientationDoc::OnUpdateExportaop)
	ON_COMMAND(ID_TPlane, &CPhotoOrientationDoc::OnTplane)
	ON_COMMAND(ID_RoughOrientation, &CPhotoOrientationDoc::OnRoughorientation)
END_MESSAGE_MAP()


// CPhotoOrientationDoc 构造/析构

CPhotoOrientationDoc::CPhotoOrientationDoc()
{
	// TODO: 在此添加一次性构造代码

}

CPhotoOrientationDoc::~CPhotoOrientationDoc()
{
}

BOOL CPhotoOrientationDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CPhotoOrientationDoc 序列化

void CPhotoOrientationDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CPhotoOrientationDoc 诊断

#ifdef _DEBUG
void CPhotoOrientationDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPhotoOrientationDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

#define  MAX_STRING 10240
// CPhotoOrientationDoc 命令
bool ReadPOS_ALS50(char* pPosFileName,  vector<POSData_ALS50> &PosData)
{
	double	s=PI/180;
	char	strLine[MAX_STRING];
	FILE 	*file=NULL;
	POSData_ALS50	imgPos;
	int ID;
	int i;
	double time, Xs, Ys, Zs;
	double omega, phi, kappa;
	double lat, lon;
	double	R[9];
	char imgName[128];


	file=fopen(pPosFileName, "r");
	if(file==0)
		return false;

	//	file.ReadString(strLine);
	//	file.ReadString(strLine);
	while (fgets(strLine, MAX_STRING, file))
	{

		sscanf(strLine,"%s%d%lf%lf%lf%lf%lf%lf%lf%lf%lf\n", imgName,&ID,&time,
			&Xs,&Ys,&Zs,&omega,&phi,&kappa,&lat,&lon);

		imgPos.ImageID=ID;
		strcpy(imgPos.ImageName, imgName);
		//		imgPos.ImageName=_T(imgName);
		imgPos.GPSTime=time;
		imgPos.Xs=Xs;
		imgPos.Ys=Ys;
		imgPos.Zs=Zs;



		//		RotateMat_wfk(omega*s, phi*s, (kappa+180)*s, R);
		//		R2fwk(R, &(imgPos.phi), &(imgPos.omega), &(imgPos.kappa));

		imgPos.omega=omega*PI/180;
		imgPos.phi=phi*PI/180;
		imgPos.kappa=(kappa+180)*PI/180;
		imgPos.Lat=lat*PI/180;
		imgPos.Lon=lon*PI/180;
		imgPos.hei=Zs;

		PosData.push_back( imgPos );
	}

	fclose(file); 
	return	true;
}

#include "CamSettingDlg.h"
void CPhotoOrientationDoc::OnImportrawpos()
{
	CString fnameExt=_T("txt");
	CString szFilter=_T("pos Files(*.txt)|*.txt|(*.pos)|*.pos|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE,fnameExt,NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter);

	CString filename;

	int posType=0;

	if(dlg.DoModal()!=IDOK)
		return;

	filename = dlg.GetPathName();	

	//	m_strPosFile=filename;

	m_vPosData.clear();

	// 	if(posType==DMC_POS)
	// 		ReadPOS_DMC(filename, m_vPosData);
	// 	else if(posType==ALS50_POS)
	ReadPOS_ALS50(filename.GetBuffer(128), m_vPosData);

	CCamSettingDlg camDlg;
	
	if(camDlg.DoModal()!=IDOK)
		return;

	m_camera.m_x0=camDlg.m_x0;
	m_camera.m_y0=camDlg.m_y0;
	m_camera.m_f=camDlg.m_f;

	m_camera.m_imgWid=camDlg.m_ImgWid;
	m_camera.m_imgHei=camDlg.m_ImgHei;
	m_camera.m_pixelWid=camDlg.m_PixelWid;
	m_camera.m_pixelHei=camDlg.m_PixelHei;

	m_camera.m_k1=camDlg.m_k1;
	m_camera.m_k2=camDlg.m_k2;
	m_camera.m_k3=camDlg.m_k3;
	m_camera.m_p1=camDlg.m_p1;
	m_camera.m_p2=camDlg.m_p2;

	// 	else
	// 		ReadPOS_XML(filename, m_vPosData);
}

//
void CPhotoOrientationDoc::OnExportiop()
{
	CString strFolder;
	if(!_SelectFolderDialog("设置输出目录...", &strFolder, NULL))
		return;

	int i, imgNum;
	int pos;
	CString	strImgName, iopName;

	
	imgNum=m_vPosData.size();


	for(i=0; i<imgNum; i++)
	{
		strImgName=m_vPosData[i].ImageName;

//		pos = strImgName.ReverseFind( '.' );

//		iopName = strImgName.Left( pos );
		iopName += ".iop";
		iopName=strFolder+"\\"+iopName;

		//	ImgList->GetAt(i).camera.CalRotateMatrix();
		m_camera.WriteIntOrFile_iop(iopName.GetBuffer(256));
	}
}



void CPhotoOrientationDoc::OnExportaop()
{
	CString strFolder;
	if(!_SelectFolderDialog("设置输出目录...", &strFolder, NULL))
		return;

	int i, imgNum;
	int pos;
	CString	strImgName, iopName;


	imgNum=m_vPosData.size();


	for(i=0; i<imgNum; i++)
	{
		strImgName=m_vPosData[i].ImageName;

//		pos = strImgName.ReverseFind( '.' );

//		iopName = strImgName.Left( pos );
		iopName += ".aop";
		iopName=strFolder+"\\"+iopName;

		//	ImgList->GetAt(i).camera.CalRotateMatrix();
		m_camera.m_Xs=m_vPosData[i].Xs;
		m_camera.m_Ys=m_vPosData[i].Ys;
		m_camera.m_Zs=m_vPosData[i].Zs;
		m_camera.m_omega=m_vPosData[i].omega;
		m_camera.m_phi=m_vPosData[i].phi;
		m_camera.m_kappa=m_vPosData[i].kappa;
		m_camera.WriteExtOrFile_aop_ALS50(iopName.GetBuffer(256));
	}
}

void CPhotoOrientationDoc::OnUpdateExportiop(CCmdUI *pCmdUI)
{
	if(m_vPosData.size()>0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CPhotoOrientationDoc::OnUpdateExportaop(CCmdUI *pCmdUI)
{
	if(m_vPosData.size()>0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

#include "..\LidCalib\LidarGeometry.h"
void CPhotoOrientationDoc::OnTplane()
{
	orsLidarGeometry	lidGeo;

	lidGeo.SetORSPlatform(getPlatform());
	lidGeo.SetTPlaneCoordinate();

	double R[9];
	int i;
	double lat, lon, h, Xs, Ys, Zs;
	double roll, pitch, heading;
	double phi, omega, kappa;
	for(i=0; i<m_vPosData.size(); i++)
	{
		lat=m_vPosData[i].Lat;
		lon=m_vPosData[i].Lon;
		h=m_vPosData[i].hei;
		roll=m_vPosData[i].omega;
		pitch=m_vPosData[i].phi;
		heading=m_vPosData[i].kappa;

		lidGeo.POS_Geographic2TangentPlane(lat, lon, h, roll, pitch, heading, &Xs, &Ys, &Zs, &phi, &omega, &kappa, R);

		m_vPosData[i].Xs=Xs;
		m_vPosData[i].Ys=Ys;
		m_vPosData[i].Zs=Zs;

		m_vPosData[i].phi=phi;
		m_vPosData[i].omega=omega;
		m_vPosData[i].kappa=kappa;
	}
}

/* Load a list of image names from a file */
void LoadImageNamesFromFile(char *fileName, vector<string> *pImgNameList)
{
	// m_image_names.clear();
	FILE *file=NULL;
	string ImgName;

	file=fopen(fileName, "r");
	if(file==NULL)
		return;

	char buf[256];
	int idx = 0;

	while (fgets(buf, 256, file)) 
	{
		if (buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = 0;

		if (buf[strlen(buf)-1] == '\r')
			buf[strlen(buf)-1] = 0;

		ImgName=buf;
		pImgNameList->push_back(ImgName);

		idx++;
	}

	fclose(file);
}

void CPhotoOrientationDoc::OnRoughorientation()
{
	int i, imgNum;
	iphImgList	*ImgList;

	CWinApp	*theApp=AfxGetApp();

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

	
	string strImgName, strKeyName;
	// 	orsIImageData *imgData;
	// 	orsRect_i rect;
	// 	orsBandSet bandSet;
	// 	int		imgWid, imgHei;
	

	CSIFT	_sift;
	//	char	cmd[1024]="-glsl -v 0 -fo 4 -lm 256 -no 2 -maxd 4096";
	char	cmd[1024]="-glsl -v 0 -fo 2 -maxd 4096";
	//	char	cmd[1024]="-glsl -v 0 -fo 0";

	_sift.Startup(cmd);

	char	keyName[256];


	CFileDialog	dlg(TRUE, "txt", NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("image list file (*.txt)|*.txt|All Files(*.*)|*.*||"));

	if(dlg.DoModal()!=IDOK)
		return;

	m_ImgNameList.clear();
	m_keyNameList.clear();
	LoadImageNamesFromFile(dlg.GetPathName().GetBuffer(128), &m_ImgNameList);
	
//	CString		strKeyListFile;
//	CString		strDir;
	vector<KEYPOINT> *pKeysList;
	vector<unsigned char> *pDescList;

	imgNum=m_ImgNameList.size();

	pKeysList=new vector<KEYPOINT> [imgNum];
	pDescList=new vector<unsigned char> [imgNum];

	process = ORS_CREATE_OBJECT( orsIProcessMsgBar, "ors.process.bar.mfc");
	process->InitProgressBar("提取Sift特征...", "wait...", imgNum);
	string::size_type pos;
	for(i=0; i<imgNum; i++)
	{
		strImgName=m_ImgNameList[i];
		imgReader->open(strImgName.c_str());

		//		_sift.RunSift(imgReader.get());
		_sift.RunSift_Integrated(imgReader.get());

		pos=strImgName.rfind('.');
		strKeyName=strImgName.substr(0, pos);
		strKeyName+=_T(".key");	//特征文件与影像同名，后缀.sift.txt
		_sift.SaveSift_txt(strKeyName.c_str());

// 		pKeysList[i]=_sift.GetKeypointList();
// 		pDescList[i]=_sift.GetDescriptors();

		m_keyNameList.push_back(strKeyName);
		//		fprintf(fp, keyName);

		imgReader->close();
		process->SetPos(i+1);
	}


	CFileDialog  outdlg(FALSE,"初始匹配",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);
	if(outdlg.DoModal()!=IDOK)
		return;

	CString MatchFileName;
	MatchFileName=outdlg.GetPathName();

 	CKeyMatch	KeyMatch;
 	KeyMatch.ANNMatch_pairwise_strip(&m_keyNameList, MatchFileName.GetBuffer(128), process.get());

//	std::vector<KeypointMatch>  matches =KeyMatch.ANNMatch_pairwise(&pKeysList[0], &pDescList[0], &pKeysList[1], &pDescList[1]);
	
	CBundler		bundler;
	camera_param camInfo;

	//	bundler.GetMultiViewerTiePoints(argv[9], argv[11]);
	//	return 0;

	bundler.SetImageNameList(&m_ImgNameList);
	bundler.SetKeyNameList(&m_keyNameList);

	m_camera.m_x0=0.0598;
	m_camera.m_y0=0.1802;
	m_camera.m_f=35.8501;

	m_camera.m_imgWid=5389;
	m_camera.m_imgHei=7162;
	m_camera.m_pixelWid=0.0068;
	m_camera.m_pixelHei=0.0068;

	m_camera.m_k1=7.86931e-005;
	m_camera.m_k2=-6.59007e-008;
	m_camera.m_k3=7.96896e-012;
	m_camera.m_p1=1.70817e-024;
	m_camera.m_p2=4.40734e-024;
	m_camera.InteriorOrientation();

	memset(&camInfo, 0, sizeof(camera_param));
	camInfo.x0=m_camera.m_x0;
	camInfo.y0=m_camera.m_y0;
	camInfo.f=m_camera.m_f;
	camInfo.dx=m_camera.m_pixelWid;
	camInfo.dy=m_camera.m_pixelHei;
	camInfo.imgWid=m_camera.m_imgWid;
	camInfo.imgHei=m_camera.m_imgHei;

	bundler.SetCameraInfo(camInfo);
//	bundler.SetMatchTable_xy(&matches, 0, 1);

	if(outdlg.DoModal()!=IDOK)
		return;

	CString pszEpipolarCons;
	pszEpipolarCons=outdlg.GetPathName();

//	char pszEpipolarCons[128]="G:/D_experiment/imgRO/df/EpipolarCons.txt";

	bundler.EpipolarConstraints(MatchFileName.GetBuffer(128), pszEpipolarCons.GetBuffer(128), camInfo, 1);


	std::vector<KeypointMatch>  matches=LoadMatchTable(pszEpipolarCons, 0, 1);
	int numOfmatches=matches.size();
	PointPAIR	*matchPts=new PointPAIR[numOfmatches];

	for(i=0; i<numOfmatches; i++)
	{
		matchPts[i].xl=matches[i].m_x1;
		matchPts[i].yl=matches[i].m_y1;
		matchPts[i].xr=matches[i].m_x2;
		matchPts[i].yr=matches[i].m_y2;
	}

	CRobustReor  reor;
	reor.SetCamera(&m_camera);
	reor.Compute(matchPts, numOfmatches, 0.0068);
	modelPARA   mParam;
	double f, w, k;
	reor.GetOrientationParam(&mParam);
	
	TRACE("%13.3lf %13.3lf %13.3lf\n", mParam.XsL, mParam.YsL, mParam.ZsL);
	R2fwk(mParam.Rl, &f, &w, &k);
	TRACE("\nRl: %13.7lf %13.7lf %13.7lf\n", f, w, k);
	for( int i=0; i<3; i++) 
	{
		for( int j=0; j<3; j++)
		{
			TRACE("%10.7lf ", mParam.Rl[i*3+j] );
		}
		TRACE("\n");
	}

	TRACE("%13.3lf %13.3lf %13.3lf\n", mParam.XsR, mParam.YsR, mParam.ZsR);
	R2fwk(mParam.Rr, &f, &w, &k);
	TRACE("\nRr: %13.7lf %13.7lf %13.7lf\n", f, w, k);
	for( int i=0; i<3; i++) 
	{
		for( int j=0; j<3; j++)
		{
			TRACE("%10.7lf ", mParam.Rr[i*3+j]);
		}
		TRACE("\n");
	}

	if(matchPts)	delete matchPts;		matchPts=NULL;
}
