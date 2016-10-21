#include "StdAfx.h"
#include "CombineAdjustment/IntATManager.h"
//#include "ATNLib/ATNIO.h"
#include "orsPointCloud/orsIPointCloudReader.h"
#include "orsImageGeometry/orsIImageGeometry.h"
#include "orsImageGeometry/orsIImageGeometryService.h"
#include "orsMath/orsIMatrixService.h"
#include "dpsMatrix.h"
//#include "qsort.h"
#include <algorithm>
#include <map>

#include "combineadjustment/orsAlgCameraCalib.h"
#include "GeometryFitting/lineFitting.h"

#include "orsLidarGeometry/orsILidarGeoModel.h"

#include "lidBase.h"
#include "\OpenRS\IntegratedPhotogrammetry\external\pba\src\pba\util.h"

#include "CombineAdjustment/LensCalib.h"
//#include "vld.h"

using namespace std;

ORS_GET_MATRIX_SERVICE_IMPL();
ORS_GET_IMAGEGEOMETRY_SERVICE_IMPL();

orsIPlatform *g_platform;
orsIPlatform* getPlatform()
{
	return g_platform;
}

//输出影像的平差结果
void printImgSBAStructureAndMotionData(FILE *fp, double *motstruct, int ncams, 
	int cnp, int nties, int pnp);


//用stl中的排序算法
int ascending_projs_by_imgID(ATT_ImagePoint &p1, ATT_ImagePoint &p2)
{
	return p1.imgID < p2.imgID;
}

int ascending_ImageEXOR_by_imgName(ATT_ImageEXOR &p1, ATT_ImageEXOR &p2)
{
	int name1, name2;
	name1 = atoi(p1.imgName);
	name2 = atoi(p2.imgName);

	return name1<name2;
}

int ascending_GCP_by_ptID(ATT_GCP &p1, ATT_GCP &p2)
{
	return p1.tieId < p2.tieId;
}

int ascending_TiePts_by_ptID(ATT_tiePoint &p1, ATT_tiePoint &p2)
{
	return p1.tieId < p2.tieId;
}

//////////////////////////////////////////////////////////////////////////
CIntATManager::CIntATManager(orsIPlatform *platform)
{
//	m_rMode = ATT_rmNONE;
	m_pCamera=NULL;
	m_AlgPrj = NULL;
	g_platform = platform;

	char pTmpDir[_MAX_PATH];
	GetTempPath(_MAX_PATH, pTmpDir);

	m_outputDir = pTmpDir;

	m_center.X=m_center.Y=m_center.Z=0;

	m_imgID_LUT=NULL;

	m_camera_mount_angle[0] = m_camera_mount_angle[1] = m_camera_mount_angle[2] = 0.0;
}

CIntATManager::~CIntATManager()
{
	if(m_pCamera)	delete m_pCamera;	m_pCamera=NULL;
	if(m_imgID_LUT) delete m_imgID_LUT;	m_imgID_LUT=NULL;
}

void CIntATManager::setOutputDir(const char *pszDir)
{
	m_outputDir = pszDir;
}

void CIntATManager::setCameraParam(double x0, double y0, double f, double imgWid, double imgHei, 
	double pixWid, double pixHei, lensDistortion lensDist, double camera_mount_angle[3])
{
	if(m_pCamera == NULL)
		m_pCamera=new _iphCamera;

	m_pCamera->SetInorParameter(x0, y0, f, imgWid, imgHei, pixWid, pixHei, lensDist.k0, lensDist.k1, lensDist.k2, 
		lensDist.k3, lensDist.p1, lensDist.p2);

	if(camera_mount_angle)
	{
		m_camera_mount_angle[0] = camera_mount_angle[0];
		m_camera_mount_angle[1] = camera_mount_angle[1];
		m_camera_mount_angle[2] = camera_mount_angle[2];
	}
//	m_rMode = mode;
}

void CIntATManager::setAlignPrj(CAlignPrj *pAlg)
{
	m_AlgPrj = pAlg;

	m_blockInfo.clear();
	m_imgNameVec.clear();

	CArray<Align_LidLine, Align_LidLine> *pLidList=NULL;
	pLidList=pAlg->GetLidList();
	CArray<Align_Image, Align_Image> *pImgList=NULL;

	int i;
	int stripNum = pLidList->GetSize();
	for(i=0; i<stripNum; i++)
	{
		ATT_StripInfo  attStrip;
		Align_LidLine lidLine = pLidList->GetAt(i);

		attStrip.stripID = lidLine.LineID;
//		attStrip.atnName.clear();

		attStrip.atnName =  lidLine.LasName.GetBuffer(0);
		
		m_blockInfo.push_back(attStrip);
	}

	orsIImageGeometryService *pImageGeometryService = getImageGeometryService();
	if( !pImageGeometryService )
		return ;

	pImgList = pAlg->GetImgList();
	if(pImgList->GetSize()>0)
	{
		if(m_imgID_LUT==NULL)
			m_imgID_LUT = new myLUT;

		m_imgID_LUT->clear();
	}

	int idx=0;
	for(i=0; i<pImgList->GetSize(); i++)
	{
		Align_Image  imgItem = pImgList->GetAt(i);
		int stripID = getStripID(imgItem.nImgID);
//			imgItem.nStripID-1;
		if(stripID>m_blockInfo.size())
		{
			assert(false);
			continue;
		}

		if(m_imgID_LUT->find(imgItem.nImgID) == m_imgID_LUT->end())
		{
			m_imgID_LUT->insert(myLUT::value_type(imgItem.nImgID, idx));
			idx++;
		}

		m_imgNameVec.push_back(imgItem.ImgName.GetBuffer(0));

		ATT_ImageEXOR exor;
		exor.imgID = imgItem.nImgID;

		ref_ptr<orsIImageGeometry> pImageGeometry 
			= pImageGeometryService->loadImageGeometry( imgItem.ImgName );

		if(pImageGeometry.get())
		{
			orsString imgGeoType = pImageGeometry->GetImageGeometryType();
			if(strstr(imgGeoType.c_str(), "MetricFrame")){
				orsIProperty *pProperty = pImageGeometry->getProperty();
				pProperty->getAttr("GPSTime",exor.eoParam.time);
				pProperty->getAttr("Xs", exor.eoParam.coord.X);
				pProperty->getAttr("Ys", exor.eoParam.coord.Y);
				pProperty->getAttr("Zs", exor.eoParam.coord.Z);

				orsArray<ors_float64> arrR;
				pProperty->getAttr("R",arrR);
				for(int j=0; j<9; j++)
					exor.eoParam.R[j] = arrR[j];
			}
		}
		
		exor.eoParam.time = imgItem.timestamp;
		m_blockInfo[stripID-1].imgEOVec.push_back(exor);
	}

}

bool CIntATManager::loadnvmFile(const char* pszFileName)
{
	int minOverlap = 3;

	if(pszFileName == NULL)
		return false;

	ifstream in(pszFileName); 

	if(!in.is_open()) 
		return false;

	vector<CameraT> camera_data;
	vector<string> names;
	vector<Point3D> point_data;
	vector<Point2D> measurements;
	vector<int> ptidx;
	vector<int> camidx; 
	vector<int> ptc;

	if(strstr(pszFileName, ".nvm"))
		LoadNVM(in, camera_data, point_data, measurements, ptidx, camidx, names, ptc);

	if(m_AlgPrj==NULL)
		return false;

	CArray<Align_Image, Align_Image> *pImgList=NULL;
	pImgList = m_AlgPrj->GetImgList();

	//1. 根据文件名找到相片编号，再确定相片的索引号
	int i;
	vector<int> imgID_vec;
	for(vector<string>::iterator nameIter=names.begin(); nameIter!=names.end(); nameIter++)
	{
		CString imgName = nameIter->c_str();
		CString pureName = _ExtractFileName(imgName);
		
		for(i=0; i<pImgList->GetSize(); i++)
		{
			CString pureName1 = _ExtractFileName(pImgList->GetAt(i).ImgName);

			if(pureName.Compare(pureName1)==0)
			{
				imgID_vec.push_back(pImgList->GetAt(i).nImgID);//按nvm文件中的影像顺序记录影像ID
				break;
			}
		}
	}

	//2.读入连接点坐标
	m_tiePts.clear();
	m_nprojs=0;
	int tieNum = point_data.size();
//	m_tiePts.resize(tieNum);
	int tieID=0;

	double centX, centY;  //相片中心坐标
	centX = m_pCamera->m_imgWid*0.5;
	centY = m_pCamera->m_imgHei*0.5;

	ATT_tiePoint tiePt;
	for(i=0; i<ptidx.size(); i++)
	{//遍历全部连接点
		if(ptidx[i] != tieID)
		{
			if(tiePt.projs.size() >= minOverlap)
			{
				m_tiePts.push_back(tiePt);
				m_nprojs += tiePt.projs.size();
			}
			tiePt.projs.clear();
			tieID = ptidx[i];
		}


		ATT_ImagePoint  imgPt;
		int cidx = camidx[i];
		imgPt.imgID = imgID_vec[cidx];

		imgPt.xi = measurements[i].x + centX;
		imgPt.yi = measurements[i].y + centY;

		m_pCamera->Image2Photo(imgPt.xi, imgPt.yi, imgPt.xp, imgPt.yp);	//像素坐标转换

		tiePt.projs.push_back(imgPt);
	}

	//最后一个连接点
	if(tiePt.projs.size() >= minOverlap){
		m_tiePts.push_back(tiePt);
		m_nprojs += tiePt.projs.size();
	}

	//对每个连接点按相片号重排
	std::vector<ATT_tiePoint>::iterator tieIter;
	for(tieIter=m_tiePts.begin(); tieIter!=m_tiePts.end(); tieIter++)
	{
		sort(tieIter->projs.begin(), tieIter->projs.end(), ascending_projs_by_imgID);
	}
	
	return true;
}

