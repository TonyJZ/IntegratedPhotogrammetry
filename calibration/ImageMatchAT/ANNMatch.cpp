#include "StdAfx.h"
#include "ImageMatchAT\ANNMatch.h"

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


bool KeyMatch_pairwise_ransac(const char *FileName, const char *atTiePtsFileName, 
	_iphCamera *pCamera, const char *imgKeyDir, int zoom, int minOverlap)
{
	ors_int32 blkSizeX = 512, blkSizeY = 512;		//提点时的分块大小
	ors_int32 gridNumX = 0, gridNumY = 0;
	ors_int32 gridSizeX = 1024, gridSizeY = 1024;	//匹配时的分块大小

	myPtPair *ptPairVec=NULL;
	int maxPairNum=0;

	FILE *fp=NULL;
	orsArray<orsString> vStripFiles;

	fp=fopen(FileName, "r");
	if(fp==NULL)
		return false;

	char pLine[1024];
	while(fgets(pLine, 1024, fp))
	{
		if (pLine[strlen(pLine) - 1] == '\n')
			pLine[strlen(pLine) - 1] = 0;

//		fscanf(fp, "%s", pLine);
		if(pLine[0] == 0)
			continue;

		vStripFiles.push_back(orsString(pLine));
	}

	fclose(fp);

	char ovLevel[4];
	_itoa(zoom, ovLevel, 10);
	
	// 2，根据缩略图的特征点计算不同影像的，计算不同影像的变换关系，
	//	根据下视的ROI，匹配同名点（特征点已经提取了）
	orsMatchLinks	mtLinkMap;
	int iStrip;
	{
		orsPtPairsAffinePARA pairAffine;

		// 匹配算法
		ref_ptr<orsIAlg2DFeatureRegistering> keyMatch;

		keyMatch = ORS_CREATE_OBJECT( orsIAlg2DFeatureRegistering, ORS_ALG_2DFEATURE_REGISTER_DEFAULT );

		if( NULL == keyMatch.get() )
			return NULL;

//		keyMatch->setThresholds( 0.65, 0.65, 5, 15 );


		ref_ptr<orsI2DFeatureSet> featureSet0_ov = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );
		ref_ptr<orsI2DFeatureSet> featureSet1_ov = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );

		ref_ptr<orsI2DFeatureSet> featureSet0 = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );
		ref_ptr<orsI2DFeatureSet> featureSet1 = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );

		ref_ptr<orsIAlg2DFeatureRegistering> blkKeyMatch;

		blkKeyMatch = ORS_CREATE_OBJECT( orsIAlg2DFeatureRegistering, ORS_ALG_2DFEATURE_REGISTER_DEFAULT );

		//pairwise match
		for( iStrip=0; iStrip < vStripFiles.size()-1; iStrip++ )
		{			
			int refWid, refHei;

			{
				ref_ptr<orsIImageSource> refImg = getImageService()->openImageFile( vStripFiles[iStrip] );

				refWid = refImg->getWidth();
				refHei = refImg->getHeight();
			}

			
			//匹配缩略图
			if(zoom > 1)
			{
				//orsString siftFileName_ov = vStripFiles[iStrip] + _T("_zoomOut16.sift.bin");
				orsString siftFileName_ov = vStripFiles[iStrip] + _T("_zoomOut") + ovLevel + _T(".sift.bin");
				// 读入特征点文件
				featureSet0_ov->load( siftFileName_ov );

				if( !keyMatch->setDstFeatureSet( featureSet0_ov.get() ) )
					continue;
			}

			//////////////////////////////////////////////////////////////////////////
			

			orsString siftFileName = vStripFiles[iStrip] + _T(".sift.bin");

			featureSet0->load( siftFileName );

			//////////////////////////////////////////////////////////////////////////

			int iStrip1;
			for( iStrip1 = iStrip + 1; iStrip1 < vStripFiles.size(); iStrip1++ )
			{
// 				// 是否第一条
// 				if( iStrip1 < 0 || iStrip1 == iStrip )
// 					continue;
				int n;
				//估算缩略图的变换参数
				if(zoom > 1)
				{
					orsString siftFileName1_ov = vStripFiles[iStrip1] + _T("_zoomOut") + ovLevel + _T(".sift.bin");

					// 读入特征点文件
					featureSet1_ov->load( siftFileName1_ov );
					
					orsAdjPtPAIR *pPairs;

					pPairs = keyMatch->findMatchedPairs( featureSet1_ov.get(), &n );

					if(n < 6) //无重叠
						continue;

					if(maxPairNum < n)
					{
						if(ptPairVec)	delete	ptPairVec;
						ptPairVec = new myPtPair[n];
						maxPairNum = n;
					}

// 					orsPointPairSet ptPairSet;
// 
// 					ptPairSet.alloc( n );
// 
 					orsKeyPOINT keyPoint;

					int i;
					for( i=0; i<n; i++ )
					{
						featureSet0_ov->getFetureVector( pPairs[i].dstId, &keyPoint, NULL );

// 						ptPairSet.m_ptPairs[i].x0 = keyPoint.x;
// 						ptPairSet.m_ptPairs[i].y0 = keyPoint.y;
						ptPairVec[i].xi0 = keyPoint.x;
						ptPairVec[i].yi0 = keyPoint.y;

						featureSet1_ov->getFetureVector( pPairs[i].srcId, &keyPoint, NULL );

// 						ptPairSet.m_ptPairs[i].x1 = keyPoint.x;
// 						ptPairSet.m_ptPairs[i].y1 = keyPoint.y;
						ptPairVec[i].xi1 = keyPoint.x;
						ptPairVec[i].yi1 = keyPoint.y;
					}

					int refineNum;
					double M_affine[9];
//					EstimateTransform_Epipolar(ptPairVec, n, refineNum, pCamera, 3);

					EstimateTransform_Affine(ptPairVec, n, refineNum, 5, M_affine);

// 					ref_ptr<orsIAlg2DPointPairsCheck> pairsCheck;
// 
// 					pairsCheck = ORS_CREATE_OBJECT( orsIAlg2DPointPairsCheck, ORS_ALG_2DPOINTPAIR_CHECK_DEFAULT );
// 
// 					if( NULL == pairsCheck.get() )	{
// 						return false;
// 					}
// 					//////////////////////////////////////////////////////////////////////////
// 
// 					const orsMatchedFtPAIR *pCnd;
// 
// 					if( n > 6 ) {
// 						refineNum = n;
// 						pCnd = pairsCheck->check_2DTransform( pPairs, &refineNum, ORS_GEOMETRY_TRANSFORM2D_AFFINE, 3 );
// 					}
					
					if(refineNum < 5) //无重叠
						continue;


//					ptPairSet.getAffineTransorm( &pairAffine );	//, true );

					printf("strip(%d, %d) a = %8.1lf %8.4lf %8.4lf\n", iStrip, iStrip1, M_affine[0], M_affine[1], M_affine[2] );
					printf("strip(%d, %d) b = %8.1lf %8.4lf %8.4lf\n", iStrip, iStrip1, M_affine[3], M_affine[4], M_affine[5] );
					
					pairAffine.a[0]=M_affine[2];	pairAffine.a[1]=M_affine[0];	pairAffine.a[2]=M_affine[1];
					pairAffine.b[0]=M_affine[5];	pairAffine.b[1]=M_affine[3];	pairAffine.b[2]=M_affine[4];
				}
				else
				{//不做变换
					pairAffine.a[0]=0.0;	pairAffine.a[1]=1.0;	pairAffine.a[2]=0.0;
		
					pairAffine.b[0]=0.0;	pairAffine.b[1]=0.0;	pairAffine.b[2]=1.0;
				}
				// 相对关系确定完毕

				///////////////////////////////////////////////
				orsString siftFileName1 = vStripFiles[iStrip1] + _T(".sift.bin");

				// 读入特征点文件
				
				featureSet1->load( siftFileName1 );

				//////////////////////////////////////////////////////////////////////////
				

				//////////////////////////////////////////////////////////////////////////
				// 按下视的点位设置，匹配同名点

				// 匹配算法


				// 尺度差异阈值，方向差异阈值
				blkKeyMatch->setThresholds( 0.6, 0.65, 1.5, 15 );

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

				std::vector<orsMatchedFtPAIR> initMatchPair;

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
						orsAdjPtPAIR *pPairs;

						// 
						pPairs = blkKeyMatch->findMatchedPairs( featureSet1.get(), &n, &rect1 );
					

						int k;
						for( k=0; k<n; k++ )
						{
							orsMatchedFtPAIR mtPair;
							mtPair.dstFtId = pPairs[k].dstId;
							mtPair.srcFtId = pPairs[k].srcId;
							initMatchPair.push_back(mtPair);
						}
					}
				}

				//对整幅影像进行核线挑点
				n = initMatchPair.size();
				int refineNum = n;

				if(maxPairNum < n)
				{
					if(ptPairVec)	delete	ptPairVec;
					ptPairVec = new myPtPair[n];
					maxPairNum = n;
				}

				orsKeyPOINT dstPt, srcPt;
				for(int i=0; i<n; i++)
				{
					// iStrip
					featureSet0->getFetureVector( initMatchPair[i].dstFtId, &dstPt );

					// iStrip1
					featureSet1->getFetureVector( initMatchPair[i].srcFtId, &srcPt );

					ptPairVec[i].xi0 = dstPt.x;
					ptPairVec[i].yi0 = dstPt.y;
//					dstPt.z = 0;

					ptPairVec[i].xi1 = srcPt.x;
					ptPairVec[i].yi1 = srcPt.y;
//					srcPt.z = 0;

					ptPairVec[i].bAccepted = false;
				}

				if(pCamera)
					EstimateTransform_Epipolar(ptPairVec, n, refineNum, pCamera, 1);
				else
				{
					double affineM[9];
					EstimateTransform_Affine(ptPairVec, n, refineNum, 3, affineM) ;
				}

				if( refineNum > 6 )	{
					int k;
					for( k=0; k<n; k++ )
					{
						// iStrip
						featureSet0->getFetureVector( initMatchPair[k].dstFtId, &dstPt );

						// iStrip1
						featureSet1->getFetureVector( initMatchPair[k].srcFtId, &srcPt );

						if(ptPairVec[k].bAccepted)
							mtLinkMap.AddPtPair( iStrip1, initMatchPair[k].srcFtId, &srcPt, iStrip, initMatchPair[k].dstFtId, &dstPt );
					}
				}

			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	int count = mtLinkMap.RemoveDuplicatePoints();
	printf("%d duplicate points are removed\n", count );

	mtLinkMap.CountOverlap( vStripFiles.size() );

	mtLinkMAP &ptLinks = mtLinkMap.GetMatchLinks();

	//////////////////////////////////////////////////////////////////////////
	//orsTiePoints tiePts;

