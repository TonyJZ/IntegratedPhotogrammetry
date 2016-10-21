#include "StdAfx.h"
#include "ImageCalib.h"
//#include "lastools/CPFReadWrite.h"
#include "Calib_Camera.h"
#include "orsBase/orsTypedef.h"
#include "orsBase/orsString.h"
#include "Align_def.h"
#include "Geometry.h"
#include "bundler/matrix.h"
#include "math.h"
#include "ransac_projection.h"
//#include "Transform.h"
#include "\OpenRS\IntegratedPhotogrammetry\calibration\ATNLib\ATNPointIndex.h"

#include <fstream>
using namespace std;


bool g_bRotateBack = false;
double g_imgHei = 0;


#ifndef MAX_STRING
#define MAX_STRING	512
#endif

static Misalign_Param imgMisalign;
static ors_int32 *pLut=0;	//每张像片上的投影点查找表，记录各片投影点数
static ors_int32 *pOffset=0;	//当前像片上的偏移

static ors_int32 posNum=0;
static imgExOrientParam *pExorSequen=0;	//pos序列
static CATNPtIndex *pLiDIdx=NULL;


#define smallFloat 1e-10
void eliminate(double *A,double *b,int ii,int n);
int findlm(double *A,int ic,int n,double *maxx);
void exlij(double *A,double *b,int li,int lj,int n);


int Gauss(double *A,double *b,int n)
{
	int i,j,lm;
	double maxx;

	/*	first step:  eliminatio of row main element */

	for (i=0; i<n-1; i++)
	{
		lm = findlm(A,i,n,&maxx);

		if( fabs(maxx) < smallFloat )
			return 0;

		if ( lm )
			exlij(A,b,i,lm,n);

		eliminate(A,b,i,n);
		A += n+1;
		b++;
	}

	/*	second step:  backward solution */

	if( fabs( *A ) < smallFloat )
		return 0;

	*b /= *A;
	for (i=1; i<n; i++)
	{
		A -= n+1;
		b--;
		for (j=1; j<i+1; j++)
			*b -= A[j] * b[j];

		if( fabs( *A ) < smallFloat )
			return 0;

		*b /= *A;
	}

	return(1);
}

void eliminate(double *A,double *b,int ii,int n)
{
	int i,j;
	double fac,b0,*line,*row;

	line = A + n;
	b0 = *b++;
	for ( i=ii+1; i<n; i++)
	{
		row = A+1;
		fac = *line++ / *A;
		for ( j = ii + 1; j<n; j++)
			*line++ -= *row++ * fac;
		line += ii;
		*b++ -= b0 * fac;
	}
}

int findlm(double *A,int ic,int n,double *maxx)
{
	int i,im=ic;

	*maxx = fabs(*A);
	if ( *maxx > 1 ) return 0;
	for ( i=ic+1; i<n; i++)
	{
		A += n;
		if ( fabs(*A) > *maxx )
		{
			*maxx = fabs(*A);
			im=i;
		}
	}

	if (im == ic )	return(0);
	else		return(im);
}

void exlij(double *A,double *b,int li,int lj,int n)
{
	int i,j;
	double t,*l;

	i = lj-li;
	l = A + i * n;
	for ( j = li; j<n; j++)
		t = *A,	*A++ = *l, *l++ = t;
	t = *b,	*b = b[i];
	b[i] = t;
}


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

bool InitializeCamera(const char *pImgName, _iphCamera *pCamera)
{
	char pLine[MAX_STRING];
	orsString aopName;
	if (!pImgName )
	{
		return false;
	}

	orsString imageFileName = pImgName;
	orsString pureImageFileName;

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
			
			if(! in.is_open())
				return false;
		}

		double omega, phi, kapa;
		double minZ, meanZ, maxZ;
		while( !in.eof() ) {
			in.getline(pLine , MAX_STRING , '\n' );
			if( strstr( pLine, "[Xs,Ys,Zs]")  ) {
				in >> pCamera->m_Xs >> pCamera->m_Ys >> pCamera->m_Zs;
//				char c_Xs[12],c_Ys[10],c_Zs[9];
//				in>>c_Xs >> c_Ys >> c_Zs ;
//				m_Xs = atof(c_Xs);
//				m_Ys = atof(c_Ys );
//				m_Zs =atof(c_Zs);
			}
			else if( strstr( pLine, "[Rotation Axis]"  ) ) {
				in.getline(pLine , MAX_STRING , '\n' );
				char strAxis[20];

				sscanf( pLine, "%s", strAxis );
				
				orsString::findSubStr_i( pLine, "ZXY");
			}

			else if( strstr( pLine, "[Rotation Angle]"  ) ) {
				in >>  omega >> phi >> kapa;
			}
			else if( strstr( pLine, "[Rotation Matrix]"  ) ) {
				char fStr[80];
				for( int i=0; i<9; i++)
				{
					in >> fStr;
					pCamera->m_RMatrix[i] = atof( fStr );
				}
			}
			else if( strstr( pLine, "[GPS time]"  ) )
			{
				char fStr[80];
				in >> fStr;
				pCamera->m_GPSTime = atof( fStr );
			}
			else if( strstr( pLine, "[Elevation Range]"  ) ) {
				in >> minZ >> meanZ >> maxZ;
				break;
			}
			
		};

		in.close();
	}


	//////////////////////////////
	strcpy( pLine, pImgName );

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
			if( ! in.is_open() )
				return false;
		}

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
					pCamera->m_inorPixel_a[i] = atof( fStr );
				}

				for( i=0; i<3; i++)
				{
					in >> fStr;
					pCamera->m_inorPixel_b[i] = atof( fStr );
				}
			}
			else if( strstr( pLine, "[Pixel to Image Parameters]" ) ) {
				char fStr[80];
				int i;

				for( i=0; i<3; i++)
				{
					in >> fStr;
					pCamera->m_inorImage_a[i] = atof( fStr );
				}
				for( i=0; i<3; i++)
				{
					in >> fStr;
					pCamera->m_inorImage_b[i] = atof( fStr );
				}

//				break;
			}
			else if(strstr( pLine, "[x0, y0]" ))
			{
				char fStr[80];
				in >> fStr;
				pCamera->m_x0 = atof(fStr);
				in >> fStr;
				pCamera->m_y0 = atof(fStr);
			}
			else if(strstr( pLine, "[focal length]" ))
			{
				char fStr[80];
				in >> fStr;
				pCamera->m_f = atof(fStr);
			}
			else if(strstr( pLine, "[radial distortion: k1,k2,k3]" ))
			{
				char fStr[80];
				in >> fStr;
				pCamera->m_k1 = atof(fStr);
				in >> fStr;
				pCamera->m_k2 = atof(fStr);
				in >> fStr;
				pCamera->m_k3 = atof(fStr);

				//pCamera->m_clbType = ors_ctCOEFFICIENT;
			}
			else if(strstr( pLine, "[radial distortion: k0,k1,k2]" ))
			{
				char fStr[80];
				in >> fStr;
				pCamera->m_k0 = atof(fStr);
				in >> fStr;
				pCamera->m_k1 = atof(fStr);
				in >> fStr;
				pCamera->m_k2 = atof(fStr);

				//m_clbType = ors_ctCOEFFICIENT;
			}
			else if(strstr( pLine, "[radial distortion: k0,k1,k2,k3]" ))
			{
				char fStr[80];
				in >> fStr;
				pCamera->m_k0 = atof(fStr);
				in >> fStr;
				pCamera->m_k1 = atof(fStr);
				in >> fStr;
				pCamera->m_k2 = atof(fStr);
				in >> fStr;
				pCamera->m_k3 = atof(fStr);
				
				//m_clbType = ors_ctCOEFFICIENT;
			}
			else if(strstr( pLine, "[tangential distortion: p1,p2]" ))
			{
				char fStr[80];
				in >> fStr;
				pCamera->m_p1 = atof(fStr);
				in >> fStr;
				pCamera->m_p2 = atof(fStr);
			}
			else if(strstr( pLine, "[Pixel Size (mm)]"))
			{
				char fStr[80];
				in >> fStr;
				pCamera->m_pixelX = atof(fStr);
				in >> fStr;
				pCamera->m_pixelY = atof(fStr);
			}
		};

		// 旋转回到本体坐标系
		if( g_bRotateBack )	{
			double t;

			g_imgHei = 2*pCamera->m_inorPixel_b[0];

			t = pCamera->m_inorPixel_a[0];	pCamera->m_inorPixel_a[0] =-pCamera->m_inorPixel_b[0];	pCamera->m_inorPixel_b[0] = t;
			t = pCamera->m_inorPixel_a[1];	pCamera->m_inorPixel_a[1] =-pCamera->m_inorPixel_b[2];	pCamera->m_inorPixel_b[2] =-t;
			t = pCamera->m_inorPixel_a[2];	pCamera->m_inorPixel_a[2] = pCamera->m_inorPixel_b[1];	pCamera->m_inorPixel_b[1] = t;

			t = pCamera->m_inorImage_a[0];	pCamera->m_inorImage_a[0] =-pCamera->m_inorImage_b[0];	pCamera->m_inorImage_b[0] =-t;
			t = pCamera->m_inorImage_a[1];	pCamera->m_inorImage_a[1] =-pCamera->m_inorImage_b[2];	pCamera->m_inorImage_b[2] =-t;
			t = pCamera->m_inorImage_a[2];	pCamera->m_inorImage_a[2] = pCamera->m_inorImage_b[1];	pCamera->m_inorImage_b[1] = t;

		}


		in.close();

	}

	return true;
}