bool CIntATManager::loadPATBconFile(const char *pFileName)
{
	char pLine[1024];
	int MAX_STRING=1024;
	FILE *fp=NULL;

	fp = fopen(pFileName, "rt");
	if(fp==NULL)
		return false;

	typedef std::map<int, ATT_GCP*> my_GCP_map;
	my_GCP_map::iterator map_element;
	my_GCP_map	mapGCP;
//	std::vector<ATT_GCP> vecGCP;
	ATT_GCP gcp;

	m_GCP.clear();
	m_GCP.reserve(5000);	//预先开足够大的空间，避免重新分配内存造成map中的地址错误
	
	int segmark;
	int flag;
	int ID = 0;
	int imgID=0; 
	char imgName[128];
	double X, Y, Z;
	double time;
	int stripID = 0;
	ATT_StripInfo stripInfo;
//	ATT_ImageEXOR  imgExor;

	m_imgNameVec.clear();
	m_blockInfo.clear();
	while(!feof(fp)) 
	{
		fgets(pLine, MAX_STRING, fp);
	
		if(sscanf(pLine, "%s%lf%lf%lf%lf%d", imgName, &X, &Y, &Z, &time, &flag)==6)
		{
			ATT_ImageEXOR exor;
			exor.imgID = imgID;

			exor.eoParam.time = time;
			exor.eoParam.coord.X = X;
			exor.eoParam.coord.Y = Y;
			exor.eoParam.coord.Z = Z;

			strncpy(exor.imgName, imgName, 64);

			stripInfo.imgEOVec.push_back(exor);
			m_imgNameVec.push_back(imgName);

			imgID++;
		}
		else if(sscanf(pLine, "%d%lf%lf%d", &ID, &X, &Y, &flag)==4)
		{
			map_element = mapGCP.find(ID);
			if(map_element == mapGCP.end())
			{
				gcp.tieId = ID;
				gcp.X = X;
				gcp.Y = Y;
				gcp.tType=TiePtTYPE_GCP_XY;
				m_GCP.push_back(gcp);

				mapGCP.insert(my_GCP_map::value_type(ID, &(m_GCP[m_GCP.size()-1])));
			}
			else
			{
				ATT_GCP *ptr = (*map_element).second;
				ptr->X = X;
				ptr->Y = Y;
				ptr->tType = ptr->tType | TiePtTYPE_GCP_XY;
			}
		}
		else if(sscanf(pLine, "%d%lf%d", &ID, &Z, &flag)==3)
		{
			map_element = mapGCP.find(ID);
			if(map_element == mapGCP.end())
			{
				gcp.tieId = ID;
				gcp.Z = Z;
				gcp.tType=TiePtTYPE_GCP_Z;
				m_GCP.push_back(gcp);

				mapGCP.insert(my_GCP_map::value_type(ID, &(m_GCP[m_GCP.size()-1])));
			}
			else
			{
				ATT_GCP *ptr = (*map_element).second;
				ptr->Z = Z;
				ptr->tType = ptr->tType | TiePtTYPE_GCP_Z;
			}

		}
		else if(sscanf(pLine, "%d", &segmark)==1)
		{
			if(segmark == 0 || segmark == -99)
			{
				continue;
			}
			else if(segmark == -9)
			{
				stripInfo.stripID = stripID;
				stripID++;
				m_blockInfo.push_back(stripInfo);
				stripInfo.imgEOVec.clear();
				continue;
			}
		}
	}
	fclose(fp);
	fp=NULL;

	//整理控制点
	sort(m_GCP.begin(), m_GCP.end(), ascending_GCP_by_ptID);

	return true;
}

bool CIntATManager::loadGPSEOFile(const char *pFileName)
{
	char pLine[1024];
	int MAX_STRING=1024;
	FILE *fp=NULL;

	fp = fopen(pFileName, "rt");
	if(fp==NULL)
		return false;

	int ID = 0;
	char imgName[128];
	double X, Y, Z;
	double phi, omega, kappa;
	double time, t0;
	int stripID = 0;
	bool bFirst=false;
	ATT_StripInfo stripInfo;
	//	ATT_ImageEXOR  imgExor;

	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

	m_imgNameVec.clear();
	m_blockInfo.clear();
	while(!feof(fp)) 
	{
		pLine[0] = '\0';
		fgets(pLine, MAX_STRING, fp);
		if(strstr(pLine, "strip seted"))
			continue;
		if(strstr(pLine, "begin strip"))
		{
			stripInfo.stripID = stripID;
			stripID++;
			stripInfo.imgEOVec.clear();

			bFirst = true;
			continue;
		}
		
		if(sscanf(pLine, "%s%lf%lf%lf%lf%lf%lf%lf", imgName, &time, &X, &Y, &Z, &phi,
			&omega, &kappa)==8)
		{
			if(bFirst)
			{
				t0 = time;
				bFirst = false;
			}

			ATT_ImageEXOR exor;
			exor.imgID = ID;
			exor.survID = /*stripInfo.stripID*/-1;
			exor.t0 = t0;

			exor.eoParam.time = time;
			exor.eoParam.coord.X = X;
			exor.eoParam.coord.Y = Y;
			exor.eoParam.coord.Z = Z;
			
			matrixService->RotateMatrix_fwk(phi/180.0*PI, omega/180.0*PI, 
				kappa/180.0*PI, exor.eoParam.R);
			
			strncpy(exor.imgName, imgName, 64);

			stripInfo.imgEOVec.push_back(exor);
			m_imgNameVec.push_back(imgName);

			ID++;
			continue;
		}
		else if(strstr(pLine, "end"))
		{
			m_blockInfo.push_back(stripInfo);
			continue;
		}
	}
	fclose(fp);
	fp=NULL;

	return true;
}

void CIntATManager::calImageAngleElement_byGPS()
{
	int stripNum = getStripNum();
	
	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

	vector<ATT_StripInfo>::iterator stripIter;
	vector<orsPOINT2D>	gpsVec;
	vector<double> lineParam;
	double R[9];
	for(stripIter=m_blockInfo.begin(); stripIter!=m_blockInfo.end(); stripIter++)
	{
		int stripID = stripIter->stripID;
		vector<ATT_ImageEXOR>::iterator imgIter;
		
		gpsVec.clear();
		for(imgIter=stripIter->imgEOVec.begin(); imgIter!=stripIter->imgEOVec.end(); imgIter++)
		{
			gpsVec.push_back(orsPOINT2D(imgIter->eoParam.coord.X, imgIter->eoParam.coord.Y));
		}

//		line2DFitting(gpsVec, lineParam);

// 		lineParam[0] = gpsVec[0].x - gpsVec[gpsVec.size()-1].x;
// 		lineParam[1] = gpsVec[0].y - gpsVec[gpsVec.size()-1].y;

		double phi=0;
		double omega=0;
		double kappa;
		double nx = gpsVec[0].x - gpsVec[gpsVec.size()-1].x;
		double ny = gpsVec[0].y - gpsVec[gpsVec.size()-1].y;
		if(nx == 0)
			kappa = PI/2;
		else
			kappa = atan(ny/nx);

		kappa += PI;

		matrixService->RotateMatrix_fwk(phi, omega, kappa, R);

		for(imgIter=stripIter->imgEOVec.begin(); imgIter!=stripIter->imgEOVec.end(); imgIter++)
		{
			memcpy(imgIter->eoParam.R, R, sizeof(double)*9);
		}
	}
}

bool CIntATManager::loadoriFile(const char *pFileName)
{
	char pLine[1024];
	int MAX_STRING=1024;
	FILE *fp=NULL;

	fp = fopen(pFileName, "rt");
	if(fp==NULL)
		return false;

	int ID = 0;
	char imgName[128];
	double X, Y, Z;
	double phi, omega, kappa;
	double time;
	int stripID = 0;
	ATT_StripInfo stripInfo;
	//	ATT_ImageEXOR  imgExor;

// 	m_imgNameVec.clear();
 	m_blockInfo.clear();
	
	while(!feof(fp)) 
	{
//		pLine[0] = '\0';
		ATT_ImageEXOR exor;
		double time, X, Y, Z;
		double R[9];
//		fgets(pLine, MAX_STRING, fp);
		if(fscanf(fp, "%s%lf%lf%lf%lf", exor.imgName, &time, &X, &Y, &Z)!=5)
			continue;

//		fgets(pLine, MAX_STRING, fp);
		fscanf(fp, "%lf%lf%lf%lf%lf", R, R+1, R+2, R+3, R+4);
		
//		fgets(pLine, MAX_STRING, fp);
		fscanf(fp, "%lf%lf%lf%lf", R+5, R+6, R+7, R+8);

		exor.eoParam.time = time;
		exor.eoParam.coord.X = X;
		exor.eoParam.coord.Y = Y;
		exor.eoParam.coord.Z = Z;

		memcpy(exor.eoParam.R, R, sizeof(double)*9);

		stripInfo.stripID = stripID;
		stripInfo.imgEOVec.push_back(exor);

	}
	
	//按相片名排序
	sort(stripInfo.imgEOVec.begin(), stripInfo.imgEOVec.end(), ascending_ImageEXOR_by_imgName);
	
	int i=0;
	std::vector<ATT_ImageEXOR>::iterator eoIter;
	for(eoIter=stripInfo.imgEOVec.begin(); eoIter!=stripInfo.imgEOVec.end(); eoIter++, i++)
	{
		eoIter->imgID = i;
	}

	m_blockInfo.push_back(stripInfo);

	fclose(fp);
	fp=NULL;

	return true;
}

