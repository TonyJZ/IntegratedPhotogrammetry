/******************************************************************************
 * $Id: DPSDataFile.cpp, v 1.0 date: 04/22/2004 江万寿 Exp$
 *
 * Project:
 * Purpose:  Implementation of DPSDataFile
 * Author:   江万寿, wsjws@163.com
 *
 ******************************************************************************
 * Copyright (c) 2006, 江万寿
 *
 *		未得到授权请勿拷贝
 *
 ******************************************************************************
 *
 * $Log: DPSDataFile.cpp,v $
 *
 *
 * Revision 1.2  05/13/2007  江万寿
 * 由于多线程时流存在内存泄露，改为FILE *fp
 *
 * Revision 1.2  05/8/2007  江万寿
 * 增加相机文件检校格网支持，并把相机文件改为文本格式
 *
 * Revision 1.0  04/22/2004  江万寿
 * 增加高程范围
 *
 * Revision 1.0  04/22/2004  江万寿
 * New
 *
 */
#include "stdafx.h"
#include <stdio.h>

#ifdef _WINDOWS
#include <io.h>
#else

#endif

#include <string.h>
// #include <fstream>
//
// using namespace std;

#include "DPSDataType.h"

//////////////////////////////////////////////////////////////////////////


orsIPlatform *g_pPlatform = NULL;

orsIPlatform *getPlatform()
{
	return g_pPlatform;
}


void SetPlatform( orsIPlatform *pPlatform )
{
	g_pPlatform = pPlatform;
}


//#include "ImageCacheImplementor.h"


#define MAX_STRING 512

//struct HeadImgFile;


void RemoveDirFromPath(const char *dir, char *path)
{
	char *s, *s1;
	
	if( (s = strstr( path, dir ) ) == path ) {
		s1 = path + strlen( dir );
		while( *s1 == '\\' || *s1 == '/' )
			s1++;
		while( *s1 ) 
			*s++ = *s1++;
		
		*s = '\0';
	}
}

void AddDirToPath(const char *dir, char *path)
{
	char title[256];
	
	if( path[0] == '\\' )  return;
	if( path[0] == '/' )  return;
	if( path[1] == ':' ) return;
	
	if( path[0] == '\0' )
		return ;
	
	strcpy( title, path );
	strcpy( path, dir );
	strcat( path, "\\");
	strcat( path, title );
}


void GetDirFromPath(char *path)
{
	char *title, *s = path;
	while( *s ) {
		if( *s == '\\' || *s == '/' ) 
			title = s;
		s++;
	}
	*title = '\0';
}



const char *FindSubString( const char *str, const char *subStr )
{
	int i, n1, n2;
	char *upStr, *upSub;
	
	n1 = strlen( str );	n2 = strlen( subStr );
	
	if( n1 < n2 )
		return NULL;
	
	upStr = (char *)malloc( n1 + 1 );
	upSub = (char *)malloc( n2 + 1 );
	
	for( i =0; i<n1; i++ )
		*upStr++ = toupper( *str++ );
	*upStr = 0;	upStr -= n1;	str -= n1;
	
	for( i =0; i<n2; i++ )
		*upSub++ = toupper( *subStr++ );
	*upSub = 0;	 upSub -= n2; subStr -= n2;	
	
	char *subS = strstr( upStr, upSub );
	
	if( NULL != subS )
		n1 = (subS - upStr);
	
	free( upStr );
	free( upSub );
	
	if( NULL == subS )
		return NULL;
	
	return str + n1;
}

//////////////////////////////////////////////////////////////////////////

bool ReadProjectData( const char *fileName, prjDATA *prjData)
{
	FILE *fp = fopen( fileName, "rb" );
	if( fp == NULL )
		return false;

	fread(prjData, 1, sizeof(prjDATA), fp);
	fclose( fp );

	if( stricmp( prjData->tag, "LMARSDPS  project file: V1.0" ) != 0  &&
		stricmp( prjData->tag, "GeowayDPS project file: V1.0" ) != 0 ) {
		//AfxMessageBox( "It is not a LMARSDPS  project file");
		printf( "It is not a LMARSDPS  project file" );
		return false;
	}

	orsString prjDir = orsString::getDirFromPath( fileName );

	strcpy( prjData->prjDir, prjDir.c_str() );

 	AddDirToPath( prjData->prjDir, prjData->imgDir );
 	AddDirToPath( prjData->prjDir, prjData->gcpFile );
 	AddDirToPath( prjData->prjDir, prjData->cmrFile );

	return true;
}


bool SaveProjectData( const char *fileName, const prjDATA &prjData0 )
{
	FILE *fp = fopen( fileName, "wb" );
	if( fp == NULL )
		return false;

	prjDATA prjData = prjData0;

	strcpy( prjData.tag, "LMARSDPS  project file: V1.0");

	RemoveDirFromPath( prjData.prjDir, prjData.imgDir );
	RemoveDirFromPath( prjData.prjDir, prjData.gcpFile );
	RemoveDirFromPath( prjData.prjDir, prjData.cmrFile );

	fwrite( &prjData, 1, sizeof(prjDATA), fp );

	fclose( fp );

	return true;
}

//////////////////////////////////


