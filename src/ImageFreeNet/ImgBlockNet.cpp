// ImgFreeNet.cpp: implementation of the CImgBlockNet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageFreeNet.h"
#include "ImgBlockNet.h"
#include "ALMPrjManager.h"
#include "TinDLL/TinClass.h"
#include "..\ImageMatch\sift.h"
#include "orsGuiBase/orsIProcessMsgBar.h"
#include "bundler\keys2a.h"
//#include "matrix.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int crCode2TopoIndex[11] = {
	-1, tiRIGHT, tiLEFT, 
		-1, tiBOTTOM, tiBottomRIGHT, tiBottomLEFT, 
	-1, tiTop, tiTopRIGHT, tiTopLEFT };


CImgBlockNet::CImgBlockNet()
{
	m_pBlockPrj=0;
	m_pImgNet=0;
	m_pbOrientation=0;
	m_ImgOverlapM=0;
}

CImgBlockNet::~CImgBlockNet()
{
	if(m_pbOrientation)
	{
		delete[] m_pbOrientation;
		m_pbOrientation=0;
	}

	if(m_ImgOverlapM)
	{
		delete[] m_ImgOverlapM;
		m_ImgOverlapM=0;
	}
	if(m_pImgNet)
	{
		delete m_pImgNet;
		m_pImgNet=0;
	}

	m_prjData.images.clear();
	m_prjData.strips.clear();
}

void CImgBlockNet::SetBlockPrjInfo(CALMPrjManager *pBlockPrj)
{
	ASSERT(pBlockPrj);
	m_pBlockPrj=pBlockPrj;
	ASSERT(m_pBlockPrj->GetImgList());
	
	int		imgNum=m_pBlockPrj->GetImgList()->GetSize();

	if(m_pbOrientation)
		delete[] m_pbOrientation;
	
	m_pbOrientation=new bool[imgNum];
	memset(m_pbOrientation, 0, sizeof(bool)*imgNum);

	if(m_ImgOverlapM)
		delete[] m_ImgOverlapM;

	m_ImgOverlapM=new bool[imgNum*imgNum];
	memset(m_ImgOverlapM, 0, sizeof(bool)*imgNum*imgNum);	//默认所有像片均不重叠

}