bool CIntATManager::loadadjFile(const char *pFileName)
{
	char pLine[1024];
	int MAX_STRING=1024;
	FILE *fp=NULL;

	fp = fopen(pFileName, "rt");
	if(fp==NULL)
		return false;

	int ID = 0;
	double X, Y, Z;
	int flag;
	
	std::vector<ATT_tiePoint>::iterator tieIter;
	tieIter=m_tiePts.begin();

	while(!feof(fp)) 
	{
		fgets(pLine, MAX_STRING, fp);
		if(sscanf(pLine, "%d%lf%lf%lf%d", &ID, &X, &Y, &Z, &flag)==5)
		{//连接点
			if(ID == tieIter->tieId)
			{
				tieIter->X = X;
				tieIter->Y = Y;
				tieIter->Z = Z;
			}

			if(tieIter!=m_tiePts.end())
				tieIter++;
		}
	}


	fclose(fp);
	fp=NULL;

	return true;
}

bool CIntATManager::loadPATBimFile(const char *pFileName)
{
	char pLine[1024];
	int MAX_STRING=1024;
	FILE *fp=NULL;

	fp = fopen(pFileName, "rt");
	if(fp==NULL)
		return false;

	double x, y;
	double f;
	int ptID, flag, segmark;
	char imgName[128];
	int imgID;

	m_tiePts.clear();

	typedef std::map<int, int> my_tie_map;
	my_tie_map::iterator map_element;
	my_tie_map	mapTies;
	ATT_tiePoint  tiePt;
	ATT_ImagePoint proj;

	while(!feof(fp)) 
	{
		fgets(pLine, MAX_STRING, fp);

		if(sscanf(pLine, "%d%lf%lf%d", &ptID, &x, &y, &flag)==4)
		{//连接点
			proj.imgID = imgID;
			proj.xi = proj.yi = 0;
			proj.xp = x/1000;
			proj.yp = y/1000;

			map_element = mapTies.find(ptID);
			if(map_element == mapTies.end())
			{
				tiePt.projs.clear();

				tiePt.tieId = ptID;
				tiePt.projs.push_back(proj);
				m_tiePts.push_back(tiePt);

				mapTies.insert(my_tie_map::value_type(ptID, m_tiePts.size()-1));//存索引值
			}
			else
			{
				int idx = (*map_element).second;
				ATT_tiePoint *ptr = &(m_tiePts[idx]);

				ptr->projs.push_back(proj);
			}
		}
		else if(sscanf(pLine, "%s%lf%d", &imgName, &f, &flag)==3)
		{//相片
			imgID = getImageID(imgName);

			if(imgID<0)
			{
				imgID = m_imgNameVec.size();
				m_imgNameVec.push_back(imgName);
			}

			continue;
		}
		else if(sscanf(pLine, "%d", &segmark)==1)
		{
			if(segmark == 0 || segmark == -99)
			{
				continue;
			}

		}
	}
	fclose(fp);
	fp=NULL;

	std::vector<ATT_tiePoint>::iterator tieIter;
	for(tieIter=m_tiePts.begin(); tieIter!=m_tiePts.end(); tieIter++)
	{//像点按相片号排序
		sort(tieIter->projs.begin(), tieIter->projs.end(), ascending_projs_by_imgID);
	}

	//连接点按点号排序
	sort(m_tiePts.begin(), m_tiePts.end(), ascending_TiePts_by_ptID);

	//将控制点信息更新到tiePts数组中
	if(m_GCP.size()>0)
	{
		for(int i=0, j=0; i<m_GCP.size(); i++)
		{
			for(; j<m_tiePts.size(); j++)
			{
				if(m_GCP[i].tieId == m_tiePts[j].tieId)
				{
					m_tiePts[j].tType = m_GCP[i].tType;
					m_tiePts[j].X = m_GCP[i].X;
					m_tiePts[j].Y = m_GCP[i].Y;
					m_tiePts[j].Z = m_GCP[i].Z;
					break;
				}
			}
		}
	}

	//删除两度连接点
// 	for(tieIter=m_tiePts.begin(); tieIter!=m_tiePts.end(); )
// 	{
// 		if(tieIter->projs.size()<3 && tieIter->tType == TiePtTYPE_TIE)
// 		{
// 			TRACE("error tie %d\n", tieIter->tieId);
// 			tieIter = m_tiePts.erase(tieIter);
// 		}
// 		else
// 			tieIter++;
// 	}

	return true;
}

int CIntATManager::getStripNum()
{
	return m_blockInfo.size();
}

int CIntATManager::getSurvAreaNum()
{
	int nSurv=0;

	std::vector<ATT_StripInfo>::iterator stripIter;
	std::vector<ATT_ImageEXOR>::iterator imgIter;
	
	for(stripIter=m_blockInfo.begin(); stripIter!=m_blockInfo.end(); stripIter++)
	{
		for(imgIter=stripIter->imgEOVec.begin(); imgIter!=stripIter->imgEOVec.end(); imgIter++)
		{
			if(nSurv <= imgIter->survID)
				nSurv++;
		}
	}

	return nSurv;
}

int CIntATManager::getSumofImages()
{
	std::vector<ATT_StripInfo>::iterator iter;
	int num = 0;
	for(iter=m_blockInfo.begin(); iter!=m_blockInfo.end(); iter++)
	{
		num += iter->imgEOVec.size();
	}

	return num;
}

std::string CIntATManager::getImageName(int imgId)
{
	if(imgId<0 || imgId>getSumofImages())
		return "";

	return m_imgNameVec[imgId];
}

int CIntATManager::getImageID(const char *imgName)
{
	for(int i=0; i<m_imgNameVec.size(); i++)
	{
		if(strstr(m_imgNameVec[i].c_str(), imgName))
			return i;
	}

	return -1;
}

//只提取定向锚点
std::vector<orsPOSParam>* CIntATManager::get_OrientedArchorsFromLIDAR(double interval)
{
	if(m_blockInfo.size()==0)
		return NULL;

	m_OrAVec.clear();

	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );


	vector<ATT_StripInfo>::iterator stripIter;
	for(stripIter=m_blockInfo.begin(); stripIter!=m_blockInfo.end(); stripIter++)
	{
		int stripID = stripIter->stripID;
		std::string atnName = stripIter->atnName;
		vector<ATT_ImageEXOR>::iterator imgIter;
		double mint, maxt;

		MAX_DOUBLE(mint);
		MIN_DOUBLE(maxt);

		//在相片曝光时间范围内等间距取定向锚点
		for(imgIter=stripIter->imgEOVec.begin(); imgIter!=stripIter->imgEOVec.end(); imgIter++)
		{
			double t = imgIter->eoParam.time;
			if(t<mint)
				mint = t;
			if(t>maxt)
				maxt=t;
		}
		
		mint -= interval/2;
		maxt += interval/2;

		orsPointObservedInfo info_reserved;

		ref_ptr<orsIPointCloudReader> reader = ORS_CREATE_OBJECT(orsIPointCloudReader, ORS_INTERFACE_POINTCLOUDSOURCE_READER_ATN);
		if(reader.get()==NULL)
			return NULL;

		reader->open(atnName.c_str());
		int preID=m_OrAVec.size();	//合并多个条带的定向点
		double xyz[3];
		double gpstime, res_time;
		while (reader->read_point(xyz))
		{//提取定向点航迹
			
			if(mint > maxt+3*interval)
				break;

			gpstime = reader->get_gpstime();
			if(gpstime < mint)
			{
//				resPt = atnpt;
				reader->get_point_observed_info(&info_reserved);
				res_time = gpstime;
				continue;
			}
			else
			{
				orsPOSParam orpt;
				orpt.stripID = stripID;
				orpt.time         = res_time;
				orpt.coord.lat = info_reserved.pos.lat;
				orpt.coord.lon = info_reserved.pos.lon;
				orpt.coord.h   = info_reserved.pos.h;
				orpt.r = info_reserved.roll;
				orpt.p = info_reserved.pitch;
				orpt.h = info_reserved.heading;

				matrixService->RotateMatrix_rph(orpt.r, orpt.p, orpt.h, orpt.R);

				m_OrAVec.push_back(orpt);

				mint += interval;
			}
		}
		reader->close();

		std::vector<orsPOSParam>::iterator orIter;
		imgIter=stripIter->imgEOVec.begin(); //遍历当前条带的影像，并确定每张相片的定向锚点

		for(orIter=m_OrAVec.begin()+preID; orIter!=m_OrAVec.end(); orIter++)
		{//找到影像的定向点
			if(orIter->time > imgIter->eoParam.time)
			{
//				orIter._Ptr;
				imgIter->orientedTraj.push_back(*(orIter-1));//目前用线性内插，只记录2个定向点
				imgIter->orientedTraj.push_back(*orIter);

// 				orsPOSParam* pos1 = imgIter->orientedTraj[0];
// 				orsPOSParam* pos2 = imgIter->orientedTraj[1];

				imgIter++;
				if(imgIter == stripIter->imgEOVec.end())
					break;
			}
		}

	}

	return &m_OrAVec;
}

