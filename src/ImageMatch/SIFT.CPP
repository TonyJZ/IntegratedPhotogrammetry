#include "StdAfx.h"
#include "sift.h"
#include "glew.h"
#include "math.h"

#ifdef _WIN32
#define SIFTGPU_DLL
#ifdef _DEBUG
#pragma comment(lib, "siftgpu_d.lib")
#else
#pragma comment(lib, "siftgpu.lib")
#endif

#endif


CSIFT::CSIFT()
{
	m_BufLen=0; 
	m_dataBuf=0;
	m_pSiftEx=0;
	m_blockHei=m_blockWid=1024;
}

CSIFT::~CSIFT()
{
 	if(m_dataBuf)
 	{
 		delete[] m_dataBuf;
 		m_dataBuf=0;
 	}

	if(m_pSiftEx)
	{
		delete m_pSiftEx;
		m_pSiftEx=0;
	}
}

bool CSIFT::Startup(LPSTR cmd)
{
	int argc=0;
	char*argv[256]/* = new char*[256]*/;

	if(m_pSiftEx==0)
		m_pSiftEx=new SiftGPU;

	if(*cmd != 0) 
		do
		{
			while(*cmd ==' ') cmd++;
			if(*cmd)
			{
				argv[argc++] = cmd;
			}
			while(*cmd && *cmd != ' ') cmd++;
			if(*cmd==' ')	*cmd++ = 0;
			
		}while(*cmd && argc <256);
		
		m_pSiftEx->ParseParam(argc, argv);
		
		if(m_pSiftEx->CreateContextGL() ==0) 
			return false;
		
		m_pSiftEx->AllocatePyramid(m_blockWid*2, m_blockHei*2);
		
	return true;
}

void CSIFT::SetBlock(int blockWid/* =1024 */, int blockHei/* =1024 */)
{
	m_blockWid=blockWid;
	m_blockHei=blockHei;

	if(m_pSiftEx)
		m_pSiftEx->AllocatePyramid(m_blockWid*2, m_blockHei*2);
}