//建立区域网：利用工程文件信息建立区域网	
//只需要外方位元素
bool CImgBlockNet::CreateBlockNet_POS()
{
	int	i;
	iphImgList	*imgList;
	int	num;
	int	unoriented;
	double ang2rad;

	m_envelope.XMin = 99999999.0;	m_envelope.XMax = -99999999.0;
	m_envelope.YMin = 99999999.0;	m_envelope.YMax = -99999999.0;
	
	if(m_pBlockPrj==NULL)
		return	false;

	imgList=m_pBlockPrj->GetImgList();
	if(imgList==NULL)
		return	false;

	m_prjData.prjDir=m_pBlockPrj->GetAlmFileDir();
	m_prjData.flyOverlap=m_pBlockPrj->GetFlyOverlap();
	m_prjData.stripOverlap=m_pBlockPrj->GetStripOverlap();

	m_prjData.images.clear();
	m_prjData.strips.clear();
	num = imgList->GetSize();
	photoINFO *imgInfo = new photoINFO;

	for( i=0; i<num; i++ )
	{
		memset( &imgInfo->extOr, 0, sizeof(extorPARA ) );
			
		imgInfo->fileName = imgList->GetAt(i).strImgName;

		m_prjData.images.push_back( *imgInfo );
	}	
	delete imgInfo;

	unoriented=0;
	for( i=0; i<num; i++ )			
	{
		m_prjData.images[i].marker = 0;

		if(imgList->GetAt(i).camera.m_bInor==false)
		{
			CString	str;
			str.Format("%s没有像机参数!", imgList->GetAt(i).strImgName);
			AfxMessageBox(str);
			return	false;
		}
		//内参数
		m_prjData.images[i].intOr.x0=imgList->GetAt(i).camera.m_x0;
		m_prjData.images[i].intOr.y0=imgList->GetAt(i).camera.m_y0;
		m_prjData.images[i].intOr.f =imgList->GetAt(i).camera.m_f;
		m_prjData.images[i].intOr.k1=imgList->GetAt(i).camera.m_k1;
		m_prjData.images[i].intOr.k2=imgList->GetAt(i).camera.m_k2;
		m_prjData.images[i].intOr.k3=imgList->GetAt(i).camera.m_k3;
		m_prjData.images[i].intOr.p1=imgList->GetAt(i).camera.m_p1;
		m_prjData.images[i].intOr.p2=imgList->GetAt(i).camera.m_p2;

		double imgWid, imgHei;
		double pixelX, pixelY;
		imgWid=imgList->GetAt(i).camera.m_imgWid;
		imgHei=imgList->GetAt(i).camera.m_imgHei;
		pixelX=imgList->GetAt(i).camera.m_pixelWid;
		pixelY=imgList->GetAt(i).camera.m_pixelHei;

		m_prjData.images[i].intOr.inorImage_a[0] = -imgWid*pixelX/2;
		m_prjData.images[i].intOr.inorImage_a[1] = pixelX;
		m_prjData.images[i].intOr.inorImage_a[2] = 0;	
		//y方向
		m_prjData.images[i].intOr.inorImage_b[0] = imgHei*pixelY/2;
		m_prjData.images[i].intOr.inorImage_b[1] = 0;
		m_prjData.images[i].intOr.inorImage_b[2] = -pixelY;
		
		m_prjData.images[i].intOr.inorPixel_a[0] = imgWid/2;	
		m_prjData.images[i].intOr.inorPixel_a[1] = 1/pixelX;
		m_prjData.images[i].intOr.inorPixel_a[2] = 0;
		m_prjData.images[i].intOr.inorPixel_b[0] = imgHei/2;
		m_prjData.images[i].intOr.inorPixel_b[1] = 0;
		m_prjData.images[i].intOr.inorPixel_b[2] = -1/pixelY;


		//转弧度
		if(imgList->GetAt(i).camera.m_AngleUnit==Unit_Degree360)
		{
			ang2rad=PI/180;
		}
		else if(imgList->GetAt(i).camera.m_AngleUnit==Unit_Degree400)
		{
			ang2rad=PI/200;
		}
		else if(imgList->GetAt(i).camera.m_AngleUnit==Unit_Radian)
		{
			ang2rad=1.0;
		}
		else
		{
			ang2rad=1.0;
		}

		//外参数
		if(imgList->GetAt(i).camera.m_bExor==false)
		{
			unoriented++;
			m_pbOrientation[i]=false;
// 			for(j=0; j<num; j++)
// 			{//未定向，无法得到重叠关系
// 				m_ImgOverlapM[i*num+j]=0;
// 			}
			m_prjData.images[i].extOr.Xs=0;
			m_prjData.images[i].extOr.Ys=0;
			m_prjData.images[i].extOr.Zs=0;

			m_prjData.images[i].extOr.phi=0;
			m_prjData.images[i].extOr.omega=0;
			m_prjData.images[i].extOr.kappa=0;
	
			m_prjData.images[i].extOr.R[0]=1;
			m_prjData.images[i].extOr.R[1]=0;
			m_prjData.images[i].extOr.R[2]=0;

			m_prjData.images[i].extOr.R[3]=0;
			m_prjData.images[i].extOr.R[4]=1;
			m_prjData.images[i].extOr.R[5]=0;

			m_prjData.images[i].extOr.R[6]=0;
			m_prjData.images[i].extOr.R[7]=0;
			m_prjData.images[i].extOr.R[8]=1;

			m_prjData.images[i].extOr.GPSTime=0;
		}
		else
		{
			m_pbOrientation[i]=true;

			m_prjData.images[i].extOr.Xs=imgList->GetAt(i).camera.m_Xs;
			m_prjData.images[i].extOr.Ys=imgList->GetAt(i).camera.m_Ys;
			m_prjData.images[i].extOr.Zs=imgList->GetAt(i).camera.m_Zs;
			
			m_prjData.images[i].extOr.phi=imgList->GetAt(i).camera.m_phi*ang2rad;
			m_prjData.images[i].extOr.omega=imgList->GetAt(i).camera.m_omega*ang2rad;
			m_prjData.images[i].extOr.kappa=imgList->GetAt(i).camera.m_kappa*ang2rad;
			
			memcpy(m_prjData.images[i].extOr.R, imgList->GetAt(i).camera.m_RMatrix, sizeof(double)*9);
					
			m_prjData.images[i].extOr.GPSTime=imgList->GetAt(i).camera.m_GPSTime;
		}
		
		if( m_envelope.XMax < m_prjData.images[i].extOr.Xs )
			m_envelope.XMax = m_prjData.images[i].extOr.Xs;
		if( m_envelope.XMin > m_prjData.images[i].extOr.Xs )
			m_envelope.XMin = m_prjData.images[i].extOr.Xs;
		
		if( m_envelope.YMax < m_prjData.images[i].extOr.Ys )
			m_envelope.YMax = m_prjData.images[i].extOr.Ys;
		if( m_envelope.YMin > m_prjData.images[i].extOr.Ys )
			m_envelope.YMin = m_prjData.images[i].extOr.Ys;
	}
	
	if(unoriented>0)
	{
		CString	str;
		str.Format("%d张像片没有定向信息,请先进行自动定向!", unoriented);
		AfxMessageBox(str);
		return	false;
	}

	ConstructImageTopo();	//建立影像间的拓扑关系

	OutputExtOrFile_aop();	//导出外参数	
	OutputIntOrFile_iop();	//导出内参数

	return	true;
}