bool CIntATManager::output_OrientedArchors(const char *pFileName)
{
	FILE *fp=NULL;

	fp = fopen(pFileName, "wt");
	if(fp==NULL)
		return false;

	int id=1;
	for(std::vector<orsPOSParam>::iterator archIter = m_OrAVec.begin();
		archIter!=m_OrAVec.end(); archIter++)
	{
		fprintf(fp, "%d %d %lf %f %f %f %f %f %f\n", id, archIter->stripID, 
			archIter->time, archIter->coord.X, archIter->coord.Y, archIter->coord.Z,
			archIter->r, archIter->p, archIter->h);

		id++;
	}

	fclose(fp);
	return true;
}

bool CIntATManager::loadTiePoints(const char* pszFileName)
{
	FILE *fp=NULL;
	char pLine[1024];
	int MAX_STRING=1024;
	int gcpNum, conNum;
	int i, j;
	double xg,yg,zg;
	int  imgId;
	double xp, yp, zp;

//	int nprojs;
	int n3Dpts;
	int projN;
	int imgNum;
//	int *pFrameProjs=NULL;

	fp = fopen(pszFileName, "rt");
	if(fp==NULL)
		return false;

	m_tiePts.clear();
	m_nprojs=0;

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

				ors_int64 tieID;
				fscanf(fp, "%I64d%d ", &tieID, &projN);
				tiePt.tieId = tieID;

				if( projN< 3 )
				{//至少三度重叠
					for(j=0; j<projN; j++)
					{
						fscanf(fp, "%d %lf %lf %lf ", &imgId, &xp, &yp, &zp);
					}				
					continue;
				}

				m_nprojs += projN;
				for(j=0; j<projN; j++)
				{
					fscanf(fp, "%d %lf %lf %lf ", &imgId, &xp, &yp, &zp);

					int stripID = getStripID(imgId);

					if(stripID == 0)
					{//Lidar条带点
						ATT_LaserPoint lidPt;
						lidPt.lidID = imgId;
						lidPt.X = xp;
						lidPt.Y = yp;
						lidPt.Z = zp;

						tiePt.laspts.push_back(lidPt);
					}
					else
					{//影像点
						ATT_ImagePoint  imgPt;
						imgPt.imgID = imgId;
						imgPt.xi = xp;
						imgPt.yi = yp;

						m_pCamera->Image2Photo(xp, yp, imgPt.xp, imgPt.yp);	//像素坐标转换

						tiePt.projs.push_back(imgPt);
					}
					
					

//					pFrameProjs[imgId]++;
				}
				fscanf(fp, "\n");
				
				m_tiePts.push_back(tiePt);
			}
		}
	}

// 	fseek( fplog, 0L, SEEK_SET );
// 	fprintf( fplog, "[tracks]\n%10d\n", m_tiePts.size());
// 	fclose(fplog);

	fclose(fp);
	//对每个连接点按相片号重排
	std::vector<ATT_tiePoint>::iterator tieIter;
	for(tieIter=m_tiePts.begin(); tieIter!=m_tiePts.end(); tieIter++)
	{
		sort(tieIter->projs.begin(), tieIter->projs.end(), ascending_projs_by_imgID);
	}

#if 0
	//导出连接点坐标
	std::string logName = m_outputDir+"\\selectTies.log";
	FILE *fplog=NULL;
	fplog = fopen(logName.c_str(), "wt");

	fprintf( fplog, "[tracks]\n%10d\n", m_tiePts.size() );
	fprintf(fplog, "[imageNum]\n");
	fprintf(fplog, "%3d\n", imgNum );

	for(tieIter=m_tiePts.begin(); tieIter!=m_tiePts.end(); tieIter++)
	{
		fprintf(fplog, "%10ld ", tieIter->tieId);
		fprintf(fplog, "%2ld ", tieIter->projs.size());
		
		std::vector<ATT_ImagePoint>::iterator projIter;
		for(projIter=tieIter->projs.begin(); projIter!=tieIter->projs.end(); projIter++)
		{
			fprintf(fplog, "%3ld %7.6f %7.6f ", projIter->imgID, projIter->xp, projIter->yp);
		}
		fprintf(fplog, "\n");
	}

	fclose(fplog);
#endif


	return true;
}

void CIntATManager::cal_tiePtCoord(std::vector<ATT_tiePoint> *pTiesVec, double *pExor, orsCamSysExtraParam *calib_param, double *p3Ds)
{
	register int i, j;
	int cnp, pnp, mnp;
	double *exor, *pb, *pqr, *pt, *ppt/*, *pmeas*/;
	double *ptr1, *ptr2, *ptr3;
// 	int n;
// 	int m, nnz, max_nnz;
// 	
	//	double x0, y0,f;
	double f = m_pCamera->m_f;
	double	*A=0, *L=0;
	double x, y;
	double a1, a2, a3, b1, b2, b3, c1, c2, c3;
	double Xs, Ys, Zs;
	//	double u, v, w;
	//	double phi, omega, kappa;
	double Rimu[9], Rmis[9], R[9]; //R = Rimu*Rmis
	double misVec[3];

	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
	//计算偏心角和偏心分量
	matrixService->RotateMatrix_fwk(calib_param->boresight_angle[0], calib_param->boresight_angle[1], calib_param->boresight_angle[2], Rmis);

	int imgNum = getSumofImages();

	A=(double*)malloc(2*3*imgNum*sizeof(double));
	L=(double*)malloc(2*imgNum*sizeof(double));

	std::vector<ATT_tiePoint>::iterator tieIter;
	for(tieIter=pTiesVec->begin(), i=0; tieIter!=pTiesVec->end(); tieIter++, i++)
	{
		ppt = p3Ds+i*3;//点坐标

		int vis_num=tieIter->projs.size();
//		assert(vis_num>2);
	
		for(j=0; j<vis_num; j++)
		{
			ATT_ImagePoint imgpt = tieIter->projs[j];
			x = imgpt.xp;
			y = imgpt.yp;

			myLUT::iterator it = m_imgID_LUT->find(imgpt.imgID);
			int idx = it->second;
			exor = pExor + idx*6;
			pqr = exor+3;
			matrixService->RotateMatrix_fwk(pqr[0], pqr[1], pqr[2], Rimu);

			matrix_product(3, 3, 3, 3, Rimu, Rmis, R);
			matrix_product(3, 3, 3, 1, Rimu, calib_param->lever_arm, misVec);

			a1=R[0];		a2=R[1];		a3=R[2];
			b1=R[3];		b2=R[4];		b3=R[5];
			c1=R[6];		c2=R[7];		c3=R[8];
			Xs=exor[0]-misVec[0];	Ys=exor[1]-misVec[1];	Zs=exor[2]-misVec[2];

			ptr1=A+j*2*3;
			ptr2=L+j*2;

			ptr1[0]=a1*f+a3*x;
			ptr1[1]=b1*f+b3*x;
			ptr1[2]=c1*f+c3*x;
			ptr1[3]=a2*f+a3*y;
			ptr1[4]=b2*f+b3*y;
			ptr1[5]=c2*f+c3*y;

			ptr2[0]=(a1*f+a3*x)*Xs+(b1*f+b3*x)*Ys+(c1*f+c3*x)*Zs;
			ptr2[1]=(a2*f+a3*y)*Xs+(b2*f+b3*y)*Ys+(c2*f+c3*y)*Zs;
		}
		dgelsy_driver(A, L, ppt, 2*vis_num, 3, 1);
	}

	if(A)	free(A);
	if(L)	free(L);
}

