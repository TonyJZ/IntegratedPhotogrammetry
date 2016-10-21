#include "StdAfx.h"
#include "ImageMatchAT/featureMatch.h"

#include "orsBase/orsTypedef.h"
#include "orsBase/orsArray.h"
#include "orsBase/orsString.h"

#include "orsBase/orsIExe.h"
#include "orsBase/orsUtil.h"
#include "orsBase/orsIUtilityService.h"
#include "orsImage/orsIImageService.h"
#include "orsGuiBase/orsIProcessMsgBar.h"

#include "orsFeature2D\orsMatchLinks.h"
#include "orsGeometry\orsPointPairSet.h"
#include "orsFeature2d\orsIAlg2DFeatureRegistering.h"

//ransac
#include "RansacEstimate/ransac_affine.h"
#include "RansacEstimate/ransac_epipolar.h"

#include "ATNPointIndex.h"

#include "flann/flann.h"

//ptPairVec: 外部传入的匹配数组
//maxLen: 匹配数组的最大长度
//matchNum: 匹配点数
bool KeyMatch_images(const char *imgName1, const char *imgName2, 
	std::vector<orsAdjPtPAIR> *ptPairVec, int &matchNum, int zoom)
{

	ors_int32 blkSizeX = 512, blkSizeY = 512;		//提点时的分块大小
	ors_int32 gridNumX = 0, gridNumY = 0;
	ors_int32 gridSizeX = 1024, gridSizeY = 1024;	//匹配时的分块大小

	//	my2DPtPair *ptPairVec=NULL;
//	int maxPairNum=0;

	char ovLevel[4];
	_itoa(zoom, ovLevel, 10);

	// 2，根据缩略图的特征点计算不同影像的，计算不同影像的变换关系，
	//	根据下视的ROI，匹配同名点（特征点已经提取了）

	orsPtPairsAffinePARA pairAffine;

	// 匹配算法
	ref_ptr<orsIAlg2DFeatureRegistering> keyMatch;

	keyMatch = ORS_CREATE_OBJECT( orsIAlg2DFeatureRegistering, ORS_ALG_2DFEATURE_REGISTER_DEFAULT );

	if( NULL == keyMatch.get() )
		return NULL;

//	keyMatch->setThresholds( 0.75, 0.65, 5, 15 );


	ref_ptr<orsI2DFeatureSet> featureSet0_ov = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );
	ref_ptr<orsI2DFeatureSet> featureSet1_ov = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );

	ref_ptr<orsI2DFeatureSet> featureSet0 = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );
	ref_ptr<orsI2DFeatureSet> featureSet1 = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );

	ref_ptr<orsIAlg2DFeatureRegistering> blkKeyMatch;
	blkKeyMatch = ORS_CREATE_OBJECT( orsIAlg2DFeatureRegistering, ORS_ALG_2DFEATURE_REGISTER_DEFAULT );
	if( NULL == blkKeyMatch.get() )	{
		getPlatform()->logPrint( ORS_LOG_FATAL, "Can not create %s", ORS_ALG_2DFEATURE_REGISTER_DEFAULT );
		return false;
	}

	ref_ptr<orsIAlg2DPointPairsCheck> keyCheck;
	keyCheck = ORS_CREATE_OBJECT( orsIAlg2DPointPairsCheck, ORS_ALG_2DPOINTPAIR_CHECK_DEFAULT );
	if( NULL == keyCheck.get() )	{
		getPlatform()->logPrint( ORS_LOG_FATAL, "Can not create %s", ORS_ALG_2DPOINTPAIR_CHECK_DEFAULT );
		return false;
	}


	int refWid, refHei;

	{
		ref_ptr<orsIImageSource> refImg = getImageService()->openImageFile( imgName1 );

		refWid = refImg->getWidth();
		refHei = refImg->getHeight();
	}

	ptPairVec->clear();
	orsAdjPtPAIR *pPairs=NULL;
	orsMatchedFtPAIR *pMtPairs=NULL;
	

	//匹配缩略图
	if(zoom > 1)
	{
		//orsString siftFileName_ov = vStripFiles[iStrip] + _T("_zoomOut16.sift.bin");
		orsString siftFileName_ov = imgName1;
		siftFileName_ov = siftFileName_ov + _T("_zoomOut") + ovLevel + _T(".sift.bin");
		// 读入特征点文件
		featureSet0_ov->load( siftFileName_ov );

		if( !keyMatch->setDstFeatureSet( featureSet0_ov.get() ) )
			return false;
	}

	//////////////////////////////////////////////////////////////////////////


	orsString siftFileName = imgName1;
	siftFileName += _T(".sift.bin");

	featureSet0->load( siftFileName );

	//////////////////////////////////////////////////////////////////////////
	int n;
	//估算缩略图的变换参数
	if(zoom > 1)
	{
		orsString siftFileName1_ov = imgName2;
		siftFileName1_ov = siftFileName1_ov + _T("_zoomOut") + ovLevel + _T(".sift.bin");

		// 读入特征点文件
		featureSet1_ov->load( siftFileName1_ov );

		pPairs = keyMatch->findMatchedPairs( featureSet1_ov.get(), &n );
		pMtPairs = keyCheck->check_2DTransform( pPairs, &n, ORS_GEOMETRY_TRANSFORM2D_AFFINE, 3 );
		orsPointPairSet ptPairSet;

		if( n > 7 )	{
			ptPairSet.alloc( n );

			orsKeyPOINT keyPoint;

			int i;
			for( i=0; i<n; i++ )
			{
				featureSet0_ov->getFetureVector( pMtPairs[i].dstFtId, &keyPoint, NULL );

				ptPairSet.m_ptPairs[i].x0 = keyPoint.x;
				ptPairSet.m_ptPairs[i].y0 = keyPoint.y;

				featureSet1_ov->getFetureVector( pMtPairs[i].srcFtId, &keyPoint, NULL );

				ptPairSet.m_ptPairs[i].x1 = keyPoint.x;
				ptPairSet.m_ptPairs[i].y1 = keyPoint.y;
			}

			ptPairSet.getAffineTransorm( &pairAffine );

			printf("overView, a = %8.1lf %8.4lf %8.4lf\n", pairAffine.a[0], pairAffine.a[1], pairAffine.a[2] );
			printf("overView, b = %8.1lf %8.4lf %8.4lf\n", pairAffine.b[0], pairAffine.b[1], pairAffine.b[2] );

		}
		else
		{//缩略图匹配失败
			pairAffine.a[0]=0.0;	pairAffine.a[1]=1.0;	pairAffine.a[2]=0.0;
			pairAffine.b[0]=0.0;	pairAffine.b[1]=0.0;	pairAffine.b[2]=1.0;
		}
	}
	else
	{//不做变换
		pairAffine.a[0]=0.0;	pairAffine.a[1]=1.0;	pairAffine.a[2]=0.0;
		pairAffine.b[0]=0.0;	pairAffine.b[1]=0.0;	pairAffine.b[2]=1.0;
	}
	// 相对关系确定完毕

	///////////////////////////////////////////////
	orsString siftFileName1 = imgName2;
	siftFileName1 += _T(".sift.bin");

	// 读入特征点文件

	featureSet1->load( siftFileName1 );

	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 按下视的点位设置，匹配同名点

	// 匹配算法


	// 尺度差异阈值，方向差异阈值