/*
bool CImgBlockNet::RebuildFreeNet()
{
	int	num;
	int uselessNum;
	int i;

	m_envelope.XMin = 99999999.0;	m_envelope.XMax = -99999999.0;
	m_envelope.YMin = 99999999.0;	m_envelope.YMax = -99999999.0;

	num = m_prjData.images.size();
	
	uselessNum=0;
	for( i=0; i<num; i++ )			
	{
		if(m_prjData.images[i].useless)
		{
			uselessNum++;
			continue;
		}

		m_prjData.images[i].marker = 0;
				
		if( m_envelope.XMax < m_prjData.images[i].extOr.Xs )
			m_envelope.XMax = m_prjData.images[i].extOr.Xs;
		if( m_envelope.XMin > m_prjData.images[i].extOr.Xs )
			m_envelope.XMin = m_prjData.images[i].extOr.Xs;
		
		if( m_envelope.YMax < m_prjData.images[i].extOr.Ys )
			m_envelope.YMax = m_prjData.images[i].extOr.Ys;
		if( m_envelope.YMin > m_prjData.images[i].extOr.Ys )
			m_envelope.YMin = m_prjData.images[i].extOr.Ys;
	}
	
	TRACE("%d image can't add to block. Total num is \%d \n", uselessNum, num);
	if(uselessNum>=num)
	{
		CString	str;
		str.Format("影像构网失败!");
		AfxMessageBox(str);
		return	false;
	}
	
	ConstructImageTopo();
	
	return	true;
}*/