//"顾及同步差的ＰＯＳ系统视准轴误差检校"
//"一种基于欧拉角序列变换的影像外方位元素解算方法"
//"GPS/INS系统HPR与OPK角元素的剖析与转换"
//摄影测量系下的旋转矩阵
//R_p = R_n2m * R_b2n * R_c2b * R_i2c * R_p2i
bool CIntATManager::extractImgOrParam(double *pImgExor, orsCamSysExtraParam_TYPE epType, CamPOS_TYPE posType)
{
	bool ret=true;
//	double *pImgExor=NULL;
	int imgNum, id;
//	double R_p2i[9];	//像空间辅助 - 像空间
	double R_i2c[9];	//像空间 - 传感器，安装角
	double R_c2b[9];	//传感器 - POS， 偏心角
// 	double R_b2n[9];	//POS - 导航系   
// 	double R_n2m[9];	//导航系 - 局部切平面

// 	double R_delt[9], Rtmp[9];
// 	double Ric[9];	//像空间坐标系到传感器坐标系的旋转矩阵，不同POS系统的坐标系定义不同
// 	double Rcb[9];	//传感器坐标系到IMU本体坐标系

	//不是联合平差工程
	if(m_blockInfo.size() == 0)
		return false;

	ref_ptr<orsIMatrixService> matrixService;

	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
	if( NULL == matrixService.get() )	
		return false;

	double deltKappa;

// 	switch (m_rMode)
// 	{
// 	case ATT_rmNONE:
// 		deltKappa = 0;
// 		break;
// 	case ATT_rm180:
// 		deltKappa = PI;
// 		break;
// 	case ATT_rmANTICLOCKWISE90:
// 		deltKappa = -PI/2;
// 		break;
// 	case ATT_rmCLOCKWISE90:
// 		deltKappa = PI/2;
// 		break;
// 	}

// 	R_p2i[0] = sin(deltKappa);		R_p2i[1] = cos(deltKappa);		R_p2i[2] = 0;
// 	R_p2i[3] = -cos(deltKappa);		R_p2i[4] = sin(deltKappa);		R_p2i[5] = 0;
// 	R_p2i[6] = 0;					R_p2i[7] = 0;					R_p2i[8] = 1;

	//需要根据坐标轴的定义进行调整
	//相机安装角
	matrixService->RotateMatrix_fwk(m_camera_mount_angle[0], m_camera_mount_angle[1], 
		m_camera_mount_angle[2], R_i2c);
// 	R_i2c[0] = 1;	R_i2c[1] = 0;	R_i2c[2] = 0;
// 	R_i2c[3] = 0;	R_i2c[4] = 1;	R_i2c[5] = 0;
// 	R_i2c[6] = 0;	R_i2c[7] = 0;	R_i2c[8] = 1;


	matrix_ident(3, R_c2b);	//暂时忽略偏心角


	imgNum = getSumofImages();
	if(imgNum<=0)
		return false;

	std::vector<ATT_StripInfo>::iterator stripIter;
	std::vector<ATT_ImageEXOR>::iterator imgIter;
	double R[9];
	if(posType == POS_Raw)
	{//直接提取外方位元素

		id=0;
		for(stripIter=m_blockInfo.begin(); stripIter!=m_blockInfo.end(); stripIter++)
		{
			for(imgIter=stripIter->imgEOVec.begin(); imgIter!=stripIter->imgEOVec.end(); imgIter++, id++)
			{
				double phi,omega,kappa;

//				matrix_product(3, 3, 3, 3, R_delt, imgIter->eoParam.R, R);

				matrixService->R2fwk(imgIter->eoParam.R, &phi, &omega, &kappa);

				pImgExor[id*6]	 = imgIter->eoParam.coord.X;
				pImgExor[id*6+1] = imgIter->eoParam.coord.Y;
				pImgExor[id*6+2] = imgIter->eoParam.coord.Z;
				pImgExor[id*6+3] = phi;
				pImgExor[id*6+4] = omega;
				pImgExor[id*6+5] = kappa;

//				memcpy(imgIter->eoParam.R, R, sizeof(double)*9);
				//				matrixService->RotateMatrix_fwk(phi, omega, kappa, imgIter->eoParam.R);
			}
		}
	}
	else if(posType == POS_IntepolatedFromLIDAR)
	{//从点云航迹中内插外方位元素

		ref_ptr<orsITrajectory> traj = ORS_CREATE_OBJECT(orsITrajectory, ORS_LIDARGEOMETRY_TRAJECTORY_DEFAULT);
		if(traj.get()==NULL)
			return false;

// 		ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_DEFAULT);
// 		ref_ptr<orsIImageGeometry> g_pFrameGeo = ORS_CREATE_OBJECT(orsIImageGeometry, "ors.imageGeometry.frame.MetricFrame");
// 		ref_ptr<orsIProperty> property = getPlatform()->createProperty();

		id=0;	
//		i=0;
		for(stripIter=m_blockInfo.begin(); stripIter!=m_blockInfo.end(); stripIter++)
		{
			for(imgIter=stripIter->imgEOVec.begin(); imgIter!=stripIter->imgEOVec.end(); imgIter++, id++)
			{
				traj->Interpolate_linear(&(imgIter->eoParam), &(imgIter->orientedTraj[0]), &(imgIter->orientedTraj[1]));

				matrixService->R2rph(imgIter->eoParam.R, &imgIter->eoParam.r,
					&imgIter->eoParam.p, &imgIter->eoParam.h);

// 				LidPt_SurvInfo pt;
// 				pt.vecRange[0] = pt.vecRange[1] = 0;
// 				pt.vecRange[2] = 1000;
// 				pt.POS_Info = imgIter->eoParam;
// 				lidGeo->cal_Laserfootprint(&pt, 1);
// 
// 				property->removeAll();
// 				property->addAttr("x0", 0.0);
// 				property->addAttr("y0", 0.0);
// 				property->addAttr("f", m_pCamera->m_f);
// 				property->addAttr("imgCol", ors_int32(m_pCamera->m_imgWid));
// 				property->addAttr("imgRow", ors_int32(m_pCamera->m_imgHei));
// 				property->addAttr("pixelSizeX", m_pCamera->m_pixelX);
// 				property->addAttr("pixelSizeY", m_pCamera->m_pixelY);
// 
// 				property->addAttr("Xs", imgIter->eoParam.coord.X);
// 				property->addAttr("Ys", imgIter->eoParam.coord.Y);
// 				property->addAttr("Zs", imgIter->eoParam.coord.Z);
// 
// 				for(int j=0; j<9; j++)
// 				{
// 					property->addAttr("R", imgIter->eoParam.R[j]);
// 				}
// 				property->addAttr("ZMax", ors_float64(1070.0));
// 				property->addAttr("ZMin", ors_float64(1050.0));
// 				g_pFrameGeo->initFromProperty(property.get());
// 
// 				double X0, Y0;
// 				g_pFrameGeo->IntersectWithZ(m_pCamera->m_imgWid/2, m_pCamera->m_imgHei/2, 1060, &X0, &Y0);

				double phi,omega,kappa;

				//imgIter->eoParam.R = R_n2m * R_b2n
				double R1[9], R2[9], R3[9], R4[9];
 				matrix_product33(imgIter->eoParam.R, R_c2b, R1);
//				matrix_ident(3, R_i2c);
				matrix_product33(R1, R_i2c, R2);

// 				matrixService->RotateMatrix_wfk(vomega[i]*s, vphi[i]*s, vkappa[i]*s, R3);
// 				matrix_transpose_product_old(3, 3, 3, 3, R2, R3, R4);
// 
// 				matrixService->R2wfk(R4, &phi, &omega, &kappa);
// 
// 				TRACE("%d %.6f %.6f %.6f \n", i, omega, phi, kappa);
// 				i++;

// 				matrix_product33(R1, R_i2c, R2);


// 				matrix_product33(R2, R_p2i, R);

// 				R4[0] = -1;	R4[1] = 0;	R4[2] = 0;
// 				R4[3] = 0;	R4[4] = 1;	R4[5] = 0;
// 				R4[6] = 0;	R4[7] = 0;	R4[8] = 1;
				
//				matrix_product33(R3, R4, R);

				memcpy(R, R2, sizeof(double)*9);
				matrixService->R2fwk(R, &phi, &omega, &kappa);

				pImgExor[id*6]	 = imgIter->eoParam.coord.X;
				pImgExor[id*6+1] = imgIter->eoParam.coord.Y;
				pImgExor[id*6+2] = imgIter->eoParam.coord.Z;
				pImgExor[id*6+3] = phi;
				pImgExor[id*6+4] = omega;
				pImgExor[id*6+5] = kappa;

				matrixService->RotateMatrix_fwk(phi, omega, kappa, R);

				memcpy(imgIter->eoParam.R, R, sizeof(double)*9);
				//				matrixService->RotateMatrix_fwk(phi, omega, kappa, imgIter->eoParam.R);
			}
		}

	}

#if 1
	//导出影像初始外方位元素
	std::string initExorName = m_outputDir+"\\initExor.txt";
	orsCamSysExtraParam camMisalign;
	memset(&camMisalign, 0, sizeof(orsCamSysExtraParam));

	output_imgExor(initExorName.c_str(), pImgExor, &camMisalign);
// 	FILE *fpExor=NULL;
// 	fpExor = fopen(initExorName.c_str(), "wt");
// 	double	s=180.0/PI;
// 
// 	for(stripIter=m_blockInfo.begin(); stripIter!=m_blockInfo.end(); stripIter++)
// 	{
// 		for(imgIter=stripIter->imgEOVec.begin(); imgIter!=stripIter->imgEOVec.end(); imgIter++)
// 		{
// 			std::string imgName = getImageName(imgIter->imgID);
// 
// 			double phi,omega,kappa;
// 
// /*			matrixService->RotateMatrix_fwk(imgIter->eoParam.R);*/
// 
// 			matrixService->R2wfk(imgIter->eoParam.R, &phi, &omega, &kappa);
// 
// 			fprintf(fpExor,"%s ", imgName.c_str());
// 
// 			fprintf(fpExor,"%lf %lf %lf %lf %lf %lf %lf %lf %lf\n", imgIter->eoParam.time,
// 				imgIter->eoParam.coord.X,imgIter->eoParam.coord.Y,imgIter->eoParam.coord.Z,
// 				omega*s,phi*s,kappa*s,0,0);
// 
// 		}
// 	}
// 
// 	fclose(fpExor); fpExor=NULL;
#endif

	return ret;
}

bool CIntATManager::extractImgExposureTime(double *timeArr)
{
	std::vector<ATT_StripInfo>::iterator stripIter;
	std::vector<ATT_ImageEXOR>::iterator imgIter;
	int	id=0;
	double *iTime=NULL;

	for(stripIter=m_blockInfo.begin(); stripIter!=m_blockInfo.end(); stripIter++)
	{
		for(imgIter=stripIter->imgEOVec.begin(); imgIter!=stripIter->imgEOVec.end(); imgIter++, id++)
		{
			iTime = timeArr + id*2;
			iTime[0] = imgIter->eoParam.time;
			iTime[1] = imgIter->t0;
		}
	}

	return true;
}

bool CIntATManager::extractGPSInfo(GPSinfo *pGPS)
{
	std::vector<ATT_StripInfo>::iterator stripIter;
	std::vector<ATT_ImageEXOR>::iterator imgIter;
	int	id=0;

	for(stripIter=m_blockInfo.begin(); stripIter!=m_blockInfo.end(); stripIter++)
	{
		for(imgIter=stripIter->imgEOVec.begin(); imgIter!=stripIter->imgEOVec.end(); imgIter++, id++)
		{
			pGPS[id].XYZ[0] = imgIter->eoParam.coord.X;
			pGPS[id].XYZ[1] = imgIter->eoParam.coord.Y;
			pGPS[id].XYZ[2] = imgIter->eoParam.coord.Z;

			pGPS[id].gpstime = imgIter->eoParam.time;
			pGPS[id].t0 = imgIter->t0;
			pGPS[id].survID = imgIter->survID;
		}
	}

	return true;
}