// 	orsString matchedFeatureFileName = argv[1];
// 	matchedFeatureFileName += ".tiePts.txt";
// 
// 	getPlatform()->logPrint( ORS_LOG_DEBUG, "orsSE_KeyMatch: Begin saving match links" );
// 
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	{
// 		orsString matchedFeatureFileName = argv[1];
// 		matchedFeatureFileName += ".tiePts.txt";
// 
// 		FILE *fpOut = fopen( matchedFeatureFileName, "wt");
// 
// 		mtLinkMapITER iter;
// 
// 		for( iter = ptLinks.begin(); iter != ptLinks.end(); iter++ )
// 		{
// 			if( !mtLinkMap.isVaild(iter) )
// 				continue;
// 
// 			int iImg, iPt;
// 			//orsKeyPOINT keyPt;
// 
// 			iImg = mtLinkMap.getImgNum( iter->first );
// 			iPt = mtLinkMap.getPtNum( iter->first );
// 
// 			//			features[iImg]->getFetureVector( iPt, &keyPt );
// 
// 			// srcId, dstId, link, xl, yl,xr, yr
// 
// 			fprintf( fpOut, "%ld %ld %ld %.1f %.1f %.2f %f\n", 
// 				iter->first, iter->second.prev, iter->second.next, iter->second.x, iter->second.y, iter->second.scale, iter->second.orient );				
// 		}
// 
// 		fclose( fpOut );		
// 	}
// 
// 	getPlatform()->logPrint( ORS_LOG_DEBUG, "orsSE_KeyMatch: End saving match links" );

	//////////////////////////////////////////////////////////////////////////
	// 输出空三需要的格式
	std::vector<int> Null_vec;
	OutputMatchLinks(&mtLinkMap, &vStripFiles, Null_vec, minOverlap, atTiePtsFileName);

	//////////////////////////////////////////////////////////////////////////
	// 生成各影像的连接点文件
	if(imgKeyDir != NULL)
	{
		for( iStrip=0; iStrip<vStripFiles.size(); iStrip++ )
		{
			orsString imgFile = orsString::getPureFileName(vStripFiles[iStrip]);
			
			orsString outputFile = imgKeyDir;
			outputFile = outputFile + "\\" + imgFile + _T(".tiePts.txt");

			FILE *fpOut;

			fpOut = fopen( outputFile, 	"wt");

			if( NULL != fpOut ) {
				mtLinkMapITER iter;	

				for( iter = ptLinks.begin(); iter != ptLinks.end(); iter++ )
				{
					if( !mtLinkMap.isVaild(iter) )
						continue;

					long tieNum = iter->second.count;
					int imgIndex;

					imgIndex = mtLinkMap.getImgNum( iter->first );

					if( iStrip == imgIndex )	{

						long uniqueId =	mtLinkMap.getRootId( iter->first );

						if( tieNum < 100 && tieNum >= minOverlap ) {

							fprintf( fpOut, "%ld %.6f %.6f %.2f %f\n", uniqueId, iter->second.x, iter->second.y, iter->second.scale, iter->second.orient );										
						}
					}
				}

				fclose( fpOut );
			}
		}
	}

	if(ptPairVec)	delete ptPairVec;	ptPairVec=NULL;
	return true;
}