//多片前方交会计算物方坐标, 像点坐标的改正不在此函数中进行
//AX=L	最小二乘解
//				|a1f + a3x,   b1f + b3x,  c1f + c3x|
//			A=	|											  |				A=2*3    X=[X, Y, Z]^T
//				|a2f + a3y,   b2f + b3y,  c2f + c3y|
//
//              |(a1f+a3x)Xs + (b1f+b3x)Ys + (c1f+c3x)Zs |
//         L = |									                       |		L=2*1	
//              |(a2f+a3y)Xs + (b2f+b3y)Ys + (c2f+c3y)Zs |

int ForwardIntersctForTiePoints( std::vector<imgExOrientParam>  *exorParam, _iphCamera *camera, vector<AT_tiePoint> &pData, bool bZWithLidar )
{
	int nFrame=exorParam->size();
	int n3Dpoint=pData.size();

	double	*A=0, *L=0;
	double *ptr1, *ptr2, *ptr3;
	double R[9], Rmis[9], Rimu[9];
	double x, y;
	double a1, a2, a3, b1, b2, b3, c1, c2, c3;
	double Xgps, Ygps, Zgps;
	double Xs, Ys, Zs;
	double pX[3];
	double x0, y0,f;

	x0 = camera->m_x0;
	y0 = camera->m_y0;
	f  = camera->m_f;
//	f  = imgMisalign.f;

	A=(double*)malloc(2*3*nFrame*sizeof(double));
	L=(double*)malloc(2*nFrame*sizeof(double));

	RotateMat_fwk(imgMisalign.phi, imgMisalign.omega, imgMisalign.kappa, Rmis);

	for(int i=0; i<n3Dpoint; i++)
	{
		AT_tiePoint tiePt = pData[i];

		int nnz=tiePt.projs.size();
		for(int j=0; j<nnz; j++)
		{
			imgPoint imgPt=tiePt.projs[j];

			int imgID=imgPt.imgID;
			imgExOrientParam exor=(*exorParam)[imgID];

			Xgps=exor.Xs;	Ygps=exor.Ys;	Zgps=exor.Zs;	//线元素初值

			//像点坐标
			////////////////////// 主点改正 ////////////////////////////////////////
			x = imgPt.xi - camera->m_x0;			
			y = imgPt.yi - camera->m_y0;

			////////////////////// 畸变改正 ////////////////////////////////////////
			double r2 = x*x + y*y;

			// 径向
			double dx = x*( camera->m_k0 + r2*( camera->m_k1 + r2*( camera->m_k2 + r2*camera->m_k3 ) ) );
			double dy = y*( camera->m_k0 + r2*( camera->m_k1 + r2*( camera->m_k2 + r2*camera->m_k3 ) ) );

			// 切向，


			x += dx;
			y += dy;

			//////////////////////////////////////////////////////////////////////////

			ptr1=A+j*2*3;
			ptr2=L+j*2;

			RotateMat_fwk(exor.phi, exor.omega, exor.kappa, Rimu );

			Xs = Xgps + Rimu[0]*imgMisalign.u + Rimu[1]*imgMisalign.v + Rimu[2]*imgMisalign.w;
			Ys = Ygps + Rimu[3]*imgMisalign.u + Rimu[4]*imgMisalign.v + Rimu[5]*imgMisalign.w;
			Zs = Zgps + Rimu[6]*imgMisalign.u + Rimu[7]*imgMisalign.v + Rimu[8]*imgMisalign.w;

			matrix_product(3,3,3,3, Rimu, Rmis, R );

			a1=R[0];		a2=R[1];		a3=R[2];
			b1=R[3];		b2=R[4];		b3=R[5];
			c1=R[6];		c2=R[7];		c3=R[8];

			ptr1[0] = a1*f+a3*x;		ptr1[1] = b1*f+b3*x;		ptr1[2] = c1*f+c3*x;
			ptr1[3] = a2*f+a3*y;		ptr1[4] = b2*f+b3*y;		ptr1[5] = c2*f+c3*y;

			ptr2[0] = (a1*f+a3*x)*Xs+(b1*f+b3*x)*Ys+(c1*f+c3*x)*Zs;
			ptr2[1] = (a2*f+a3*y)*Xs+(b2*f+b3*y)*Ys+(c2*f+c3*y)*Zs;

		}
		dgelsy_driver(A, L, pX, 2*nnz, 3, 1);
		
		pData[i].X = pX[0];
		pData[i].Y = pX[1];
		pData[i].Z = pX[2];

		if( bZWithLidar )	{
			ATNPoint pt;
			if( pLiDIdx->QueryNNPoint(pX[0], pX[1], &pt) )
				pData[i].Z = pt.z;
		}
	}


FUNC_END:
	if(A)	free(A);
	if(L)	free(L);

	return	1;
}

static void ImageProject(double xg, double yg, double zg, double *xi, double *yi, 
						 double *R, double Xs, double Ys, double Zs, double f)
{
	double dx, dy, dz, u,v,w, x, y, landa;
	double *r = R;

	dx = xg - Xs;	dy = yg - Ys;	dz= zg - Zs;

	u = r[0] * dx + r[3] * dy + r[6]*dz;
	v = r[1] * dx + r[4] * dy + r[7]*dz;
	w = r[2] * dx + r[5] * dy + r[8]*dz;

	landa = -f / w;

	x = landa*u;
	y = landa*v;


	*xi=x;
	*yi=y;

	return ;
}

#include "orsBase/orsMatrix.h"

#include "orsMath/orsIMatrixService.h"

ORS_GET_MATRIX_SERVICE_IMPL();