//SiftGPU只能对BYTE类型数据处理，其他类型的处理结果不对，RunSift首先将影像转换为Byte类型
bool CSIFT::RunSift(orsIImageSourceReader *ImgReader, int nband)
{
	orsIImageData *imgData;
	orsRect_i rect;
	orsBandSet bandSet;
	int		imgWid, imgHei;
	ors_uint32 numBand;
	int i, j;
	ors_int32	NumOfPixels;
	ors_byte *pbuf;
	unsigned int gl_format;

	vector<SiftGPU::SiftKeypoint> blKeys;
	vector<float> blDescrips;

	int siftnum;

 	imgWid=ImgReader->getWidth();
 	imgHei=ImgReader->getHeight();

	rect=ImgReader->getBoundingRect();
	rect.m_xmax+=1;
	rect.m_ymax+=1;

	numBand=ImgReader->getNumberOfInputBands();
	for(i = 0; i < numBand; i++)
	{
		bandSet.push_back(i);
	}

	if(nband>=numBand)	nband=0;	//默认用第0波段

//	dataType=imgData->getDataType();
//	SizeOfType=imgData->getBytesOfBandBuf()/imgData->getNumOfPixels();
	
//	BytesOfBandBuf=imgData->getBytesOfBandBuf();
	NumOfPixels=imgWid*imgHei;

// 	curBufLen=numBand*NumOfPixels;	//全部转换为ors_byte类型
// 
// 	if(curBufLen>m_BufLen)
// 	{
// 		if(m_dataBuf)
// 			delete[] m_dataBuf;
// 
// 		m_BufLen=curBufLen;
// 		m_dataBuf=new ors_byte[m_BufLen];
// 	}
	
	if(m_pSiftEx==0)
		return false;
	
	switch(numBand)
	{
	case 1:
		gl_format = GL_LUMINANCE;
		break;
	case 2:
		gl_format = GL_RGBA;
		break;
	case 3:
		gl_format = GL_RGB;
		break;
	case 4:
		gl_format = GL_RGBA;
		break;
	case 5:
		gl_format = GL_RGBA;
		break;
	case 6:
		gl_format = GL_RGBA;
		break;
	case 7:
		gl_format = GL_RGBA;
		break;
	default :
		assert(0);
		return false;
	}
	gl_format = GL_LUMINANCE;

	ors_int32 nx, ny;

	//分块
	if(imgWid%m_blockWid!=0)
	{
		nx=imgWid/m_blockWid+1;
	}
	else
	{
		nx=imgWid/m_blockWid;		
	}

	if(imgHei%m_blockHei!=0)
	{
		ny=imgHei/m_blockHei+1;
	}
	else
	{
		ny=imgHei/m_blockHei;
	}
	

	m_BufLen=m_blockHei*m_blockWid/**numBand*/;
	if(m_dataBuf)
	{
		delete[] m_dataBuf;
	}
	m_dataBuf=new ors_byte[m_BufLen];
	

//	pbuf=new ors_byte[maxBlkHei*maxBlkWid*numBand];	//像素都是byte型，for siftGPU处理
	pbuf=m_dataBuf;

	m_SiftGPU_keys.clear();
	m_SiftGPU_descriptors.clear();
	ors_int32 x0, y0;
	for(i=0; i<ny; i++)		//规则块
	{
		y0=i*m_blockHei;
		for(j=0; j<nx; j++)
		{
			x0=j*m_blockWid;

			rect.m_xmin=x0;
			if(x0+m_blockWid>imgWid)
				rect.m_xmax=imgWid;
			else
				rect.m_xmax=x0+m_blockWid;

			rect.m_ymin=y0;
			if(y0+m_blockHei>imgHei)
				rect.m_ymax=imgHei;
			else
				rect.m_ymax=y0+m_blockHei;

			imgData=ImgReader->getImageData(rect, 1.0, bandSet);

			TransImageData(imgData, pbuf, nband);

			m_pSiftEx->RunSIFT(rect.width(), rect.height(), pbuf, gl_format, GL_UNSIGNED_BYTE);
	
			siftnum = m_pSiftEx->GetFeatureNum();
			if(siftnum==0)
				continue;

			blKeys.resize(siftnum);	
			blDescrips.resize(128*siftnum);
			m_pSiftEx->GetFeatureVector(&blKeys[0], &blDescrips[0]);
		
			for( vector<SiftGPU::SiftKeypoint>::iterator it_k = blKeys.begin(); it_k!=blKeys.end(); ++it_k )
			{
				it_k->x+=x0;
				it_k->y+=y0;
				m_SiftGPU_keys.push_back(*it_k);
			}  

			for( vector<float>::iterator it_d = blDescrips.begin(); it_d!=blDescrips.end(); ++it_d )
				m_SiftGPU_descriptors.push_back(*it_d);
		}
	}

	return	true;
}

