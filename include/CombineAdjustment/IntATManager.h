#ifndef _INTEGRATED_AT_MANAGER_INCLUDE_H__
#define _INTEGRATED_AT_MANAGER_INCLUDE_H__

#include "AlignPrj.h"
#include "Calib_Camera.h"
#include "CombineAdjustment\CATTStruct_def.h"
#include <vector>
#include <map>

#include "orsBase/orsIPlatform.h"

#include "CombineAdjustment/orsAlgCameraCalib.h"
#include "CombineAdjustment/orsAlgImageBA.h"

using namespace std;

#ifdef COMBINEADJUSTMENT_EXPORTS
#define  _CADJ_DLL_  __declspec(dllexport)
#else
#define  _CADJ_DLL_  __declspec(dllimport)	
#endif

//first key: 相片ID(条带编码过的), second key: 影像列表中的编号
typedef std::map<int, int> myLUT;


class  _CADJ_DLL_ CIntATManager
{
public:
	CIntATManager(orsIPlatform *platform);
	virtual ~CIntATManager();

	//设置结果输出目录
	void setOutputDir(const char *pszDir);

	//设置相机参数
	void setCameraParam(double x0, double y0, double f, double imgWid, double imgHei, double pixWid, double pixHei,
		lensDistortion lensDist, double camera_mount_angle[3]=NULL);

	//设置测区信息，自动提取影像初始外方位元素
	void setAlignPrj(CAlignPrj *pAlg);

	//输入同名观测值
	bool loadTiePoints(const char* pszFileName);

	//读取visualSFM的nvm文件
	bool loadnvmFile(const char* pszFileName);

	//读取控制点文件
	bool loadPATBconFile(const char *pFileName);

	//读取GPS数据
	bool loadGPSEOFile(const char *pFileName);

	//读取PATB平差结果ori文件
	bool loadoriFile(const char *pFileName);
	//读取平差后的连接点坐标
	bool loadadjFile(const char *pFileName);

    //根据GPS坐标估算相片的偏角
	void calImageAngleElement_byGPS();

	bool loadPATBimFile(const char *pFileName);

	//从lidar点云中提取定向锚点
	std::vector<orsPOSParam>* get_OrientedArchorsFromLIDAR(double interval);

	//导出定向点
	bool output_OrientedArchors(const char *pFileName);

	//取条带数
	int getStripNum();
	//取测区数
	int getSurvAreaNum();
	//取相片总数
	int getSumofImages();
	//取影像名，相片号(整体的)
	std::string getImageName(int imgId);

	//根据影像名取索引号，不存在返回-1
	int getImageID(const char *imgName);

	//提取影像外定向参数
	//POS_IntepolatedFromLIDAR: 从POS轨迹中提取相片定向数据
	//POS_Raw: 直接使用初始参数（aop文件中提取）
	bool extractImgOrParam(double *pImgExor, orsCamSysExtraParam_TYPE epType, CamPOS_TYPE posType);
	//t_i, t0_i
	bool extractImgExposureTime(double *timeArr);

	//提取GPS观测信息
	bool extractGPSInfo(GPSinfo *pGPS);

//	bool extractImgOrDirectly(double *pImgExor);
	/////////////////////////////////////////////////////////////////
	//        1.相机安装误差标定                                   //
	//利用安装参数纠正影像外方位元素
	void cameraMountingError_correction(double *pImgExor, orsCamSysExtraParam *param);

	//相机安置参数标定
	bool camera_calibration_BA(char *pszCorrespFile, orsCamSysExtraParam *param, orsCamSysExtraParam_TYPE epType, CamPOS_TYPE posType);

	//前交-后交法
	bool camera_calibration_IP(char *pszCorrespFile, orsCamSysExtraParam *param, orsCamSysExtraParam_TYPE epType, CamPOS_TYPE posType);

	/////////////////////////////////////////////////////////////////
	//                  2.影像光束法平差                           //
	//经典光束法平差，改6个外方位元素
	//GPS辅助的光束法平差
	//POS辅助的光束法平差
	bool imageBA(/*char *pszCorrespFile, */char *pszSBAResultFile, orsImageBA_TYPE BA_type);


protected:
	
	//计算连接点坐标
	void cal_tiePtCoord(std::vector<ATT_tiePoint> *pTiesVec, double *pExor, orsCamSysExtraParam *calib_param, double *p3Ds);

	//与工程中的相片名对应
	//pExor: 相片外方位元素
	//calib_param: 相机安装参数
	bool output_imgExor(const char *pszFileName, double *pExor, orsCamSysExtraParam *calib_param);

private:
//	ATT_CameroRoateMODE	m_rMode;	//相机旋转模式

	double m_camera_mount_angle[3]; //相机安装角（radian）
	
	_iphCamera	*m_pCamera;
	CAlignPrj	*m_AlgPrj;
	std::vector<std::string>	 m_imgNameVec;
	std::vector<ATT_StripInfo>   m_blockInfo;


	myLUT   *m_imgID_LUT;	//相片ID的查找表

	std::vector<orsPOSParam>	m_OrAVec;	//定向锚点
	std::vector<ATT_tiePoint>	m_tiePts;	//连接点

	std::vector<ATT_GCP>		m_GCP;		//控制点

	int		m_nprojs;//像点数，观测值个数（一个连接点对应多个像点）

	std::string  m_outputDir;

	POINT3D		m_center;
};


#endif