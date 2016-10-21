#ifndef __bundler_extend_interface_by_zj_h__
#define __bundler_extend_interface_by_zj_h__


#include "BundlerApp.h"
#include "BundlerExport.h"
#include "AT_StructureDef.h"


#ifndef _camera_param
#define _camera_param
//像机内参数
typedef struct camera_param
{
	double x0, y0; 
	double f;
	double k1, k2, k3;
	double p1, p2;
	double _b1, _b2;

	double dx, dy;		//像素大小
	int		imgWid, imgHei;	//像素大小
}camera_param;

#else
#pragma message("camera_param already defined!" )
#endif

#ifndef _camera_extorparam
#define _camera_extorparam
typedef struct extor_param
{
	double X, Y, Z;
	double f, w, k;

	double R[9];

}extor_param;
#endif

void calcImgProj(camera_param camInfo, double t[3], double R[9], double objpt[3], double imgpt[2]);

_Bunder_dll_ std::vector<KeypointMatch> LoadMatchTable(const char *filename, int i1, int i2);

class _Bunder_dll_ CBundler : public BundlerApp
{
public:
	CBundler();
	~CBundler();

	//设置影像列表
	bool	SetImageNameList(const char*  imgList);		//文件名
	bool	SetImageNameList(vector<string>*  imgList); //列表

	//设置特征点列表
	bool	SetKeyNameList(const char* keyList);		//文件名
	bool   SetKeyNameList(vector<string>* keyList); //列表


	int		LoadImagePOSFile(const char*  pFileName);
	void  SetCameraInfo(camera_param camInfo);


	//设置匹配点对，i1, i2为对应的影像号  index is 0 based
	bool  SetMatchTable_xy(vector<KeypointMatch> *pMatches, int i1, int i2);


	/******自动挑点*******/

	//一致性约束测试，删除多对一的匹配点以及坐标相同的匹配点
	void  ConsistencyConstraints(const char *initMatchFile, const char *pruneMatchFile);

	//尺度约束检测，在预先知道像片的尺度时，删除尺度差异较大的匹配点
	void  ScaleConstraints(const char *initMatchFile, const char *pruneMatchFile/*, double scaleThresh*/);

	//方向约束：两张像片之间的旋角固定  todo 5.24
	void  OrientationConstraints(const char *initMatchFile, const char *pruneMatchFile/*, double scaleThresh*/);

	//利用F矩阵进行核线约束
	void  EpipolarConstraints(const char *initMatchFile, const char *pruneMatchFile, camera_param camInfo, int thresh=9);


	//对已设置好的匹配像对进行核线约束   先调用SetMatchTable_xy()将多个初始匹配点对设置进去
	void  EpipolarConstraints(camera_param camInfo, int thresh=9);

	//利用单应矩阵进行，利用物方点的共面假设来进行约束
	void  HomographyConstraints(const char *initMatchFile, const char *pruneMatchFile, camera_param camInfo, int thresh=6);

	//利用相对定向挑点 2张像片
	void  RelativePose2Constraints();

	//三片约束 (仿射变换)
	void  TripleConstraints_Affine(const char *tracksFile, const char *pruneMatchFile, camera_param camInfo, int reprojth=4);

	//有POS的三片检测
//	void  TripleConstraints_POS(const char *initMatchFile, const char *pruneMatchFile, camera_param camInfo, int reprojth=4);

	/*******************************************************/
	//***************生成连接点******************************/
	void  TrackTiePoints(const char *initMatchFile, const char *tiePFile/*, camera_param camInfo*/);

	void  GetMultiViewerTiePoints(const char *filename, const char *multiTiePtsName);

	void  RefineMultiViewerTiePoints(const char *filename, const char *multiTiePtsName);

//	void  ExportATImgPair();

	/******************************************************/                                                                     
	/******************************************************/
	/***********************定向***************************/

	
private:
	//void  PruneMultiMatches();
	//导出匹配点对，bLoadKeys是否从特征点文件中读取点坐标
	bool  WriteMatchTable_xy(const char *filename, bool bLoadKeys=true);
	
	bool  LoadMatchTable_xy(const char *filename);
	

	//将追踪得到的连接点写入文件
	bool  WriteTrackPoints(const char *filename);

	bool	ReadTrackPairs(const char *filename, AT_TiePoint *&pTiePts, int &tiePtsNum);
	bool  WriteTrackPairs(const char *filename, AT_TiePoint *pTiePts, int tiePtsNum);
	
	//像素转框标
	void  Image2Photo(double xi, double yi, double& xp, double& yp);

	//框标转像素
	void  Photo2Image(double xp, double yp, double& xi, double& yi);

	//加载特征点坐标(转换为photo坐标)
	void LoadKeys_TransPhotoCoord(bool descriptor = false);
	//直接加载特征点(像素坐标)
	void LoadKeys_PixelCoord(bool descriptor = false);

	void DoTrack();

	void intersection(AT_3DTiePoint *pData, int num, int lImgID, int rImgID);

	void projection(AT_3DTiePoint *pData, int num, AT_ImgPoint *pImgPts);

private:
	char m_pImgNameList[1024];
	char m_pKeyNameList[1024];

	// interior orientation
	double m_inorImage_a[3];	// pixel to photo
	double m_inorImage_b[3];

	double m_inorPixel_a[3];	// photo to pixel
	double m_inorPixel_b[3];

	camera_param  m_camInfo;
	extor_param	*m_pCamExor;
	int		m_imgNum;
	bool	m_bSetPOS;
};




#endif