bool CSIFT::RunSift_Integrated(orsIImageSourceReader *ImgReader, int maxd/* =2560 */)
{
	if(m_pSiftEx==0)
		return false;
	
	m_pSiftEx->SetMaxDimension(maxd);

	orsIImageData *imgData;
	orsRect_i rect;
	orsBandSet bandSet;
	int		imgWid, imgHei;
	ors_uint32 numBand;
	orsDataTYPE dataType;
	ors_uint32	curBufLen;
	int SizeOfType;
	int i;
	ors_uint32 BytesOfBandBuf;
	ors_int32	NumOfPixels;
	ors_byte *pbuf;
//	ors_byte  resample;
	unsigned int gl_format;
//	int offset;
//	double maxPixel, minPixel, span, dValue;


	imgWid=ImgReader->getWidth();
	imgHei=ImgReader->getHeight();

	rect=ImgReader->getBoundingRect();
// 	rect.m_xmax+=1;
// 	rect.m_ymax+=1;

	numBand=ImgReader->getNumberOfInputBands();
	for(i = 0; i < numBand; i++)
	{
		bandSet.push_back(i);
	}

	
	imgData=ImgReader->getImageData(rect, 1.0, bandSet);
	dataType=imgData->getDataType();
	SizeOfType=imgData->getBytesOfBandBuf()/imgData->getNumOfPixels();
	
	BytesOfBandBuf=imgData->getBytesOfBandBuf();
	NumOfPixels=imgWid*imgHei;

	curBufLen=numBand*NumOfPixels;	//全部转换为ors_byte类型

	if(curBufLen>m_BufLen)
	{
		if(m_dataBuf)
			delete[] m_dataBuf;

		m_BufLen=curBufLen;
		m_dataBuf=new ors_byte[m_BufLen];
	}
	
	if(m_pSiftEx==0)
		return false;
	
	switch(numBand)
	{
	case 1:
		gl_format = GL_LUMINANCE;
		break;
	case 2:
		gl_format = GL_RGBA;
		break;
	case 3:
		gl_format = GL_RGB;
		break;
	case 4:
		gl_format = GL_RGBA;
		break;
	case 5:
		gl_format = GL_RGBA;
		break;
	case 6:
		gl_format = GL_RGBA;
		break;
	case 7:
		gl_format = GL_RGBA;
		break;
	default :
		assert(0);
		return false;
	}

	pbuf=m_dataBuf;

	imgData=ImgReader->getImageData(rect, 1.0, bandSet);
	TransImageData(imgData, pbuf);

// 	if(dataType==ORS_DT_BYTE)
// 	{//直接转换存储顺序
// 		pbuf=m_dataBuf;
// 		for(i=0; i<numBand; i++)
// 		{//将影像数据导到一个缓冲中
// 			ors_byte *bandbuf=imgData->getBandBuf(i);
// 			for(j=0; j<NumOfPixels; j++)
// 			{
// 				offset=j*SizeOfType*numBand;
// 				pbuf[offset+i*SizeOfType]=bandbuf[j*SizeOfType];
// 			}
// 		}
// 	}
// 	else
// 	{
// 		pbuf=m_dataBuf;
// 		for(i=0; i<numBand; i++)
// 		{//将影像数据导到一个缓冲中
// 			ors_byte *bandbuf=imgData->getBandBuf(i);
// 			maxPixel=imgData->getMaxSampleValue(i);
// 			minPixel=imgData->getMinSampleValue(i);
// 			span=maxPixel-minPixel;
// 			for(j=0; j<NumOfPixels; j++)
// 			{
// 				dValue=imgData->getSampleValue(j, i);
// 				resample=((dValue-minPixel)/span)*255;
// 				offset=j*numBand;
// 				pbuf[offset+i]=resample;
// 			}
// 		}
// 	}

	m_pSiftEx->RunSIFT(imgWid, imgHei, m_dataBuf, gl_format, GL_UNSIGNED_BYTE);

	m_SiftGPU_keys.clear();
	m_SiftGPU_descriptors.clear();


	int siftnum = m_pSiftEx->GetFeatureNum();
	m_SiftGPU_keys.resize(siftnum);	
	m_SiftGPU_descriptors.resize(128*siftnum);
	m_pSiftEx->GetFeatureVector(&m_SiftGPU_keys[0], &m_SiftGPU_descriptors[0]);

	m_keypoints.clear();
	m_desc.clear();
	m_keypoints.resize(siftnum);
	m_desc.resize(siftnum*128);


	KEYPOINT* kp=&m_keypoints[0];
	float * pd = &m_SiftGPU_descriptors[0];
	unsigned char* desc=&m_desc[0];
	for(i=0; i<siftnum; i++)
	{
		kp->x=m_SiftGPU_keys[i].x;
		kp->y=m_SiftGPU_keys[i].y;
		kp->scale=m_SiftGPU_keys[i].s;
		kp->ori=m_SiftGPU_keys[i].o;

		for(int k = 0; k < 128; k ++, pd++, desc++) 
		{
			*desc= (unsigned int)floor(0.5+512.0f*(*pd));
		}
		kp++;
	}

	return	true;
}

vector<KEYPOINT> CSIFT::GetKeypointList()
{
	
	return m_keypoints;
}