void CIntATManager::cameraMountingError_correction(double *pImgExor, orsCamSysExtraParam *param)
{
	ref_ptr<orsIMatrixService> matrixService;

	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
	if( NULL == matrixService.get() )	
		return ;

	double Rmis[9], Rimu[9], Rcam[9];
	matrixService->RotateMatrix_fwk(param->boresight_angle[0], param->boresight_angle[0], param->boresight_angle[0], Rmis);
	
	int imgNum = getSumofImages();
	for(int i=0; i<imgNum; i++)
	{
		double *ppt = pImgExor + i*6;
		double *pqr = ppt+3;
		matrixService->RotateMatrix_fwk(pqr[0], pqr[1], pqr[2], Rimu);
		matrix_product(3,3,3,3,Rimu,Rmis, Rcam);

		matrixService->R2fwk(Rcam, pqr, pqr+1, pqr+2);

		double misDis[3];
		matrix_product(3,3,3,1,Rimu, param->lever_arm, misDis);

		ppt[0] -= misDis[0];
		ppt[1] -= misDis[1];
		ppt[2] -= misDis[2];
	}
}

bool CIntATManager::camera_calibration_BA(char *pszCorrespFile, orsCamSysExtraParam *param, orsCamSysExtraParam_TYPE epType, CamPOS_TYPE posType)
{
	bool ret=true;
	double *pImgExor=NULL;
	int imgNum, id;

	ref_ptr<orsIMatrixService> matrixService;

	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
	if( NULL == matrixService.get() )	
		return false;

	ret = loadTiePoints(pszCorrespFile);
	if(!ret)
		return false;

	imgNum = getSumofImages();
	if(imgNum<=0)
		return false;

	pImgExor=new double[6*imgNum];

	extractImgOrParam(pImgExor, epType, posType);

	std::vector<ATT_StripInfo>::iterator stripIter;
	std::vector<ATT_ImageEXOR>::iterator imgIter;
	double R[9];

	int tieNum, i, j;
	char *mask=NULL;//标记连接点在哪些相片上可见
	double *pImgObs=NULL;	//像点观测值数组
//	double *p3Ds=NULL;		//物方点坐标
	double *pUnknowns=NULL;


	tieNum = m_tiePts.size();
	mask = new char[tieNum*imgNum];
	memset(mask, 0, sizeof(char)*tieNum*imgNum);
//	p3Ds = new double[tieNum*3];

	//assert(m_nprojs>0);


	pImgObs = new double[m_nprojs*2];

	int k=0;
	for(i=0; i<tieNum; i++)
	{
		int vis_num=m_tiePts[i].projs.size();
		for(j=0; j<vis_num; j++)
		{
			ATT_ImagePoint imgpt = m_tiePts[i].projs[j];
			
// 			double xi, yi;
// 			m_pCamera->Photo2Image(imgpt.x, imgpt.y, xi, yi);
			myLUT::iterator it = m_imgID_LUT->find(imgpt.imgID);
			int idx = it->second;
			mask[i*imgNum+idx] = 1;
			pImgObs[k*2]=imgpt.xp;
			pImgObs[k*2+1]=imgpt.yp;
			k++;
		}
	}

	orsAlgCameraCalib calib;
	imgCalib_globs  pdata;

	pdata.calibType = epType;
	pdata.calibParam = param;
	pdata.pImgExor = pImgExor;	
	pdata.pImgpts = pImgObs;
//	pdata.p3Ds = p3Ds;
	pdata.mask = mask;
	pdata.nimg = imgNum;
	pdata.ntie = tieNum;
	pdata.nproj = m_nprojs;

	double *hx=NULL, *hxx=NULL;

	hx = new double[m_nprojs*2];
	hxx = new double[m_nprojs*2];

	pdata.hx = hx;
	pdata.hxx = hxx;

	switch (epType)
	{
	case CamGeo_rph:
		pdata.cnp = 3;
		break;
	case CamGeo_rphxyz:
		pdata.cnp = 6;
		break;
	}
	pdata.nVFrame = 1;
	pdata.pnp = 3;
	pdata.mnp = 2; 
	pdata.ngcp = 0;	
	pdata.nfixedImg = 0;	
	pdata.camInfo = m_pCamera;

	pUnknowns = new double[pdata.nVFrame*pdata.cnp+pdata.ntie*pdata.pnp];

	pdata.pUnknowns = pUnknowns;

	switch (epType)
	{
	case CamGeo_rph:
		pUnknowns[0] = param->boresight_angle[0];
		pUnknowns[1] = param->boresight_angle[1];
		pUnknowns[2] = param->boresight_angle[2];
		break;
	case CamGeo_rphxyz:
		pUnknowns[0] = param->boresight_angle[0];
		pUnknowns[1] = param->boresight_angle[1];
		pUnknowns[2] = param->boresight_angle[2];
		pUnknowns[3] = param->lever_arm[0];
		pUnknowns[4] = param->lever_arm[1];
		pUnknowns[5] = param->lever_arm[2];
		break;
	}
	
	cal_tiePtCoord(&m_tiePts, pImgExor, param, pUnknowns+pdata.nVFrame*pdata.cnp);  //计算初始的物方点坐标

	std::string logName = m_outputDir+"\\cameraCalib_BA.log";
	FILE *fplog=NULL;

	fplog = fopen(logName.c_str(), "wt");
	if(fplog==NULL)
	{
		ret = false;
		goto FLAG_END;
	}

	pdata.fplog = fplog;

	calib.bundle(&pdata);
	
	fclose(fplog);	fplog=NULL;

	switch (epType)
	{
	case CamGeo_rph:
		param->boresight_angle[0] = pUnknowns[0];
		param->boresight_angle[1] = pUnknowns[1];
		param->boresight_angle[2] = pUnknowns[2];
		break;
	case CamGeo_rphxyz:
		param->boresight_angle[0] = pUnknowns[0];
		param->boresight_angle[1] = pUnknowns[1];
		param->boresight_angle[2] = pUnknowns[2];
		param->lever_arm[0] = pUnknowns[3];
		param->lever_arm[1] = pUnknowns[4];
		param->lever_arm[2] = pUnknowns[5];
		break;
	}

	{
		std::string calibExorName = m_outputDir+"\\CalibExor_BA.txt";
		output_imgExor(calibExorName.c_str(), pImgExor, param);
	}

FLAG_END:

	if(pImgExor)	delete[] pImgExor;	pImgExor=NULL;
	if(pImgObs)		delete[] pImgObs;	pImgObs=NULL;
	if(mask)		delete[] mask;		mask=NULL;
//	if(p3Ds)		delete[] p3Ds;		p3Ds=NULL;
	if(pUnknowns)	delete[] pUnknowns;	pUnknowns=NULL;
	if(hx)			delete[] hx;		hx=NULL;
	if(hxx)			delete[] hxx;		hxx=NULL;
	return ret;
}

