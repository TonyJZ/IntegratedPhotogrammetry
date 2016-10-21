#include "StdAfx.h"
#include "IPhBaseDef.h"
#include "Geometry.h"
///////////////////////////////////////////////////////
//2010.4.14		航遥数据的kappa角需要+180°才能得到正确的旋转矩阵


//void RotateMat_fwk( double f, double w, double k, double *r);
//void R2fwk(double *R, double *phi, double *omega, double *kapa);
//void RotateMat_wfk( double w, double f, double k, double *r);
//void RotationMatrixOPK(double omega, double phi, double kappa, double *r );
//void R2wfk(double *R, double *phi, double *omega, double *kapa);

#define MAX_STRING 1024

void iphCamera::SetInorParameter(double x0, double y0, double f, double k1, double k2, double k3, double p1, double p2)
{
	m_x0=x0;
	m_y0=y0;
	m_f=f;

	m_k1=k1;
	m_k2=k2;
	m_k3=k3;
	m_p1=p1;
	m_p2=p2;

//	InteriorOrientation();
	m_bInor=true;
}

void iphCamera::SetExorParameter(double xs, double ys, double zs, double phi, double omega, double kappa)
{
	m_Xs = xs;
	m_Ys = ys;
	m_Zs = zs;
	m_phi = phi;
	m_omega = omega;
	m_kappa = kappa;

//	RotateMat_fwk(phi, omega, kappa, m_RMatrix);
	m_bExor=true;
}

void iphCamera::SetCoordinateSystem(char *HorizontalDatum, char *VerticalDatum, iphUnit HorizontalUnit, iphUnit VerticalUnit)
{
	strncpy(m_HorizontalDatum, HorizontalDatum, strlen(HorizontalDatum)*sizeof(char));
	strncpy(m_VerticalDatum, VerticalDatum, strlen(VerticalDatum)*sizeof(char));

	m_HorizontalUnit = HorizontalUnit;
	m_VerticalUnit = VerticalUnit;
}

void iphCamera::SetRotateSystem(iphRotateSys RotateSys, iphUnit AngleUnit)
{
	m_RotateSys = RotateSys;
	m_AngleUnit = AngleUnit;
}