void CImgBlockNet::ConstructImageTopo()
{
	if( m_pImgNet )
		delete m_pImgNet;
	
	m_pImgNet = new CTINClass;
	
	int i, imgNum;
	for( i=0; i<m_prjData.images.size(); i++ )
	{
		m_prjData.images[i].tinPt = m_pImgNet->AddPoint( m_prjData.images[i].extOr.Xs,  m_prjData.images[i].extOr.Ys,  m_prjData.images[i].extOr.Zs, i );
	}
	
	m_pImgNet->FastConstruct();
	
	//////////////////////////////////////////////////////////////////////////
	
	double imageSize = 0; 
	imageSize = EstimateGroundSizeOfImages( 8 );
	if( imageSize == 0 )
		imageSize = EstimateGroundSizeOfImages( 5 );
	if( imageSize == 0 )
		imageSize = EstimateGroundSizeOfImages( 3 );
	if( imageSize == 0 )
		imageSize = EstimateGroundSizeOfImages( 2 );
	
	m_prjData.imgGrdSize = imageSize;
	
	m_envelope.XMin -= imageSize;	m_envelope.XMax += imageSize;
	m_envelope.YMin -= imageSize;	m_envelope.YMax += imageSize;
	//////////////////////////////////////////////////////////////////////////
	
	int numOfNb,j;
	Clipper minClip, maxClip;
	
	double minBoxSizeX   = 0.20*imageSize;	// 航向最小距离
	double minBoxSizeY   = 0.35*imageSize;	// 旁向最小距离
	double maxBoxSizeX   = 0.50*imageSize;	// 航向至少重叠50%
	double maxBoxSizeY   = 0.80*imageSize;	// 旁向至少重叠20%
	
	minClip.SetClipWin( -minBoxSizeX, -minBoxSizeY, minBoxSizeX, minBoxSizeY );
	maxClip.SetClipWin( -maxBoxSizeX, -maxBoxSizeY, maxBoxSizeX, maxBoxSizeY );
	
	imgNum=m_prjData.images.size();
	for( i=0; i<imgNum; i++ )
	{
		triPOINT **nbPoints = m_pImgNet->GetNeighborPoints( m_prjData.images[i].tinPt, &numOfNb );
		
		double dx, dy, dxt;
		extorPARA *extOr = &m_prjData.images[i].extOr;
		double *R = extOr->R;
		
		int topIndex;
		
		for( j=0; j<8; j++ )
			m_prjData.images[i].nbImages[j].imgIndex = -1;
		
		for( j=0; j<numOfNb; j++ )	
		{
			dxt = nbPoints[j]->x - m_prjData.images[i].tinPt->x;
			dy = nbPoints[j]->y - m_prjData.images[i].tinPt->y;
			
			// 按方位旋转
			dx = R[0]*dxt + R[3]*dy;
			dy = R[1]*dxt + R[4]*dy;
			
			// 判断重叠
			if( maxClip.pointChk( dx, dy) == POINTIN )	
			{
				
				// 判断方位
				topIndex = crCode2TopoIndex[ minClip.regionCode( dx, dy ) ];
				
				ASSERT( topIndex >= 0 );
				
				ASSERT( m_prjData.images[i].nbImages[topIndex].imgIndex == -1 );
				
				m_prjData.images[i].nbImages[topIndex].imgIndex = nbPoints[j]->marker;

				m_ImgOverlapM[i*imgNum+nbPoints[j]->marker]=true;
			}
		}
		
		if( -1 == m_prjData.images[i].nbImages[tiLEFT].imgIndex )	
		{
			stripINFO strip;
			strip.fisrtImgIndex = i;
			
			m_prjData.strips.push_back( strip );
		}
	}
}

float CImgBlockNet::EstimateGroundSizeOfImages( const int numOfNbGiven )
{
	int numOfNb,i, j;
	
	
	for( i=0; i< m_prjData.images.size(); i++ )
	{
		triPOINT **nbPoints = m_pImgNet->GetNeighborPoints( m_prjData.images[i].tinPt, &numOfNb );
		
		if( numOfNbGiven == numOfNb )	{
			double imgSize;
			double dx, dy, dist;
			vector <float> dists;
			
			for( j=0; j<numOfNb; j++ )	
			{
				dx = nbPoints[j]->x - m_prjData.images[i].tinPt->x;
				dy = nbPoints[j]->y - m_prjData.images[i].tinPt->y;
				
				dist = sqrt(dx*dx + dy*dy );
				dists.push_back( dist );
			}
			
			sort( dists.begin(), dists.end() );
			
			imgSize = 0;
			switch( numOfNbGiven )	{
			case 8:	// 左右2个，上下2个，对角4个
				{
					// 航向
					dist = (dists[0]+dists[1] )/2;
					imgSize += dist / (1.0 - m_prjData.flyOverlap/100.0 );
					
					// 旁向
					dist = (dists[2]+dists[3] )/2;
					imgSize += dist / (1.0 - m_prjData.stripOverlap/100.0 );
					
					imgSize /= 2;
				}
				break;
				
			case 5:	// 左右2个，上下1个，对角2个(或者左右1个，上下2个，对角2个)
				{
					// 航向
					dist = (dists[0]+dists[1] )/2;
					imgSize += dist / (1.0 - m_prjData.flyOverlap/100.0 );
					
					// 旁向
					dist = dists[2];
					imgSize += dist / (1.0 - m_prjData.stripOverlap/100.0 );
					
					imgSize /= 2;
				}
				break;
			case 3:	// 左右1个，上下1个，对角1个
				{
					// 航向
					imgSize += dists[0] / (1.0 - m_prjData.flyOverlap/100.0 );
					
					// 旁向
					imgSize += dists[1] / (1.0 - m_prjData.stripOverlap/100.0 );
					
					imgSize /= 2;
				}
				break;
			case 2:
				// 左右2个，上下0个，对角0个
				{
					// 航向
					dist = (dists[0]+dists[1] )/2;
					imgSize += dist / (1.0 - m_prjData.flyOverlap/100.0 );
				}
				break;
			}
			
			return imgSize;
		}
	}
	
	return 0;
}