bool CIntATManager::camera_calibration_IP(char *pszCorrespFile, orsCamSysExtraParam *param, 
	orsCamSysExtraParam_TYPE epType, CamPOS_TYPE posType)
{
	bool ret=true;
	double *pImgExor=NULL;
	int imgNum, id;

	ref_ptr<orsIMatrixService> matrixService;

	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
	if( NULL == matrixService.get() )	
		return false;

	ret = loadTiePoints(pszCorrespFile);
	if(!ret)
		return false;

	imgNum = getSumofImages();
	if(imgNum<=0)
		return false;

	pImgExor=new double[6*imgNum];

	extractImgOrParam(pImgExor, epType, posType);

	std::vector<ATT_StripInfo>::iterator stripIter;
	std::vector<ATT_ImageEXOR>::iterator imgIter;
	double R[9];

	int tieNum, i, j;
	char *mask=NULL;//标记连接点在哪些相片上可见
	double *pImgObs=NULL;	//像点观测值数组
	//	double *p3Ds=NULL;		//物方点坐标
	double *pUnknowns=NULL;


	tieNum = m_tiePts.size();
	mask = new char[tieNum*imgNum];
	memset(mask, 0, sizeof(char)*tieNum*imgNum);
	//	p3Ds = new double[tieNum*3];

	assert(m_nprojs>0);
	pImgObs = new double[m_nprojs*2];

	int k=0;
	for(i=0; i<tieNum; i++)
	{
		int vis_num=m_tiePts[i].projs.size();
		for(j=0; j<vis_num; j++)
		{
			ATT_ImagePoint imgpt = m_tiePts[i].projs[j];

			// 			double xi, yi;
			// 			m_pCamera->Photo2Image(imgpt.x, imgpt.y, xi, yi);
			myLUT::iterator it = m_imgID_LUT->find(imgpt.imgID);
			int idx = it->second;
			mask[i*imgNum+idx] = 1;
			pImgObs[k*2]=imgpt.xp;
			pImgObs[k*2+1]=imgpt.yp;
			k++;
		}
	}

	orsAlgCameraCalib calib;
	imgCalib_globs  pdata;

	pdata.calibType = epType;
	pdata.calibParam = param;
	pdata.pImgExor = pImgExor;	
	pdata.pImgpts = pImgObs;
	//	pdata.p3Ds = p3Ds;
	pdata.mask = mask;
	pdata.nimg = imgNum;
	pdata.ntie = tieNum;
	pdata.nproj = m_nprojs;

	double *hx=NULL, *hxx=NULL;

	hx = new double[m_nprojs*2];
	hxx = new double[m_nprojs*2];

	pdata.hx = hx;
	pdata.hxx = hxx;

	switch (epType)
	{
	case CamGeo_rph:
		pdata.cnp = 3;
		break;
	case CamGeo_rphxyz:
		pdata.cnp = 6;
		break;
	}
	pdata.nVFrame = 1;
	pdata.pnp = 3;
	pdata.mnp = 2; 
	pdata.ngcp = 0;	
	pdata.nfixedImg = 0;	
	pdata.camInfo = m_pCamera;

	pUnknowns = new double[pdata.nVFrame*pdata.cnp+pdata.ntie*pdata.pnp];

	pdata.pUnknowns = pUnknowns;

	switch (epType)
	{
	case CamGeo_rph:
		pUnknowns[0] = param->boresight_angle[0];
		pUnknowns[1] = param->boresight_angle[1];
		pUnknowns[2] = param->boresight_angle[2];
		break;
	case CamGeo_rphxyz:
		pUnknowns[0] = param->boresight_angle[0];
		pUnknowns[1] = param->boresight_angle[1];
		pUnknowns[2] = param->boresight_angle[2];
		pUnknowns[3] = param->lever_arm[0];
		pUnknowns[4] = param->lever_arm[1];
		pUnknowns[5] = param->lever_arm[2];
		break;
	}

	cal_tiePtCoord(&m_tiePts, pImgExor, param, pUnknowns+pdata.nVFrame*pdata.cnp);  //计算初始的物方点坐标

	std::string logName = m_outputDir+"\\cameraCalib_IP.log";
	FILE *fplog=NULL;

	fplog = fopen(logName.c_str(), "wt");
	if(fplog==NULL)
	{
		ret = false;
		goto FLAG_END;
	}

	pdata.fplog = fplog;

//	calib.calib_IP(&pdata);
	calib.calib_IP_LM(&pdata);

	fclose(fplog);	fplog=NULL;

	switch (epType)
	{
	case CamGeo_rph:
		param->boresight_angle[0] = pUnknowns[0];
		param->boresight_angle[1] = pUnknowns[1];
		param->boresight_angle[2] = pUnknowns[2];
		break;
	case CamGeo_rphxyz:
		param->boresight_angle[0] = pUnknowns[0];
		param->boresight_angle[1] = pUnknowns[1];
		param->boresight_angle[2] = pUnknowns[2];
		param->lever_arm[0] = pUnknowns[3];
		param->lever_arm[1] = pUnknowns[4];
		param->lever_arm[2] = pUnknowns[5];
		break;
	}

	{
		std::string calibExorName = m_outputDir+"\\CalibExor_IP.txt";
		output_imgExor(calibExorName.c_str(), pImgExor, param);
	}

FLAG_END:

	if(pImgExor)	delete[] pImgExor;	pImgExor=NULL;
	if(pImgObs)		delete[] pImgObs;	pImgObs=NULL;
	if(mask)		delete[] mask;		mask=NULL;
	//	if(p3Ds)		delete[] p3Ds;		p3Ds=NULL;
	if(pUnknowns)	delete[] pUnknowns;	pUnknowns=NULL;
	if(hx)			delete[] hx;		hx=NULL;
	if(hxx)			delete[] hxx;		hxx=NULL;
	return ret;
}

bool CIntATManager::output_imgExor(const char *pszFileName, double *pExor, orsCamSysExtraParam *calib_param)
{
	FILE *fp=NULL;

	fp = fopen(pszFileName, "wt");
	if(fp==NULL)
		return false;

	double	s=180.0/PI;
	int i=0;
	double *exor=NULL, *pqr=NULL;
	double uvw[3];
	double Rmis[9], Rimu[9], R[9];

	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

	assert(matrixService.get());

	matrixService->RotateMatrix_fwk(calib_param->boresight_angle[0], calib_param->boresight_angle[1], calib_param->boresight_angle[2], Rmis);

	uvw[0]=calib_param->lever_arm[0];
	uvw[1]=calib_param->lever_arm[1];
	uvw[2]=calib_param->lever_arm[2];
	

	std::vector<ATT_StripInfo>::iterator stripIter;
	std::vector<ATT_ImageEXOR>::iterator imgIter;
	for(stripIter=m_blockInfo.begin(); stripIter!=m_blockInfo.end(); stripIter++)
	{
		for(imgIter=stripIter->imgEOVec.begin(); imgIter!=stripIter->imgEOVec.end(); imgIter++)
		{
			myLUT::iterator it = m_imgID_LUT->find(imgIter->imgID);
			int idx = it->second;

			std::string imgName = getImageName(idx);

			CString pureName = _ExtractFileFullName(imgName.c_str());

			double phi,omega,kappa;

			exor = pExor + i*6;
			pqr = exor + 3;

			matrixService->RotateMatrix_fwk(pqr[0], pqr[1], pqr[2], Rimu);

			matrix_product(3, 3, 3, 3, Rimu, Rmis, R);

			matrixService->R2wfk(R, &phi, &omega, &kappa);

			double vec1[3], vec2[3];

			matrix_product(3, 3, 3, 1, Rimu, uvw, vec1);

			vec2[0] = exor[0] - vec1[0];
			vec2[1] = exor[1] - vec1[1];
			vec2[2] = exor[2] - vec1[2];

			fprintf(fp,"%s ", pureName);

			fprintf(fp,"%d %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", imgIter->imgID,
				imgIter->eoParam.time, vec2[0],vec2[1],vec2[2], omega*s,phi*s,kappa*s,0,0);

			i++;
		}
	}

	fclose(fp); fp=NULL;

	return true;
}


