// OPTransformation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "orsBase/orsUtil.h"
#include "orsBase/orsString.h"
#include "orsBase/orsIPlatform.h"
#include "orsGuiBase/orsIProcessMsgBar.h"
#include <vector>
#include "math.h"
#include "Geometry.h"
#include "matrix.h"
#include <fstream>

using namespace std;


//-KRT2P -i L:\test\P2KRT\imgList.txt -o L:\test\P2KRT
//-P2KRT -i L:\test\P2KRT\73-75.proj -o L:\test\P2KRT\test -imgDim 8184 6114 -pixelsize 0.006
void usage(bool wait=false)
{
	printf("OPTransformation V1.0: IDP @Liesmars \n");
	printf("usage:\n");
//	printf("OPTransformation -pos pos.txt -proj output_directory \n");	//POS文件转投影矩阵
	printf("OPTransformation -KRT2P -i imgList.txt -o output_directory \n"); //影像定向文件转投影矩阵
	printf("OPTransformation -P2KRT -i proj.txt -o output_directory -imgDim wid hei -pixelsize 0.006 -\n");	//投影矩阵文件转影像定向文件  像素大小(mm)
	printf("---------------------------------------------------\n");
	if (wait)
	{
		printf("<press ENTER>\n");
		getc(stdin);
	}
	exit(1);
}

static void byebye(bool wait=false)
{
	if (wait)
	{
		fprintf(stderr,"<press ENTER>\n");
		getc(stdin);
	}
	exit(1);
}

struct camera_params
{
	orsString  imgName;				//image name without folder name
	
	double f;
	double x0, y0;
	double k0, k1, k2, k3;
	double p1, p2;

	double imgWid, imgHei;
	double pixelWid, pixelHei;

	double Xs, Ys, Zs;
	double phi, omega, kappa;		//radian
	double R[9];

	double P[12];

	camera_params()
	{
		k0=k1=k2=k3=0;
		p1=p2=0;
	};
};



/* allow a comment starting with '#' to end-of-line.  
 * Skip white space including any comments. */
void skip_comments(FILE *fp)
{
	int ch;

	fscanf(fp," ");      /* Skip white space. */
	while ((ch = fgetc(fp)) == '#') {
		while ((ch = fgetc(fp)) != '\n'  &&  ch != EOF)
			;
		fscanf(fp," ");
	}
	ungetc(ch, fp);      /* Replace last character read. */
}

orsIPlatform *g_pPlatform = NULL;

orsIPlatform *getPlatform()
{
	return g_pPlatform;
}

#define MAX_STRING 256

