#include "StdAfx.h"
#include "PointInterpolation.h"
#include "\dps_geoway\include\tindll.h"
#include "\dps_geoway\include\TinToGrid.h"
#include "\dps_geoway\include\ParallaxGrid.h"
#include "\openrs\desktop\include\orsImage\orsIImageWriter.h"
#include "\openrs\desktop\include\orsImage\orsIImageSource.h"
#include "\openrs\desktop\include\orsSRS\orsISpatialReference.h"
#include "\openrs\desktop\include\orsImageGeometry\orsIImageGeometryService.h"


//	style=0		强度模式
//	style=1		高程模式
bool ConvertToImage(char *pszName, LASpointXYZI *pData, int ptNum, double gridsize, int style/* =0 */)
{
//	bool bReturn=true;
	double xmax, ymax, xmin, ymin;
	int i;
	int imgWid, imgHei;

	xmax=ymax=-1.7e+308;
	xmin=ymin=1.7e+308;
	for(i=0; i<ptNum; i++)
	{
		if(pData[i].x<xmin)
			xmin=pData[i].x;
		else if(pData[i].x>xmax)
			xmax=pData[i].x;
		
		if(pData[i].y<ymin)
			ymin=pData[i].y;
		else if(pData[i].y>ymax)
			ymax=pData[i].y;
	}
	xmin+=0.5;	ymin+=0.5;
	xmax-=0.5;	ymax-=0.5;

	imgWid=int((xmax-xmin)/gridsize+0.5);
	imgHei=int((ymax-ymin)/gridsize+0.5);

	CTINClass	tin("pczAenVQ");
	CTinToGrid	tinGrid(&tin);
	CDEMGrid	demGrid;

	tin.BeginAddPoints();
	if(style==0)
	{
		for(i=0; i<ptNum; i++)
		{
			tin.AddPoint(pData[i].x, pData[i].y, pData[i].intensity);
		}
	}
	else if(style==1)
	{
		for(i=0; i<ptNum; i++)
		{
			tin.AddPoint(pData[i].x, pData[i].y, pData[i].z);
		}
	}

	tin.EndAddPoints();
	tin.FastConstruct();

	demGrid.CreateGridBuffer(imgWid, imgHei);
	demGrid.Initialize(xmin,ymax,gridsize,gridsize,imgWid,imgHei);
	tinGrid.RasterParallaxGrid(&demGrid);

//	pImgBuf=demGrid.Data();

	ref_ptr<orsIImageGeometry> pWriterGeom = ORS_CREATE_OBJECT( orsIImageGeometry, ORS_IMAGEGEOMETRY_AFFINE );

	ref_ptr<orsIImageWriter> pImgWriter = NULL;
	pImgWriter = ORS_CREATE_OBJECT( orsIImageWriter, ORS_IMAGE_WRITER_DEFAULT );
	{
		geoImgINFO	imgInfo;
		
		imgInfo.tile_wid = 256;
		imgInfo.tile_hei = 256;
		
		imgInfo.transType = geoImgTranOFFSETSCALE;
		imgInfo.i0 = 0;	imgInfo.x0 = xmin;
		imgInfo.j0 = 0;	imgInfo.y0 = ymax;
		imgInfo.k0 = 0;	imgInfo.z0 = 0;

		imgInfo.xScale = gridsize;
		imgInfo.yScale = gridsize;
		imgInfo.zScale = 1.0;
		imgInfo.noDataValue=pNoValue;

		
		orsSIZE size(imgWid, imgHei);
		
		ref_ptr<orsIOGRString> hcsWkt;
		orsISpatialReference *pSRS = pWriterGeom->GetSpatialReference();
		
		if( NULL != pSRS )
				pSRS->exportToWkt( hcsWkt );
		
		if( !pImgWriter->Create( pszName, size, 1, ORS_DT_FLOAT32/*ORS_DT_BYTE*/, &imgInfo, /*hcsWkt->getStr()*/NULL))
		{
			return false;
		}
		
		if( NULL != pWriterGeom.get() ) {
			
			ref_ptr<orsIProperty> affinePropert = getPlatform()->createProperty();
			
			double adfGeoTransform[6];
			// 平移，缩放转换为仿射变换
			adfGeoTransform[0] = imgInfo.x0 - imgInfo.i0 * imgInfo.xScale;
			adfGeoTransform[1] = imgInfo.xScale;
			adfGeoTransform[2] = 0.0;
			adfGeoTransform[3] = imgInfo.y0 - imgInfo.j0 * imgInfo.yScale;;
			adfGeoTransform[4] = 0;
			adfGeoTransform[5] = imgInfo.yScale;
			
			for( int i=0; i<6; i++ )
				affinePropert->addAttr( ORS_PROPERTY_AFFINE, adfGeoTransform[i] );
			
			pWriterGeom->initFromProperty( affinePropert.get() );
		} 

//		float *pImgBuf=new float[imgWid*imgHei];
//		memset(pImgBuf, 0, sizeof(float)*imgWid*imgHei);
		
		float *buf=demGrid.Data();
// 		for(i=0; i<imgHei; i++)
// 		{
// 			for(j=0; j<imgHei; j++)
// 			{
// 				if(buf[i*imgWid+j]>0)
// 				{
// 					pImgBuf[i*imgWid+j]=BYTE(buf[i*imgWid+j]+0.5);
// 				}
// 			}
// 		}

		
		pImgWriter->WriteBandRect( 0, 0, 0, imgWid, imgHei, (BYTE*)buf );
		
//		delete pImgBuf;
	}
	
	return true;
}