//后方交会解算安装误差
double CalibrateWithForwardBackwardIntersection( std::vector<imgExOrientParam>  *exorParam, _iphCamera *camera, vector<AT_tiePoint> &pData, 
							 Proj_Point *projs, int nprojs, double projThresh)
{
	int ret=1;
	
	double	*A0=0, *L0=0;
	double *ptr1, *ptr2, *ptr3;
	double R[9], Rmis[9], Rimu[9];
	double x, y;
	double a1, a2, a3, b1, b2, b3, c1, c2, c3;
	double Xgps, Ygps, Zgps;
	double X, Y, Z;
	double _X, _Y, _Z;
	double pX[3];

	/////////////////////////////////////////////////
	int iteration=0;

	double thresh = 1e-12;	//0.1'
	double rmsVxy, minRmsVxy = 99999;
	double maxVx, maxVy;
	
	for( iteration=0; iteration<300; iteration++ )
	{
		int nxAngles = 3;	// max: 3
		int nxXyz = 2;		// max: 3
		int nxPrinciplePoint=0;	// max:2
		int nxPrincipleLength=0;	// max:1
		int nxRadiusDistorions = 0;	// max:4
		int nxTangentDistorions = 0;// max:2

		if( iteration > 20 )	{
			nxPrinciplePoint=2;	
			//nxPrincipleLength =1;
			nxRadiusDistorions = 4;
		}

		int ixXyz = nxAngles;
		int ixPP = ixXyz + nxXyz;
		int ixPL = ixPP + nxPrinciplePoint;
		int ixRD= ixPL + nxPrincipleLength;
		int ixTD = ixRD + nxRadiusDistorions;
		int nx = ixTD + nxTangentDistorions;

		ForwardIntersctForTiePoints( exorParam, camera, pData, false );		//前交

		int nFrame=exorParam->size();
		int n3Dpoint = pData.size();

		int i, j;
		
		memset(pOffset, 0, sizeof(ors_int32)*nFrame);

		for(i=0; i<n3Dpoint; i++)
		{
			AT_tiePoint tiePt = pData[i];
			for(j=0; j<tiePt.projs.size(); j++)
			{
				imgPoint imgpt=tiePt.projs[j];
				int imgID=imgpt.imgID;

				int offset=pLut[imgID]+pOffset[imgID];

				projs[offset].iXyz = i;

				projs[offset].xi=imgpt.xi;
				projs[offset].yi=imgpt.yi;

				pOffset[imgID]++;
			}
		}

		//////////////////////////////////////////////////////////////////////////

		RotateMat_fwk( imgMisalign.phi, imgMisalign.omega, imgMisalign.kappa, R);

		double cosf = cos( imgMisalign.phi ), sinf = sin(imgMisalign.phi);
		double cosw = cos( imgMisalign.omega), sinw=sin(imgMisalign.omega);
		double cosk = cos( imgMisalign.kappa), sink=sin(imgMisalign.kappa);

		double AA[225], AL[15], a[15], b[15], lx, ly;

		memset( AA, 0, nx*nx*sizeof(double));
		memset( AL, 0, nx*sizeof(double));

		rmsVxy = 0;
		maxVx = maxVy = 0;
		
		for(i=0; i<nFrame; i++)
		{
			Xgps= (*exorParam)[i].Xs;
			Ygps= (*exorParam)[i].Ys;
			Zgps= (*exorParam)[i].Zs;

			RotateMat_fwk( (*exorParam)[i].phi, (*exorParam)[i].omega, (*exorParam)[i].kappa, Rimu );
			
			for(j=pLut[i]; j<pLut[i+1]; j++)
			{
// 				if(pBuf1[j].flag!=keypoint_inlier)
// 					continue;

				int iXyz = projs[j].iXyz;

				AT_tiePoint tiePt = pData[iXyz];

				X = Rimu[0]*( tiePt.X -Xgps ) + Rimu[3]*(tiePt.Y-Ygps) + Rimu[6]*(tiePt.Z-Zgps);
				Y = Rimu[1]*( tiePt.X -Xgps ) + Rimu[4]*(tiePt.Y-Ygps) + Rimu[7]*(tiePt.Z-Zgps);
				Z = Rimu[2]*( tiePt.X -Xgps ) + Rimu[5]*(tiePt.Y-Ygps) + Rimu[8]*(tiePt.Z-Zgps);

				_X = R[0]* ( X - imgMisalign.u) + R[3]*(Y- imgMisalign.v) + R[6]*(Z- imgMisalign.w);
				_Y = R[1]* ( X - imgMisalign.u) + R[4]*(Y- imgMisalign.v) + R[7]*(Z- imgMisalign.w);
				_Z = R[2]* ( X - imgMisalign.u) + R[5]*(Y- imgMisalign.v) + R[8]*(Z- imgMisalign.w);

				// 投影坐标
				double f = camera->m_f;

				double xp = -f*_X/_Z;
				double yp = -f*_Y/_Z;

				//////////////////////// 相点改正坐标
				///////////////// 主点改正 /////////////
				double x= projs[j].xi - camera->m_x0;	
				double y= projs[j].yi - camera->m_y0;

				//////////////////////// 畸变改正 ////////
				double r2 = x*x + y*y;

				// 径向
				double dx = x*( camera->m_k0 + r2*( camera->m_k1 + r2*( camera->m_k2 + r2*camera->m_k3 ) ) );
				double dy = y*( camera->m_k0 + r2*( camera->m_k1 + r2*( camera->m_k2 + r2*camera->m_k3 ) ) );

				// 切向
				x += dx;
				y += dy;
				
				////////////////////////////// 误差方程 
				// 常数项
				projs[j].vx = lx = ( x - xp );
				projs[j].vy = ly = ( y - yp );

				rmsVxy += lx*lx + ly*ly;

				if( maxVx < fabs(lx) )
					maxVx = fabs(lx);
			
				if( maxVy < fabs(ly) )
					maxVy = fabs(ly);

				a[0]= y*sinw - ( x/f* (x*cosk - y*sink) + f*cosk)*cosw;
				a[1]=-f*sink -   x/f* (x*sink + y*cosk);
				a[2]= y;
				a[3]= (R[0]*f + R[2]*x)/_Z;
				a[4]= (R[3]*f + R[5]*x)/_Z;

				b[0]= -x*sinw - ( y/f* (x*cosk - y*sink) - f*sink)*cosw;
				b[1]=-f*cosk -   y/f* (x*sink + y*cosk);
				b[2]=-x;
				b[3]= (R[1]*f + R[2]*y)/_Z;
				b[4]= (R[4]*f + R[5]*y)/_Z;
	
				//////////////////////////// 可选项 ////////////////////////
				if( 3 == nxXyz  ) {	// Zs
					a[5]= (R[6]*f + R[8]*x)/_Z;
					b[5]= (R[7]*f + R[8]*y)/_Z;
				}

				if( 2 == nxPrinciplePoint )	{	// x0, y0
					a[ixPP] = 1;	a[ixPP+1] = 0;
					b[ixPP] = 0;	b[ixPP+1] = 1;
				}
				if( 1 == nxPrincipleLength ) {	// f
					a[ixPL]= -_X/_Z;
					b[ixPL]= -_Y/_Z;
				}

				if( nxRadiusDistorions >= 3 )	{ // k0, k1, k2, k3
					int iDist = ixRD;

					if( 4 == nxRadiusDistorions )	{
						a[iDist] = -x;
						b[iDist] = -y;
						iDist++;
					}
					double r2 = x*x + y*y;
					double r4 = r2*r2;
					double r6 = r4*r2;

					a[iDist] = -x*r2;	b[iDist] = -y*r2;	iDist++;
					a[iDist] = -x*r4;	b[iDist] = -y*r4;	iDist++;
					a[iDist] = -x*r6;	b[iDist] = -y*r6;
				}

				//////////////////////////////////////////////////////////////////////////

				for( int row=0; row<nx; row++)
				{
					for( int col=0; col<nx; col++)
						AA[row*nx+col] += a[row]*a[col] + b[row]*b[col];
					AL[row] += a[row]*lx + b[row]*ly;
				}
			}
		}

		rmsVxy = sqrt( rmsVxy / (nprojs-4) );

		Gauss( AA, AL, nx );

		if( minRmsVxy > rmsVxy )
			minRmsVxy = rmsVxy;
		else	{
			printf("diverse?\n");
			break;
		}

		if( rmsVxy < projThresh )
			break;

		imgMisalign.phi += AL[0];
		imgMisalign.omega += AL[1];
		imgMisalign.kappa += AL[2];

		if( nx > 3 )
			imgMisalign.u += AL[3];
		if( nx > 4 )
			imgMisalign.v += AL[4];

		if( 3 == nxXyz  ) {	// Zs
			imgMisalign.w += AL[5];
		}

		if( 2 == nxPrinciplePoint )	{	// x0, y0
			camera->m_x0 += AL[ixPP];
			camera->m_y0 += AL[ixPP+1];
		}
		if( 1 == nxPrincipleLength ) {	// f
			camera->m_f += AL[ixPL];
		}

		if( nxRadiusDistorions >= 3 )	{ // k0, k1, k2, k3
			int iDist = ixRD;

			if( 4 == nxRadiusDistorions )	{
				camera->m_k0 += AL[iDist];
				iDist++;
			}
	
			camera->m_k1 += AL[iDist];	iDist++;
			camera->m_k2 += AL[iDist];	iDist++;
			camera->m_k3 += AL[iDist];	iDist++;
		}

		if( fabs(AL[0]) < thresh && fabs(AL[1]) < thresh && abs(AL[2]) < thresh )
			break;

		iteration++;
	}


FUNC_END:

	return	minRmsVxy;

}







void  Image2Photo(double xi, double yi, double& xp, double& yp);

static void readNpointsAndNprojections_tracks(FILE *fp, int *n3Dpts, int *nprojs, ors_int32 *pFrameProjs, vector<AT_tiePoint> &pData)
{
	char pLine[MAX_STRING];
	int gcpNum, conNum;
	int i, j;
	double xg,yg,zg;
	int  projN;
	int  imgId;
	double xp, yp;
	AT_tiePoint  tiePt;
	imgPoint imgPt;

	if(fp==NULL)
		return	;

	*nprojs=0;
	while(!feof(fp)) 
	{
		fgets(pLine, MAX_STRING, fp);
		if(strstr(pLine, "[tracks]"))
		{
			fscanf( fp, "%d", n3Dpts );
			
			
			for(i=0; i<*n3Dpts; i++)
			{
				tiePt.X=0;
				tiePt.Y=0;
				tiePt.Z=0;

				tiePt.projs.clear();

				fscanf(fp, "%ld%d ", &tiePt.ptID, &projN);

				if( projN< 1 )
				{
					for(j=0; j<projN; j++)
					{
						fscanf(fp, "%d %lf %lf ", &imgId, &xp, &yp);
					}				
					continue;
				}

				*nprojs+=projN;
				for(j=0; j<projN; j++)
				{
					fscanf(fp, "%d %lf %lf ", &imgId, &xp, &yp);

//					Image2Photo( xp, yp, xp, yp  );

					// 旋转回到本体坐标系
					if( g_bRotateBack )	{
						double t;

						t = yp;	
						yp = xp;
						xp = g_imgHei - t;
					}

					imgPt.imgID=imgId;
					imgPt.xi=xp;
					imgPt.yi=yp;

					tiePt.projs.push_back(imgPt);

					pFrameProjs[imgId]++;
				}
				fscanf(fp, "\n");
				
				pData.push_back(tiePt);
			}
		}

	};
	
	*n3Dpts= pData.size();
}



static double m_inorImage_a[3];	// pixel to photo
static double m_inorImage_b[3];

static double m_inorPixel_a[3];	// photo to pixel
static double m_inorPixel_b[3];

void  Image2Photo(double xi, double yi, double& xp, double& yp)
{
	double xf, yf;
	double r2, dr, dx, dy;

	xf = m_inorImage_a[0] + m_inorImage_a[1]*xi + m_inorImage_a[2]*yi;
	yf = m_inorImage_b[0] + m_inorImage_b[1]*xi + m_inorImage_b[2]*yi;

	xp=xf;
	yp=yf;
};

void  Photo2Image(double xp, double yp, double& xi, double& yi)
{
	double xf, yf;
	double r2, dr, dx, dy;

	xf=xp;
	yf=yp;

	xi = m_inorPixel_a[0] + m_inorPixel_a[1]*xf + m_inorPixel_a[2]*yf;
	yi = m_inorPixel_b[0] + m_inorPixel_b[1]*xf + m_inorPixel_b[2]*yf;

};
 