//.ibn	image block network file
bool CImgBlockNet::WriteFile_ibn(const char *pImgNetName)
{
	FILE	*fp=NULL;
	CString	strPath;
	int	pos, length;
	int	imgNum, i, j;
	int	uselessNum;

	fp = fopen( pImgNetName, "wt" );
	if(fp==0)
		return false;
	
	strPath=pImgNetName;
	
	pos=strPath.ReverseFind('\\');
	length=strPath.GetLength();
	strPath=strPath.Right(length-pos-1);
	
	fprintf( fp, "dps survey block data file: V1.0\n" );
	fprintf( fp, "[BLOCK NAME]\n%s\n", strPath.GetBuffer(128) );
	fprintf( fp, "[BLOCK DIRECTORY]\n%s\n", m_prjData.prjDir );
	
	int	flyOverlap=m_prjData.flyOverlap;
	int stripOverlap=m_prjData.stripOverlap;
	
	fprintf( fp, "[FLY OVERLAY]\n%d\n", flyOverlap);
	fprintf( fp, "[STRIP OVERLAY]\n%d\n", stripOverlap );
	
	imgNum=m_prjData.images.size();
	if( imgNum > 0 )
	{
		fprintf( fp, "[NUM OF IMAGES]\n%d\n", imgNum );
		uselessNum=0;
		for (i=0; i<imgNum; i++)
		{
			fprintf(fp, "%s\n", m_prjData.images[i].fileName );
			if(m_prjData.images[i].useless)
				uselessNum++;
		}

		fprintf(fp, "[NUM OF USELESS IMAGE]\n%d\n", uselessNum);
		for(i=0; i<imgNum; i++)
		{//无法加入区域网的影像 
			if(m_prjData.images[i].useless)
				fprintf(fp, "%d\n", i);
		}

		fprintf(fp, "[Image Adjacency Matrix]\n");
		for(i=0; i<imgNum; i++)
		{
			for(j=0; j<imgNum; j++)
			{
				fprintf(fp, "%d ", m_ImgOverlapM[i*imgNum+j]);
			}
			fprintf(fp, "\n");
		}
	}

	if(fp)
		fclose(fp);

	return	true;
}

bool CImgBlockNet::ReadFile_ibn(const char *pImgNetName)
{
	FILE	*fp=0;
	char pLine[1024];
	int	 maxString=1024;
	int imgNum, uselessNum;
	int i, j, uselessID;
	photoINFO imgInfo;
	
	fp=fopen(pImgNetName, "r");
	if(fp==NULL)
		return	false;
	
	fgets(pLine, maxString, fp );
	if( !strstr( pLine, "dps survey block data file: V1.0") )	
	{
		fclose(fp);
		return false;
	}
	
	m_prjData.images.clear();
	m_prjData.strips.clear();
	if(m_ImgOverlapM)
	{
		delete[] m_ImgOverlapM;
	}

	imgNum=0;	uselessNum=0;
	while(!feof(fp)) 
	{
		fgets(pLine, maxString, fp);
		if(strstr(pLine, "[BLOCK NAME]"))
		{
			fscanf( fp, "%s\n", &(m_prjData.prjName));
		}
		else if(strstr(pLine, "[BLOCK DIRECTORY]"))
		{
			fscanf(fp, "%s\n", &(m_prjData.prjDir));
		}
		else if(strstr(pLine, "[FLY OVERLAY]"))
		{
			fscanf( fp, "%d", &(m_prjData.flyOverlap) );
		}
		else if(strstr(pLine, "[STRIP OVERLAY]"))
		{
			fscanf(fp, "%d", &(m_prjData.stripOverlap));
		}
		else if(strstr(pLine, "[NUM OF IMAGES]"))
		{
			fscanf(fp, "%d\n", &imgNum);
			for(i=0; i<imgNum; i++)
			{
				fgets(pLine,maxString,fp);
				imgInfo.fileName=pLine;
				imgInfo.useless=0;
				m_prjData.images.push_back(imgInfo);
			}
		}
		else if(strstr(pLine, "[NUM OF USELESS IMAGE]"))
		{
			fscanf(fp, "%d\n", &uselessNum);
			for(i=0; i<uselessNum; i++)
			{
				fscanf(fp, "%d\n", &uselessID);
				m_prjData.images[uselessID].useless=1;
			}
		}
		else if(strstr(pLine, "[Image Adjacency Matrix]"))
		{
			if(imgNum>0)
			{
				m_ImgOverlapM=new bool[imgNum*imgNum];
				for(i=0; i<imgNum; i++)
				{
					for(j=0; j<imgNum; j++)
					{
						fscanf(fp, "%d ", &(m_ImgOverlapM[i*imgNum+j]));
					}
					fscanf(fp, "\n");
				}
			}
			
		}
	}

	if(fp)
		fclose(fp);

	return	true;
}