vector<unsigned char> CSIFT::GetDescriptors()
{
	return m_desc;
}

bool CSIFT::SaveSift_txt(const char * szFileName)
{
	FILE *fp=0;
	
	fp=fopen(szFileName, "w");
	if(fp==0)
	{
		AfxMessageBox("can't save Sift key file!");
		return false;
	}
	
	int keyNum=0;
	int i, j;
	int realNum;
	
	keyNum=m_SiftGPU_keys.size();
	
	float * pd = &m_SiftGPU_descriptors[0] ;
	bool	*pruned=0;
	pruned=new bool[keyNum];
	memset(pruned, 0, sizeof(bool)*keyNum);
// 	for(i=0; i<keyNum; i++)
// 	{
// 		if(pruned[i])
// 			continue;
// 
// 		for(j=i+1; j<keyNum; j++)
// 		{
// 			if(pruned[j])
// 				continue;
// 
// 			if(fabs(m_SiftGPU_keys[i].x-m_SiftGPU_keys[j].x)<1e-3 &&
// 				fabs(m_SiftGPU_keys[i].y-m_SiftGPU_keys[j].y)<1e-3)
// 				pruned[j]=true;
// 		}
// 	}
// 
// 	for(i=0, realNum=0; i<keyNum; i++)
// 	{
// 		if(pruned[i])
// 			continue;
// 
// 		realNum++;
// 	}
	realNum=keyNum;
	fprintf(fp,  "%d 128\n", realNum);
	for(i=0; i<keyNum; i++)
	{
		if(pruned[i])
			continue;

		fprintf(fp, "%f %f %f %f\n", m_SiftGPU_keys[i].x, m_SiftGPU_keys[i].y, m_SiftGPU_keys[i].s, m_SiftGPU_keys[i].o);
		
		for(int k = 0; k < 128; k ++, pd++) 
		{
			fprintf(fp, "%d ", (unsigned int)floor(0.5+512.0f*(*pd)));
			
			if((k+1)%20 == 0 ) 
				fprintf(fp, "\n"); //suggested by Martin Schneider
			
		}
		fprintf(fp, "\n");
	}
	
	fclose(fp);

//	if(pruned)	delete[] pruned;
	return	true;
}

bool CSIFT::SaveSift_bin(const char * szFileName)
{
	FILE	*fp=0;

	fp=fopen(szFileName, "wb");

	if(fp==0)
	{
		AfxMessageBox("can't save Sift key file!");
		return false;
	}
	
	int keyNum=0;
	int desNum=128;
	unsigned int descriptor;
	
	keyNum=m_SiftGPU_keys.size();
	fwrite(&keyNum, sizeof(int), 1, fp);
	fwrite(&desNum, sizeof(int), 1, fp);
	
	float * pd = &m_SiftGPU_descriptors[0] ;
	for(int i=0; i<keyNum; i++)
	{
		fwrite(&(m_SiftGPU_keys[i].x), sizeof(float), 1, fp);
		fwrite(&(m_SiftGPU_keys[i].y), sizeof(float), 1, fp);
		fwrite(&(m_SiftGPU_keys[i].s), sizeof(float), 1, fp);
		fwrite(&(m_SiftGPU_keys[i].o), sizeof(float), 1, fp);
		for(int k = 0; k < 128; k ++, pd++) 
		{
			descriptor=(unsigned int)floor(0.5+512.0f*(*pd));
			fwrite(&descriptor, sizeof(unsigned int), 1, fp);

			//fprintf(fp, "%d ", (unsigned int)floor(0.5+512.0f*(*pd)));
			
			//if((k+1)%20 == 0 ) 
			//	fprintf(fp, "\n"); //suggested by Martin Schneider
			
		}
		//fprintf(fp, "\n");
	}


	if(fp)	
		fclose(fp);
	return	true;
}


//自动判断是txt格式的还是bin格式的
std::vector<KEYPOINT>  CSIFT::ReadSiftKeyFile(const char * szFileName)
{
	FILE *file=0;
	vector<KEYPOINT> keys; 

    file = fopen (szFileName, "r");
//	if(file==0)
//		return  false;

	return	keys;
}