bool ReadModelData( const char *fileName, modelDATA *modelData, const prjDATA *pPrjData)
{
	printf("here\n");
	FILE *fp = fopen( fileName, "rb" );
	if( fp == NULL )
	{
		printf("can't open %s\n", fileName);
		return false;
	}

	fread( modelData, 1, sizeof(modelDATA), fp);
	fclose( fp );


	if( stricmp( modelData->tag, "LMARSDPS  stereo model file: V1.0" ) != 0 &&
		 stricmp( modelData->tag, "GeowayDPS stereo model file: V1.0" ) != 0 ) {
		//AfxMessageBox( "It is not a LMARSDPS  stereo model file");
		printf( "It is not a LMARSDPS  stereo model file");
		return false;
	}

	prjDATA prjData;
	if( pPrjData == NULL )
		ReadProjectData( modelData->prjFile, &prjData );
	else	{
		prjData = *pPrjData;
		if( strstr( modelData->prjFile, prjData.prjName ) == NULL ) {
			//AfxMessageBox( "不能打开不是本工程的模型");
			printf( "不能打开不是本工程的模型");
			return false;
		}

		if( strstr( modelData->prjFile, prjData.prjDir ) == NULL ) {
			// fix
			sprintf( modelData->prjFile, "%s\\%s.prj", prjData.prjDir, prjData.prjName );

			FILE *fp = fopen( fileName, "wb" );
			if( fp != NULL ) {
				fwrite( modelData, 1, sizeof(modelDATA), fp);
				fclose( fp );
			}
		}
	}

	strcpy( modelData->modelDir, fileName );
	GetDirFromPath( modelData->modelDir );

	AddDirToPath( prjData.imgDir, modelData->lImgFile );
	AddDirToPath( prjData.imgDir, modelData->rImgFile );

	return true;
}


bool SaveModelData( const char *fileName, const modelDATA &modelData0, const prjDATA &prjData )
{
	FILE *fp = fopen( fileName, "wb" );
	if( fp == NULL )
		return false;

	modelDATA modelData = modelData0;

	strcpy( modelData.tag, "LMARSDPS  stereo model file: V1.0");

	RemoveDirFromPath( prjData.imgDir, modelData.lImgFile );
	RemoveDirFromPath( prjData.imgDir, modelData.rImgFile );

	fwrite( &modelData, 1, sizeof(modelDATA), fp );

	fclose( fp );

	return true;
}


// 
// ///////////////////////////////////////////
// //#include "GeowayImageInterface.h"
// 
// // create image info silently 
// bool CreateImgInfo( const char *imgFileName, const prjDATA &prjData, bool bOverWrite )
// {
// 	char infoFile[256];
// 
// 	sprintf( infoFile, "%s.gnf", imgFileName );
// 
// 	FILE *fp = fopen( infoFile, "rb" );
// 	if( fp && !bOverWrite ) return true;
// 
// 	/////////////////////////////
// 	imgINFO imgInfo;
// 
// 	memset( &imgInfo, 0, sizeof(imgINFO));
// 	
// 	//////////////////////////////////////////////////////////////////////////
// 	CImageCache *pImgCache;
// 
// 	pImgCache = LoadImageFromFile(imgFileName, &imgInfo);
// 
// 	imgInfo.width = pImgCache->GetDimension().cx;
// 	imgInfo.height = pImgCache->GetDimension().cy;
// 	imgInfo.nBandCount = pImgCache->GetNumOfBands();
// 	imgInfo.offset = 0;	
// 
// 	delete pImgCache;
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	strcpy( imgInfo.cmrFile, prjData.cmrFile );
// 	imgInfo.cmrOrient = prjData.cmrOr;
// 	
// 	imgInfo.imgOr = prjData.imgOr;
// 	
// 	imgInfo.imgType = prjData.imgType;
// 	imgInfo.pixelSize = prjData.pixelSize;
// 	imgInfo.photoScale = prjData.photoScale;
// 
// 	// 高程范围
// 	imgInfo.zMin = prjData.zMin;
// 	imgInfo.zMax = prjData.zMax;
// 	imgInfo.zCurrent = prjData.zAverage;
// 	
// 	imgInfo.ovHeight = 0;
// 	imgInfo.ovWidth  = 0;
// 	imgInfo.ovZoomOut  =0;
// 	imgInfo.pOvImgBits = NULL;
// 	
// 	SaveImgInfo( imgFileName, imgInfo, prjData.prjDir );
// 
// 	return true;
// }
// 
// bool ReadImgInfo( const char *imgFileName, imgINFO *imgInfo, const char *prjDir, float photoScale, bool bOverviewImg)
// {
// 	char infoFile[256];
// 
// 	sprintf( infoFile, "%s.gnf", imgFileName );
// 
// 	FILE *fp = fopen( infoFile, "r+b" );
// 	if( fp == NULL ) {
// 		return false;
// 	}
// 
// 	fread( imgInfo, 1, sizeof(imgINFO), fp);
// 
// 	if( stricmp( imgInfo->tag, "GeowayDPS Image Info file: V1.0" ) == 0 ) {
// 		imgINFO_10 *imgInfo_10 = new imgINFO_10;
// 
// 		fseek( fp, 0L, SEEK_SET );
// 		fread( imgInfo_10, 1, sizeof(imgINFO_10), fp);
// 		memcpy( &imgInfo->offset, &imgInfo_10->offset, sizeof( imgINFO_10 ) - 44 );
// 
// 		imgInfo->width = imgInfo_10->width;
// 		imgInfo->height = imgInfo_10->height;
// 
// 		delete imgInfo_10;
// 
// 		sprintf( imgInfo->tag, "GeowayDPS Image Info file: V1.1" );
// 
// 	}
// 	else if( stricmp( imgInfo->tag, "GeowayDPS Image Info file: V1.1" ) != 0 ) {
// 		fclose( fp ); 
// 		AfxMessageBox( "It is not a GeowayDPS image info file");
// 		return false;	
// 	}
// 
// 	//////////////////////////////
// 	if( imgInfo->width <= 0 || imgInfo->height <= 0 || imgInfo->nBandCount <=0 ) {
// 		AfxMessageBox( "Invalid image info file!");
// 		return false;
// 	}	
// 
// 	//////////////////////////////////
// 	if( photoScale > 100 && photoScale != imgInfo->photoScale ) {
// 		imgInfo->photoScale = photoScale;
// 		fseek( fp, 0L, SEEK_SET );
// 		fwrite( imgInfo, 1, sizeof(imgINFO), fp);
// 	}
// 
// 	imgInfo->pOvImgBits  = NULL;
// 	if( bOverviewImg ) {
// 		if( imgInfo->ovHeight <= 0 && imgInfo->ovWidth <= 0 ) {
// 
// 			CImageCache *img = LoadImageFromFile( imgFileName, imgInfo );
// 			
//             ////使左右片的比例一致
// 		    float ratio = imgInfo->width / 350.0;
// 			imgInfo->ovZoomOut = short(ratio+0.5);
// 			  ratio = imgInfo->height / 350.0+0.5;
// 			  
// 			if( imgInfo->ovZoomOut <short(ratio))
// 				imgInfo->ovZoomOut = short(ratio);//imgInfo->height / 350;
// 
// 			SIZE size;
// 
// 			imgInfo->pOvImgBits = img->WriteOverviewImg( fp, sizeof(imgINFO), imgInfo->ovZoomOut, &size, &imgInfo->lineBytes );
// 			
// 			delete img;
// 
// 			//////////////////////////////////////////////////////////////////////////
// 			imgInfo->ovWidth = size.cx;
// 			imgInfo->ovHeight = size.cy;
// 
// 			fseek( fp, 0L, SEEK_SET);
// 			fwrite( imgInfo, 1, sizeof(imgINFO), fp);
// 		}
// 		else	{
// 			fseek( fp, sizeof(imgINFO), SEEK_SET );
// 			imgInfo->pOvImgBits  = new BYTE[imgInfo->ovHeight*imgInfo->lineBytes];
// 			fread( imgInfo->pOvImgBits, imgInfo->ovHeight, imgInfo->lineBytes, fp);
// 		}
// 	}
// 
// 	fclose( fp ); 
// 
// 	AddDirToPath( prjDir, imgInfo->cmrFile );
// 
// 	return true;
// }
// 
// 
// bool SaveImgInfo( const char *imgFileName, const imgINFO &imgInfo0, const char *prjDir)
// {
// 	char infoFile[256];
// 
// 	sprintf( infoFile, "%s.gnf", imgFileName );
// 
// 	FILE *fp;
// 	fp = fopen( infoFile, "r+b" );
// 	if( fp == NULL ) 
// 		fp = fopen( infoFile, "wb" );
// 
// 	if( fp == NULL )
// 		return false;
// 
// 	imgINFO imgInfo = imgInfo0;
// 	
// 	strcpy( imgInfo.tag, "GeowayDPS Image Info file: V1.1");
// 
// 	RemoveDirFromPath( prjDir, imgInfo.cmrFile );
// 
// 	fwrite( &imgInfo, 1, sizeof(imgINFO), fp );
// 
// //	if( imgInfo.pOvImgBits )
// //		fwrite( imgInfo.pOvImgBits, imgInfo.nBandCount , imgInfo.ovWidth*imgInfo.ovHeight, fp );
// 
// 	fclose( fp ); 
// 
// 	return true;
// }
// 
// 
// 