bool CImgBlockNet::OutputExtOrFile_aop()
{
	FILE	*fp=0;
//	double ang2rad;
	int imgNum;
	int i, j, k, pos;
	CString aopName;
	double	minHei, maxHei, averHei;

	minHei=m_pBlockPrj->GetMinGrdElev();
	maxHei=m_pBlockPrj->GetMaxGrdElev();
	averHei=m_pBlockPrj->GetAverGrdElev();

	imgNum=m_prjData.images.size();
	for (i=0; i<imgNum; i++)
	{
		//if(m_prjData.images[i].useless)	//只导出区域网中的像片
		//	continue;

		pos=m_prjData.images[i].fileName.ReverseFind( '.' );
		
		aopName = m_prjData.images[i].fileName.Left( pos );
		aopName += ".aop.txt";
		
		fp=0;
		fp=fopen(aopName.GetBuffer(256), "wt");
		if(fp==0)
			return	false;
		
		fprintf(fp,  "[Xs,Ys,Zs]\n");
		fprintf(fp,  "%13.3lf %13.3lf %10.3lf\n\n", m_prjData.images[i].extOr.Xs,
			m_prjData.images[i].extOr.Ys, m_prjData.images[i].extOr.Zs );
		
		fprintf(fp,  "[Rotation Angle]\n");
		fprintf(fp,  "%13.7lf %13.7lf %10.7lf\n\n", m_prjData.images[i].extOr.phi, 
			m_prjData.images[i].extOr.omega, m_prjData.images[i].extOr.kappa );
		
// 		fprintf(fp,  "[Rotation Sys]\n");
// 		if(m_RotateSys==RotateSys_XYZ)
// 			fprintf(fp,  "RotateSys_XYZ\n\n" );
// 		else if(m_RotateSys==RotateSys_YXZ)
// 			fprintf(fp,  "RotateSys_YXZ\n\n" );
// 		else if(m_RotateSys==RotateSys_ZYZ)
// 			fprintf(fp,  "RotateSys_ZYZ\n\n" );
		
//		CalRotateMatrix();
		fprintf(fp,  "[Rotation Matrix]\n"  );
		for( j=0; j<3; j++) 
		{
			for( k=0; k<3; k++)
			{
				fprintf(fp,  "%10.7lf ", m_prjData.images[i].extOr.R[j*3+k] );
			}
			fprintf(fp, "\n");
		}
		
		fprintf(fp,  "\n[Elevation Range]\n"  );
		fprintf(fp,  "%.1f %.1f %.1f\n", minHei, averHei, maxHei);
		
		fclose(fp);

	}
	
	return	true;
}