//读取内外方位元素文件
bool LoadKRTParam( const orsChar *imageFilePath, camera_params *element)
{
	char pLine[MAX_STRING];
//	CString aopName;
	orsString aopName;
//	FILE *fp;
	if (!imageFilePath )
	{
		return false;
	}

	orsString imageFileName = imageFilePath;
	element->imgName = imageFilePath;

//	imageFileName
	// 去掉后缀
	aopName = imageFileName.left( imageFileName.reverseFind('.') );

	aopName += ".aop.txt";
	{
		ifstream in;
// 		istrstream inp;
		in.open(aopName, ios::in );

		if( ! in.is_open() )
		{
			in.close();
			in.clear();

			aopName = imageFileName + ".aop";
			
			in.open(aopName, ios::in );
			if(! in.is_open())
				return false;
		}

//		double omega, phi, kapa;
		double minZ, maxZ, meanZ;
		while( !in.eof() ) {
			in.getline(pLine , MAX_STRING , '\n' );
			if( strstr( pLine, "[Xs,Ys,Zs]")  ) {
				in >> element->Xs >> element->Ys >> element->Zs;
			}
			else if( strstr( pLine, "[Rotation Axis]"  ) ) {
				in.getline(pLine , MAX_STRING , '\n' );
				char strAxis[20];

				sscanf( pLine, "%s", strAxis );
				
				orsString::findSubStr_i( pLine, "ZXY");
			}

			else if( strstr( pLine, "[Rotation Angle]"  ) ) {
				in >>  element->phi >> element->omega >> element->kappa;
			}
			else if( strstr( pLine, "[Rotation Matrix]"  ) ) {
				char fStr[80];
				for( int i=0; i<9; i++)
				{
					in >> fStr;
					element->R[i] = atof( fStr );
				}
			}
			else if( strstr( pLine, "[Elevation Range]"  ) ) {
				in >> minZ >> meanZ >> maxZ;

				break;
			}
		};

		in.close();
	}


	//////////////////////////////
	strcpy( pLine, imageFilePath );

	orsString iopName = imageFileName.left( imageFileName.reverseFind('.') );
	iopName += ".iop.txt";

	orsString cmrName;
	{
		ifstream in;
		in.open( iopName, ios::in );

		if( ! in.is_open() )  
		{
			in.close();
			in.clear();

		//	orsString msg = "Can not Open:" + iopName;
		//	AfxMessageBox( msg );
			iopName=imageFileName + ".iop";

			in.open( iopName, ios::in );
			if( ! in.is_open() )
				return false;
		}

		double inorPixel_a[3], inorPixel_b[3];
		double inorImage_a[3], inorImage_b[3];
		while( !in.eof() ) {
			in.getline(pLine, MAX_STRING);
			if( strstr( pLine, "[Camera Calibration File]") ) {
				in.getline(pLine, MAX_STRING);
				cmrName = pLine;
			}
			else if( strstr( pLine, "[Image to Pixel Parameters]" ) ) {
				char fStr[80];
				int i;

				for( i=0; i<3; i++)
				{
					in >> fStr;
					inorPixel_a[i] = atof( fStr );
				}

				for( i=0; i<3; i++)
				{
					in >> fStr;
					inorPixel_b[i] = atof( fStr );
				}

				// 	m_inorPixel_a[0] = imgCol*1.0/2;
				// 	m_inorPixel_b[0] = imgRow*1.0/2;

				element->imgWid = inorPixel_a[0] * 2;
				element->imgHei = inorPixel_b[0] * 2;
			}
			else if( strstr( pLine, "[Pixel to Image Parameters]" ) ) {
				char fStr[80];
				int i;

				for( i=0; i<3; i++)
				{
					in >> fStr;
					inorImage_a[i] = atof( fStr );
				}
				for( i=0; i<3; i++)
				{
					in >> fStr;
					inorImage_b[i] = atof( fStr );
				}

				//inorImage_a[1] = cam.m_pixelX;
				//inorImage_b[2] = -cam.m_pixelY;

				element->pixelWid = inorImage_a[1];
				element->pixelHei = -inorImage_b[2];
//				break;
			}
			else if(strstr( pLine, "[x0, y0]" ))
			{
				char fStr[80];
				in >> fStr;
				element->x0 = atof(fStr);
				in >> fStr;
				element->y0 = atof(fStr);
			}
			else if(strstr( pLine, "[focal length]" ))
			{
				char fStr[80];
				in >> fStr;
				element->f = atof(fStr);
			}
			else if(strstr( pLine, "[radial distortion: k1,k2,k3]" ))
			{
				char fStr[80];
				in >> fStr;
				element->k1 = atof(fStr);
				in >> fStr;
				element->k2 = atof(fStr);
				in >> fStr;
				element->k3 = atof(fStr);

			}
			else if(strstr( pLine, "[radial distortion: k0,k1,k2]" ))
			{
				char fStr[80];
				in >> fStr;
				element->k0 = atof(fStr);
				in >> fStr;
				element->k1 = atof(fStr);
				in >> fStr;
				element->k2 = atof(fStr);
			}
			else if(strstr( pLine, "[radial distortion: k0,k1,k2,k3]" ))
			{
				char fStr[80];
				in >> fStr;
				element->k0 = atof(fStr);
				in >> fStr;
				element->k1 = atof(fStr);
				in >> fStr;
				element->k2 = atof(fStr);
				in >> fStr;
				element->k3 = atof(fStr);
			}
			else if(strstr( pLine, "[tangential distortion: p1,p2]" ))
			{
				char fStr[80];
				in >> fStr;
				element->p1 = atof(fStr);
				in >> fStr;
				element->p2 = atof(fStr);
			}
		};

		in.close();

	}

	return true;
}