//	blkKeyMatch->setThresholds( 0.75, 0.65, 1.5, 15 );

	/////// 分块匹配
	if( /*bGridSizeSet*/true )	{
		gridNumX = refWid / gridSizeX;
		gridNumY = refHei / gridSizeY;
	}
	else	{
		gridSizeX = refWid / gridNumX;
		gridSizeY = refHei / gridNumY;
	}


	int iRow, iCol;
	int iBlkX, iBlkY;

//	std::vector<orsMatchedFtPAIR> initMatchPair;

	for( iBlkY=0; iBlkY<gridNumY; iBlkY++)
	{
		iRow = iBlkY*gridSizeY + gridSizeY/2 - blkSizeY/2;

		assert( iRow >=0 );

		for( iBlkX=0; iBlkX<gridNumX; iBlkX++)
		{
			iCol = iBlkX*gridSizeX + gridSizeX/2 - blkSizeX/2;

			assert( iCol >= 0 );

			orsRect_i rect0( iCol, iRow, iCol+blkSizeX, iRow+blkSizeY );					

			// 主影像，dst
			if( !blkKeyMatch->setDstFeatureSet( featureSet0.get(), &rect0 ) )
				continue;

			//////////////////////////////////////////////////////////////////////////
			orsRect_i rect1;

			{
				int x[4], y[4];

				x[0] = iCol;	y[0] = iRow;
				x[1] = iCol+blkSizeX;	y[1] = iRow;
				x[2] = iCol+blkSizeX;	y[2] = iRow + blkSizeY;
				x[3] = iCol;	y[3] = iRow + blkSizeY;

				int xf, yf, xfMin=999999, yfMin=999999, xfMax=-999999, yfMax=-999999;

				double *a = pairAffine.a;
				double *b = pairAffine.b;

				int i; 
				for( i=0; i<4; i++ )
				{
					xf = a[0] + a[1]*x[i] + a[2]*y[i];
					yf = b[0] + b[1]*x[i] + b[2]*y[i];

					if( xfMin > xf )	xfMin = xf;
					if( yfMin > yf )	yfMin = yf;

					if( xfMax < xf )	xfMax = xf;
					if( yfMax < yf )	yfMax = yf;
				}

				rect1.m_xmin = xfMin;	rect1.m_xmax = xfMax;
				rect1.m_ymin = yfMin;	rect1.m_ymax = yfMax;
			}								

			//////////////////////////////////////////////////////////////////////////
			pPairs = blkKeyMatch->findMatchedPairs( featureSet1.get(), &n, &rect1 );

			int k;
			for( k=0; k<n; k++ )
			{
				ptPairVec->push_back(pPairs[k]);
			}
		}
	}

	matchNum = ptPairVec->size();

	//////////////////////////////////////////////////////////////////////////
	return true;
}