bool ConvertToImage(char *pszName, POINT3D *pData, int ptNum, double gridsize)
{
	double xmax, ymax, xmin, ymin;
	int i;
	int imgWid, imgHei;

	xmax=ymax=-1.7e+308;
	xmin=ymin=1.7e+308;
	for(i=0; i<ptNum; i++)
	{
		if(pData[i].X<xmin)
			xmin=pData[i].X;
		else if(pData[i].X>xmax)
			xmax=pData[i].X;
		
		if(pData[i].Y<ymin)
			ymin=pData[i].Y;
		else if(pData[i].Y>ymax)
			ymax=pData[i].Y;
	}
	xmin+=0.5;	ymin+=0.5;
	xmax-=0.5;	ymax-=0.5;

	imgWid=int((xmax-xmin)/gridsize+0.5);
	imgHei=int((ymax-ymin)/gridsize+0.5);

	CTINClass	tin("pczAenVQ");
	CTinToGrid	tinGrid(&tin);
	CDEMGrid	demGrid;

	tin.BeginAddPoints();
	for(i=0; i<ptNum; i++)
	{
		tin.AddPoint(pData[i].X, pData[i].Y, pData[i].Z);
	}

	tin.EndAddPoints();
	tin.FastConstruct();

	demGrid.CreateGridBuffer(imgWid, imgHei);
	demGrid.Initialize(xmin,ymax,gridsize,gridsize,imgWid,imgHei);
	tinGrid.RasterParallaxGrid(&demGrid);


	ref_ptr<orsIImageGeometry> pWriterGeom = ORS_CREATE_OBJECT( orsIImageGeometry, ORS_IMAGEGEOMETRY_AFFINE );

	ref_ptr<orsIImageWriter> pImgWriter = NULL;
	pImgWriter = ORS_CREATE_OBJECT( orsIImageWriter, ORS_IMAGE_WRITER_DEFAULT );
	{
		geoImgINFO	imgInfo;
		
		imgInfo.tile_wid = 256;
		imgInfo.tile_hei = 256;
		
		imgInfo.transType = geoImgTranOFFSETSCALE;
		imgInfo.i0 = 0;	imgInfo.x0 = xmin;
		imgInfo.j0 = 0;	imgInfo.y0 = ymax;
		imgInfo.k0 = 0;	imgInfo.z0 = 0;

		imgInfo.xScale = gridsize;
		imgInfo.yScale = gridsize;
		imgInfo.zScale = 1.0;
		imgInfo.noDataValue=pNoValue;

		
		orsSIZE size(imgWid, imgHei);
		
		ref_ptr<orsIOGRString> hcsWkt;
		orsISpatialReference *pSRS = pWriterGeom->GetSpatialReference();
		
		if( NULL != pSRS )
				pSRS->exportToWkt( hcsWkt );
		
		if( !pImgWriter->Create( pszName, size, 1, ORS_DT_FLOAT32/*ORS_DT_BYTE*/, &imgInfo, /*hcsWkt->getStr()*/NULL))
		{
			return false;
		}
		
		if( NULL != pWriterGeom.get() ) {
			
			ref_ptr<orsIProperty> affinePropert = getPlatform()->createProperty();
			
			double adfGeoTransform[6];
			// 平移，缩放转换为仿射变换
			adfGeoTransform[0] = imgInfo.x0 - imgInfo.i0 * imgInfo.xScale;
			adfGeoTransform[1] = imgInfo.xScale;
			adfGeoTransform[2] = 0.0;
			adfGeoTransform[3] = imgInfo.y0 - imgInfo.j0 * imgInfo.yScale;;
			adfGeoTransform[4] = 0;
			adfGeoTransform[5] = imgInfo.yScale;
			
			for( int i=0; i<6; i++ )
				affinePropert->addAttr( ORS_PROPERTY_AFFINE, adfGeoTransform[i] );
			
			pWriterGeom->initFromProperty( affinePropert.get() );
		} 

		float *buf=demGrid.Data();
		
		pImgWriter->WriteBandRect( 0, 0, 0, imgWid, imgHei, (BYTE*)buf );
		
	}
	
	return true;
}