//读取投影矩阵文件
bool LoadProjParam(const orsChar *projFilePath, std::vector<camera_params>	&camList)
{
	FILE *fin=NULL;

	fin=fopen(projFilePath, "rt");
	if(fin==NULL)
	{
		return false;
	}

	skip_comments(fin);

	char pLine[256];
	camera_params element;

	while(feof(fin)==0)
	{
		fscanf(fin, "%s", pLine);

		element.imgName = pLine;

		for( int i=0; i<12; i++)
		{
			fscanf(fin, "%lf", element.P+i);
			
		}
		fscanf(fin, "\n");

		camList.push_back(element);
	}

	fclose(fin);	
	fin=NULL;

	return true;
}

void WriteProjFile(char *outputDir, std::vector<camera_params> &cameras)
{
	int num_cameras = (int) cameras.size();

	orsString pOutput = outputDir;

	pOutput = pOutput + "\\prep_pmvs.sh";

	FILE *f_scr = fopen(pOutput, "w");

	fprintf(f_scr, "# Script for preparing images and calibration data \n"
		"#   for Yasutaka Furukawa's PMVS system\n\n");
	fprintf(f_scr, "# Apply radial undistortion to the images\n");
//	fprintf(f_scr, "RadialUndistort %s %s\n", list_file, bundle_file);
	fprintf(f_scr, "\n# Create directory structure\n");
	fprintf(f_scr, "mkdir -p pmvs/\n");
	fprintf(f_scr, "mkdir -p pmvs/txt/\n");
	fprintf(f_scr, "mkdir -p pmvs/visualize/\n");
	fprintf(f_scr, "mkdir -p pmvs/models/\n");
	fprintf(f_scr, "\n# Copy and rename files\n");

	int count = 0;
	for (int i = 0; i < num_cameras; i++) 
	{
		if (cameras[i].f == 0.0)
			continue;

		orsString imgName=cameras[i].imgName;

		int pos=imgName.reverseFind('.');
		if(pos>0)
			imgName=imgName.left(pos);

		pos=imgName.reverseFind('\\');
		if(pos>0)
			imgName=imgName.right(imgName.length()-pos-1);


		orsString projName=outputDir;
		projName+="\\";
		projName+=imgName;
		projName+=".txt";	


		FILE *f = fopen(projName, "w");
		assert(f);

		fprintf(f, "CONTOUR\n");
		fprintf(f, "%0.6f %0.6f %0.6f %0.6f\n", cameras[i].P[0], cameras[i].P[1], cameras[i].P[2],  cameras[i].P[3]);
		fprintf(f, "%0.6f %0.6f %0.6f %0.6f\n", cameras[i].P[4], cameras[i].P[5], cameras[i].P[6],  cameras[i].P[7]);
		fprintf(f, "%0.6f %0.6f %0.6f %0.6f\n", cameras[i].P[8], cameras[i].P[9], cameras[i].P[10], cameras[i].P[11]);

		fclose(f);

	}

	fprintf(f_scr, "\n# Sample commands for running pmvs:\n");
	fprintf(f_scr, "#   affine %d pmvs/ 4\n", count);
	fprintf(f_scr, "#   match %d pmvs/ 2 0 0 1 0.7 5\n", count);

	fclose(f_scr);
}