bool KeyMatch_Lidarimage(const char *atnName, const char *intImgName, const char *optImgName, 
	myPtPair **ptPairVec, int &maxLen, int &matchNum, int zoom, int minOverlap)
{
	CATNPtIndex	index(getPlatform());

	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, getPlatform()->getService(ORS_SERVICE_IMAGE) );

	ref_ptr<orsIImageSourceReader> atnImg, optImg;
	orsIImageGeometry *pAtnGeo=NULL, *pOptGeo=NULL;

	atnImg = imageService->openImageFile(intImgName);
	pAtnGeo = atnImg->GetImageGeometry();

	optImg = imageService->openImageFile(optImgName);
	pOptGeo = optImg->GetImageGeometry();

	int imgWid = optImg->getWidth();
	int imgHei = optImg->getHeight();

	index.Open(atnName);

	ref_ptr<orsI2DFeatureSet> featureAtnSet = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );
	ref_ptr<orsI2DFeatureSet> featureOptSet = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );

	//读入特征点文件
	orsString siftFileName1 = intImgName;
	siftFileName1 += _T(".sift.bin");
	featureAtnSet->load( siftFileName1 );

	orsString siftFileName2 = optImgName;
	siftFileName2 += _T(".sift.bin");
	featureOptSet->load( siftFileName2 );

	//先找出最邻近的候选
	FLANNParameters flannPara;
	FLANN_INDEX flannIndex;

	int optkeyNum = featureOptSet->getNumOfFeatures();
	orsKeyPOINT keyPoints;
	
	float *dataset=NULL;
	int cols = 2;  //空间距离做判据

	dataset = new float[optkeyNum*cols];

	int i, j;
	for(i=0; i<optkeyNum; i++)
	{
		featureOptSet->getFetureVector( i, &keyPoints);
		dataset[i*cols] = keyPoints.x;
		dataset[i*cols+1] = keyPoints.y;
	}

	int tcount = 1;
	int nn = 5;	//最近邻的数量
	int* result = new int[tcount*nn];
	float* dists = new float[tcount*nn];
	float *testset = new float[tcount*cols];
	double distTh = 8;
	std::vector<myPtPair> initPair;

	flannPara.log_level = LOG_INFO;
	flannPara.log_destination = NULL;

	flannPara.algorithm = KDTREE;
	flannPara.checks = 32;
	flannPara.trees = 8;
	flannPara.branching = 32;
	flannPara.iterations = 7;
	flannPara.target_precision = -1;

	float speedup;

	flannIndex = flann_build_index(dataset, optkeyNum, cols, &speedup, &flannPara);	//kdTree index
	
	double avHei = pOptGeo->GetMeanZ();