bool ReadCamera_V1( const char *cmrFile, CAMERA_V1 *cmr )
{
	char tag[40];

	sprintf( tag, "GeowayDPS  Camera file: V1.0" );

	FILE *fp = fopen( cmrFile, "rb" );
	if( fp == NULL )
		return false;

	fread( tag, 1,  40, fp);
	fread( cmr, 1, sizeof(CAMERA_V1), fp);
	fclose( fp );

	if( stricmp( tag, "GeowayDPS Camera file: V1.0" ) != 0 ) {
		//AfxMessageBox( "It is not a LMARSDPS  camera file");
		printf( "It is not a LMARSDPS  camera file");
		return false;
	}

	return true;
}



bool ReadCamera( const char *cmrFile, CAMERA *cmr )
{
	char buf[MAX_STRING];
	FILE *fp;

	memset( cmr, 0, sizeof(CAMERA) );

	fp = fopen( cmrFile, "rt" );

	if( NULL == fp )
		return false;

	//////////////////////////////////////////////////////////////////////////
	fgets( buf,  40, fp );

	if( NULL != strstr( buf, "GeowayDPS Camera file: V1.0" ) ) {
		fclose( fp );
		return ReadCamera_V1( cmrFile, (CAMERA_V1 *)cmr );
	}

	//////////////////////////////////////////////////////////////////////////
	if( NULL == strstr( buf, "LMARSDPS  Camera file: V2.0" ) &&
			NULL == strstr( buf, "GeowayDPS Camera file: V2.0" ) ) {
		fclose( fp );
		//AfxMessageBox( "It is not a LMARSDPS  camera file");
		printf( "It is not a LMARSDPS  camera file");
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	cmr->clbType = ctNONE;

	int i, j;
	while( !feof( fp ) )
	{
		fgets(buf, MAX_STRING, fp );

		if( strstr(buf, "[Principal Point]" ) ) {
			fscanf( fp, "%lf%lf", &cmr->x0, &cmr->y0 );
		}
		else if( strstr(buf, "[Principal Distance]" )	) {
			fscanf( fp, "%lf", &cmr->f );
		}
		else if( strstr(buf, "[Fiducial Marks]" )	) {
			fscanf( fp, "%d", &cmr->numOfMarks );

			for( i=0; i<cmr->numOfMarks; i++ )
			{
				fscanf( fp, "%d%f%f", &j, &cmr->xf[i], &cmr->yf[i] );
			}
		}
		else if( strstr(buf, "[Radial Distortion Coefficients]" ) ) {
			fgets(buf, MAX_STRING, fp );

			char *p = strstr( buf, "[k1,k2,k3]" );
			if( NULL != p )
				sscanf( p, "[k1,k2,k3]: %lf%lf%lf", &cmr->k1, &cmr->k2, &cmr->k3 );
			else if(p=strstr( buf, "[k0,k1,k2]" ))
				sscanf( p, "[k0,k1,k2]: %lf%lf%lf", &cmr->k0, &cmr->k1, &cmr->k2 );
			else if(p=strstr( buf, "[k0,k1,k2,k3]" ))
				sscanf( p, "[k0,k1,k2,k3]: %lf%lf%lf%lf", &cmr->k0, &cmr->k1, &cmr->k2, &cmr->k3 );
			
			fgets(buf, MAX_STRING, fp );
			p = strstr( buf, "[p1,p2]" );

			if( NULL != p )
				sscanf( p, "[p1,p2]: %lf%lf", &cmr->p1, &cmr->p2 );
			else	{
				char *s=buf;
				while( *s )
					ungetc( *s++, fp );
			}
			if( cmr->k1 != 0 || cmr->k2 != 0 || cmr->k3 != 0 )
				cmr->clbType = ctCOEFFICIENT;
		}
		else if( strstr(buf, "[Radial Distortion Value]" ) ) {
			fgets(buf, MAX_STRING, fp );
			sscanf( buf, "[num of values]: %d", &cmr->clb_r.n);
			fgets(buf, MAX_STRING, fp );
			sscanf( buf, "[step]: %f", &cmr->clb_r.dr );

			cmr->clb_r.d = new float[cmr->clb_r.n];
			for( i=0; i<cmr->clb_r.n; i++ )
			{
				fscanf( fp, "%f", &cmr->clb_r.d[i] );
			}

			if( cmr->clb_r.n > 0 )
				cmr->clbType = ctPROFILE_R;
		}
		else if( strstr(buf, "[Distortion GRID]" ) ) {
			fgets(buf, MAX_STRING, fp );
			sscanf( buf, "[x0,y0]: %lf%lf", &cmr->clb_xy.x0, &cmr->clb_xy.y0);
			fgets(buf, MAX_STRING, fp );
			sscanf( buf, "[nx,ny]: %d%d", &cmr->clb_xy.nx, &cmr->clb_xy.ny );
			fgets(buf, MAX_STRING, fp );
			sscanf( buf, "[dx,dy]: %lf%lf", &cmr->clb_xy.dx, &cmr->clb_xy.dy );

			orsPOINT2Df *d = cmr->clb_xy.d = new orsPOINT2Df[cmr->clb_xy.ny*cmr->clb_xy.nx];
			for( i=0; i<cmr->clb_xy.ny; i++ )
			{
				for( j=0; j<cmr->clb_xy.nx; j++ )
				{
					fscanf( fp, "%f%f", &d->x, &d->y );
					d++;
				}
			}

			if( cmr->clb_xy.ny > 0 && cmr->clb_xy.nx > 0 )
				cmr->clbType = ctGRID_XY;
		}
		else if( strstr(buf, "[Pixel Size]" ) ) {
			fscanf( fp, "%lf%lf", &cmr->pixelX, &cmr->pixelY );
		}
	}

	fclose( fp );

	return true;
}


//
// Version 2.0
//
bool SaveCamera( const char *cmrFile,  const CAMERA &cmr )
{
	char tag[40];

	sprintf( tag, "LMARSDPS  Camera file: V2.0" );

	FILE *fp = fopen( cmrFile, "wb" );
	if( fp == NULL )
		return false;

	fprintf( fp, "%s\r\n", tag );			// 文件标记
	fprintf( fp, "[Principal Point]\r\n");
	fprintf( fp, "%lf %lf\r\n", cmr.x0, cmr.y0 );
	fprintf( fp, "[Principal Distance]\r\n");
	fprintf( fp, "%lf\r\n", cmr.f );
	fprintf( fp, "[Pixel Size]\r\n");
	fprintf( fp, "%lf %lf\r\n", cmr.pixelX, cmr.pixelY );


	int i, j;
	if( cmr.numOfMarks > 0 )	{
		fprintf( fp, "[Fiducial Marks]\r\n");
		fprintf( fp, "%d\r\n", cmr.numOfMarks );


		for( i=0; i<cmr.numOfMarks; i++ )
		{
			fprintf( fp, "%d %13.4lf %13.4lf\r\n", i+1, cmr.xf[i], cmr.yf[i] );
		}
	}

	//
	// 畸变参数或检校格网
	//


	switch( cmr.clbType )	{
	case ctCOEFFICIENT:
		if( cmr.k1 !=0 || cmr.k2 != 0 )	{
			fprintf( fp, "[Radial Distortion Coefficients]\r\n");
			fprintf( fp, "[k1,k2,k3]: %e %e %e\r\n", cmr.k1, cmr.k2, cmr.k3 );
			if( cmr.p1 != 0 || cmr.p2 != 0 )
				fprintf( fp, "[p1,p2]: %e %e\r\n", cmr.p1, cmr.p2 );
		}
		break;

	case ctPROFILE_R:
		fprintf( fp, "[Radial Distortion Value]\r\n");
		fprintf( fp, "[num of values]: %d\r\n", cmr.clb_r.n);
		fprintf( fp, "[step]: %.4f\r\n", cmr.clb_r.dr );

		for( i=0; i<cmr.clb_r.n; i++ )
		{
			fprintf( fp, "%.1f ", cmr.clb_r.d[i] );
		}
		fprintf( fp, "\r\n");
		break;

	case ctGRID_XY:
		fprintf( fp, "[Distortion GRID]\r\n");
		fprintf( fp, "[x0,y0]: %.1f %.1f\r\n", cmr.clb_xy.x0,  cmr.clb_xy.y0 );
		fprintf( fp, "[nx,ny]: %d %d\r\n", cmr.clb_xy.nx, cmr.clb_xy.ny );
		fprintf( fp, "[dx,dy]: %.1f %.1f\r\n", cmr.clb_xy.dx, cmr.clb_xy.dy );

		orsPOINT2Df *d = cmr.clb_xy.d;
		for( i=0; i<cmr.clb_xy.ny; i++ )
		{
			for( j=0; j<cmr.clb_xy.nx; j++ )
			{
				fprintf( fp, "%.1f ", *d++ );
			}
			fprintf( fp, "\r\n");
		}
		break;
	};

	fclose( fp );

	return true;
}


bool ReadPxyFile( const char *pxyFileName, pxyFILE *pxyFile )
{
	char buf[MAX_STRING];

	FILE *fp;
	fp = fopen( pxyFileName, "rt" );

	if( NULL != fp  )  {
		fgets(buf, 40, fp );

		if( strstr( buf, "LMARSDPS  PXY file: V1.0" ) == NULL &&
			strstr( buf, "GeowayDPS PXY file: V1.0" ) == NULL ) {

			//AfxMessageBox( "It is not a LMARSDPS  PXY file");
			printf( "It is not a LMARSDPS  PXY file");
			fclose( fp );
			return false;
		}

		memset( pxyFile, 0, sizeof( pxyFILE ));

		while( !feof( fp ) )
		{
			fgets(buf, MAX_STRING, fp );

			if( strstr( buf, "[Number of PXYS]"  ) )
			{
				fscanf( fp, "%d", &pxyFile->numOfPxys );
				pxyFile->pts = new pxyPOINT[ pxyFile->numOfPxys ];
			}
			else if( strstr( buf, "[Image Coordinates]"  ) )
			{
				pxyPOINT *pt = pxyFile->pts;
				for( int i=0; i< pxyFile->numOfPxys; i++ )
				{
					fscanf( fp, "%s%lf%lf%lf%lf", pt->name, &pt->xl, &pt->yl, &pt->xr, &pt->yr );
					pt++;
				}
				break;
			}
		};

		fclose( fp );
		return true;
	}

	return false;
}


bool SavePxyFile( const char *pxyFileName,  const pxyFILE &pxyFile )
{
	FILE *fp;

	fp = fopen( pxyFileName, "wt" );

	if( fp == NULL )
		return false;


	fprintf( fp, "LMARSDPS  PXY file: V1.0\n\n" );

	fprintf( fp, "[Number of PXYS]\n" );
	fprintf( fp, "%ld\n\n", pxyFile.numOfPxys );

	fprintf( fp, "[Image Coordinates]\n" );

	pxyPOINT *pt = pxyFile.pts;
	for( int i=0; i< pxyFile.numOfPxys; i++ )
	{
		fprintf( fp, "%-8s %-13.3lf %-13.3lf %-13.3lf %-13.3lf\n", pt->name, pt->xl ,pt->yl , pt->xr,pt->yr );
		pt++;
	}

	fclose( fp );

	return true;
}



bool ReadGCPFile( const char *gcpFileName, gcpFILE *gcpFile )
{
	char buf[MAX_STRING];

	if( gcpFileName == NULL )
		return false;

	if( gcpFileName[0] == 0 )
		return false;

	//////////////////////////////////////////////////////////////////////////
	FILE *fp;

	fp = fopen( gcpFileName, "rb" );
	if( NULL == fp )
		return false;

	fgets( buf, 40, fp  );

	if( NULL == strstr( buf, "LMARSDPS  GCP file: V1.0" ) &&
		 NULL == strstr( buf, "GeowayDPS GCP file: V1.0" ) ) {
		//AfxMessageBox( "It is not a LMARSDPS  GCP file");
		printf( "It is not a LMARSDPS  GCP file");
		fclose( fp );
		return false;
	}

	memset( gcpFile, 0, sizeof( gcpFILE ));

	while( !feof( fp ) ) {
		fgets(buf, MAX_STRING, fp );

		if( strstr( buf, "[Coordinate System ID]")  )
			fscanf( fp, "%d", &gcpFile->coordSysId );

		else if( strstr( buf, "[Coordinate System Name]" )  )
			fscanf( fp, "%s", gcpFile->coordSysName );

		else if( strstr( buf, "[Number of GCPS]"  ) ) {
			fscanf( fp, "%d", &gcpFile->numOfGCPs );

			gcpFile->pts = new gcpPOINT[ gcpFile->numOfGCPs ];
			break;
		}
	};

	gcpPOINT *pt = gcpFile->pts;
	for( int i=0; i< gcpFile->numOfGCPs; i++ )
	{
		fscanf( fp, "%s%lf%lf%f", pt->name, &pt->X, &pt->Y, &pt->Z );
		pt++;
	}

	fclose( fp );

	return true;
}


bool SaveGCPFile( const char *gcpFileName,  const gcpFILE &gcpFile )
{
	FILE *fp;

	fp = fopen( gcpFileName, "wt" );

	if( fp == NULL )
		return false;


	fprintf( fp, "LMARSDPS  GCP file: V1.0\n\n" );

	fprintf( fp, "[Number of GCPS]\n" );
	fprintf( fp, "%d\n", gcpFile.numOfGCPs );

	gcpPOINT *pt = gcpFile.pts;
	for( int i=0; i< gcpFile.numOfGCPs; i++ )
	{
		fprintf( fp, "%-8s %-13.3lf %-13.3lf %-13.3f\n", pt->name, pt->X ,pt->Y , pt->Z );
		pt++;
	}

	fclose( fp );

	return true;

}



/////////////////////////////////////////////////////////////////

bool ReadInorFile( const char *imageFileName, inorPARA *inorPara, double *imgX/*=NULL*/, double *imgY/*=NULL*/, int n/*=0*/ )
{
	FILE *fp;
	char pLine[MAX_STRING];

	orsString iopName = imageFileName;

	iopName += ".iop";

	fp = fopen( iopName, "rt" );
	if(  NULL == fp  )
	{
		iopName = imageFileName;
		iopName += ".ioi";

		fp = fopen( iopName, "rt" );
	}
	if( NULL == fp )
		return false;

	fgets(pLine, MAX_STRING, fp );
	if( !strstr( pLine, "LMARSDPS  INOR file: V1.0") &&
		!strstr( pLine, "GeowayDPS INOR file: V1.0") )
		return false;

	int i;
	int	nRead = 0;
	while( !feof(fp) ) {
		fgets(pLine, MAX_STRING, fp );
		if( strstr( pLine, "[Image to Pixel Parameters]" ) ) {
			for( i=0; i<3; i++)
				fscanf( fp, "%lf", &inorPara->fm2p_x[i] );
			for( i=0; i<3; i++)
				fscanf( fp, "%lf", &inorPara->fm2p_y[i] );
		}
		else if( strstr( pLine, "[Pixel to Image Parameters]" ) ) {
			for( i=0; i<3; i++)
				fscanf( fp, "%lf", &inorPara->p2fm_x[i] );
			for( i=0; i<3; i++)
				fscanf( fp, "%lf", &inorPara->p2fm_y[i] );
		}
		else if( strstr( pLine, "[Image Coordinates and Errors]" ) ) {
			fgets(pLine, MAX_STRING, fp );

			char fStr[80];
			int pn;
			for( int i=0; i < n; i++)
			{
				fscanf( fp, "%d", &pn );		// skip point #

				if( pn != i ) {
					fclose( fp );
					return false;
				}

				nRead = i+1;

				fscanf( fp, "%lf%lf%s%s", &imgX[i], &imgY[i], fStr, fStr );		// skip vx
			}
			break;
		}
	};

	fclose( fp );

	if( nRead < n )
		return false;

	return true;
}


FILE *SaveInorFile( const char *imageFileName, inorPARA *inorPara )
{
	FILE *fp;

	orsString iopName = imageFileName;
	iopName += ".iop";

	fp = fopen( iopName, "wt" );

	if( fp == NULL )
		return NULL;

	fprintf( fp, "LMARSDPS  INOR file: V1.0\n\n" );

	fprintf( fp, "[Image to Pixel Parameters]\n" );
	fprintf( fp, "%e\t%e\t%e\n", inorPara->fm2p_x[0], inorPara->fm2p_x[1], inorPara->fm2p_x[2] );
	fprintf( fp, "%e\t%e\t%e\n", inorPara->fm2p_y[0], inorPara->fm2p_y[1], inorPara->fm2p_y[2] );

	fprintf( fp, "\n[Pixel to Image Parameters]\n" );
	fprintf( fp, "%e\t%e\t%e\n", inorPara->p2fm_x[0], inorPara->p2fm_x[1], inorPara->p2fm_x[2] );
	fprintf( fp, "%e\t%e\t%e\n", inorPara->p2fm_y[0], inorPara->p2fm_y[1], inorPara->p2fm_y[2] );

	//////////////////////////////////////////////////////////////////////////

	{

		orsString iopName1 = imageFileName;
		iopName1 = iopName1.left( iopName1.reverseFind('.' ) );

		iopName1 += ".iop.txt";

		FILE *fp1 = fopen( iopName1, "wt" );

		if( fp1 == NULL )
			return NULL;

		fprintf( fp1, "[Camera Calibration File]\n" );
		fprintf( fp1, "KMRMK.cmr.txt\n\n" );

		fprintf( fp1, "[Image to Pixel Parameters]\n" );
		fprintf( fp1, "%e\t%e\t%e\n", inorPara->fm2p_x[0], inorPara->fm2p_x[1], inorPara->fm2p_x[2] );
		fprintf( fp1, "%e\t%e\t%e\n", inorPara->fm2p_y[0], inorPara->fm2p_y[1], inorPara->fm2p_y[2] );

		fprintf( fp1, "\n[Pixel to Image Parameters]\n" );
		fprintf( fp1, "%e\t%e\t%e\n", inorPara->p2fm_x[0], inorPara->p2fm_x[1], inorPara->p2fm_x[2] );
		fprintf( fp1, "%e\t%e\t%e\n", inorPara->p2fm_y[0], inorPara->p2fm_y[1], inorPara->p2fm_y[2] );

		fclose(fp1);
	}

	return fp;
}


/////////////////////////////////////////////////////////////////

bool ReadExtOrFile( const char *imageFileName, extorPARA *extorPara )
{
	FILE *fp;
	char pLine[MAX_STRING];

	orsString eopName = imageFileName;

	eopName += ".eop";

	fp = fopen( eopName,"rt");

	if( NULL == fp )	{
		eopName = imageFileName;

		int pos = eopName.reverseFind( '.' );

		eopName = eopName.left( pos );

		eopName += ".aop.txt";

		fp = fopen( eopName,"rt");
	}
	
	if(fp==NULL)
	{
		eopName = imageFileName;
		
			eopName += ".aop";
		
		fp = fopen( eopName,"rt");
	}
	
	if( NULL == fp )
		return false;

	if(fp!=NULL)	
	{
		fgets(pLine, MAX_STRING, fp );
		if( !strstr( pLine, "LMARSDPS  ExtOrPara file: V1.0") )	{
			fclose(fp);
			return false;
		}
	}

	

	while(1) {
		fgets(pLine, MAX_STRING, fp);
		if( strstr( pLine, "[Xs,Ys,Zs]" ) || strstr( pLine, "[Xs Ys Zs]" )  ) {
			fscanf( fp, "%lf%lf%lf", &extorPara->Xs, &extorPara->Ys, &extorPara->Zs );
		}
		else if( strstr( pLine, "[Rotation Angle]" ) ) {
			fscanf(  fp, "%lf%lf%lf", &extorPara->phi, &extorPara->omega, &extorPara->kappa );
		}
		else if( strstr( pLine, "[Rotation Matrix]" ) ) {
			for( int i=0; i<9; i++)
			{
				fscanf( fp, "%lf", &extorPara->R[i] );
			}
			break;
		}
	};

	fclose( fp );

	return true;
}


bool SaveExtOrFile( const char *imageFileName, extorPARA *extorPara )
{
	FILE *fp;

	orsString eopName = imageFileName;
	eopName += ".eop";

	fp = fopen( eopName, "wt" );

	if( fp == NULL )
		return false;

	fprintf( fp, "LMARSDPS  ExtOrPara file: V1.0\n\n" );

	fprintf( fp, "[Xs,Ys,Zs]\n" );
	fprintf( fp, "%13.3lf %13.3lf %13.3lf\n\n", extorPara->Xs, extorPara->Ys, extorPara->Zs );

	fprintf( fp, "[Rotation Angle]\n" );
	fprintf( fp, "%13.8lf %13.8lf %13.8lf\n\n", extorPara->phi, extorPara->omega, extorPara->kappa );

	fprintf( fp, "[Rotation Matrix]\n" );
	for( int i=0; i<3; i++)
	{
		for( int j=0; j<3; j++)
			fprintf( fp, "%18.12lf", extorPara->R[i*3+j] );
		fprintf( fp, "\n" );
	}

	fclose( fp );

	return true;
}



bool ReadModelParaFile( const char *modelFileName, modelPARA *modelPara, bool bAop /* = false */, int *epipRange /*= NULL */  )
{
	FILE *fp;
	char pLine[MAX_STRING];

	orsString eopName = modelFileName;

	if( bAop )
		eopName += ".aop";
	else
		eopName += ".rop";

//	printf("eopName: %s\n", eopName.c_str());

	fp = fopen( eopName, "rt");
	if( NULL == fp )
	{
		eopName = modelFileName;
		if( bAop )
			eopName += ".aoi";
		else
			eopName += ".roi";

		fp = fopen( eopName,"rt");
	}
//	printf("eopName: %s\n", eopName.c_str());
	if( NULL == fp )	{
		printf("error \n");
		modelDATA mdl;
		extorPARA eop;

		orsString mdlFilePath = modelFileName;
		mdlFilePath += ".mdl";

		if(!ReadModelData( mdlFilePath, &mdl, NULL ))
		{
			mdlFilePath = modelFileName;
			mdlFilePath += ".smp";
			ReadModelData( mdlFilePath, &mdl, NULL );
		}

		int nOk = 0;
		if( ReadExtOrFile( mdl.lImgFile, &eop ) )	{
			modelPara->XsL = eop.Xs;
			modelPara->YsL = eop.Ys;
			modelPara->ZsL = eop.Zs;
			memcpy( modelPara->Rl, eop.R, 9*sizeof(double) );
			nOk++;
		}

		if( ReadExtOrFile( mdl.rImgFile, &eop ) )	{
			modelPara->XsR = eop.Xs;
			modelPara->YsR = eop.Ys;
			modelPara->ZsR = eop.Zs;
			memcpy( modelPara->Rr, eop.R, 9*sizeof(double) );
			nOk++;
		}

		if( nOk == 2 )
			return true;

		return false;
	}

	fgets(pLine, MAX_STRING, fp );
	if( !strstr( pLine, "LMARSDPS  modelPara file: V1.0") &&
			!strstr( pLine, "GeowayDPS modelPara file: V1.0") )
		return false;

	while( !feof(fp) ) {
		fgets(pLine, MAX_STRING, fp );
		if( strstr( pLine, "[XsL YsL ZsL]" ) ) {
			fscanf( fp, "%lf%lf%lf", &modelPara->XsL, &modelPara->YsL, &modelPara->ZsL );
		}
		else if( strstr( pLine, "[Left Rotation MATRIX]" ) ) {
			for( int i=0; i<9; i++)
				fscanf( fp, "%lf", &modelPara->Rl[i] );
		}
		else if( strstr( pLine, "[XsR YsR ZsR]" ) ) {
			fscanf( fp, "%lf%lf%lf", &modelPara->XsR, &modelPara->YsR, &modelPara->ZsR);
		}
		else if( strstr( pLine, "[Right Rotation MATRIX]" ) ) {
			for( int i=0; i<9; i++)
				fscanf( fp, "%lf", &modelPara->Rr[i]);
		}
		else if( strstr( pLine, "[Eipipolar Image Range]" ) ) {
			if( epipRange != NULL ) {
				for( int i=0; i<4; i++)
					fscanf( fp, "%d", epipRange+i );
			}
			break;
		}
	};

	printf("read over\n");
	fclose( fp );

	return true;
}



FILE *SaveModelParaFile( const char *modelFileName, modelPARA *modelPara , bool bAop /* = false */)
{
	int i;
	FILE *fp;

	orsString ropName = modelFileName;

	if( bAop )
		ropName += ".aop";
	else
		ropName += ".rop";

	fp = fopen( ropName, "wt" );

	if( fp == NULL )
		return NULL;

	fprintf( fp, "LMARSDPS  modelPara file: V1.0\n" );

	fprintf( fp, "\n[XsL YsL ZsL]\n" );
	fprintf( fp, "%13.3lf %13.3lf %13.3lf\n\n", modelPara->XsL, modelPara->YsL, modelPara->ZsL );

	fprintf( fp, "[Left Rotation MATRIX]\n" );
	for( i=0; i<3; i++)
	{
		//for(  j=0; j<3; j++)
			fprintf( fp, "%13.8lf %13.8lf %13.8lf\n", modelPara->Rl[i*3+0],modelPara->Rl[i*3+1],modelPara->Rl[i*3+2] );
		//fprintf( fp, "\n" );
	}

	fprintf( fp, "\n[XsR YsR ZsR]\n" );
	fprintf( fp, "%13.3lf %13.3lf %13.3lf\n\n", modelPara->XsR, modelPara->YsR, modelPara->ZsR );

	fprintf( fp, "[Right Rotation MATRIX]\n" );

	for( i=0; i<3; i++)
	{
		//for(  j=0; j<3; j++)
			fprintf( fp, "%13.8lf %13.8lf %13.8lf\n", modelPara->Rr[i*3+0],modelPara->Rr[i*3+1],modelPara->Rr[i*3+2] );
		//fprintf( fp, "\n" );
	}

	return fp;

}











///////////////////////////////////////////
//#include "GeowayImageInterface.h"

#include "orsImage/orsIImageService.h"

ORS_GET_IMAGE_SERVICE_IMPL();

#include "orsImage/orsIImageSource.h"

bool CreateImgInfo( const char *imgFileName, const prjDATA &prjData, bool bOverWrite )
{
	char infoFile[256];

	sprintf( infoFile, "%s.gnf", imgFileName );

	FILE *fp = fopen( infoFile, "rb" );
	if( fp && !bOverWrite ) return true;

	/////////////////////////////
	imgINFO imgInfo;

	memset( &imgInfo, 0, sizeof(imgINFO));

	//////////////////////////////////////////////////////////////////////////
	{
		ref_ptr<orsIImageSource> pImgCache = getImageService()->openImageFile( imgFileName );
		
		//pImgCache = LoadImageFromFile(imgFileName, &imgInfo);
		
		imgInfo.width = pImgCache->getWidth();
		imgInfo.height = pImgCache->getHeight();
		imgInfo.nBandCount = pImgCache->getNumberOfOutputBands();
		imgInfo.offset = 0;
		
		//delete pImgCache;
	}
	//////////////////////////////////////////////////////////////////////////

	strcpy( imgInfo.cmrFile, prjData.cmrFile );
	imgInfo.cmrOrient = prjData.cmrOr;

	imgInfo.imgOr = prjData.imgOr;

	imgInfo.imgType = prjData.imgType;
	imgInfo.pixelSize = prjData.pixelSize;
	imgInfo.photoScale = prjData.photoScale;

	// 高程范围
	imgInfo.zMin = prjData.zMin;
	imgInfo.zMax = prjData.zMax;
	imgInfo.zCurrent = prjData.zAverage;

	imgInfo.ovHeight = 0;
	imgInfo.ovWidth  = 0;
	imgInfo.ovZoomOut  =0;
	imgInfo.pOvImgBits = NULL;

	SaveImgInfo( imgFileName, imgInfo, prjData.prjDir );

	return true;
}


bool ReadImgInfo( const char *imgFileName, imgINFO *imgInfo, const char *prjDir, float photoScale, bool bOverviewImg)
{
	char infoFile[256];

	if( FindSubString( imgFileName, ".gnf" ))
		sprintf( infoFile, "%s", imgFileName );
	else
		sprintf( infoFile, "%s.gnf", imgFileName );

	FILE *fp = fopen( infoFile, "r+b" );
	if( fp == NULL ) {
		return false;
	}

	fread( imgInfo, 1, sizeof(imgINFO), fp);

	if( stricmp( imgInfo->tag, "LMARSDPS  Image Info file: V1.0" ) == 0  ||
		stricmp( imgInfo->tag, "GeowayDPS Image Info file: V1.0" ) == 0 ) {
		imgINFO_10 *imgInfo_10 = new imgINFO_10;

		fseek( fp, 0L, SEEK_SET );
		fread( imgInfo_10, 1, sizeof(imgINFO_10), fp);
		memcpy( &imgInfo->offset, &imgInfo_10->offset, sizeof( imgINFO_10 ) - 44 );

		imgInfo->width = imgInfo_10->width;
		imgInfo->height = imgInfo_10->height;

		delete imgInfo_10;

		sprintf( imgInfo->tag, "LMARSDPS  Image Info file: V1.1" );

	}
	else if( stricmp( imgInfo->tag, "LMARSDPS  Image Info file: V1.1" ) != 0 &&
			stricmp( imgInfo->tag, "GeowayDPS Image Info file: V1.1" ) != 0 ) {
		fclose( fp );
		//AfxMessageBox( "It is not a LMARSDPS  image info file");
		printf( "It is not a LMARSDPS  image info file");
		return false;
	}

	//////////////////////////////
	if( imgInfo->width <= 0 || imgInfo->height <= 0 || imgInfo->nBandCount <=0 ) {
		//AfxMessageBox( "Invalid image info file!");
		printf( "Invalid image info file!");
		return false;
	}

	//////////////////////////////////
	if( photoScale > 100 && photoScale != imgInfo->photoScale ) {
		imgInfo->photoScale = photoScale;
		fseek( fp, 0L, SEEK_SET );
		fwrite( imgInfo, 1, sizeof(imgINFO), fp);
	}

	imgInfo->pOvImgBits  = NULL;
	if( bOverviewImg ) {
		if( imgInfo->ovHeight <= 0 && imgInfo->ovWidth <= 0 ) {

			// CImageCache *img = LoadImageFromFile( imgFileName, imgInfo );
			{
				ref_ptr<orsIImageSource> img = getImageService()->openImageFile( imgFileName );
				
				////使左右片的比例一致
				float ratio = imgInfo->width / 350.0;
				imgInfo->ovZoomOut = short(ratio+0.5);
				ratio = imgInfo->height / 350.0+0.5;
				
				if( imgInfo->ovZoomOut <short(ratio))
					imgInfo->ovZoomOut = short(ratio);//imgInfo->height / 350;
				
				orsSIZE size;
				
				imgInfo->pOvImgBits = NULL;	//img->WriteOverviewImg( fp, sizeof(imgINFO), imgInfo->ovZoomOut, &size, &imgInfo->lineBytes );
				
				//delete img;
				
			}

			//////////////////////////////////////////////////////////////////////////
			imgInfo->ovWidth = 0;	// size.cx;
			imgInfo->ovHeight = 0;	// size.cy;

			fseek( fp, 0L, SEEK_SET);
			fwrite( imgInfo, 1, sizeof(imgINFO), fp);
		}
		else	{
			fseek( fp, sizeof(imgINFO), SEEK_SET );
			imgInfo->pOvImgBits  = new BYTE[imgInfo->ovHeight*imgInfo->lineBytes];
			fread( imgInfo->pOvImgBits, imgInfo->ovHeight, imgInfo->lineBytes, fp);
		}
	}

	fclose( fp );

	AddDirToPath( prjDir, imgInfo->cmrFile );

	return true;
}


bool SaveImgInfo( const char *imgFileName, const imgINFO &imgInfo0, const char *prjDir)
{
	char infoFile[256];

	sprintf( infoFile, "%s.gnf", imgFileName );

	FILE *fp;
	fp = fopen( infoFile, "r+b" );
	if( fp == NULL )
		fp = fopen( infoFile, "wb" );

	if( fp == NULL )
		return false;

	imgINFO imgInfo = imgInfo0;

	strcpy( imgInfo.tag, "LMARSDPS  Image Info file: V1.1");

	RemoveDirFromPath( prjDir, imgInfo.cmrFile );

	fwrite( &imgInfo, 1, sizeof(imgINFO), fp );

//	if( imgInfo.pOvImgBits )
//		fwrite( imgInfo.pOvImgBits, imgInfo.nBandCount , imgInfo.ovWidth*imgInfo.ovHeight, fp );

	fclose( fp );

	return true;
}