typedef struct
{
	long imgID;
	double x, y, z;

} LinkPt;

#include <algorithm>
//用stl中的排序算法
int ascending_projs_by_imgID(LinkPt &p1, LinkPt &p2)
{
	return p1.imgID < p2.imgID;
}

//将orsMatchLinks中的连接点导出到连接点文件中
bool OutputMatchLinks(orsMatchLinks *mtLinkMap, orsArray<orsString> *vStripFiles, std::vector<int> &imgIDVec,
	int minOverlap, const char *tieFileName)
{
	// 影像号，点id，x, y, z, 连接点id

	// 		orsString atTiePtsFileName = argv[1];
	// 		atTiePtsFileName += ".track.txt";

	// 		if( !outputTiePtsFile.isEmpty() )	{
	// 			orsArray<orsString> vImgPaths;
	// 
	// 			AddImage( prjPath, vImgPaths, outputTiePtsFile );
	// 			atTiePtsFileName = vImgPaths[0];
	// 		}

	mtLinkMAP &ptLinks = mtLinkMap->GetMatchLinks();

	LinkPt lpt;
	std::vector<LinkPt> vecLinks;

	FILE *fpOut;

	fpOut = fopen( tieFileName,  "wt");

	if(fpOut == NULL)
		return false;

	if( NULL != fpOut ) {

		mtLinkMapITER iter, next;	


		fprintf( fpOut, "[tracks]\n");
		fprintf(fpOut, "%10d\n", ptLinks.size()); //tie points num

		fprintf(fpOut, "[imageNum]\n");
		fprintf(fpOut, "%3d\n", vStripFiles->size()); //image num

		int n=0;
		for( iter = ptLinks.begin(); iter != ptLinks.end(); iter++ )
		{//遍历连接点
			if( !mtLinkMap->isVaild(iter) )
				continue;

			long tieNum = iter->second.count;

			if( tieNum < 100 && tieNum >= minOverlap ) 
			{
				n++;

				fprintf( fpOut, "%10ld ", iter->first);
				fprintf( fpOut, "%2ld ", tieNum);

				vecLinks.clear();

				long iImg = mtLinkMap->getImgNum( iter->first );						
				long uniqueId =	mtLinkMap->getRootId( iter->first );

				lpt.imgID = iImg;
				lpt.x = iter->second.x;
				lpt.y = iter->second.y;
				lpt.z = iter->second.z;

				vecLinks.push_back(lpt);

				////////////////////////////////////////////////////
				int i;
				next = iter;
				for( i=1; i<tieNum; i++)
				{
					next = ptLinks.find( next->second.next );

					long iImg = mtLinkMap->getImgNum( next->first );							
					long uniqueId =	mtLinkMap->getRootId( next->first );

					lpt.imgID = iImg;
					lpt.x = next->second.x;
					lpt.y = next->second.y;
					lpt.z = next->second.z;
					vecLinks.push_back(lpt);
				}

				//按相片号排序
				std::vector<LinkPt>::iterator tieIter;
// 				for(tieIter=vecLinks.begin(); tieIter!=vecLinks.end(); tieIter++)
// 				{
					sort(vecLinks.begin(), vecLinks.end(), ascending_projs_by_imgID);
//				}

				for(tieIter=vecLinks.begin(); tieIter!=vecLinks.end(); tieIter++)
				{
					if(imgIDVec.size()>tieIter->imgID)
						fprintf( fpOut, "%3ld %7.6f %7.6f %7.6f ",/*imgIDVec[tieIter->imgID]*/tieIter->imgID, tieIter->x, tieIter->y, tieIter->z );
					else
						fprintf( fpOut, "%3ld %7.6f %7.6f %7.6f ",tieIter->imgID, tieIter->x, tieIter->y, tieIter->z );
				}
				fprintf( fpOut, "\n");
			}
		}
		
		fseek( fpOut, 0L, SEEK_SET );

		fprintf( fpOut, "[tracks]\n%10d\n", n );

		fclose( fpOut );
	}

	return true;
}