//坐标转换
void Batch_Image2Photo(vector<AT_tiePoint> &pData)
{
	int num=pData.size();

	vector<AT_tiePoint>::iterator iter;
	vector<imgPoint>::iterator imgPt_iter;
	int i=0;

	for(iter=pData.begin(); iter!=pData.end(); iter++)
	{
		for(imgPt_iter=iter->projs.begin(); imgPt_iter!=iter->projs.end(); imgPt_iter++)
		{
			double xp, yp;
			Image2Photo(imgPt_iter->xi, imgPt_iter->yi, xp, yp);

			imgPt_iter->xi=xp;
			imgPt_iter->yi=yp;
		}
	}
}



//用前交后交法解算安装参数
int Img_misalign_calib_delay_ZConstraint(const char* pImgList, const char* tracks, const char *posSequence, const char *pLasIdxName)
{
	int ret=0;
	int i, j;
	
	_iphCamera	camera;
	std::vector<imgExOrientParam>  exorParam, best_exorParam;
	FILE *fp=NULL;

	////////////////////取得像片的外方位元素，假设所有像片的内参数一致
	fp=fopen(pImgList, "rt");
	if(fp==NULL)
	{
		ASSERT(false);
	}

	int nFrame=0;
	char buf[256];
	while (fgets(buf, 256, fp)) 
	{
		if (buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = 0;

		if (buf[strlen(buf)-1] == '\r')
			buf[strlen(buf)-1] = 0;

		InitializeCamera(buf, &camera);

		imgExOrientParam param;

		param.Xs=camera.m_Xs;
		param.Ys=camera.m_Ys;
		param.Zs=camera.m_Zs;

		// 旋转回到本体坐标系
		if( g_bRotateBack )	{
 			double R[9], Rt[9];
 
 			RotateMat_fwk( 0, 0, 90.0/180*3.1415926535897932384626433832795, R  );
 			matrix_product( 3, 3, 3, 3, camera.m_RMatrix, R, Rt );
 
 			memcpy( camera.m_RMatrix, Rt, 9*sizeof(double) );

			//param.kappa += 90.0/180*3.1415926535897932384626433832795;

		}		
		
		R2fwk(camera.m_RMatrix, &(param.phi), &(param.omega), &(param.kappa));

		param.gps_time=camera.m_GPSTime;

		exorParam.push_back(param);

		nFrame++;
	}
	
	fclose(fp); fp=NULL;

	if(pLut)	delete[] pLut;
	pLut=new ors_int32[nFrame+1];
	if(pOffset)	delete[] pOffset;
	pOffset=new ors_int32[nFrame];
	

	memset(pLut, 0, sizeof(ors_int32)*nFrame);
	memset(pOffset, 0, sizeof(ors_int32)*nFrame);

	//提取连接点
	vector<AT_tiePoint> tiePtBuf;
	int n3Dpts, nprojs;
	fp=fopen(tracks, "rt");
	if(fp==NULL)
	{
		ASSERT(false);
	}

	ors_int32 *pFrameProjs=NULL;
	pFrameProjs=new ors_int32[nFrame];
	memset(pFrameProjs, 0, sizeof(ors_int32)*nFrame);

	readNpointsAndNprojections_tracks(fp, &n3Dpts, &nprojs, pFrameProjs, tiePtBuf);

	fclose(fp);	fp=NULL;

#ifdef _DEBUG
	int sum=0;
	for(i=0; i<nFrame; i++)
	{
		sum+=pFrameProjs[i];
	}
	
 	ASSERT(sum==nprojs);
#endif

	int offset=0;
	for(i=1; i<nFrame; i++)
	{
		offset+=pFrameProjs[i-1];
		pLut[i]=offset;
	}
	pLut[0]=0;
	pLut[nFrame]=nprojs;

	memcpy(m_inorImage_a, camera.m_inorImage_a, sizeof(double)*3);
	memcpy(m_inorImage_b, camera.m_inorImage_b, sizeof(double)*3);
	memcpy(m_inorPixel_a, camera.m_inorPixel_a, sizeof(double)*3);
	memcpy(m_inorPixel_b, camera.m_inorPixel_b, sizeof(double)*3);

	Batch_Image2Photo( tiePtBuf);

	//读取POS序列，用于时间内插
	if(pExorSequen)	delete[] pExorSequen;

	fp=fopen(posSequence, "rt");

	fscanf( fp, "%d\n", &posNum );
	pExorSequen=new imgExOrientParam[posNum];

	for(i=0; i<posNum; i++)
	{
		fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf", &(pExorSequen[i].gps_time), &(pExorSequen[i].Xs), &(pExorSequen[i].Ys), 
			&(pExorSequen[i].Zs), &(pExorSequen[i].phi), &(pExorSequen[i].omega), &(pExorSequen[i].kappa));

	}
	
	fclose(fp); fp=NULL;

	//打开LiDAR索引
	if(pLiDIdx)	delete pLiDIdx;
	pLiDIdx=new CATNPtIndex;

	pLiDIdx->OpenIdx((char*)pLasIdxName);

	//偏心分量初值
	memset(&imgMisalign, 0, sizeof(imgMisalign));	//安装参数赋初值

	imgMisalign.phi = 0.0104527933;
	imgMisalign.omega = -0.0148748028;
	imgMisalign.kappa = 0.0290897563;
	imgMisalign.u=-1.889270;
	imgMisalign.v=0.906281;
	imgMisalign.w=-1.666181;

	
	//前交后交解算安装参数
	Proj_Point  *projBuf=NULL;

	projBuf=new Proj_Point[nprojs];
	
	double projTh=1;
	projTh*=camera.m_pixelX;
	int iter=0;
	
	double p_eL2;
	double min_eL2=1.7e+308;

	double Rmis[9];
	double leverarm[3], translate[3];
	leverarm[0]=imgMisalign.u;
	leverarm[1]=imgMisalign.v;
	leverarm[2]=imgMisalign.w;

	RotateMat_fwk(imgMisalign.phi, imgMisalign.omega, imgMisalign.kappa, Rmis);

	//时间延迟解算
	double delay;	//统一时间延迟
	double minTime=0, maxTime=150*1e-6;	//0, 150microsecond
	double interval = 2*1e-6;
	double best_delay;

	iter=0;
	while(1)
	{
		delay=minTime+iter*interval;
		if(delay>maxTime)
			break;

		for(i=0, j=0; i<nFrame; i++)
		{//找到时间最接近的LiDAR点
			double time = exorParam[i].gps_time+delay;
			
			while(j<posNum)
			{
				
				if(time>pExorSequen[j].gps_time)
				{
					j++;
				}
				else
				{
					break;
				}
				
			}

			ASSERT(j>0 && j<posNum);

			double _t=pExorSequen[j-1].gps_time;
			double t_=pExorSequen[j].gps_time;
			
			if(fabs(_t-time)<fabs(time-t_))
			{
//				exorParam[i].gps_time=_t;
				exorParam[i].Xs=pExorSequen[j-1].Xs;
				exorParam[i].Ys=pExorSequen[j-1].Ys;
				exorParam[i].Zs=pExorSequen[j-1].Zs;
				exorParam[i].phi=pExorSequen[j-1].phi;
				exorParam[i].omega=pExorSequen[j-1].omega;
				exorParam[i].kappa=pExorSequen[j-1].kappa;
			}
			else
			{
//				exorParam[i].gps_time=t_;
				exorParam[i].Xs=pExorSequen[j].Xs;
				exorParam[i].Ys=pExorSequen[j].Ys;
				exorParam[i].Zs=pExorSequen[j].Zs;
				exorParam[i].phi=pExorSequen[j].phi;
				exorParam[i].omega=pExorSequen[j].omega;
				exorParam[i].kappa=pExorSequen[j].kappa;
			}
		}

		ForwardIntersctForTiePoints( &exorParam, &camera, tiePtBuf, false );		//前交

		memset(pOffset, 0, sizeof(ors_int32)*nFrame);

		for(i=0; i<n3Dpts; i++)		//按投影点组织数据
		{
			AT_tiePoint tiePt=tiePtBuf[i];
			for(j=0; j<tiePt.projs.size(); j++)
			{
				imgPoint imgpt=tiePt.projs[j];
				int imgID=imgpt.imgID;

				int offset=pLut[imgID]+pOffset[imgID];

// 				projBuf[offset].X=tiePt.X;
// 				projBuf[offset].Y=tiePt.Y;
// 				projBuf[offset].Z=tiePt.Z;

				projBuf[offset].iXyz = i;

				projBuf[offset].xi=imgpt.xi;
				projBuf[offset].yi=imgpt.yi;

				pOffset[imgID]++;
			}
		}
		
		p_eL2=0;
		for(i=0; i<nFrame; i++)
		{
			double Xs, Ys, Zs, Xg, Yg, Zg;
			//double X, Y, Z;
			double Rimu[9], R[9];
			Xg= exorParam[i].Xs;
			Yg= exorParam[i].Ys;
			Zg= exorParam[i].Zs;

			RotateMat_fwk(exorParam[i].phi, exorParam[i].omega, exorParam[i].kappa, Rimu);

			matrix_product(3, 3, 3, 1, Rimu, leverarm, translate);
			matrix_product(3, 3, 3, 3, Rimu, Rmis, R);

			for(j=pLut[i]; j<pLut[i+1]; j++)
			{
				int iXyz = projBuf[j].iXyz;
				
				AT_tiePoint tiePt=tiePtBuf[iXyz];

// 				X = projBuf[j].X;
// 				Y=projBuf[j].Y;
// 				Z=projBuf[j].Z;
				
				Xs = Xg + translate[0];
				Ys = Yg + translate[1];
				Zs = Zg + translate[2];
				
				double xi, yi;
				ImageProject( tiePt.X, tiePt.Y, tiePt.Z, &xi, &yi, R, Xs, Ys, Zs, camera.m_f);

				double lx, ly;
				lx= projBuf[j].xi - xi;
				//L[1]=
				ly= projBuf[j].yi - yi;

				p_eL2+=lx*lx;
				p_eL2+=ly*ly;
			}
		}

		p_eL2=sqrt(p_eL2/(nprojs-4));

		if(min_eL2>p_eL2)
		{
			min_eL2=p_eL2;

			best_delay=delay;
			best_exorParam=exorParam;
		}

		iter++;
	}
	
	

	exorParam=best_exorParam;

//	CFileDialog dlg();
	char pszCamsName[256]="J:\\imgRO\\AT\\TPlane\\原始\\Cams_delay.txt";
	fp=fopen(pszCamsName, "wt");
	fprintf(fp, "delay: %f\n",best_delay );
	for(i=0; i<exorParam.size(); i++)
	{
		fprintf(fp, "image %d\n", i+1);
		fprintf(fp, "%.7lf %.7lf %.7lf\n", exorParam[i].Xs, exorParam[i].Ys, exorParam[i].Zs);
		
		double R[9];
		RotateMat_fwk(exorParam[i].phi, exorParam[i].omega, exorParam[i].kappa, R);
		for( j=0; j<3; j++) 
		{
			for(int k=0; k<3; k++)
			{
				fprintf(fp,  "%.7f ", R[j*3+k] );
			}
			fprintf(fp, "\n");
		}

		fprintf(fp, "\n");
	}

	fclose(fp);	fp=NULL;

	CalibrateWithForwardBackwardIntersection( &exorParam, &camera, tiePtBuf, projBuf, nprojs, projTh );	

	RotateMat_fwk(imgMisalign.phi, imgMisalign.omega, imgMisalign.kappa, Rmis);

	CString strOutput;
	
	strOutput="J:\\imgRO\\AT\\misAlign.txt";

	fp=fopen( strOutput, "wt");
	fprintf(fp, "#misalign matrix\n");

	for( int i=0; i<3; i++) 
	{
		for( int j=0; j<3; j++)
		{
			fprintf(fp,  "%.10lf ", Rmis[i*3+j] );
		}
		fprintf(fp, "\n");
	}

	fprintf(fp, "#misalign translation\n");
	fprintf(fp, "%.6lf %.6lf %.6lf\n", imgMisalign.u, imgMisalign.v, imgMisalign.w);
	fprintf(fp, "#roll, pitch, heading\n");
	fprintf(fp, "%.10lf %.10lf %.10lf\n", imgMisalign.phi, imgMisalign.omega, imgMisalign.kappa );

	fprintf(fp, "#RSM\n");
	fprintf(fp, "%.10lf\n", min_eL2);

	fclose(fp);	
	fp=NULL;

FLAG_END:
	
	if(pLut)	delete[] pLut;		pLut=NULL;
	if(pOffset) delete[] pOffset;	pOffset=NULL;
	if(pFrameProjs) delete[] pFrameProjs;	pFrameProjs=NULL;
	if(projBuf) delete[] projBuf;	projBuf=NULL;
	if(pExorSequen) delete[] pExorSequen;   pExorSequen=NULL;
	if(pLiDIdx)	delete pLiDIdx;	pLiDIdx=NULL;
	return ret;
}