void Writeaop_iopFile(char *outputDir, std::vector<camera_params> &cameras)
{
	int i, j, k;
	camera_params element;
	FILE *faop=NULL, *fiop=NULL;
	double inorImage_a[3], inorImage_b[3], inorPixel_a[3], inorPixel_b[3];
	double imgWid, imgHei;

	for(i=0; i<cameras.size(); i++)
	{
		element=cameras[i];
		orsString imgName=element.imgName;

		int pos=imgName.reverseFind('.');
		if(pos>0)
			imgName=imgName.left(pos);

		pos=imgName.reverseFind('\\');
		if(pos>0)
			imgName=imgName.right(imgName.length()-pos-1);

		orsString aopName=outputDir;
		aopName+="\\";
		aopName+=imgName;
		aopName+=".aop.txt";	

		//export aop
		faop=fopen(aopName, "wt");
		if(faop==0)
			continue;

		fprintf(faop,  "[Xs,Ys,Zs]\n");
		fprintf(faop,  "%13.3lf %13.3lf %10.3lf\n\n", element.Xs, element.Ys, element.Zs );

		fprintf(faop,  "[Rotation Angle]\n");
		fprintf(faop,  "%13.9lf %13.9lf %13.9lf\n\n", element.phi, element.omega, element.kappa );
		fprintf(faop,  "[Rotation Matrix]\n"  );
		for( k=0; k<3; k++) 
		{
			for( j=0; j<3; j++)
			{
				fprintf(faop,  "%10.7lf ", element.R[k*3+j] );
			}
			fprintf(faop, "\n");
		}

		fprintf(faop,  "\n[Elevation Range]\n"  );
		fprintf(faop,  "%d %d %d\n", 0, 50, 100);

		fclose(faop);	faop=NULL;

		//export iop
		imgWid = element.imgWid;
		imgHei = element.imgHei;

		//x方向
		inorImage_a[0] = -imgWid*element.pixelWid/2;
		inorImage_a[1] = element.pixelWid;
		inorImage_a[2] = 0;	
		//y方向
		inorImage_b[0] = imgHei*element.pixelHei/2;
		inorImage_b[1] = 0;
		inorImage_b[2] = -element.pixelHei;

		inorPixel_a[0] = imgWid*1.0/2;		//保证不丢失精度
		inorPixel_a[1] = 1.0/element.pixelWid;
		inorPixel_a[2] = 0;

		inorPixel_b[0] = imgHei*1.0/2;
		inorPixel_b[1] = 0;
		inorPixel_b[2] = -1.0/element.pixelHei;

		orsString iopName=outputDir;

		iopName+="\\";
		iopName+=imgName;
		iopName+=".iop.txt";	
		
		fiop=fopen(iopName, "wt");
		if(fiop==0)
			continue;

		fprintf(fiop, "LMARSDPS  INOR file: V1.0\n\n");

		fprintf( fiop, "[Image to Pixel Parameters]\n" );
		fprintf( fiop, "%e\t%e\t%e\n", inorPixel_a[0], inorPixel_a[1], inorPixel_a[2] );
		fprintf( fiop, "%e\t%e\t%e\n", inorPixel_b[0], inorPixel_b[1], inorPixel_b[2] );

		fprintf( fiop, "\n[Pixel to Image Parameters]\n" );
		fprintf( fiop, "%e\t%e\t%e\n", inorImage_a[0], inorImage_a[1], inorImage_a[2] );
		fprintf( fiop, "%e\t%e\t%e\n", inorImage_b[0], inorImage_b[1], inorImage_b[2] );

		fprintf( fiop, "\n[x0, y0]\n");
		fprintf(fiop, "%.6lf %.6lf\n\n", element.x0, element.y0);
		fprintf(fiop, "[focal length]\n");
		fprintf(fiop, "%.6lf\n\n", element.f);
		fprintf(fiop, "[radial distortion: k0,k1,k2,k3]\n");
		fprintf(fiop, "%e\t%e\t%e\t%e\n\n", element.k0, element.k1, element.k2, element.k3);
		fprintf(fiop, "[tangential distortion: p1,p2]\n");
		fprintf(fiop, "%e\t%e\n\n", element.p1, element.p2);

		fclose(fiop); fiop=NULL;
	}

}

void KRT_To_P(std::vector<camera_params> &cameras)
{

	camera_params element;
	
	for(int i=0; i<cameras.size(); i++)
	{
		element = cameras[i];

		double K[9] = 
		{ element.f/element.pixelWid, 0.0, element.x0/element.pixelWid,
		0.0, element.f/element.pixelHei, element.y0/element.pixelHei,
		0.0, 0.0, 1.0 };	//fx=f/dx, 主点在像片中心（不考虑主点偏移）

		double Ptmp[12] = 
		{ element.R[0], element.R[1], element.R[2], element.Xs,
		element.R[3], element.R[4], element.R[5], element.Ys,
		element.R[6], element.R[7], element.R[8], element.Zs };  //[R, C] = R[I, C] 

		double P[12];
		matrix_product(3, 3, 3, 4, K, Ptmp, P);
		matrix_scale(3, 4, P, -1.0, cameras[i].P);
	}

};