//修改iop文件格式，增加相机参数
bool CImgBlockNet::OutputIntOrFile_iop()
{
	FILE	*fp=0;
//	double inorImage_a[3];	// pixel to image
//	double inorImage_b[3];
//	double inorPixel_a[3];	// image to pixel
//	double inorPixel_b[3];
	int imgNum, i;
	int pos;
	CString	iopName;

	imgNum=m_prjData.images.size();
	for(i=0; i<imgNum; i++)
	{
		pos = m_prjData.images[i].fileName.ReverseFind( '.' );
		
		iopName = m_prjData.images[i].fileName.Left( pos );
		iopName += ".iop.txt";

		fp=0;
		fp=fopen(iopName.GetBuffer(256), "wt");
		if(fp==0)
			return	false;
		
//		fprintf( fp, "[Camera Calibration File]\n" );
//		fprintf( fp, "KMRMK.cmr.txt\n\n" );
		
		fprintf( fp, "[Image to Pixel Parameters]\n" );
		fprintf( fp, "%e\t%e\t%e\n", m_prjData.images[i].intOr.inorPixel_a[0], 
			m_prjData.images[i].intOr.inorPixel_a[1], 
			m_prjData.images[i].intOr.inorPixel_a[2] );
		fprintf( fp, "%e\t%e\t%e\n", m_prjData.images[i].intOr.inorPixel_b[0], 
			m_prjData.images[i].intOr.inorPixel_b[1], 
			m_prjData.images[i].intOr.inorPixel_b[2] );
		
		fprintf( fp, "\n[Pixel to Image Parameters]\n" );
		fprintf( fp, "%e\t%e\t%e\n", m_prjData.images[i].intOr.inorImage_a[0], 
			m_prjData.images[i].intOr.inorImage_a[1], 
			m_prjData.images[i].intOr.inorImage_a[2] );
		fprintf( fp, "%e\t%e\t%e\n", m_prjData.images[i].intOr.inorImage_b[0],
			m_prjData.images[i].intOr.inorImage_b[1],
			m_prjData.images[i].intOr.inorImage_b[2] );

		fprintf( fp, "\n[x0, y0]\n");
		fprintf(fp, "%.6lf %.6lf\n\n", m_prjData.images[i].intOr.x0, m_prjData.images[i].intOr.y0);
		fprintf(fp, "[focal length]\n");
		fprintf(fp, "%.6lf\n\n", m_prjData.images[i].intOr.f);
		fprintf(fp, "[radial distortion: k1,k2,k3]\n");
		fprintf(fp, "%e\t%e\t%e\n\n", m_prjData.images[i].intOr.k1, 
			m_prjData.images[i].intOr.k2, 
			m_prjData.images[i].intOr.k3);
		fprintf(fp, "[tangential distortion: p1,p2]\n");
		fprintf(fp, "%e\t%e\n\n", m_prjData.images[i].intOr.p1, 
			m_prjData.images[i].intOr.p2);
		
		fclose(fp);
	}
	
	return	true;	
}

bool CImgBlockNet::LoadExtOrFile_aop()
{
	return	true;
}

bool CImgBlockNet::LoadIntOrFile_iop()
{	
	return	true;
}


//对所有影像检测sift特征: 放在影像同级目录下
//特征文件后缀: .sift.txt(文本格式)		.sift.bin(二进制格式)
bool CImgBlockNet::DetectSIFT(char *option)
{	
	int	imgNum;
	int	i, pos;
	CString	strImgName;
	CString	siftName;

	orsIPlatform*  pPlatform = getPlatform();
	
	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, pPlatform->getService(ORS_SERVICE_IMAGE) );
	
	ref_ptr<orsIImageSourceReader> imgReader;
	
	//创建一个orsIImageSourceReader对象
	orsIRegisterService *registerService = getPlatform()->getRegisterService();
	
	ref_ptr<orsIProcessMsgBar> process;
	
	imgReader = ORS_CREATE_OBJECT( orsIImageSourceReader, "ors.dataSource.image.reader.gdal");
	if( imgReader == NULL)
	{
		AfxMessageBox("不能获得orsIImageSourceReader!");
		return false;
	}

	CSIFT	_sift;
	_sift.Startup(option);

	imgNum=m_prjData.images.size();
	process = ORS_CREATE_OBJECT( orsIProcessMsgBar, "ors.process.bar.mfc");
	process->InitProgressBar("提取Sift特征...", "wait...", imgNum);
	for(i=0; i<imgNum; i++)
	{
		strImgName=m_prjData.images[i].fileName;
		imgReader->open(strImgName.GetBuffer(128));
		
		if(_sift.RunSift(imgReader.get())==false)
			return	false;

		pos=m_prjData.images[i].fileName.ReverseFind( '.' );
		
		siftName = m_prjData.images[i].fileName.Left( pos );
		siftName += ".sift.txt";
		_sift.SaveSift_txt(siftName.GetBuffer(128));

		imgReader->close();
		process->SetPos(i+1);
	}
	
	return	true;
}