/*
int tracks2tiepoints(const char* pTrackFile, const char* pImgList)
{
	FILE *fp=NULL;
	int n3Dpts;
	int nprojs;

	fp=fopen(pImgList, "rt");
	if(fp==NULL)
	{
		ASSERT(false);
	}

	int nFrame=0;
	char buf[256];
	while (fgets(buf, 256, fp)) 
	{
		if (buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = 0;

		if (buf[strlen(buf)-1] == '\r')
			buf[strlen(buf)-1] = 0;

		nFrame++;
	}

	fclose(fp); fp=NULL;


	if(pLut)	delete[] pLut;
	pLut=new ors_int32[nFrame+1];
	if(pOffset)	delete[] pOffset;
	pOffset=new ors_int32[nFrame];


	memset(pLut, 0, sizeof(ors_int32)*nFrame);
	memset(pOffset, 0, sizeof(ors_int32)*nFrame);

	//提取连接点
	vector<AT_tiePoint> tiePtBuf;
//	int n3Dpts, nprojs;
	fp=fopen(pTrackFile, "rt");
	if(fp==NULL)
	{
		ASSERT(false);
	}

	ors_int32 *pFrameProjs=NULL;
	pFrameProjs=new ors_int32[nFrame];
	memset(pFrameProjs, 0, sizeof(ors_int32)*nFrame);

	readNpointsAndNprojections_tracks(fp, &n3Dpts, &nprojs, pFrameProjs, &tiePtBuf);
	fclose(fp);	fp=NULL;


	if(pLut)	delete[] pLut;		pLut=NULL;
	if(pOffset) delete[] pOffset;	pOffset=NULL;
	if(pFrameProjs) delete[] pFrameProjs;	pFrameProjs=NULL;
	
//	if(projBuf) delete[] projBuf;	projBuf=NULL;
	return 1;
}*/

void ExportReprojectError(const char *pDir, std::vector<imgExOrientParam> *exorParam, _iphCamera *camera, vector<AT_tiePoint> &pData, Proj_Point *projs, bool bAdjusted )
{
	ForwardIntersctForTiePoints( exorParam, camera, pData, false );		//前交

	int nFrame=exorParam->size();
	int n3Dpoint=pData.size();
	int i, j;


	memset(pOffset, 0, sizeof(ors_int32)*nFrame);

	for(i=0; i<n3Dpoint; i++)
	{
		AT_tiePoint tiePt= pData[i];

		for(j=0; j<tiePt.projs.size(); j++)
		{
			imgPoint imgpt=tiePt.projs[j];

			int imgID=imgpt.imgID;

			int offset=pLut[imgID]+pOffset[imgID];

// 			projs[offset].X=tiePt.X;
// 			projs[offset].Y=tiePt.Y;
// 			projs[offset].Z=tiePt.Z;

			projs[offset].iXyz = i;

			projs[offset].xi=imgpt.xi;
			projs[offset].yi=imgpt.yi;

			pOffset[imgID]++;
		}
	}

	
	for(i=0; i<nFrame; i++)
	{
		CString id;
		CString fileName;

		id.Format("%d", i+1);

		fileName=pDir;
		fileName+="\\";
		fileName+=id;

		if( bAdjusted )
			fileName+="_reproj_adjusted.txt";
		else 
			fileName+="_reproj.txt";


		FILE *fp=NULL;
		fp=fopen(fileName, "wt");
		fprintf(fp, "%d\n", pLut[i+1]-pLut[i]);

		for(j=pLut[i]; j<pLut[i+1]; j++)
		{
			double xi, yi, vx, vy;

			Photo2Image( projs[j].xi, projs[j].yi, xi, yi );

			int iXyz = projs[j].iXyz;

			AT_tiePoint tiePt= pData[iXyz];

			if( projs[j].vx == 0 && projs[j].vy == 0 ) 
			{
				double xp, yp;
				double R[9];

				imgExOrientParam &exor = (*exorParam)[i];

				RotateMat_fwk(exor.phi, exor.omega, exor.kappa, R );

				ImageProject( tiePt.X, tiePt.Y, tiePt.Z, &xp, &yp,  R, exor.Xs, exor.Ys, exor.Zs, camera->m_f  );
				Photo2Image( xp, yp, xp, yp );

				//projs[j].
				vx = xi - xp;
				//projs[j].
				vy = yi - yp;
			}
			else	{
				double xp, yp;

				xp = projs[j].xi - projs[j].vx;
				yp = projs[j].yi - projs[j].vy;

				Photo2Image( xp, yp, xp, yp );

				vx = xi - xp;
				vy = yi - yp;
			}
			
			fprintf(fp, "%10ld %7.1lf %7.1lf %10.3lf %10.3lf %8.3lf %7.1lf %7.1lf\n", tiePt.ptID, xi, yi, tiePt.X, tiePt.Y, tiePt.Z, vx , vy );

		}
		fclose(fp);
	}
}

//读取POS序列，用于时间内插
// 	if(pExorSequen)	delete[] pExorSequen;
// 
// 	fp=fopen(posSequence, "rt");
// 
// 	fscanf( fp, "%d\n", &posNum );
// 	pExorSequen=new imgExOrientParam[posNum];
// 
// 	for(i=0; i<posNum; i++)
// 	{
// 		fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf", &(pExorSequen[i].gps_time), &(pExorSequen[i].Xs), &(pExorSequen[i].Ys), 
// 			&(pExorSequen[i].Zs), &(pExorSequen[i].phi), &(pExorSequen[i].omega), &(pExorSequen[i].kappa));
// 
// 	}
// 
// 	fclose(fp); fp=NULL;

