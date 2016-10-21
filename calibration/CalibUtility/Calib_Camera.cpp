#include "Calib_Camera.h"
#include "Geometry.h"

#define MAX_STRING 1024

void _iphCamera::SetInorParameter(double x0, double y0, double f, int imgWid, int imgHei, double pixelX, double pixelY, 
								 double k0, double k1, double k2, double k3, double p1, double p2)
{
	m_x0=x0;
	m_y0=y0;
	m_f=f;

	m_imgWid=imgWid;
	m_imgHei=imgHei;
	m_pixelX=pixelX;
	m_pixelY=pixelY;

	m_k0=k0;
	m_k1=k1;
	m_k2=k2;
	m_k3=k3;
	m_p1=p1;
	m_p2=p2;

	InteriorOrientation();

	m_bInor=true;
}

void _iphCamera::SetExorParameter(double xs, double ys, double zs, double phi, double omega, double kappa)
{
	m_Xs = xs;
	m_Ys = ys;
	m_Zs = zs;
	m_phi = phi;
	m_omega = omega;
	m_kappa = kappa;

	RotateMat_fwk(phi, omega, kappa, m_RMatrix);

	m_bExor=true;
}

void _iphCamera::SetCoordinateSystem(char *HorizontalDatum, char *VerticalDatum, iphUnit HorizontalUnit, iphUnit VerticalUnit)
{
// 	strncpy(m_HorizontalDatum, HorizontalDatum, strlen(HorizontalDatum)*sizeof(char));
// 	strncpy(m_VerticalDatum, VerticalDatum, strlen(VerticalDatum)*sizeof(char));
// 
// 	m_HorizontalUnit = HorizontalUnit;
// 	m_VerticalUnit = VerticalUnit;
}

void _iphCamera::SetRotateSystem(iphRotateSys RotateSys, iphUnit AngleUnit)
{
	m_RotateSys = RotateSys;
	m_AngleUnit = AngleUnit;
}

const char IPLR_camera_flag[32]="IPLR camera file";
const char LMARSDPS_camera_flag[128]="LMARSDPS  Camera file";

bool _iphCamera::WriteCameraFile(const char *pCamName)
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
	fprintf(fp, "%.6lf %.6lf\n\n", m_pixelX, m_pixelY);
	fprintf(fp, "[radial distortion: k0,k1,k2,k3]\n");
	fprintf(fp, "%e\t%e\t%e\t%e\n\n", m_k0, m_k1, m_k2, m_k3);
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
bool _iphCamera::ReadCameraFile(const char *pCamName)
{
	FILE *fp=NULL;
	char pLine[MAX_STRING];
	
	fp=fopen(pCamName, "rt");
	if(fp==NULL)
		return	false;

	fgets(pLine, MAX_STRING, fp );
	if( !strstr( pLine, IPLR_camera_flag) && !strstr( pLine, LMARSDPS_camera_flag) )	
	{
		fclose(fp);
		return false;
	}

	m_pixelX=m_pixelY=0;
	while(!feof(fp)) 
	{
		fgets(pLine, MAX_STRING, fp);
		if(strstr(pLine, "[Camera Type]"))
		{
			fscanf( fp, "%d", &m_Ctype );
		}
		else if(strstr(pLine, "[x0, y0]") || strstr( pLine, "[x0 y0]" ) || strstr( pLine, "[Principal Point]" ))
		{
			fscanf( fp, "%lf %lf", &m_x0, &m_y0 );
			m_bInor=true;
		}
		else if(strstr(pLine, "[focal length]") || strstr(pLine, "[Principal Distance]"))
		{
			fscanf(fp, "%lf", &m_f);
		}
		else if(strstr(pLine, "[imgWid, imgHei (pixel)]"))
		{
			fscanf(fp, "%ld %ld", &m_imgWid, &m_imgHei);
		}
		else if(strstr(pLine, "[Image Size]"))
		{
			fscanf(fp, "%ld %ld", &m_imgWid, &m_imgHei);
		}
		else if(strstr(pLine, "[Pixel Size (mm)]") || strstr(pLine, "[Pixel Size]"))
		{
			fscanf(fp, "%lf %lf", &m_pixelX, &m_pixelY);
		}	
		else if(strstr(pLine, "[radial distortion: k1,k2,k3]"))
		{
			fscanf(fp, "%lf\t%lf\t%lf", &m_k1, &m_k2, &m_k3);
		}
		else if(strstr(pLine, "[radial distortion: k0,k1,k2]"))
		{
			fscanf(fp, "%lf\t%lf\t%lf", &m_k0, &m_k1, &m_k2);
		}
		else if(strstr(pLine, "[radial distortion: k0,k1,k2,k3]"))
		{
			fscanf(fp, "%lf\t%lf\t%lf\t%lf", &m_k0, &m_k1, &m_k2, &m_k3);
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

	if(m_pixelX==0 || m_pixelY==0)
	{
		printf("can't load pixel size!\n");
	}

	InteriorOrientation();
//	m_bInor=true;
//	m_bExor=true;
	
	return	true;
}

void _iphCamera::CalRotateMatrix()
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

void _iphCamera::GetRotateMatrix(double *R)
{
	CalRotateMatrix();
	memcpy(R, m_RMatrix, sizeof(double)*9);
}

bool _iphCamera::WriteExtOrFile_aop( const char *aopFileName )
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
	fprintf(fp,  "800 1000 1200\n");
	
	fclose(fp);

	return	true;
}

//针对ALS50系统像机的外方位元素进行解算
bool _iphCamera::WriteExtOrFile_aop_ALS50( const char *aopFileName )
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

bool _iphCamera::WriteIntOrFile_iop( const char *iopFileName )
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
	inorImage_a[0] = -m_imgWid*m_pixelX/2;
	inorImage_a[1] = m_pixelX;
	inorImage_a[2] = 0;	
	//y方向
	inorImage_b[0] = m_imgHei*m_pixelY/2;
	inorImage_b[1] = 0;
	inorImage_b[2] = -m_pixelY;
	
	inorPixel_a[0] = m_imgWid*0.5;	
	inorPixel_a[1] = 1.0/m_pixelX;
	inorPixel_a[2] = 0;
	inorPixel_b[0] = m_imgHei*0.5;
	inorPixel_b[1] = 0;
	inorPixel_b[2] = -1.0/m_pixelY;
	

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
	fprintf(fp, "%.6lf %.6lf\n\n", m_pixelX, m_pixelY);

	fprintf(fp, "[radial distortion: k1,k2,k3]\n");
	fprintf(fp, "%e\t%e\t%e\n\n", m_k1, m_k2, m_k3);
	fprintf(fp, "[tangential distortion: p1,p2]\n");
	fprintf(fp, "%e\t%e\n\n", m_p1, m_p2);
	
	fclose(fp);

	return	true;
}