std::vector<KEYPOINTWithDesc> CSIFT::ReadSiftKeyFileWithDesc(const char *szFileName, bool descriptor)
{
	vector<KEYPOINTWithDesc> keys;
	

	return	keys;
}

//将imgData转换成SIFT需要的影像块格式
void CSIFT::TransImageData(orsIImageData *imgData, ors_byte *pBuf)
{
	orsRect_i rect;
	int wid, hei;
	orsDataTYPE	dataType;
	int i, j;
	int offset;
	ors_uint numBand;
	ors_int32 NumOfPixels;
	int SizeOfType;
	double maxPixel, minPixel, span;
	double dValue;
	ors_byte  resample;


	rect=imgData->getRect();
	wid=imgData->getWidth();
	hei=imgData->getHeight();
	dataType=imgData->getDataType();
	numBand=imgData->getNumberOfBands();
	SizeOfType=imgData->getBytesOfBandBuf()/imgData->getNumOfPixels();
	NumOfPixels=wid*hei/**SizeOfType*/;


	if(dataType==ORS_DT_BYTE)
	{//直接转换存储顺序		gdal 波段优先	GPUSift 像素优先
		
		for(i=0; i<numBand; i++)
		{//将影像数据导到一个缓冲中
			ors_byte *bandbuf=imgData->getBandBuf(i);
			for(j=0; j<NumOfPixels; j++)
			{
				offset=j*SizeOfType*numBand;
				pBuf[offset+i*SizeOfType]=bandbuf[j*SizeOfType];
			}
		}
	}
	else
	{
		for(i=0; i<numBand; i++)
		{//将影像数据导到一个缓冲中
			//ors_byte *bandbuf=imgData->getBandBuf(i);
			maxPixel=imgData->getMaxSampleValue(i);
			minPixel=imgData->getMinSampleValue(i);
			span=maxPixel-minPixel;
			for(j=0; j<NumOfPixels; j++)
			{
				dValue=imgData->getSampleValue(j, i);
				resample=((dValue-minPixel)/span)*255;
				offset=j*numBand;
				pBuf[offset+i]=resample;
			}
		}
	}

}

//转换指定波段数据
void CSIFT::TransImageData(orsIImageData *imgData, ors_byte *pBuf, int nBand)
{
	orsRect_i rect;
	int wid, hei;
	orsDataTYPE	dataType;
	int i, j;
	int offset;
	ors_uint numBand;
	ors_int32 NumOfPixels;
	int SizeOfType;
	double maxPixel, minPixel, span;
	double dValue;
	ors_byte  resample;


	rect=imgData->getRect();
	wid=imgData->getWidth();
	hei=imgData->getHeight();
	dataType=imgData->getDataType();
	numBand=imgData->getNumberOfBands();
	SizeOfType=imgData->getBytesOfBandBuf()/imgData->getNumOfPixels();
	NumOfPixels=wid*hei/**SizeOfType*/;


	if(dataType==ORS_DT_BYTE)
	{//直接转换存储顺序		gdal 波段优先	GPUSift 像素优先

//		for(i=0; i<numBand; i++)
//		{//将影像数据导到一个缓冲中
			ors_byte *bandbuf=imgData->getBandBuf(nBand);
			for(j=0; j<NumOfPixels; j++)
			{
				offset=j;
				pBuf[offset]=bandbuf[j];
			}
//		}
	}
	else
	{
//		for(i=0; i<numBand; i++)
//		{//将影像数据导到一个缓冲中
			//ors_byte *bandbuf=imgData->getBandBuf(i);
			maxPixel=imgData->getMaxSampleValue(nBand);
			minPixel=imgData->getMinSampleValue(nBand);
			span=maxPixel-minPixel;
			for(j=0; j<NumOfPixels; j++)
			{
				dValue=imgData->getSampleValue(j, nBand);
				resample=((dValue-minPixel)/span)*255;
				offset=j;
				pBuf[offset]=resample;
			}
//		}
	}

}