#ifndef _IPLR_SIFT_Feature_Detector_H__
#define _IPLR_SIFT_Feature_Detector_H__
// Scale Invariant Feature Transform class
// 对开源的SiftGPU库的调用接口	2010.3	zj
//	2010.4.28  对大影像分块提取sift特征  zj
//  2010.5.4	特征文件bin的读写
//

#ifdef _IMAGE_MATCH_
#define  _Image_Match_  __declspec(dllexport)
#else
#define  _Image_Match_  __declspec(dllimport)	
#endif

#include "\openrs\IntegratedPhotogrammetry\external\SiftGPU\SiftGPU\src\SIFTGPU.h"
#include "orsImage/orsIImageService.h"
#include <vector>
using namespace std;
#include "IPhBaseDef.h"

//#define __BUNDLER_DISTR__
//#include "..\..\BA\bundler-v0.3\src\keys.h"

//#include "keys2a.h"

class orsIImageData;

class _Image_Match_ CSIFT
{
public:
	CSIFT();
	virtual ~CSIFT();
	
	bool	Startup(LPSTR cmd);	//通过参数启动SIFT

	void	SetBlock(int blockWid=1024, int blockHei=1024);	//设置影像分块大小
	
	bool	RunSift_Integrated(orsIImageSourceReader *ImgReader, int maxd=2560);	//对整幅影像提取sift，不分块

	//2011.8.24 zj 对指定波段提取
	bool	RunSift(orsIImageSourceReader *ImgReader, int nband=0);	//对整幅影像提取sift, 自动对大影像分块
	
	bool	RunSift(orsIImageSourceReader *ImgReader, orsRect_i rect);	//对指定的影像块提取sift  todo 4.30

	//取当前提取的SIFT特征点和描述子
	std::vector<KEYPOINT> GetKeypointList();
	std::vector<unsigned char> GetDescriptors();



	bool 	SaveSift_txt(const char * szFileName);	//txt
	bool	SaveSift_bin(const char * szFileName);	//bin
	
	std::vector<KEYPOINT> ReadSiftKeyFile(const char * szFileName);	//todo 4.30
	std::vector<KEYPOINTWithDesc> ReadSiftKeyFileWithDesc(const char *szFileName, bool descriptor);

private:

	void	TransImageData(orsIImageData *imgData, ors_byte *pBuf);
	void	TransImageData(orsIImageData *imgData, ors_byte *pBuf, int nBand);

protected:

	ors_byte		*m_dataBuf;
	ors_uint32		m_BufLen;

private:

	SiftGPU		*m_pSiftEx;
	int		m_blockWid;
	int		m_blockHei;

	vector<SiftGPU::SiftKeypoint> m_SiftGPU_keys;
	vector<float> m_SiftGPU_descriptors;

	vector<KEYPOINT> m_keypoints;
	vector<unsigned char> m_desc;
};




#ifndef _IMAGE_MATCH_
#ifdef _DEBUG
#pragma comment(lib,"ImageMatchD.lib")
#else
#pragma comment(lib,"ImageMatch.lib")
#endif
#endif

#endif