// 	while(1)
// 	{
// 		delay=minTime+iter*interval;
// 		if(delay>maxTime)
// 			break;
// 
// 		for(i=0, j=0; i<nFrame; i++)
// 		{//找到时间最接近的LiDAR点
// 			double time = exorParam[i].gps_time+delay;
// 
// 			while(j<posNum)
// 			{
// 
// 				if(time>pExorSequen[j].gps_time)
// 				{
// 					j++;
// 				}
// 				else
// 				{
// 					break;
// 				}
// 
// 			}
// 
// 			ASSERT(j>0 && j<posNum);
// 
// 			double _t=pExorSequen[j-1].gps_time;
// 			double t_=pExorSequen[j].gps_time;
// 
// 			if(fabs(_t-time)<fabs(time-t_))
// 			{
// 				//				exorParam[i].gps_time=_t;
// 				exorParam[i].Xs=pExorSequen[j-1].Xs;
// 				exorParam[i].Ys=pExorSequen[j-1].Ys;
// 				exorParam[i].Zs=pExorSequen[j-1].Zs;
// 				exorParam[i].phi=pExorSequen[j-1].phi;
// 				exorParam[i].omega=pExorSequen[j-1].omega;
// 				exorParam[i].kappa=pExorSequen[j-1].kappa;
// 			}
// 			else
// 			{
// 				//				exorParam[i].gps_time=t_;
// 				exorParam[i].Xs=pExorSequen[j].Xs;
// 				exorParam[i].Ys=pExorSequen[j].Ys;
// 				exorParam[i].Zs=pExorSequen[j].Zs;
// 				exorParam[i].phi=pExorSequen[j].phi;
// 				exorParam[i].omega=pExorSequen[j].omega;
// 				exorParam[i].kappa=pExorSequen[j].kappa;
// 			}
// 		}
// 
// 		ForwardIntersctForTiePoints(&exorParam, &camera, &tiePtBuf);		//前交
// 		memset(pOffset, 0, sizeof(ors_int32)*nFrame);
// 
// 		for(i=0; i<n3Dpts; i++)		//按投影点组织数据
// 		{
// 			AT_tiePoint tiePt=tiePtBuf[i];
// 			for(j=0; j<tiePt.projs.size(); j++)
// 			{
// 				imgPoint imgpt=tiePt.projs[j];
// 				int imgID=imgpt.imgID;
// 
// 				int offset=pLut[imgID]+pOffset[imgID];
// 				projBuf[offset].X=tiePt.X;
// 				projBuf[offset].Y=tiePt.Y;
// 				projBuf[offset].Z=tiePt.Z;
// 				projBuf[offset].xi=imgpt.xi;
// 				projBuf[offset].yi=imgpt.yi;
// 
// 				pOffset[imgID]++;
// 			}
// 		}
// 
// 		p_eL2=0;
// 		for(i=0; i<nFrame; i++)
// 		{
// 			double Xs, Ys, Zs, Xg, Yg, Zg;
// 			double X, Y, Z;
// 			double Rimu[9], R[9];
// 			Xg= exorParam[i].Xs;
// 			Yg= exorParam[i].Ys;
// 			Zg= exorParam[i].Zs;
// 
// 			RotateMat_fwk(exorParam[i].phi, exorParam[i].omega, exorParam[i].kappa, Rimu);
// 
// 			matrix_product(3, 3, 3, 1, Rimu, leverarm, translate);
// 			matrix_product(3, 3, 3, 3, Rimu, Rmis, R);
// 
// 			for(j=pLut[i]; j<pLut[i+1]; j++)
// 			{
// 
// 				X=projBuf[j].X;
// 				Y=projBuf[j].Y;
// 				Z=projBuf[j].Z;
// 
// 				Xs=Xg+translate[0];
// 				Ys=Yg+translate[1];
// 				Zs=Zg+translate[2];
// 
// 				double xi, yi;
// 				ImageProject(X, Y, Z, &xi, &yi, R, Xs, Ys, Zs, camera.m_f);
// 
// 				double lx, ly;
// 				lx= projBuf[j].xi - xi;
// 				//L[1]=
// 				ly= projBuf[j].yi - yi;
// 
// 				p_eL2+=lx*lx;
// 				p_eL2+=ly*ly;
// 			}
// 		}
// 
// 		p_eL2=sqrt(p_eL2/(nprojs-4));
// 
// 		if(min_eL2>p_eL2)
// 		{
// 			min_eL2=p_eL2;
// 
// 			best_delay=delay;
// 			best_exorParam=exorParam;
// 		}
// 
// 		iter++;
// 	}
// 
// 
// 
// 	exorParam=best_exorParam;

//	CFileDialog dlg();
// 	char pszCamsName[256]="J:\\imgRO\\AT\\TPlane\\原始\\Cams_delay.txt";
// 	fp=fopen(pszCamsName, "wt");
// 	fprintf(fp, "delay: %f\n",best_delay );
// 	for(i=0; i<exorParam.size(); i++)
// 	{
// 		fprintf(fp, "image %d\n", i+1);
// 		fprintf(fp, "%.7lf %.7lf %.7lf\n", exorParam[i].Xs, exorParam[i].Ys, exorParam[i].Zs);
// 
// 		double R[9];
// 		RotateMat_fwk(exorParam[i].phi, exorParam[i].omega, exorParam[i].kappa, R);
// 		for( j=0; j<3; j++) 
// 		{
// 			for(int k=0; k<3; k++)
// 			{
// 				fprintf(fp,  "%.7f ", R[j*3+k] );
// 			}
// 			fprintf(fp, "\n");
// 		}
// 
// 		fprintf(fp, "\n");
// 	}
// 
// 	fclose(fp);	fp=NULL;

double space_resection_misalign_plus(std::vector<imgExOrientParam>  *exorParam, _iphCamera *camera, vector<AT_tiePoint> &pData, 
									 Proj_Point *projs, int nprojs, double projThresh);