void RQDecomp3x3( const double *tmpMatrixM, double *calibMatr, double *rotMatr,
               double *rotMatrX, double *rotMatrY, double *rotMatrZ, double *eulerAngles)
{
	double M[9], R[9], Q[9];  
	double z, c, s;


	memcpy(M, tmpMatrixM, sizeof(double)*9);

    /* Find Givens rotation Q_x for x axis (left multiplication). */
    /*
         ( 1  0  0 )
    Qx = ( 0  c  s ), c = m33/sqrt(m32^2 + m33^2), s = m32/sqrt(m32^2 + m33^2)
         ( 0 -s  c )
    */
    s =	M[2*3+1];	//M[2][1];
    c =	M[2*3+2];			//M[2][2];
    z = 1./sqrt(c * c + s * s + DBL_EPSILON);
    c *= z;
    s *= z;

    double Qx[9] = { 1, 0, 0, 0, c, s, 0, -s, c };

	matrix_product(3, 3, 3, 3, M, Qx, R);

    assert(fabs(R[2*3+1]) < FLT_EPSILON);
    R[2*3+1] = 0;

    /* Find Givens rotation for y axis. */
    /*
         ( c  0 -s )
    Qy = ( 0  1  0 ), c = m33/sqrt(m31^2 + m33^2), s = -m31/sqrt(m31^2 + m33^2)
         ( s  0  c )
    */
    s =	-R[2*3];		//-R[2][0];
    c =	R[2*3+2];		//R[2][2];
    z = 1./sqrt(c * c + s * s + DBL_EPSILON);
    c *= z;
    s *= z;

    double Qy[9] = { c, 0, -s, 0, 1, 0, s, 0, c };
    
	matrix_product(3, 3, 3, 3, R, Qy, M);
   
    assert(fabs(M[2*3]) < FLT_EPSILON);
    M[2*3] = 0;

    /* Find Givens rotation for z axis. */
    /*
         ( c  s  0 )
    Qz = (-s  c  0 ), c = m22/sqrt(m21^2 + m22^2), s = m21/sqrt(m21^2 + m22^2)
         ( 0  0  1 )
    */

    s =	M[1*3];		//M[1][0];
    c =	M[1*3+1];		//M[1][1];
    z = 1./sqrt(c * c + s * s + DBL_EPSILON);
    c *= z;
    s *= z;

    double Qz[9] = { c, s, 0, -s, c, 0, 0, 0, 1 };
    
	matrix_product(3, 3, 3, 3, M, Qz, R);
    assert(fabs(R[1*3]) < FLT_EPSILON);
    R[1*3] = 0;

    // Solve the decomposition ambiguity.
    // Diagonal entries of R, except the last one, shall be positive.
    // Further rotate R by 180 degree if necessary
    if( R[0] < 0 )
    {
        if( R[1*3+1] < 0 )
        {
            // rotate around z for 180 degree, i.e. a rotation matrix of
            // [-1,  0,  0],
            // [ 0, -1,  0],
            // [ 0,  0,  1]
            
			R[0] *= -1;	//R[0][0] *= -1;
            R[1] *= -1;	//R[0][1] *= -1;
            R[1*3+1] *= -1;//R[1][1] *= -1;

            Qz[0] *= -1;//Qz[0][0] *= -1;
            Qz[1] *= -1;//Qz[0][1] *= -1;
            Qz[1*3] *= -1;//Qz[1][0] *= -1;
            Qz[1*3+1] *= -1;//Qz[1][1] *= -1;
        }
        else
        {
            // rotate around y for 180 degree, i.e. a rotation matrix of
            // [-1,  0,  0],
            // [ 0,  1,  0],
            // [ 0,  0, -1]
            R[0] *= -1;//R[0][0] *= -1;
            R[2] *= -1;//R[0][2] *= -1;
            R[1*3+2] *= -1;//R[1][2] *= -1;
            R[2*3+2] *= -1;//R[2][2] *= -1;

			matrix_transpose(3, 3, Qz, Qz);
//            cvTranspose( &Qz, &Qz );

            Qy[0] *= -1;//Qy[0][0] *= -1;
            Qy[2] *= -1;//Qy[0][2] *= -1;
            Qy[2*3] *= -1;//Qy[2][0] *= -1;
            Qy[2*3+2] *= -1;//Qy[2][2] *= -1;
        }
    }
    else if( R[1*3+1] < 0 )
    {
        // ??? for some reason, we never get here ???

        // rotate around x for 180 degree, i.e. a rotation matrix of
        // [ 1,  0,  0],
        // [ 0, -1,  0],
        // [ 0,  0, -1]
        R[1] *= -1;//R[0][1] *= -1;
        R[2] *= -1;//R[0][2] *= -1;
        R[1*3+1] *= -1;//R[1][1] *= -1;
        R[1*3+2] *= -1;//R[1][2] *= -1;
        R[2*3+2] *= -1;//R[2][2] *= -1;

        matrix_transpose(3, 3, Qz, Qz);
		matrix_transpose(3, 3, Qy, Qy);
//		cvTranspose( &Qz, &Qz );
//      cvTranspose( &Qy, &Qy );

        Qx[1*3+1] *= -1;//Qx[1][1] *= -1;
        Qx[1*3+2] *= -1;//Qx[1][2] *= -1;
        Qx[2*3+1] *= -1;//Qx[2][1] *= -1;
        Qx[2*3+2] *= -1;//Qx[2][2] *= -1;
    }

    // calculate the euler angle
    if( eulerAngles )
    {
        eulerAngles[0] = acos(Qx[1*3+1]) * (Qx[1*3+2] >= 0 ? 1 : -1) * (180.0 / PI);
        eulerAngles[1] = acos(Qy[0]) * (Qy[2*3] >= 0 ? 1 : -1) * (180.0 / PI);
        eulerAngles[2] = acos(Qz[0]) * (Qz[1] >= 0 ? 1 : -1) * (180.0 / PI);
    }

    /* Calulate orthogonal matrix. */
    /*
    Q = QzT * QyT * QxT
    */
	double QxT[9], QyT[9], QzT[9];
	matrix_transpose(3, 3, Qx, QxT);
	matrix_transpose(3, 3, Qy, QyT);
	matrix_transpose(3, 3, Qz, QzT);

	matrix_product(3, 3, 3, 3, QzT, QyT, M);
	matrix_product(3, 3, 3, 3, M, QxT, Q);
    
// 	cvGEMM( &Qz, &Qy, 1, 0, 0, &M, CV_GEMM_A_T + CV_GEMM_B_T );
//  cvGEMM( &M, &Qx, 1, 0, 0, &Q, CV_GEMM_B_T );

    /* Save R and Q matrices. */
	memcpy(calibMatr, R, sizeof(double)*9);
	memcpy(rotMatr, Q, sizeof(double)*9);

//    cvConvert( &R, calibMatr );
//    cvConvert( &Q, rotMatr );

    if( rotMatrX )
        memcpy(rotMatrX, Qx, sizeof(double)*9);
		//cvConvert(&Qx, rotMatrX);
    if( rotMatrY )
		memcpy(rotMatrY, Qy, sizeof(double)*9);
        //cvConvert(&Qy, rotMatrY);
    if( rotMatrZ )
		memcpy(rotMatrZ, Qz, sizeof(double)*9);
        //cvConvert(&Qz, rotMatrZ);
}