bool CIntATManager::imageBA(/*char *pszCorrespFile, */char *pszSBAResultFile, orsImageBA_TYPE BA_type)
{
	bool ret=true;
	int i, j;
	double *pImgExor=NULL;
	double *pTimes=NULL;
	int imgNum, id;
	double *sigma0=NULL;
	int nObsCls;

	ref_ptr<orsIMatrixService> matrixService;

	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
	if( NULL == matrixService.get() )	
		return false;

// 	ret = readTiePoints(pszCorrespFile);
// 	if(!ret)
// 		return false;

	imgNum = getSumofImages();
	if(imgNum<=0)
		return false;

	pImgExor=new double[6*imgNum];
	pTimes=new double[imgNum*2];  //ti,ti0

	//对于纯相片BA，读取影像的外方位元素初值
	if(!extractImgOrParam(pImgExor, CamGeo_rph, POS_IntepolatedFromLIDAR))
	{
		return false;
	}

	//中心化
	memset(&m_center, 0, sizeof(POINT3D));
	for(i=0; i<imgNum; i++)
	{
		m_center.X += pImgExor[i*6];
		m_center.Y += pImgExor[i*6+1];
		m_center.Z += pImgExor[i*6+2];
	}
	m_center.X /= imgNum;
	m_center.Y /= imgNum;
	m_center.Z /= imgNum;

	for(i=0; i<imgNum; i++)
	{
		pImgExor[i*6] -= m_center.X;
		pImgExor[i*6+1] -= m_center.Y;
//		pImgExor[i*6+2] -= m_center.Z;
	}

	//提取GPS时间
//	extractImgExposureTime(pTimes);

// 	double t0=0;
// 	for(i=0; i<imgNum; i++)
// 	{
// 		t0 += pTimes[i];
// 	}
// 	t0 /= imgNum;
// 
// 	double maxdiff=0;	
// 	for(i=0; i<imgNum; i++)
// 	{
// 		if(fabs(pTimes[i]-t0) > maxdiff)
// 		{
// 			j=i;
// 			maxdiff=fabs(pTimes[i]-t0);
// 		}
// 	}
// 	TRACE("maxdiff: %d, %f", j, pTimes[j]-t0);


	std::vector<ATT_StripInfo>::iterator stripIter;
	std::vector<ATT_ImageEXOR>::iterator imgIter;
	double R[9];

	int tieNum;
	char *mask=NULL;//标记连接点在哪些相片上可见
	double *pObs=NULL;	//观测值数组
	double *pAdjObs=NULL;
//	double *pObsImgpt=NULL;
	GPSinfo *pGPSObs=NULL;
//	double *pGPSObs=NULL;	//GPS观测值
	//	double *p3Ds=NULL;		//物方点坐标
	double *pUnknowns=NULL;

	pGPSObs = new GPSinfo[imgNum];

	tieNum = m_tiePts.size();
	mask = new char[tieNum*imgNum];
	memset(mask, 0, sizeof(char)*tieNum*imgNum);
	//	p3Ds = new double[tieNum*3];

	int nvars;
	int nobs;
	int nsurv = 0;

	//	assert(m_nprojs>0);
	//统计像点数
	for(i=0, m_nprojs=0; i<m_tiePts.size(); i++)
	{
		m_nprojs += m_tiePts[i].projs.size();
	}

	switch (BA_type)
	{
	case ImageBA_classic:
		nvars=imgNum*6 + tieNum*3;
		nobs = m_nprojs*2;
		break;

	case ImageBA_classic_W:
		nvars=imgNum*6 + tieNum*3;
		nobs = m_nprojs*2;

		sigma0=new double[3];
		sigma0[0] = 1;
		sigma0[1] = 100;
		sigma0[2] = 100;
		nObsCls = 3;
		break;

	case ImageBA_GPS_supported:
		nvars=imgNum*6 + tieNum*3 + 3/* + 6*nSurv*/;  //后面再算漂移个数
		nobs = m_nprojs*2 + imgNum*3;

		sigma0=new double[4];
		sigma0[0] = 1;
		sigma0[1] = 1;
		sigma0[2] = 100;
		sigma0[3] = 100;
		nObsCls = 4;
		break;

	default:
		break;
	}
	pObs = new double[nobs];
	pAdjObs = new double[nobs];

	int k=0;
	for(i=0; i<tieNum; i++)
	{
		int vis_num=m_tiePts[i].projs.size();
		for(j=0; j<vis_num; j++)
		{
			ATT_ImagePoint imgpt = m_tiePts[i].projs[j];

			// 			double xi, yi;
			// 			m_pCamera->Photo2Image(imgpt.x, imgpt.y, xi, yi);
			myLUT::iterator it = m_imgID_LUT->find(imgpt.imgID);
			int idx = it->second;
			mask[i*imgNum+idx] = 1;
			pObs[k*2]=imgpt.xp;
			pObs[k*2+1]=imgpt.yp;
			k++;
		}
	}


	orsAlgImageBA calib;
	imgBA_globs  pdata;

	pdata.BA_type = BA_type;
//	pdata.calibType = epType;
//	pdata.calibParam = param;
//	pdata.pImgExor = pImgExor;	
	pdata.pObs = pObs;
	pdata.pAdjObs = pAdjObs;
	//	pdata.p3Ds = p3Ds;
	pdata.mask = mask;
	pdata.nimg = imgNum;
	pdata.ntie = tieNum;
	pdata.nproj = m_nprojs;

	double *hx=NULL, *hxx=NULL;

	hx = new double[m_nprojs*2];  
 	hxx = new double[m_nprojs*2];

	pdata.hx = hx;
	pdata.hxx = hxx;

	pdata.nObsCls=nObsCls;
	pdata.sigma0=sigma0;
	pdata.confidence=0.003;
	pdata.gross_snooping=ATT_GS_Undo;

// 	switch (epType)
// 	{
// 	case CamGeo_rph:
// 		pdata.cnp = 3;
// 		break;
// 	case CamGeo_rphxyz:
// 		pdata.cnp = 6;
// 		break;
// 	}
//	pdata.nVFrame = 1;
	pdata.cnp = 6;
	pdata.pnp = 3;
	pdata.mnp = 2; 
//	pdata.ngcp = m_GCP.size();	
	pdata.nfixedImg = 0;	
	pdata.camInfo = m_pCamera;

	pdata.nGPSoffset = 3;
	pdata.nGPSdrift = 6;
//	pdata.GPSt0 = t0;
//	pdata.pTimes = pTimes;
	pdata.pGpsObs = pGPSObs;
	pdata.nSurv = nsurv;
//	pdata.gcp = &m_GCP;
	
	
	if(BA_type == ImageBA_GPS_supported)
	{
		extractGPSInfo(pGPSObs);
		double *pGPS = pObs + m_nprojs*2;
		for(i=0; i<imgNum; i++, pGPS+=3)
		{
			pGPS[0] = pGPSObs[i].XYZ[0] -= m_center.X ;
			pGPS[1] = pGPSObs[i].XYZ[1] -= m_center.Y;
			pGPS[2] = pGPSObs[i].XYZ[2];
		}
		pdata.nSurv = getSurvAreaNum();
		nvars += (6*pdata.nSurv);//nSurv=0,不计算漂移值
	}

	//控制点中心化
	for(i=0; i<m_GCP.size(); i++)
	{
		pdata.gcp.push_back(m_GCP[i]);
		pdata.gcp[i].X -= m_center.X;
		pdata.gcp[i].Y -= m_center.Y;
	}

	
	pUnknowns = new double[nvars];
	memset(pUnknowns, 0, sizeof(double)*nvars);

	pdata.pUnknowns = pUnknowns;

	for(i=0; i<pdata.nimg*pdata.cnp; i++)
	{
		pUnknowns[i] = pImgExor[i];
	}

	orsCamSysExtraParam camMisalign;
	memset(&camMisalign, 0, sizeof(orsCamSysExtraParam));

	double *p3Ds = pUnknowns+pdata.nimg*pdata.cnp;
	//计算初始的物方点坐标
	cal_tiePtCoord(&m_tiePts, pImgExor, &camMisalign, p3Ds);
	//修改控制点坐标
	for(i=0; i<pdata.gcp.size(); i++)
	{
		switch(pdata.gcp[i].tType)
		{
		case TiePtTYPE_GCP_XY:
			p3Ds[i*3] = pdata.gcp[i].X;
			p3Ds[i*3+1] = pdata.gcp[i].Y;
			break;

		case TiePtTYPE_GCP_Z:
			p3Ds[i*3+2] = pdata.gcp[i].Z;
			break;

		case TiePtTYPE_GCP_XYZ:
			p3Ds[i*3] = pdata.gcp[i].X;
			p3Ds[i*3+1] = pdata.gcp[i].Y;
			p3Ds[i*3+2] = pdata.gcp[i].Z;
			break;
		}
	}

// 	for(i=0; i<m_tiePts.size(); i++)
// 	{
// 		p3Ds[i*3] = m_tiePts[i].X;
// 		p3Ds[i*3+1] = m_tiePts[i].Y;
// 		p3Ds[i*3+2] = m_tiePts[i].Z;
// 	}

	std::string imgPtResidual = m_outputDir+"\\imgPtResidual.txt";
	pdata.pszImgPtResidual = imgPtResidual.c_str();

	std::string logName = m_outputDir+"\\image_BA.log";
	FILE *fplog=NULL;

	fplog = fopen(logName.c_str(), "wt");
	if(fplog==NULL)
	{
		ret = false;
		goto FLAG_END;
	}

	pdata.fplog = fplog;

	calib.bundle(&pdata);

	double dist_k[7];
	memset(dist_k, 0, sizeof(double)*7);
	pdata.lens_selfCalib_param_num = 7;
	if(pdata.adj_rms/pdata.camInfo->m_pixelX > 1.0)
	{
		fprintf(fplog, "\n\nlaunch lens self-calibration\n");
		Cal_LensDistortion(&pdata, dist_k);
		double reProj_rms = sqrt(pdata.adj_rms/(pdata.nproj*2))/pdata.camInfo->m_pixelX;
		fprintf(fplog, "reproj error RSM %.7f [%.7f]\n", pdata.adj_rms, reProj_rms);
		fprintf(fplog, "lens distortion param：\n");
		for(i=0; i<pdata.lens_selfCalib_param_num; i++)
		{
			fprintf(fplog, "%e ", dist_k[i]);
		}
		fprintf(fplog, "\n");
	}

	fclose(fplog);	fplog=NULL;

// 	double *p3Ds=new double[3*tieNum];
// 	cal_tiePtCoord(&m_tiePts, pUnknowns, &camMisalign, p3Ds); 

	//恢复到原始坐标原点下
	for(i=0; i<imgNum; i++)
	{
		pUnknowns[i*6] += m_center.X;
		pUnknowns[i*6+1] += m_center.Y;
	}

	for(i=0; i<tieNum; i++)
	{
		p3Ds[i*3] += m_center.X;
		p3Ds[i*3+1] += m_center.Y;
	}

	{
//		std::string calibExorName = pszSBAResultFile;

		output_imgExor(pszSBAResultFile, pUnknowns, &camMisalign);

// 		FILE *fp=0;
// 		fp=fopen(calibExorName.c_str(), "w");
// 		if(fp==0)
// 			return false;
// 
// 		printImgSBAStructureAndMotionData(fp, pUnknowns, pdata.nimg, pdata.cnp, 
// 			pdata.ntie, pdata.pnp);
// 
// 		fclose(fp);
	}



FLAG_END:

	if(pImgExor)	delete[] pImgExor;	pImgExor=NULL;
	if(pTimes)		delete[] pTimes;	pTimes=NULL;
	if(pObs)		delete[] pObs;		pObs=NULL;
	if(pAdjObs)		delete[] pAdjObs;	pAdjObs=NULL;
	if(pGPSObs)		delete[] pGPSObs;	pGPSObs=NULL;
	if(mask)		delete[] mask;		mask=NULL;
	//	if(p3Ds)		delete[] p3Ds;		p3Ds=NULL;
	if(pUnknowns)	delete[] pUnknowns;	pUnknowns=NULL;
	if(hx)			delete[] hx;		hx=NULL;
	if(hxx)			delete[] hxx;		hxx=NULL;
	if(sigma0)		delete[] sigma0;	sigma0=NULL;
	return ret;
}

void printImgSBAStructureAndMotionData(FILE *fp, double *motstruct, int ncams, 
	int cnp, int nties, int pnp)
{
	register int i, j, k;

	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );


	fputs("#Structure parameters:\n", fp);
	for(i=0; i<ncams; i++)
	{
		fprintf(fp, "image %d\n", i);
		double *p=motstruct+i*cnp;
		for(j=0; j<cnp; ++j)
		{
			fprintf(fp, "%.7f ", p[j]);
		}
		fprintf(fp, "\n");
		double R[9];
		matrixService->RotateMatrix_fwk(p[3], p[4], p[5], R);
		for( j=0; j<3; j++) 
		{
			for( k=0; k<3; k++)
			{
				fprintf(fp,  "%.7f ", R[j*3+k] );
			}
			fprintf(fp, "\n");
		}

		fprintf(fp, "\n");
	}

	fprintf(fp, "\n\n");
	fputs("#Motion parameters:\n", fp);
	double *mot = motstruct + ncams*cnp;
	for(i=0; i<nties; i++)
	{
		double *p = mot + i*pnp;
		for(j=0; j<pnp; j++)
		{
			fprintf(fp, "%.5f ", p[j]);
		}
		fprintf(fp, "\n");
	}
}