const char cmr_file_flag[32]="IPLR camera file V1.0";
bool iphCamera::WriteCameraFile(const char *pCamName)
{
	FILE *fp=NULL;

	fp=fopen(pCamName, "wt");
	if(fp==NULL)
		return	false;

	fprintf(fp, "IPLR camera file V1.0\n\n");

//	fwrite(cmr_file_flag, sizeof(char), 32, fp);
	
	//internal parameters
	fprintf(fp, "[Camera Type]\n");
	fprintf(fp, "%d\n", m_Ctype);
	fprintf(fp, "[x0, y0]\n");
	fprintf(fp, "%.6lf %.6lf\n\n", m_x0, m_y0);
	fprintf(fp, "[focal length]\n");
	fprintf(fp, "%.6lf\n\n", m_f);
	fprintf(fp, "[imgWid, imgHei (pixel)]\n");
	fprintf(fp, "%ld %ld\n\n", m_imgWid, m_imgHei);
	fprintf(fp, "[Pixel Size (mm)]\n");
	fprintf(fp, "%.6lf %.6lf\n\n", m_pixelWid, m_pixelHei);
	fprintf(fp, "[radial distortion: k1,k2,k3]\n");
	fprintf(fp, "%e\t%e\t%e\n\n", m_k1, m_k2, m_k3);
	fprintf(fp, "[tangential distortion: p1,p2]\n");
	fprintf(fp, "%e\t%e\n\n", m_p1, m_p2);

	
	//External Parameter
	fprintf(fp, "[Xs,Ys,Zs]\n");
	fprintf(fp, "%13.3lf %13.3lf %13.3lf\n\n", m_Xs, m_Ys, m_Zs);
	fprintf(fp, "[phi,omega,kappa]\n");
	fprintf(fp, "%13.8lf %13.8lf %13.8lf\n\n", m_phi, m_omega, m_kappa);
	fprintf(fp, "[rotate system]\n");
	fprintf(fp, "%d\n\n", m_RotateSys);
	fprintf(fp, "[angle unit]\n");
	fprintf(fp, "%d\n\n", m_AngleUnit);
	fprintf(fp, "[GPS time]\n");
	fprintf(fp, "%13.8lf\n\n", m_GPSTime);

	fclose(fp);

	return	true;
}
//*.cam  文件  像机参数文件
bool iphCamera::ReadCameraFile(const char *pCamName)
{
	FILE *fp=NULL;
	char pLine[MAX_STRING];
	
	fp=fopen(pCamName, "rt");
	if(fp==NULL)
		return	false;

	fgets(pLine, MAX_STRING, fp );
	if( !strstr( pLine, cmr_file_flag) )	
	{
		fclose(fp);
		return false;
	}

	while(!feof(fp)) 
	{
		fgets(pLine, MAX_STRING, fp);
		if(strstr(pLine, "[Camera Type]"))
		{
			fscanf( fp, "%d", &m_Ctype );
		}
		else if(strstr(pLine, "[x0, y0]") || strstr( pLine, "[x0 y0]" ))
		{
			fscanf( fp, "%lf %lf", &m_x0, &m_y0 );
			m_bInor=true;
		}
		else if(strstr(pLine, "[focal length]"))
		{
			fscanf(fp, "%lf", &m_f);
		}
		else if(strstr(pLine, "[imgWid, imgHei (pixel)]"))
		{
			fscanf(fp, "%ld %ld", &m_imgWid, &m_imgHei);
		}
		else if(strstr(pLine, "[Pixel Size (mm)]"))
		{
			fscanf(fp, "%lf %lf", &m_pixelWid, &m_pixelHei);
		}	
		else if(strstr(pLine, "[radial distortion: k1,k2,k3]"))
		{
			fscanf(fp, "%lf\t%lf\t%lf", &m_k1, &m_k2, &m_k3);
		}
		else if(strstr(pLine, "[tangential distortion: p1,p2]"))
		{
			fscanf(fp, "%lf\t%lf", &m_p1, &m_p2);
		}
		else if( strstr( pLine, "[Xs,Ys,Zs]" ) || strstr( pLine, "[Xs Ys Zs]" )  ) 
		{
			fscanf( fp, "%lf %lf %lf", &m_Xs, &m_Ys, &m_Zs );
		}
		else if( strstr( pLine, "[Rotation Angle]" ) || strstr(pLine, "[phi,omega,kappa]")) 
		{
			fscanf(  fp, "%lf %lf %lf", &m_phi, &m_omega, &m_kappa );
			m_bExor=true;
		}
		else if(strstr(pLine, "[rotate system]"))
		{
			fscanf(fp, "%d", &m_RotateSys);
		}
		else if(strstr(pLine, "[angle unit]"))
		{
			fscanf(fp, "%d", &m_AngleUnit);
		}
		else if( strstr( pLine, "[GPS time]" ) ) 
		{
			fscanf( fp, "%lf", &m_GPSTime );
		}
	};
	
	fclose(fp);

//	m_bInor=true;
//	m_bExor=true;
	
	return	true;
}

void iphCamera::CalRotateMatrix()
{
	double ang2rad;

// 	double f, w, k;
// 	double cosf,sinf;
// 	double cosw,sinw;
// 	double cosk,sink;
// 	double *r;

	if(m_AngleUnit==Unit_Degree360)
	{
		ang2rad=PI/180;
	}
	else if(m_AngleUnit==Unit_Degree400)
	{
		ang2rad=PI/200;
	}
	else if(m_AngleUnit==Unit_Radian)
	{
		ang2rad=1.0;
	}
	else
	{
		ang2rad=1.0;
	}

	switch(m_RotateSys)
	{
	case RotateSys_YXZ:
		
		RotateMat_fwk(m_phi*ang2rad, m_omega*ang2rad, m_kappa*ang2rad, m_RMatrix);
		break;

	case RotateSys_XYZ:

		//RotateMat_wfk(m_omega*ang2rad, m_phi*ang2rad, m_kappa*ang2rad, m_RMatrix);
		RotateMat_wfk(m_omega*ang2rad, m_phi*ang2rad, (m_kappa+180)*ang2rad, m_RMatrix);	//航遥安阳数据需要改kappa
		break;

	case RotateSys_ZYZ:

		return;
	}

}