//CvMat* P, CvMat* K, CvMat* R, CvMat* T
void P_To_KRT_QR(std::vector<camera_params> &cameras, int imgWid, int imgHei, double pixelX)
{
	camera_params element;
	double K[9], R[9], T[3];

	for(int iter=0; iter<cameras.size(); iter++)
	{
		element = cameras[iter];

//		double tmpProjMatrData[16], tmpMatrixDData[16], tmpMatrixVData[16];
		double tmpProjMatr[16];
		double tmpMatrixD[16];
		double tmpMatrixV[16];
		double tmpMatrixU[9];
		double *projMatr=NULL;
		double posVect[4];

		double tmpMatrixM[9];

		projMatr = element.P;

		int i,j, k;
		/* Compute position vector. */
// 		cvSetZero(&tmpProjMatr); // Add zero row to make matrix square.
// 		int i, k;
// 		for(i = 0; i < 3; i++)
// 			for(k = 0; k < 4; k++)
// 				cvmSet(&tmpProjMatr, i, k, cvmGet(projMatr, i, k));

		memset(tmpProjMatr, 0, sizeof(double)*16);
		for(i = 0; i < 3; i++)
			for(k = 0; k < 4; k++)
				tmpProjMatr[i*4+k] = projMatr[i*4+k];

		dgesvd_driver(3, 4, tmpProjMatr, tmpMatrixD, tmpMatrixU, tmpMatrixV);

//		cvSVD(&tmpProjMatr, &tmpMatrixD, NULL, &tmpMatrixV, CV_SVD_MODIFY_A + CV_SVD_V_T);

		/* Save position vector. */
		for(i = 0; i < 4; i++)
			posVect[i] = tmpMatrixV[3*4+i]; // Solution is last row of V.

		/* Compute calibration and rotation matrices via RQ decomposition. */
//		cvGetCols(projMatr, &tmpMatrixM, 0, 3); // M is first square matrix of P.
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				tmpMatrixM[i*3+j]=projMatr[i*4+j];
			}
		}

		//CV_Assert(cvDet(&tmpMatrixM) != 0.0); // So far only finite cameras could be decomposed, so M has to be nonsingular [det(M) != 0].

		RQDecomp3x3(tmpMatrixM, K, R, NULL, NULL, NULL, NULL);

		assert(fabs(posVect[3]) > FLT_EPSILON);
		//matrix_scale();

		
		cameras[iter].Xs = posVect[0]/posVect[3];
		cameras[iter].Ys = posVect[1]/posVect[3];
		cameras[iter].Zs = posVect[2]/posVect[3];

		R2fwk(R, &(cameras[iter].phi), &(cameras[iter].omega), &(cameras[iter].kappa));
		memcpy(cameras[iter].R, R, sizeof(double)*9);
		
		cameras[iter].pixelHei = cameras[iter].pixelWid = pixelX;
		cameras[iter].imgWid = imgWid;
		cameras[iter].imgHei = imgHei;

		cameras[iter].f = K[0]*pixelX;
		cameras[iter].x0 = K[2]*pixelX;
		cameras[iter].y0 = K[5]*pixelX;
	}

}