bool ConvertToImage(char *pszName, CMemoryPool<POINT3D, POINT3D&> *pData, double gridsize)
{
	double xmax, ymax, xmin, ymin;
	int i;
	int imgWid, imgHei;
	int ptNum;
	POINT3D *pt;

	xmax=ymax=-1.7e+308;
	xmin=ymin=1.7e+308;
	ptNum=pData->GetNumberOfItems();
	for(i=0; i<ptNum; i++)
	{
		pt=pData->GetItem(i);

		if(pt->X<xmin)
			xmin=pt->X;
		else if(pt->X>xmax)
			xmax=pt->X;
		
		if(pt->Y<ymin)
			ymin=pt->Y;
		else if(pt->Y>ymax)
			ymax=pt->Y;
	}
	xmin+=0.5;	ymin+=0.5;
	xmax-=0.5;	ymax-=0.5;

	imgWid=int((xmax-xmin)/gridsize+0.5);
	imgHei=int((ymax-ymin)/gridsize+0.5);

	CTINClass	tin("pczAenVQ");
	CTinToGrid	tinGrid(&tin);
	CDEMGrid	demGrid;

	tin.BeginAddPoints();
	for(i=0; i<ptNum; i++)
	{
		pt=pData->GetItem(i);
		tin.AddPoint(pt->X, pt->Y, pt->Z);
	}

	tin.EndAddPoints();
	tin.FastConstruct();

	demGrid.CreateGridBuffer(imgWid, imgHei);
	demGrid.Initialize(xmin,ymax,gridsize,gridsize,imgWid,imgHei);
	tinGrid.RasterParallaxGrid(&demGrid);


	ref_ptr<orsIImageGeometry> pWriterGeom = ORS_CREATE_OBJECT( orsIImageGeometry, ORS_IMAGEGEOMETRY_AFFINE );

	ref_ptr<orsIImageWriter> pImgWriter = NULL;
	pImgWriter = ORS_CREATE_OBJECT( orsIImageWriter, ORS_IMAGE_WRITER_DEFAULT );
	{
		geoImgINFO	imgInfo;
		
		imgInfo.tile_wid = 256;
		imgInfo.tile_hei = 256;
		
		imgInfo.transType = geoImgTranOFFSETSCALE;
		imgInfo.i0 = 0;	imgInfo.x0 = xmin;
		imgInfo.j0 = 0;	imgInfo.y0 = ymax;
		imgInfo.k0 = 0;	imgInfo.z0 = 0;

		imgInfo.xScale = gridsize;
		imgInfo.yScale = gridsize;
		imgInfo.zScale = 1.0;
		imgInfo.noDataValue=pNoValue;

		
		orsSIZE size(imgWid, imgHei);
		
		ref_ptr<orsIOGRString> hcsWkt;
		orsISpatialReference *pSRS = pWriterGeom->GetSpatialReference();
		
		if( NULL != pSRS )
				pSRS->exportToWkt( hcsWkt );
		
		if( !pImgWriter->Create( pszName, size, 1, ORS_DT_FLOAT32/*ORS_DT_BYTE*/, &imgInfo, /*hcsWkt->getStr()*/NULL ) )
		{
			return false;
		}
		
		if( NULL != pWriterGeom.get() ) {
			
			ref_ptr<orsIProperty> affinePropert = getPlatform()->createProperty();
			
			double adfGeoTransform[6];
			// 平移，缩放转换为仿射变换
			adfGeoTransform[0] = imgInfo.x0 - imgInfo.i0 * imgInfo.xScale;
			adfGeoTransform[1] = imgInfo.xScale;
			adfGeoTransform[2] = 0.0;
			adfGeoTransform[3] = imgInfo.y0 - imgInfo.j0 * imgInfo.yScale;;
			adfGeoTransform[4] = 0;
			adfGeoTransform[5] = imgInfo.yScale;
			
			for( int i=0; i<6; i++ )
				affinePropert->addAttr( ORS_PROPERTY_AFFINE, adfGeoTransform[i] );
			
			pWriterGeom->initFromProperty( affinePropert.get() );
		} 

		float *buf=demGrid.Data();
		
		pImgWriter->WriteBandRect( 0, 0, 0, imgWid, imgHei, (BYTE*)buf );
		
	}
	
	return true;
}