// 	int imgHei = optImg->getHeight();
// 	int imgWid = optImg->getWidth();
	double dX[4], dY[4];

	//顺时针方向
	pOptGeo->IntersectWithZ(0, 0, avHei, dX, dY);
	pOptGeo->IntersectWithZ(imgWid, 0, avHei, dX+1, dY+1);
	pOptGeo->IntersectWithZ(imgWid, imgHei, avHei, dX+2, dY+2);
	pOptGeo->IntersectWithZ(0, imgHei, avHei, dX+3, dY+3);

	orsRect_i roi;
	double xi, yi;
	roi.m_xmin = roi.m_ymin = 10000000;
	roi.m_xmax = roi.m_ymax = -10000000;
	for(i=0; i<4; i++)
	{
		pAtnGeo->Project(dX[i], dY[i], avHei, &xi, &yi);
		if(roi.m_xmin > xi)
			roi.m_xmin = floor(xi);
		if(roi.m_xmax < xi)
			roi.m_xmax = ceil(xi);
		if(roi.m_ymin > yi)
			roi.m_ymin = floor(yi);
		if(roi.m_ymax < yi)
			roi.m_ymax = ceil(yi);
	}
	
	int atnKeyNum = featureAtnSet->getNumOfFeatures();
	for(i=0; i<atnKeyNum; i++)
	{
		featureAtnSet->getFetureVector( i, &keyPoints);
		if( !roi.PtInRect( keyPoints.x, keyPoints.y ) )
		{
			continue;
		}

		POINT3D objPt;
		orsPOINT2D imgPt;
		pAtnGeo->IntersectWithZ(keyPoints.x, keyPoints.y, 0, &(objPt.X), &(objPt.Y));
		std::vector<ATNPoint> ptVec;
		double rms;
		index.GetTriangleVertex(&objPt, 5, &ptVec, &rms);

		if(ptVec.size() < 3)
			continue;

		pOptGeo->Project(objPt.X, objPt.Y, objPt.Z, &(imgPt.x), &(imgPt.y)); //投影到像方
 
		//剔除图像范围外的点
		if(imgPt.x < 0 || imgPt.x > imgWid || imgPt.y < 0 || imgPt.y > imgHei)
			continue;

		testset[0] = imgPt.x;
		testset[1] = imgPt.y;
		flann_find_nearest_neighbors_index(flannIndex, testset, tcount, result, dists, nn, 
			flannPara.checks, &flannPara);

		double minDis=10000;
		int bestFit = -1;

		for(j=0; j<nn; j++)
		{//未加判断相似性
			if(dists[j]<minDis)
			{
				minDis = dists[j];
				bestFit = j;
			}
		}

		if(minDis < 200)
		{
			orsKeyPOINT dstKey;
			featureOptSet->getFetureVector(result[bestFit], &dstKey);

			myPtPair pair;
			pair.ptId0 = i;
			pair.xi0 = objPt.X;
			pair.yi0 = objPt.Y;
			pair.zi0 = objPt.Z;

// 			pair.xi1 = dataset[bestFit*cols];
// 			pair.yi1 = dataset[bestFit*cols+1];
			pair.ptId1 = result[bestFit];
			pair.xi1 = dstKey.x;
			pair.yi1 = dstKey.y;
			pair.zi1 = 0;

			initPair.push_back(pair);
		}
	}

	matchNum = initPair.size();
	if(matchNum>maxLen)
	{
		if(*ptPairVec)
		{
			delete[] *ptPairVec;
		}
		*ptPairVec = new myPtPair[matchNum];
		maxLen = matchNum;
	}

	for(i=0; i<matchNum; i++)
	{
		(*ptPairVec)[i] = initPair[i];
	}



	if(dataset)	delete[] dataset;	dataset=NULL;
	if(testset)	delete[] testset;	testset=NULL;
	if(result)	delete[] result;	result=NULL;
	if(dists)	delete[] dists;		dists=NULL;
	return true;
}