int _tmain(int argc, _TCHAR* argv[])
{
	int i, j, k;
	double step=1;
	char* pOutputDir=NULL;
	char* pInput=NULL;
	int	 KRT2P=1;	//1， KRT转投影矩阵； 0，投影矩阵转KRT
	double pixelX;
	int imgWid, imgHei;


	if (strcmp(argv[1],"-h") == 0)
	{
		usage(true);
	}

	for (i = 1; i < argc; i++)
	{

		if(strcmp(argv[i],"-KRT2P") == 0)
		{
			KRT2P = 1;
		}
		else if(strcmp(argv[i],"-P2KRT") == 0)
		{
			KRT2P = 0;
		}
		else if(strcmp(argv[i],"-i") == 0)
		{
			i++;
			pInput=argv[i];
		}
		else if(strcmp(argv[i],"-o") == 0)
		{
			i++;
			pOutputDir=argv[i];
		}
		else if(strcmp(argv[i],"-pixelsize") == 0)
		{
			i++;
			pixelX = atof(argv[i]);
		}
		else if(strcmp(argv[i],"-imgDim") == 0)
		{
			i++;
			imgWid = atoi(argv[i]);
			i++;
			imgHei = atof(argv[i]);
		}
		else if(strcmp(argv[i],"-imgWid") == 0)
		{
			i++;
			imgWid = atoi(argv[i]);
		}
		else if(strcmp(argv[i],"-imgHei") == 0)
		{
			i++;
			imgHei = atof(argv[i]);
		}
	}


	std::vector<camera_params> camList;
	camera_params element;
	if(KRT2P)
	{
		FILE *fp=NULL;

		fp=fopen(pInput, "r");

		char pLine[256];
		while(feof(fp)==0)
		{
			fscanf(fp, "%s\n", pLine);

			LoadKRTParam( pLine, &element);
			camList.push_back(element);
		}
		fclose(fp);
		
		KRT_To_P(camList);
		WriteProjFile(pOutputDir, camList);
	}
	else 
	{
		LoadProjParam(pInput, camList);
		
		P_To_KRT_QR(camList, imgWid, imgHei, pixelX);
		Writeaop_iopFile(pOutputDir, camList);
	}
	

	fprintf(stderr,"==========>  ok!  <===========\n");

FLAG:
	byebye(argc==1);
	return 0;
}