void _iphCamera::InteriorOrientation()
{
	m_inorImage_a[0] = -m_imgWid*m_pixelX/2;
	m_inorImage_a[1] = m_pixelX;
	m_inorImage_a[2] = 0;

	m_inorImage_b[0] = m_imgHei*m_pixelY/2;
	m_inorImage_b[1] = 0;
	m_inorImage_b[2] = -m_pixelY;

	m_inorPixel_a[0] = m_imgWid*0.5;		
	m_inorPixel_a[1] = 1.0/m_pixelX;
	m_inorPixel_a[2] = 0;
	m_inorPixel_b[0] = m_imgHei*0.5;
	m_inorPixel_b[1] = 0;
	m_inorPixel_b[2] = -1.0/m_pixelY;

}

void _iphCamera::FiducialMarkCoordToCalibrated( double xf, double yf, double *xc, double *yc )
{
	*xc = xf - m_x0;
	*yc = yf - m_y0;

	// 径向畸变
	if( m_k1 != 0 )	{
		double dx, dy, x2, y2, xy, r2, r;
		double dr;

		x2 = *xc* *xc;	xy = *xc * *yc;	y2 = *yc * *yc;
		r2 = x2 + y2;
		//			r=sqrt(r2);


		//Dx=x*(k1*r^2+k2*r^4+k3*r^6+...)
		//Dy=y*(k1*r^2+k2*r^4+k3*r^6+...)

		//			dr=( m_k0 + ( m_k1+ ( m_k2 + m_k3*r)*r )*r);
		//			dr = m_k0*r + m_k1*r2 +  m_k2 *r2*r2;
		dr=( m_k0 + ( m_k1+ ( m_k2 + m_k3*r2)*r2 )*r2);

		dx = *xc * dr;
		dy = *yc * dr;

		// 切向畸变
		if( m_p1 != 0 )	{
			dx += m_p1*( r2 + 2*x2 ) + 2*m_p2*xy;
			dy += 2*m_p1*xy + m_p2*( r2 + 2*y2 );
		}

		*xc += dx;
		*yc += dy;

// 			*xc -= dx;
// 			*yc -= dy;

	}

}

void _iphCamera::CalibratedToFiducialMarkCoord( double xc, double yc, double *xf, double *yf )
{
	*xf = xc;
	*yf = yc;

	double dxTh;
	double dyTh;	

	if(m_pixelX == 0)
	{
		dxTh=m_inorImage_a[1]*0.1;	//cam->pixelX;
		dyTh=-m_inorImage_b[2]*0.1;	//-cam->pixelY;
	}
	else
	{
		dxTh = dyTh = 0.1*m_pixelX;
	}

	if( m_k1 != 0 )	
	{	
		// 径向畸变
		double dx, dy, dr, r2 = xc*xc + yc*yc, r;
		double x2, xy, y2;

		double dfx, dfy;

		while(1)
		{
			x2 = *xf* *xf;	xy = *xf * *yf;	y2 = *yf * *yf;
			r2 = *xf* *xf + *yf* *yf;
			//			r=sqrt(r2);

			//			dr = m_k0 + ( m_k1+ ( m_k2 + m_k3*r)*r )*r;
			//			dr = m_k0*r + m_k1*r2 +  m_k2 *r2*r2;
			dr=( m_k0 + ( m_k1+ ( m_k2 + m_k3*r2)*r2 )*r2);

			dx = *xf*dr;
			dy = *yf*dr;

			// 切向畸变
			if( m_p1 != 0 )	
			{
				dx += m_p1*( r2 + 2*x2 ) + 2*m_p2*xy;
				dy += 2*m_p1*xy + m_p2*( r2 + 2*y2 );
			}

			dfx = xc - ( *xf + dx);
			dfy = yc - ( *yf + dy );

			*xf = xc - dx;	
			*yf = yc - dy;

			// 			*xf = xc + dx;	
			// 			*yf = yc + dy;

			if( fabs(dfx)< dxTh && fabs(dfy)<dyTh)
				break;
		}
	}

	*xf += m_x0;
	*yf += m_y0;
}

void _iphCamera::ImageCoordinateCorrect(double xi, double yi, double &xi_cor, double &yi_cor)
{
	double xp, yp;

	Image2Photo( xi, yi, xp, yp);

	xi_cor = m_inorPixel_a[0] + m_inorPixel_a[1]*xp + m_inorPixel_a[2]*yp;
	yi_cor = m_inorPixel_b[0] + m_inorPixel_b[1]*xp + m_inorPixel_b[2]*yp;
}