void iphCamera::GetRotateMatrix(double *R)
{
	CalRotateMatrix();
	memcpy(R, m_RMatrix, sizeof(double)*9);
}

bool iphCamera::WriteExtOrFile_aop( const char *aopFileName )
{
	FILE	*fp=0;
	double ang2rad;

	fp=fopen(aopFileName, "wt");
	if(fp==0)
		return	false;
	
	fprintf(fp,  "[Xs,Ys,Zs]\n");
	fprintf(fp,  "%13.3lf %13.3lf %10.3lf\n\n", m_Xs, m_Ys, m_Zs );
	
	fprintf(fp,  "[Rotation Angle]\n");
	if(m_AngleUnit==Unit_Degree360)
	{
		ang2rad=PI/180;
	}
	else if(m_AngleUnit==Unit_Degree400)
	{
		ang2rad=PI/200;
	}
	else if(m_AngleUnit==Unit_Radian)
	{
		ang2rad=1.0;
	}
	else
	{
		ang2rad=1.0;
	}
	fprintf(fp,  "%13.7lf %13.7lf %10.7lf\n\n", m_phi*ang2rad, m_omega*ang2rad, m_kappa*ang2rad );
	
	fprintf(fp,  "[Rotation Sys]\n");
	if(m_RotateSys==RotateSys_XYZ)
		fprintf(fp,  "RotateSys_XYZ\n\n" );
	else if(m_RotateSys==RotateSys_YXZ)
		fprintf(fp,  "RotateSys_YXZ\n\n" );
	else if(m_RotateSys==RotateSys_ZYZ)
		fprintf(fp,  "RotateSys_ZYZ\n\n" );

	CalRotateMatrix();
	fprintf(fp,  "[Rotation Matrix]\n"  );
	for( int i=0; i<3; i++) 
	{
		for( int j=0; j<3; j++)
		{
			fprintf(fp,  "%10.7lf ", m_RMatrix[i*3+j] );
		}
		fprintf(fp, "\n");
	}
	
	fprintf(fp,  "\n[Elevation Range]\n"  );
	fprintf(fp,  "0 100 200\n");
	
	fclose(fp);

	return	true;
}

//针对ALS50系统像机的外方位元素进行解算
bool iphCamera::WriteExtOrFile_aop_ALS50( const char *aopFileName )
{
	FILE	*fp=0;
	double ang2rad;

	fp=fopen(aopFileName, "wt");
	if(fp==0)
		return	false;


	fprintf(fp,  "[Xs,Ys,Zs]\n");
	fprintf(fp,  "%13.3lf %13.3lf %10.3lf\n\n", m_Xs, m_Ys, m_Zs );

	fprintf(fp,  "[Rotation Angle (radian)]\n");
// 	if(m_AngleUnit==Unit_Degree360)
// 	{
//		ang2rad=PI/180;
// 	}
// 	else if(m_AngleUnit==Unit_Degree400)
// 	{
// 		ang2rad=PI/200;
// 	}
// 	else if(m_AngleUnit==Unit_Radian)
// 	{
// 		ang2rad=1.0;
// 	}
// 	else
// 	{
// 		ang2rad=1.0;
// 	}
	fprintf(fp,  "%13.7lf %13.7lf %10.7lf\n\n", m_phi, m_omega, m_kappa );

// 	fprintf(fp,  "[Rotation Sys]\n");
// 	if(m_RotateSys==RotateSys_XYZ)
// 		fprintf(fp,  "RotateSys_XYZ\n\n" );
// 	else if(m_RotateSys==RotateSys_YXZ)
// 		fprintf(fp,  "RotateSys_YXZ\n\n" );
// 	else if(m_RotateSys==RotateSys_ZYZ)
// 		fprintf(fp,  "RotateSys_ZYZ\n\n" );

//	CalRotateMatrix();

//	RotateMat_rph(m_omega, m_phi, m_kappa, m_RMatrix);
	RotateMat_wfk(m_omega, m_phi, m_kappa, m_RMatrix);
//	RotateMat_fwk(m_phi, m_omega, m_kappa, m_RMatrix);

	fprintf(fp,  "[Rotation Matrix]\n"  );
	for( int i=0; i<3; i++) 
	{
		for( int j=0; j<3; j++)
		{
			fprintf(fp,  "%10.7lf ", m_RMatrix[i*3+j] );
		}
		fprintf(fp, "\n");
	}

	fprintf(fp,  "\n[Elevation Range]\n"  );
	fprintf(fp,  "0 100 200\n");

	fclose(fp);

	return	true;
}