bool KeyMatch_Lidarimage_SIFT(const char *atnName, const char *intImgName, const char *optImgName, 
	myPtPair **ptPairVec, int &maxLen, int &matchNum, int zoom, int minOverlap)
{
	CATNPtIndex	index(getPlatform());

	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, getPlatform()->getService(ORS_SERVICE_IMAGE) );

	ref_ptr<orsIImageSourceReader> atnImg, optImg;
	orsIImageGeometry *pAtnGeo=NULL, *pOptGeo=NULL;

	atnImg = imageService->openImageFile(intImgName);
	pAtnGeo = atnImg->GetImageGeometry();

	optImg = imageService->openImageFile(optImgName);
	pOptGeo = optImg->GetImageGeometry();

	int imgWid = optImg->getWidth();
	int imgHei = optImg->getHeight();

	index.Open(atnName);

	ref_ptr<orsI2DFeatureSet> featureAtnSet = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );
	ref_ptr<orsI2DFeatureSet> featureOptSet = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );

	//读入特征点文件
	orsString siftFileName1 = intImgName;
	siftFileName1 += _T(".sift.bin");
	featureAtnSet->load( siftFileName1 );

	orsString siftFileName2 = optImgName;
	siftFileName2 += _T(".sift.bin");
	featureOptSet->load( siftFileName2 );

	//先找出最邻近的候选
	FLANNParameters flannPara;
	FLANN_INDEX flannIndex;

	int optkeyNum = featureOptSet->getNumOfFeatures();
	orsKeyPOINT keyPoints;

	const float *dataset=NULL;

	int rows = featureOptSet->getNumOfFeatures();
	int cols = featureOptSet->getDimesionOfFeatureVectore();

	dataset = featureOptSet->getDescsMatrix();

	int i, j;
