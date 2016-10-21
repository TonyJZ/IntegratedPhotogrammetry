// ImgBlockNet.h: interface for the CImgBlockNet class.
//	create in 2010.4.16		ZJ		影像区域网结构
//  4.19         区域网(区域网)文件后缀 .ibn		
//	4.19		定向方式：三种
//				(1) GPS/IMU 精度较好	直接做光束法空三
//				(2) GPS 较好	相对定向得到初始的角元素，光束法空三
//				(3) 无序像片	自动相对定向，绝对定向，光束法空三
//
//	4.19	留出useless影像的接口，但在BlockViewer和ViewDSM中都未做相应修改，以后再进行完善
//	4.20	sift特征文件后缀: .sift.txt		.sift.bin
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMGFREENET_H__4F34F2BD_19DD_4D3E_BE92_635C7CB03906__INCLUDED_)
#define AFX_IMGFREENET_H__4F34F2BD_19DD_4D3E_BE92_635C7CB03906__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AdgDataType.h"
#include "orsBase\orsIPlatform.h"
#include "StdAfx.h"

class CALMPrjManager;
class CTINClass;

class _Image_FreeNet_ CImgBlockNet  
{
public:
	CImgBlockNet();
	virtual ~CImgBlockNet();
	
	void	SetBlockPrjInfo(CALMPrjManager *pBlockPrj);//设置工程信息
	
	bool	CreateBlockNet_POS();	//利用工程中的POS信息建立区域网，确定像片之间的拓扑关系

//	bool	RebuildFreeNet();	//重建自由网：对现有的m_prjData进行重建

	bool	WriteFile_ibn(const char *pImgNetName);	//写出影像网文件

	bool	ReadFile_ibn(const char *pImgNetName);	//读取影像网文件

	bool	OutputExtOrFile_aop();	//将外方位元素导出aop文件
	bool	OutputIntOrFile_iop();	//将内定向参数导出iop文件

	bool	LoadExtOrFile_aop();
	bool	LoadIntOrFile_iop();





	bool	DetectSIFT(char *option);	//对区域网中的影像检测SIFT特征
	bool	ANNMatch_SIFT(float ratio = 0.6, bool pairwise = false);//Approximate k-NN 匹配 sift特征点
	bool	ExtimateFMatrix();	//利用对极几何估计F矩阵
	
	
	

protected:
	void  ConstructImageTopo();
	float EstimateGroundSizeOfImages( const int numOfNbGiven );



private:
	CALMPrjManager	*m_pBlockPrj;

	bool		*m_pbOrientation;	//像片定向标志
	bool		*m_ImgOverlapM;		//影像重叠矩阵	对称阵

private:	
	//image topology
	CTINClass		*m_pImgNet;
	CEnvelope		m_envelope;
	blockINFO		m_prjData;	//对于航空影像自动重建航带，近景像片不做

private:
	CString		m_strANNMatchFile;

};

void _Image_FreeNet_ FreeNetSetPlatform(orsIPlatform *pPlatform);

#endif // !defined(AFX_IMGFREENET_H__4F34F2BD_19DD_4D3E_BE92_635C7CB03906__INCLUDED_)