bool iphCamera::WriteIntOrFile_iop( const char *iopFileName )
{
	FILE	*fp=0;
	double inorImage_a[3];	// pixel to image
	double inorImage_b[3];
	double inorPixel_a[3];	// image to pixel
	double inorPixel_b[3];
	
	fp=fopen(iopFileName, "wt");
	if(fp==0)
		return	false;

	//x方向
	inorImage_a[0] = -m_imgWid*m_pixelWid/2;
	inorImage_a[1] = m_pixelWid;
	inorImage_a[2] = 0;	
	//y方向
	inorImage_b[0] = m_imgHei*m_pixelHei/2;
	inorImage_b[1] = 0;
	inorImage_b[2] = -m_pixelHei;
	
	inorPixel_a[0] = m_imgWid*1.0/2;	
	inorPixel_a[1] = 1/m_pixelWid;
	inorPixel_a[2] = 0;
	inorPixel_b[0] = m_imgHei*1.0/2;
	inorPixel_b[1] = 0;
	inorPixel_b[2] = -1/m_pixelHei;
	

	fprintf( fp, "LMARSDPS  INOR file: V1.0\n\n" );
// 	fprintf( fp, "KMRMK.cmr.txt\n\n" );
	
	fprintf( fp, "[Image to Pixel Parameters]\n" );
	fprintf( fp, "%e\t%e\t%e\n", inorPixel_a[0], inorPixel_a[1], inorPixel_a[2] );
	fprintf( fp, "%e\t%e\t%e\n", inorPixel_b[0], inorPixel_b[1], inorPixel_b[2] );
	
	fprintf( fp, "\n[Pixel to Image Parameters]\n" );
	fprintf( fp, "%e\t%e\t%e\n", inorImage_a[0], inorImage_a[1], inorImage_a[2] );
	fprintf( fp, "%e\t%e\t%e\n", inorImage_b[0], inorImage_b[1], inorImage_b[2] );

	fprintf( fp, "\n[x0, y0]\n");
	fprintf(fp, "%.6lf %.6lf\n\n", m_x0, m_y0);
	fprintf(fp, "[focal length]\n");
	fprintf(fp, "%.6lf\n\n", m_f);

	fprintf(fp, "[imgWid, imgHei (pixel)]\n");
	fprintf(fp, "%d %d\n\n", m_imgWid, m_imgHei);
	fprintf(fp, "[PixelWid, PixelHei (mm)]\n");
	fprintf(fp, "%.6lf %.6lf\n\n", m_pixelWid, m_pixelHei);

	fprintf(fp, "[radial distortion: k1,k2,k3]\n");
	fprintf(fp, "%e\t%e\t%e\n\n", m_k1, m_k2, m_k3);
	fprintf(fp, "[tangential distortion: p1,p2]\n");
	fprintf(fp, "%e\t%e\n\n", m_p1, m_p2);
	
	fclose(fp);

	return	true;
}

//
// 采用中式的phi, omega, kappa表达法, 中式的旋转矩阵
//