// 	for(i=0; i<optkeyNum; i++)
// 	{
// 		featureOptSet->getFetureVector( i, &keyPoints);
// 		dataset[i*cols] = keyPoints.x;
// 		dataset[i*cols+1] = keyPoints.y;
// 	}

	int tcount = 1;
	int nn = 2;	//最近邻的数量
	int* result = new int[tcount*nn];
	float* dists = new float[tcount*nn];
	float *testset = NULL;
	double distTh = 8;
	std::vector<myPtPair> initPair;

	flannPara.log_level = LOG_INFO;
	flannPara.log_destination = NULL;

	flannPara.algorithm = KDTREE;
	flannPara.checks = 32;
	flannPara.trees = 8;
	flannPara.branching = 32;
	flannPara.iterations = 7;
	flannPara.target_precision = -1;

	float speedup;

	flannIndex = flann_build_index((float*)dataset, rows, cols, &speedup, &flannPara);	//kdTree index

	double avHei = pOptGeo->GetMeanZ();
	// 	int imgHei = optImg->getHeight();
	// 	int imgWid = optImg->getWidth();
	double dX[4], dY[4];

	//顺时针方向
	pOptGeo->IntersectWithZ(0, 0, avHei, dX, dY);
	pOptGeo->IntersectWithZ(imgWid, 0, avHei, dX+1, dY+1);
	pOptGeo->IntersectWithZ(imgWid, imgHei, avHei, dX+2, dY+2);
	pOptGeo->IntersectWithZ(0, imgHei, avHei, dX+3, dY+3);

	orsRect_i roi;
	double xi, yi;
	roi.m_xmin = roi.m_ymin = 10000000;
	roi.m_xmax = roi.m_ymax = -10000000;
	for(i=0; i<4; i++)
	{
		pAtnGeo->Project(dX[i], dY[i], avHei, &xi, &yi);
		if(roi.m_xmin > xi)
			roi.m_xmin = floor(xi);
		if(roi.m_xmax < xi)
			roi.m_xmax = ceil(xi);
		if(roi.m_ymin > yi)
			roi.m_ymin = floor(yi);
		if(roi.m_ymax < yi)
			roi.m_ymax = ceil(yi);
	}

	const float *pAtnDesc=NULL;
	pAtnDesc = featureAtnSet->getDescsMatrix();
	int atnKeyNum = featureAtnSet->getNumOfFeatures();
	int ptNuminImage=0;
	for(i=0; i<atnKeyNum; i++)
	{
		featureAtnSet->getFetureVector( i, &keyPoints);
		if( !roi.PtInRect( keyPoints.x, keyPoints.y ) )
		{
			continue;
		}

		POINT3D objPt;
		orsPOINT2D imgPt;
		pAtnGeo->IntersectWithZ(keyPoints.x, keyPoints.y, 0, &(objPt.X), &(objPt.Y));
		std::vector<ATNPoint> ptVec;
		double rms;
		index.GetTriangleVertex(&objPt, 5, &ptVec, &rms);

		if(ptVec.size() < 3)
			continue;

		pOptGeo->Project(objPt.X, objPt.Y, avHei, &(imgPt.x), &(imgPt.y)); //投影到像方

		//剔除图像范围外的点
		if(imgPt.x < 0 || imgPt.x > imgWid || imgPt.y < 0 || imgPt.y > imgHei)
			continue;

		ptNuminImage++;

		testset = (float*)pAtnDesc + i*cols;
		flann_find_nearest_neighbors_index(flannIndex, testset, tcount, result, dists, nn, 
			flannPara.checks, &flannPara);

// 		orsKeyPOINT dstKey;
// 		featureOptSet->getFetureVector(result[0], &dstKey);

		if( *dists < 0.8* dists[1]/* || *dists > 0.5 */)
		{
			orsKeyPOINT dstKey;
			featureOptSet->getFetureVector(result[0], &dstKey);

			//空间距离
			double sDis = sqrt((imgPt.x - dstKey.x)*(imgPt.x - dstKey.x)+(imgPt.y - dstKey.y)*(imgPt.y - dstKey.y));
// 			if(sDis > 200)
// 				continue;

			myPtPair pair;
			pair.ptId0 = i;
			pair.xi0 = objPt.X;
			pair.yi0 = objPt.Y;
			pair.zi0 = objPt.Z;

			// 			pair.xi1 = dataset[bestFit*cols];
			// 			pair.yi1 = dataset[bestFit*cols+1];
			pair.ptId1 = result[0];
			pair.xi1 = dstKey.x;
			pair.yi1 = dstKey.y;
			pair.zi1 = 0;

			pair.vx = imgPt.x - dstKey.x;
			pair.vy = imgPt.y - dstKey.y;

			initPair.push_back(pair);
		}
	}

	matchNum = initPair.size();
	if(matchNum>maxLen)
	{
		if(*ptPairVec)
		{
			delete[] *ptPairVec;
		}
		*ptPairVec = new myPtPair[matchNum];
		maxLen = matchNum;
	}

	for(i=0; i<matchNum; i++)
	{
		(*ptPairVec)[i] = initPair[i];
	}



// 	if(dataset)	delete[] dataset;	dataset=NULL;
// 	if(testset)	delete[] testset;	testset=NULL;
	if(result)	delete[] result;	result=NULL;
	if(dists)	delete[] dists;		dists=NULL;
	return true;
}