int Img_misalign_calib_ZConstraint(const char* pImgList, const char* tracks, const char *pLasIdxName)
{
	int ret=0;
	int i, j;

	CString strDir=pImgList;
	strDir=strDir.Left(strDir.ReverseFind('\\'));

	_iphCamera	camera;
	std::vector<imgExOrientParam>  exorParam, best_exorParam;
	FILE *fp=NULL;

	////////////////////取得像片的外方位元素，假设所有像片的内参数一致
	fp=fopen(pImgList, "rt");
	if(fp==NULL)
	{
		ASSERT(false);
	}

	int nFrame=0;
	char buf[256];
	while (fgets(buf, 256, fp)) 
	{
		if (buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = 0;

		if (buf[strlen(buf)-1] == '\r')
			buf[strlen(buf)-1] = 0;

		InitializeCamera(buf, &camera);

		imgExOrientParam param;
		param.Xs=camera.m_Xs;
		param.Ys=camera.m_Ys;
		param.Zs=camera.m_Zs;

		R2fwk( camera.m_RMatrix, &(param.phi), &(param.omega), &(param.kappa));

		// 旋转回到本体坐标系
		if( g_bRotateBack )	{
// 			double R[9], Rt[9];
// 
// 			RotateMat_fwk( 0, 0, 90.0/180*3.1415926535897932384626433832795, R  );
// 			matrix_product( 3, 3, 3, 3, camera.m_RMatrix, R, Rt );
// 
// 			memcpy( camera.m_RMatrix, Rt, 9*sizeof(double) );

			param.kappa += 90.0/180*3.1415926535897932384626433832795;

		}		


		param.gps_time=camera.m_GPSTime;

		exorParam.push_back( param );

		nFrame++;
	}

	fclose(fp); fp=NULL;

	if(pLut)	delete[] pLut;
	pLut=new ors_int32[nFrame+1];
	if(pOffset)	delete[] pOffset;
	pOffset=new ors_int32[nFrame];


	memset(pLut, 0, sizeof(ors_int32)*nFrame);
	memset(pOffset, 0, sizeof(ors_int32)*nFrame);

	//提取连接点
	vector<AT_tiePoint> tiePtBuf;
	int n3Dpts, nprojs;
	fp=fopen(tracks, "rt");
	if(fp==NULL)
	{
		ASSERT(false);
	}

	ors_int32 *pFrameProjs=NULL;
	pFrameProjs=new ors_int32[nFrame];
	memset(pFrameProjs, 0, sizeof(ors_int32)*nFrame);

	readNpointsAndNprojections_tracks(fp, &n3Dpts, &nprojs, pFrameProjs, tiePtBuf);

	fclose(fp);	fp=NULL;

#ifdef _DEBUG
	int sum=0;
	for(i=0; i<nFrame; i++)
	{
		sum+=pFrameProjs[i];
	}

	ASSERT(sum==nprojs);
#endif

	int offset=0;
	for(i=1; i<nFrame; i++)
	{
		offset+=pFrameProjs[i-1];
		pLut[i]=offset;
	}
	pLut[0]=0;
	pLut[nFrame]=nprojs;

	memcpy(m_inorImage_a, camera.m_inorImage_a, sizeof(double)*3);
	memcpy(m_inorImage_b, camera.m_inorImage_b, sizeof(double)*3);
	memcpy(m_inorPixel_a, camera.m_inorPixel_a, sizeof(double)*3);
	memcpy(m_inorPixel_b, camera.m_inorPixel_b, sizeof(double)*3);

	Batch_Image2Photo( tiePtBuf );

	//打开LiDAR索引
 	if(pLiDIdx)	delete pLiDIdx;
 	pLiDIdx=new CATNPtIndex;
 
 	pLiDIdx->OpenIdx((char*)pLasIdxName);

	//偏心分量初值
	memset(&imgMisalign, 0, sizeof(imgMisalign));	//安装参数赋初值

// 	imgMisalign.phi=0.0104527933;
// 	imgMisalign.omega=-0.0148748028;
// 	imgMisalign.kappa=0.0290897563;
// 	imgMisalign.u=-1.889270;
// 	imgMisalign.v=0.906281;
// 	imgMisalign.w=-1.666181;

//	imgMisalign.f = camera.m_f;

	//前交后交解算安装参数
	Proj_Point  *projBuf=NULL;

	projBuf=new Proj_Point[nprojs];

	memset( projBuf, 0, nprojs*sizeof(Proj_Point) );

	double projTh= 1*camera.m_pixelX;
	int iter=0;
	double p_eL2;
	double min_eL2=1.7e+308;
	
	double Rmis[9];
	double leverarm[3], translate[3];
	leverarm[0]=imgMisalign.u;
	leverarm[1]=imgMisalign.v;
	leverarm[2]=imgMisalign.w;

	RotateMat_fwk(imgMisalign.phi, imgMisalign.omega, imgMisalign.kappa, Rmis);

	//时间延迟解算
	double delay;	//统一时间延迟
	double minTime=0, maxTime=150*1e-6;	//0, 150microsecond
	double interval = 2*1e-6;
	double best_delay;

	iter=0;

	ExportReprojectError( strDir, &exorParam, &camera, tiePtBuf, projBuf, false );

	// 空间前交，后交迭代检校
	min_eL2 = CalibrateWithForwardBackwardIntersection( &exorParam, &camera, tiePtBuf, projBuf, nprojs, projTh );	

	ExportReprojectError( strDir, &exorParam, &camera, tiePtBuf, projBuf, true );

	//	double Rmis[9];
	RotateMat_fwk(imgMisalign.phi, imgMisalign.omega, imgMisalign.kappa, Rmis);

	CString strOutput;
 	CFileDialog  outputdlg(FALSE," ",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	
 	outputdlg.m_ofn.lpstrTitle="保存检校后的安置参数";
 	if(outputdlg.DoModal()!=IDOK)
 	{
 		goto FLAG_END;
 	}
 
 	strOutput=outputdlg.GetPathName();

//	strOutput="J:\\imgRO\\AT\\misAlign.txt";
	fp=fopen(strOutput, "wt");
	fprintf(fp, "#misalign matrix\n");

	for( int i=0; i<3; i++) 
	{
		for( int j=0; j<3; j++)
		{
			fprintf(fp,  "%.10lf ", Rmis[i*3+j] );
		}
		fprintf(fp, "\n");
	}

	fprintf(fp, "#misalign translation\n");
	fprintf(fp, "%.3lfm %.3lfm %.3lfm\n", imgMisalign.u, imgMisalign.v, imgMisalign.w);
	fprintf(fp, "#roll, pitch, heading\n");
	fprintf(fp, "%.10lf %.10lf %.10lf\n", imgMisalign.phi, imgMisalign.omega, imgMisalign.kappa);

	fprintf(fp, "\n#Camera Calibration\n");
	fprintf(fp, "x0, y0, f = %.3lfmm, %.3lfmm, %.3lfmm\n", camera.m_x0, camera.m_y0, camera.m_f ); 
	fprintf(fp, "k0, k1, k2, k3 = %E, %E, %E, %E\n", camera.m_k0, camera.m_k1, camera.m_k2, camera.m_k3 );

	fprintf(fp, "\n#RSM: %.10lf mm\n", min_eL2);

	fclose(fp);	
	fp=NULL;

FLAG_END:

	if(pLut)	delete[] pLut;		pLut=NULL;
	if(pOffset) delete[] pOffset;	pOffset=NULL;
	if(pFrameProjs) delete[] pFrameProjs;	pFrameProjs=NULL;
	if(projBuf) delete[] projBuf;	projBuf=NULL;
	if(pExorSequen) delete[] pExorSequen;   pExorSequen=NULL;
	if(pLiDIdx)	delete pLiDIdx;	pLiDIdx=NULL;
	return ret;
}





// 
double CalibrateWithBundleAdjust( std::vector<imgExOrientParam>  *exorParam, _iphCamera *camera, vector<AT_tiePoint> &pData, 
								Proj_Point *projs, int nprojs, double projThresh)
{
	int ret=1;

	//后方交会挑点
	// 	Align_Keypoint *pBuf1=NULL, *pBuf2=NULL;
	// 	
	// 	pBuf1=new Align_Keypoint[nprojs];
	// 	pBuf2=new Align_Keypoint[nprojs];
	//	refineNum=nprojs;

	double	*A0=0, *L0=0;
	//	double  *x_init=0, *y_init=0;
	double *ptr1, *ptr2, *ptr3;
	double R[9], Rmis[9], Rimu[9];
	double x, y;
	double a1, a2, a3, b1, b2, b3, c1, c2, c3;
	double Xgps, Ygps, Zgps;
	double phi, omega, kappa;
	double Xs, Ys, Zs, f;		
	double X, Y, Z;
	double _X, _Y, _Z;
	double pX[3];
	double x0, y0;
	double X_det[9];	//安置参数的改正数的改正数

	int nx = 6;

	int n3Dpoint=pData.size();

	double *N12, *N22, *L2;

	//////////////////////////////////////////////////////////////////////////

	N12 = new double[3*nx*n3Dpoint];
	N22 = new double[3*3*n3Dpoint];

	L2 = new double[3*n3Dpoint];

	//////////////////////////////////////////////////////////////////////////
	x0=camera->m_x0;
	y0=camera->m_y0;
	f=camera->m_f;

	// 	A0=(double*)malloc(nprojs*2*6*sizeof(double));
	// 	L0=(double*)malloc(2*nprojs*sizeof(double));

	// 	x_init=(double*)malloc(refineNum*sizeof(double));
	// 	y_init=(double*)malloc(refineNum*sizeof(double));


	int iteration=0;

	double thresh=1e-12;	//0.1'
	double p_eL2=0;
	double min_eL2=1.7e+308;

	double b_phi, b_omega, b_kappa, b_Xs, b_Ys, b_Zs, b_f;

	int pos=0;


	// 求连接点初值
	ForwardIntersctForTiePoints( exorParam, camera, pData, false);		//前交

	int nFrame=exorParam->size();

	{
		int i, j;

		memset(pOffset, 0, sizeof(ors_int32)*nFrame);

		for(i=0; i<n3Dpoint; i++)
		{
			AT_tiePoint tiePt= pData[i];
			for(j=0; j<tiePt.projs.size(); j++)
			{
				imgPoint imgpt=tiePt.projs[j];
				int imgID=imgpt.imgID;

				int offset=pLut[imgID]+pOffset[imgID];

				projs[offset].iXyz = i;

				projs[offset].xi=imgpt.xi;
				projs[offset].yi=imgpt.yi;

				pOffset[imgID]++;
			}
		}
	}


	ForwardIntersctForTiePoints(exorParam, camera, pData, false );

	for( iteration=0; iteration<300; iteration++ )
	{
// 		if( iteration > 50 )
// 			nx = 6;

		int nFrame=exorParam->size();

		//////////////////////////////////////////////////////////////////////////

		phi  =imgMisalign.phi;
		omega=imgMisalign.omega;
		kappa=imgMisalign.kappa;

		Xs=imgMisalign.u;	
		Ys=imgMisalign.v;	
		Zs=imgMisalign.w;

//		f=imgMisalign.f;

		RotateMat_fwk(phi, omega, kappa, R);
		pos=0;

		double cosf = cos( phi), sinf = sin(phi);
		double cosw = cos(omega), sinw=sin(omega);
		double cosk = cos( kappa), sink=sin(kappa);

		double N11[49], L1[7], a[7], b[7], lx, ly, ax[3], bx[3], axi[3], bxi[3];

		//		double AA3[9], AL3[3], a3[3], b3[3];

		memset( N11, 0, nx*nx*sizeof(double));
		memset( L1, 0, nx*sizeof(double));

		memset( N12, 0, 3*nx*n3Dpoint*sizeof(double) );
		memset( N22, 0, 3*3*n3Dpoint*sizeof(double) );
		memset( L2, 0,  3*n3Dpoint*sizeof(double) );
		

		p_eL2=0;

		int i, j;
		for( i=0; i<nFrame; i++)
		{
			Xgps= (*exorParam)[i].Xs;
			Ygps= (*exorParam)[i].Ys;
			Zgps= (*exorParam)[i].Zs;

			RotateMat_fwk((*exorParam)[i].phi, (*exorParam)[i].omega, (*exorParam)[i].kappa, Rimu);

			for(j=pLut[i]; j<pLut[i+1]; j++)
			{
				// 	if(pBuf1[j].flag!=keypoint_inlier)
				// 		continue;

// 				X = Rimu[0]*(projs[j].X-Xgps) + Rimu[3]*(projs[j].Y-Ygps) + Rimu[6]*(projs[j].Z-Zgps);
// 				Y = Rimu[1]*(projs[j].X-Xgps) + Rimu[4]*(projs[j].Y-Ygps) + Rimu[7]*(projs[j].Z-Zgps);
// 				Z = Rimu[2]*(projs[j].X-Xgps) + Rimu[5]*(projs[j].Y-Ygps) + Rimu[8]*(projs[j].Z-Zgps);

				int iXyz = projs[j].iXyz;

				AT_tiePoint tiePt = pData[iXyz];

				X = Rimu[0]*( tiePt.X -Xgps ) + Rimu[3]*(tiePt.Y-Ygps) + Rimu[6]*(tiePt.Z-Zgps);
				Y = Rimu[1]*( tiePt.X -Xgps ) + Rimu[4]*(tiePt.Y-Ygps) + Rimu[7]*(tiePt.Z-Zgps);
				Z = Rimu[2]*( tiePt.X -Xgps ) + Rimu[5]*(tiePt.Y-Ygps) + Rimu[8]*(tiePt.Z-Zgps);

				_X = R[0]* (X-Xs) + R[3]*(Y-Ys) + R[6]*(Z-Zs);
				_Y = R[1]* (X-Xs) + R[4]*(Y-Ys) + R[7]*(Z-Zs);
				_Z = R[2]* (X-Xs) + R[5]*(Y-Ys) + R[8]*(Z-Zs);

				double xi, yi;
				//ImageProject(X, Y, Z, &xi, &yi, R, Xs, Ys, Zs, f);

				xi = -f*_X/_Z;
				yi = -f*_Y/_Z;

				//x_init[pos] = xi;
				//y_init[pos] = yi;	//像点坐标初始值

				//				double *L = L0 + 2*j;
				//				double *A = A0 + 2*j;

				//L[0]=
				projs[j].vx = lx = ( projs[j].xi - xi );
				//L[1]=
				projs[j].vy = ly = (projs[j].yi - yi );

				p_eL2+=lx*lx;
				p_eL2+=ly*ly;

				x= projs[j].xi - x0;	
				y= projs[j].yi - y0;


				a[0]= y*sinw - ( x/f* (x*cosk - y*sink) + f*cosk)*cosw;
				a[1]=-f*sink -   x/f* (x*sink + y*cosk);
				a[2]= y;
				a[3]= (R[0]*f + R[2]*x)/_Z;
				a[4]= (R[3]*f + R[5]*x)/_Z;
				a[5]= (R[6]*f + R[8]*x)/_Z;
				a[6]= -_X/_Z;

				b[0]= -x*sinw - ( y/f* (x*cosk - y*sink) - f*sink)*cosw;
				b[1]=-f*cosk -   y/f* (x*sink + y*cosk);
				b[2]=-x;
				b[3]= (R[1]*f + R[2]*y)/_Z;
				b[4]= (R[4]*f + R[5]*y)/_Z;
				b[5]= (R[7]*f + R[8]*y)/_Z;
				b[6]= -_Y/_Z;

				for( int row=0; row<nx; row++)
				{
					for( int col=0; col<nx; col++)
						N11[row*nx+col] += a[row]*a[col] + b[row]*b[col];
					L1[row] += a[row]*lx + b[row]*ly;
				}

				//////////////////////////////////////////////////////////////////////////
				ax[0] = -a[3];	ax[1] = -a[4];	ax[2] = -a[5];
				bx[0] = -b[3];	bx[1] = -b[4];	bx[2] = -b[5];

				axi[0] = Rimu[0]* ax[0] + Rimu[3]* ax[1] + Rimu[6]* ax[2];
				axi[1] = Rimu[1]* ax[0] + Rimu[4]* ax[1] + Rimu[7]* ax[2];
				axi[2] = Rimu[2]* ax[0] + Rimu[5]* ax[1] + Rimu[8]* ax[2];

				bxi[0] = Rimu[0]* bx[0] + Rimu[3]* bx[1] + Rimu[6]* bx[2];
				bxi[1] = Rimu[1]* bx[0] + Rimu[4]* bx[1] + Rimu[7]* bx[2];
				bxi[2] = Rimu[2]* bx[0] + Rimu[5]* bx[1] + Rimu[8]* bx[2];

				double *N12s = N12 + nx*3*iXyz;
				double *N22s = N22 + 3*3*iXyz;
				double *L2s = L2 + 3*iXyz;

				for( int row=0; row<nx; row++)
				{
					for( int col=0; col<3; col++ )
						N12s[row*3+col] += a[row]*axi[col] + b[row]*bxi[col];					
				}

				for( int row=0; row<3; row++)
				{
					for( int col=0; col<3; col++ )
						N22s[row*3+col] += axi[row]*axi[col] + bxi[row]*bxi[col];

					L2s[row] += axi[row]*lx + bxi[row]*ly;
				}
			}
		}


		p_eL2=sqrt(p_eL2/(nprojs-4));

		if(min_eL2>p_eL2)
		{
			min_eL2=p_eL2;
		}

		if(p_eL2<projThresh)
		{
			ret=2;
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		// N22求逆
 		for(i=0; i<n3Dpoint; i++)
 		{		
 			matrix_invert( 3, N22+3*3*i,N22+3*3*i );
		}

		// 改化N11
		double N122[21], N21s[21], N221[21], N11t[49], L1t[7];

		for(i=0; i<n3Dpoint; i++)
		{		
			double *N12s = N12 + nx*3*i;
			double *N22s = N22 + 3*3*i;
			double *L2s = L2 + 3*i;

			matrix_product( nx, 3, 3, 3, N12s, N22s, N122 );

			matrix_transpose( nx, 3, N12s, N21s );
			matrix_product( nx, 3, 3, nx, N122, N21s, N11t );

			matrix_product( nx, 3, 3, 1, N122, L2s, L1t );

			for( int j=0; j<nx*nx; j++)
				N11[j] -= N11t[j];

			for( int j=0; j<nx; j++)
				L1[j] -= L1t[j];
		}

		//////////////////////////////////////////////////////////////////////////
		// 求X, 结果在L1
		Gauss( N11, L1, nx );

		double XYZ[3];
		for( i=0; i<n3Dpoint; i++)
		{		
			double *N12s = N12 + nx*3*i;
			double *N22s = N22 + 3*3*i;
			double *L2s = L2 + 3*i;

			matrix_transpose_product( nx, 3, nx, 1, N12s, L1, L1t );

			for( int j=0; j<3; j++)
				L2s[j] -= L1t[j];

			matrix_product( 3, 3, 3, 1, N22s, L2s, XYZ );

			pData[i].X += XYZ[0];
			pData[i].Y += XYZ[1];
			pData[i].Z += XYZ[2];
		}


		//////////////////////////////////////////////////////////////////////////

		//dgelsy_driver(A0, L0, X_det, refineNum*2, 6, 1);

		phi += L1[0];

		if( nx > 1 )
			omega += L1[1];
	
		if( nx > 2 )
			kappa += L1[2];

		if( nx > 3 )
			Xs += L1[3];
		if( nx > 4 )
			Ys += L1[4];
		if( nx > 5 )
			Zs += L1[5];

		if( nx > 6 )
			f += L1[6];

		// 		phi+=AL3[0];
		// 		omega+=AL3[1];
		// 		kappa+=AL3[2];

		if(fabs(L1[0])<thresh && fabs(L1[1])<thresh && abs(L1[2])<thresh)
		{
			b_phi=phi;
			b_omega=omega;
			b_kappa=kappa;
			b_Xs=Xs;
			b_Ys=Ys;
			b_Zs=Zs;
			b_f=f;
			break;
		}

		imgMisalign.phi = phi;
		imgMisalign.omega = omega;
		imgMisalign.kappa = kappa;
		imgMisalign.u = Xs;
		imgMisalign.v = Ys;
		imgMisalign.w = Zs;
//		imgMisalign.f = f;

		iteration++;
	}

	// 	if(iteration<50)
	// 	{
	// 		double *e=new double[2*nprojs];
	// 		X_det[0]=Xs;	X_det[1]=Ys;	X_det[2]=Zs;
	// 		X_det[3]=phi;	X_det[4]=omega;	X_det[5]=kappa;
	// 
	// 		matrix_product(2*nprojs, 6, 6, 1, A, X_det, e);
	// 		double p_eL2=0;
	// 
	// 		for(i=0; i<2*nprojs; i++)
	// 		{
	// 
	// 		}
	// 
	// 		if(e)	delete[] e;	e=NULL;
	// 	}

FUNC_END:
	//	if(A0)	free(A0);
	//	if(L0)	free(L0);
	//	if(x_init)	free(x_init);
	//	if(y_init)	free(y_init);
	// 	if(pBuf1)	delete[] pBuf1;	pBuf1=NULL;
	// 	if(pBuf2)	delete[] pBuf2;	pBuf2=NULL;
	return	min_eL2;
}