//pairwise = true:  每两张像片之间进行匹配
//pairwise = false: 安装邻接关系进行匹配
//默认按照邻接关系匹配
bool CImgBlockNet::ANNMatch_SIFT(float ratio /*= 0.6*/, bool pairwise /* = false */)
{
//	char *list_in;
//    char *file_out;
//    double ratio;
    int i, j;
	int	imgNum;
	FILE	*fp=0;
	CString	strMatchFile;
	
	strMatchFile=m_pBlockPrj->GetRawImageDir();
	strMatchFile+="\\annMatches.txt";

	m_strANNMatchFile=strMatchFile;

	fp=fopen(strMatchFile.GetBuffer(256), "w");
	if(fp==0)
	{
		CString str;
		str.Format("不能创建%s", strMatchFile);
		AfxMessageBox(str);
		return	false;
	}
	
	orsIPlatform*  pPlatform = getPlatform();
	ref_ptr<orsIProcessMsgBar> process;
    
    
	imgNum=m_prjData.images.size();
	
	
	unsigned char **keys;
    int *num_keys;
	
    /* Read the list of files */
    keys = new unsigned char *[imgNum];
    num_keys = new int[imgNum];
	
	process->InitProgressBar("匹配特征点...", "wait...", imgNum);
    /* Read all keys */
    for (i = 0; i < imgNum; i++) 
	{
        keys[i] = NULL;
        num_keys[i] = ReadKeyFile(m_prjData.images[i].fileName.GetBuffer(256), keys+i);
    }
	
	if(pairwise)
	{//每两张像片之间都做匹配
		for (i = 0; i < imgNum; i++) 
		{
			if (num_keys[i] == 0)
				continue;
			
			/* Create a tree from the keys */
			ANNkd_tree *tree = CreateSearchTree(num_keys[i], keys[i]);
			
			for (j = 0; j < i; j++)
			{
				if (num_keys[j] == 0)
					continue;
				
				/* Compute likely matches between two sets of keypoints */
				std::vector<KeypointMatch> matches = 
					MatchKeys(num_keys[j], keys[j], tree, ratio);
				
				int num_matches = (int) matches.size();
				
				if (num_matches >= 16/*0*/) 
				{
					/* Write the pair */
					fprintf(fp, "%d %d\n", j, i);
					
					/* Write the number of matches */
					fprintf(fp, "%d\n", (int) matches.size());
					
					for (int i = 0; i < num_matches; i++) 
					{
						fprintf(fp, "%d %d\n", 
							matches[i].m_idx1, matches[i].m_idx2);
					}
				}
			}
			
			// annDeallocPts(tree->pts);
			delete tree;
			
			process->SetPos(i+1);
		}
	}
	else
	{//按邻接关系进行匹配
		for (i = 0; i < imgNum; i++) 
		{
			if (num_keys[i] == 0)
				continue;
			
			/* Create a tree from the keys */
			ANNkd_tree *tree = CreateSearchTree(num_keys[i], keys[i]);
			
			for (j = 0; j < i; j++)
			{
				if (num_keys[j] == 0)
					continue;

				if(m_ImgOverlapM[i*imgNum+j])
				{
					/* Compute likely matches between two sets of keypoints */
					std::vector<KeypointMatch> matches = 
						MatchKeys(num_keys[j], keys[j], tree, ratio);
					
					int num_matches = (int) matches.size();
					
					if (num_matches >= 16/*0*/) 
					{
						/* Write the pair */
						fprintf(fp, "%d %d\n", j, i);
						
						/* Write the number of matches */
						fprintf(fp, "%d\n", (int) matches.size());
						
						for (int i = 0; i < num_matches; i++) 
						{
							fprintf(fp, "%d %d\n", 
								matches[i].m_idx1, matches[i].m_idx2);
						}
					}
				}	
			}
			
			// annDeallocPts(tree->pts);
			delete tree;
			
			process->SetPos(i+1);
		}
	}
    
    
    /* Free keypoints */
    for (i = 0; i < imgNum; i++) 
	{
        if (keys[i] != NULL)
            delete [] keys[i];
    }
    delete [] keys;
    delete [] num_keys;
    
	if(fp)
		fclose(fp);
    return true;
}

#include "bundler\BundlerExport.h"
//#pragma   commemt(lib,   "BundlerDLL.lib") 
bool CImgBlockNet::ExtimateFMatrix()
{
// 	double	n;
// 
// 	n=sum(10,11);
// 
 	return true;
}