void _CalRotateMatrix(iphRotateSys Rsys, double A1, double A2, double A3, double *R)
{
	switch(Rsys)
	{
	case RotateSys_YXZ:
		RotateMat_fwk(A1, A2, A3, R);
		break;

	case RotateSys_XYZ:
		RotateMat_wfk(A1,A2,A3,R);
		break;

	case RotateSys_ZYZ:
		break;
	}
}

void _CalRotateAngle(iphRotateSys Rsys, double *R, double *A1, double *A2, double *A3)
{
	switch(Rsys)
	{
	case RotateSys_YXZ:
		R2fwk(R, A1, A2, A3);
		break;
		
	case RotateSys_XYZ:
		R2wfk(R, A1, A2, A3);
		break;
		
	case RotateSys_ZYZ:
		break;
	}
}


void iphCamera::InteriorOrientation()
{
	m_inorImage_a[0] = -m_imgWid*m_pixelWid/2;
	m_inorImage_a[1] = m_pixelWid;
	m_inorImage_a[2] = 0;

	m_inorImage_b[0] = m_imgHei*m_pixelHei/2;
	m_inorImage_b[1] = 0;
	m_inorImage_b[2] = -m_pixelHei;

	m_inorPixel_a[0] = m_imgWid*1.0/2;		
	m_inorPixel_a[1] = 1.0/m_pixelWid;
	m_inorPixel_a[2] = 0;
	m_inorPixel_b[0] = m_imgHei*1.0/2;
	m_inorPixel_b[1] = 0;
	m_inorPixel_b[2] = -1.0/m_pixelHei;

}

//采用的计算公式：
//	x_corr = x_meas - x0 - det(x)
//	y_corr = y_meas - y0 - det(y)
//有些系统在检校时采用的是 + det(x)的方式，在做直接定向时要注意
// void iphCamera::Image2Photo(double xi, double yi, double& xp, double& yp)
// {
// 	double xf, yf;
// 	double r2, dr, dx, dy;
// 
// 	xf = m_inorImage_a[0] + m_inorImage_a[1]*xi + m_inorImage_a[2]*yi;
// 	yf = m_inorImage_b[0] + m_inorImage_b[1]*xi + m_inorImage_b[2]*yi;
// 
// 	//畸变纠正
// 	xp = xf - m_x0;
// 	yp = yf - m_y0;
// 
// 	r2 = xp*xp + yp*yp;
// 
// 	dr = ( m_k1+ ( m_k2 + m_k3*r2)*r2 )*r2;
// 	dx = xp*dr;
// 	dy = xp*dr;
// 
// 	dx += m_p1*( r2 + 2*xp*xp ) + 2*m_p2*xp*yp;
// 	dy += 2*m_p1*xp*yp + m_p2*( r2 + 2*yp*yp );
// 
// 	dx=m_b1 * xp + m_b2 * yp;
// 
// 	xp -= dx;	
// 	yp -= dy;
// }

// void iphCamera::Photo2Image(double xp, double yp, double& xi, double& yi)
// {
// 	double xf, yf;
// 	double r2, dr, dx, dy;
// 
// 	r2 = xp*xp + yp*yp;
// 
// 	dr = ( m_k1+ ( m_k2 + m_k3*r2)*r2 )*r2;
// 	dx = xp*dr;
// 	dy = xp*dr;
// 
// 	dx += m_p1*( r2 + 2*xp*xp ) + 2*m_p2*xp*yp;
// 	dy += 2*m_p1*xp*yp + m_p2*( r2 + 2*yp*yp );
// 
// 	dx=m_b1 * xp + m_b2 * yp;
// 
// 	xf=xp+dx+m_x0;
// 	yf=yp+dy+m_y0;
// 
// 	xi = m_inorPixel_a[0] + m_inorPixel_a[1]*xf + m_inorPixel_a[2]*yf;
// 	yi = m_inorPixel_b[0] + m_inorPixel_b[1]*xf + m_inorPixel_b[2]*yf;
// 
// }