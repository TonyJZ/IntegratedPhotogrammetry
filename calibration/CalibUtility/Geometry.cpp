#include "stdafx.h"

#include "math.h"


#include "geometry.h"


/********************************************************************************
L1-----P-----L2
       |
       P1
********************************************************************************/
double Perpendicular( double x1, double y1, double x2, double y2, double px, double py, double *x, double *y )
{
	double dx, dy, t;
	double dx1, dy1;
	
	dx = x2-x1;		dy = y2-y1;
	dx1 = x1 - px;	dy1 = y1 - py;

	t = -( dx* dx1 + dy*dy1 ) / (dx*dx + dy*dy);

	*x = x1 + dx * t;
	*y = y1 + dy * t;

	return t;

}



double P3ArcPara( POINT3D pts[3], double *xc,double *yc, double ang[3]) 
{ 
	int		i; 
	double	da[2]; 
	double	dx0,dy0,dx1,dy1; 
	double	r0,r1,r2,det,dx,dy;
	double	xoff,yoff; 
	
	xoff = pts[1].X;
	yoff = pts[1].Y;
	for( i=0; i<3; i++)
	{
		pts[i].X -= xoff;
		pts[i].Y -= yoff;
	}
	
	dx0=pts[0].X - pts[1].X;	dx1=pts[1].X-pts[2].X; 
	dy0=pts[0].Y - pts[1].Y;	dy1=pts[1].Y-pts[2].Y; 
	
	r0 = pts[0].X*pts[0].X + pts[0].Y*pts[0].Y; 
	r1 = pts[1].X*pts[1].X + pts[1].Y*pts[1].Y; 
	r2 = pts[2].X*pts[2].X + pts[2].Y*pts[2].Y; 
	
	det = dx0*dy1 - dx1*dy0; 
	
	*xc = ( dy1*(r0-r1) - dy0*(r1-r2) )/(2*det); 
	*yc = (-dx1*(r0-r1) + dx0*(r1-r2) )/(2*det); 
	
	r2 = double( sqrt( *xc * ( *xc - 2*pts[0].X) + *yc * (*yc - 2*pts[0].Y) + r0 ) ); 
	
	for( i=0; i<3; i++) 
	{ 
		dx = pts[i].X - *xc; 
		dy = pts[i].Y - *yc; 
		if( dy == 0 )	
		{ 
			if( dx > 0 ) ang[i]=0; 
			else	ang[i]=180; 
			continue; 
		} 
		if( dx == 0 )	
		{ 
			if( dy > 0 ) ang[i]=90; 
			else	ang[i]=270; 
			continue; 
		} 
		ang[i] = double( atan2(dy,dx)*180/3.14159 ); 
		if( ang[i] < 0 )	ang[i] += 360; 
	} 
	
	da[0] = ang[1] - ang[0]; 
	if( da[0] < 0 )	da[0] += 360; 
	da[1] = ang[2] - ang[0]; 
	if( da[1] < 0 )	da[1] += 360;
	
	*xc += xoff;	*yc += yoff;
	for( i=0; i<3; i++)
	{
		pts[i].X += xoff;
		pts[i].Y += yoff;
	}
	
	if( da[1] > da[0] ) return ((double)r2); 
	else return ((double)-r2); 
} 



double TanP2ArcPara(double a,double b,POINT3D pts[2],double *xc,double *yc,double ang[2]) 
{ 
	double dx,dy; 
	double  t,r,da; 
	
	dx  = pts[1].X - pts[0].X; 
	dy  = pts[1].Y - pts[0].Y; 
	t   = double( (dx*dx+dy*dy)/(b*dx-a*dy)*0.5 ); 
	*xc = pts[0].X + b*t; 
	*yc = pts[0].Y - a*t; 
	
	dx = pts[0].X - *xc; 
	dy = pts[0].Y - *yc; 
	
	if( dy == 0 )	
	{ 
		if( dx > 0 )	ang[0]=0,  da= b; 
		else		ang[0]=180,da=-b; 
	} 
	else if( dx == 0 )	
	{ 
		if( dy > 0 )	ang[0]=90, da=-a; 
		else 		ang[0]=270,da= a; 
	} 
	else 	
	{ 
		ang[0] = double( atan2(dy,dx)*180/3.14159 ); 
		if( fabs(dx) > fabs(dy) ) 
			da = -a/(pts[0].Y - *yc); 
		else 	da =  b /(pts[0].X - *xc); 
	} 
	
	dx = pts[1].X - *xc; 
	dy = pts[1].Y - *yc; 
	if( dy == 0 )	
	{ 
		if( dx > 0 ) ang[1]=0; 
		else	ang[1]=180; 
	} 
	else if( dx == 0 )	
	{ 
		if( dy > 0 ) ang[1]=90; 
		else	ang[1]=270; 
	} 
	else ang[1] = double( atan2(dy,dx)*180/3.14159 ); 
	
	if( ang[0] < 0 ) ang[0] += 360; 
	if( ang[1] < 0 ) ang[1] += 360; 
	
	r = double( fabs(t) ); 
	if( da < 0 ) r=-r; 
	
	return r; 
} 



/************************************ RotateMat.c ****************************
	function :	set up the rotation matrix
	input 	 :	a,b,c--------- angle array
	output	 :	*r--------- rotation matrix

				                 | 0 -c -b |      | 0 -k -f |
	R=(I+S)*transpose(I-S)   S== | c  0 -a |/2 == | k  0 -w |/2
				                 | b  a  0 |      | f  w  0 |

****************************************************************************/
void RotateMat_abc(double a,double b,double c,double *r)
{
	double aa,bb,cc,ss,ab,ac,bc;

	aa=(double)a*a*0.25;	ab=(double)a*b*0.5;
	bb=(double)b*b*0.25;	bc=(double)b*c*0.5;
	cc=(double)c*c*0.25;	ac=(double)a*c*0.5;

	ss=1/(1+aa+bb+cc);

	*r++ =ss*(1+aa-bb-cc); 	*r++ =ss*(-ab-c); 	*r++ =ss*( ac-b);
	*r++ =ss*(-ab+c);	*r++ =ss*(1+bb-aa-cc);	*r++ =ss*(-bc-a);
	*r++ =ss*( ac+b);	*r++ =ss*(-bc+a);	*r =ss*(1+cc-aa-bb);
}


double R2abc(double *R, double *ao, double *bo, double *co)
{
	double a,b,c,d, t;
	
	c = (R[3] - R[1])/2;
	b = (R[6] - R[2])/2;
	a = (R[7] - R[5])/2;
	
	t = a*a + b*b + c*c;

	d = 2*( 1 - sqrt( 1 - t) ) / t;

	if(  fabs( 1/d + (a*a-b*b-c*c)/4*d - R[0] ) > 0.00001  )
		d = 2*( 1 + sqrt( 1 - t) ) / t;
	
	*ao = a*d;	*bo = b*d;	*co = c*d;

	return d;
}

//////////////////////////////////////////////////////////////////////////

void RotMatMat(double *R1, double *R2, double *R )
{
	int i,j,k;

	for( i=0; i<3; i++)
	{
		for( j=0; j<3;j++)
		{
			*R = 0;
			for( k=0; k<3; k++)
				*R++ += *R1++ * R2[3*k];
			R1 -= 3;
		}

		R2++;	R1 += 3;
	}
}


void RotateMat_X( double angle, double *R )
{
	memset( R, 0, 9*sizeof(double));

	double cosA, sinA;

	cosA = cos( angle);	sinA = sin( angle);

	R[0] = 1;	/*R[1] =		R[2] = */
	/*R[3]=*/	R[4] = cosA;	R[5] = sinA;
	/*R[6]=*/	R[7] =-sinA;	R[8] = cosA;
}


void RotateMat_Y( double angle, double *R )
{
	memset( R, 0, 9*sizeof(double));

	double cosA, sinA;

	cosA = cos( angle);	sinA = sin( angle);

	R[0] = cosA;	/*R[1] =*/	 R[2] = sinA;
	/*R[3]=*/		R[4] = 1;	/* R[5] = */
	R[6] = -sinA;	/*R[7] =*/	 R[8] = cosA;
}


void RotateMat_Z( double angle, double *R )
{
	memset( R, 0, 9*sizeof(double));

	double cosA, sinA;

	cosA = cos( angle);	sinA = sin( angle);

	R[0] = cosA;	R[1] = sinA;	/*R[2] =*/	
	R[3] = -sinA;	R[4] = cosA;	/* R[5] = */
	/*R[6]=*/		/*R[7] =*/		R[8] = 1;
}


//
// 采用中式的phi, omega, kappa表达法, 中式的旋转矩阵
//
void RotateMat_fwk( double f, double w, double k, double *r)
{
	double cosf,sinf;
	double cosw,sinw;
	double cosk,sink;

	cosf = cos( f );	sinf = sin( f );
	cosw = cos( w );	sinw = sin( w );
	cosk = cos( k );	sink = sin( k );

	*r++ =  cosf*cosk - sinf*sinw*sink;		*r++ = -cosf*sink - sinf*sinw*cosk;	*r++ = -sinf*cosw;
	*r++ =  cosw*sink;						*r++ =  cosw*cosk;					*r++ = -sinw;
	*r++ =  sinf*cosk + cosf*sinw*sink;		*r++ = -sinf*sink + cosf*sinw*cosk;	*r   =  cosf*cosw;
}

void R2fwk(double *R, double *phi, double *omega, double *kapa)
{

	*phi   = atan2( -R[2], R[8] );
	*omega = asin( -R[5] );
	*kapa  = atan2(  R[3], R[4] );
}


//
// 采用欧美的omega, phi, kappa表达法, 中式的旋转矩阵
//
void RotateMat_wfk( double w, double f, double k, double *r)
{

	f = -f;

	//////////////////////////////////////////////////////////////////////////
	double cosf,sinf;
	double cosw,sinw;
	double cosk,sink;

	cosw = cos( w );	sinw = sin( w );
	cosf = cos( f );	sinf = sin( f );
	cosk = cos( k );	sink = sin( k );

	*r++ =  cosf*cosk;						*r++ =-cosf*sink;					*r++ =-sinf;
	*r++ =  cosw*sink - sinw*sinf*cosk;		*r++ = cosw*cosk+sinw*sinf*sink;	*r++ =-sinw*cosf;
	*r++ =  sinw*sink + cosw*sinf*cosk;		*r++ = sinw*cosk-cosw*sinf*sink;	*r   = cosw*cosf;
}                         

//
// 欧美的旋转矩阵
// Calculate rotation matrix based on omega, phi, and kappa 
void RotationMatrixOPK(double omega, double phi, double kappa, double *r )
{
	/* Compute trig functions */
	double sinw, cosw, sinf, cosf, sink, cosk;

	sinw = sin(omega);	cosw = cos(omega);
	sinf = sin(phi);	cosf = cos(phi);
	sink = sin(kappa);	cosk = cos(kappa);

	/* Compute rotation matrix elements */
	*r++ = cosf * cosk;		*r++ = sinw * sinf * cosk + cosw * sink;	*r++ = -cosw * sinf * cosk + sinw * sink;
	*r++ = -cosf * sink;	*r++ = -sinw * sinf * sink + cosw * cosk;	*r++ = cosw * sinf * sink + sinw * cosk;
	*r++ = sinf;			*r++ = -sinw * cosf;						*r++ = cosw * cosf;
}


//
// 采用欧美的omega, phi, kappa表达法, 中式的旋转矩阵
//
void R2wfk(double *R, double *phi, double *omega, double *kapa)
{
	*omega  = atan2( -R[5], R[8] );	
	*phi = asin( -R[2] );
	*kapa  = atan2( -R[1], R[0] );

	//////////////////////////////////////////////////////////////////////////
	*phi = - *phi;
}

void RotateMat_roll(double roll, double *r)
{
	memset( r, 0, 9*sizeof(double));
	
	double cosR, sinR;
	
	cosR = cos(roll);	sinR = sin(roll);
	
	r[0] = 1;	/*r[1] =		r[2] = */
	/*r[3]=*/	r[4] = cosR;	r[5] = -sinR;
	/*r[6]=*/	r[7] = sinR;	r[8] = cosR;
}

void RotateMat_pitch(double pitch, double *r)
{
	memset( r, 0, 9*sizeof(double));
	
	double cosP, sinP;
	
	cosP = cos(pitch);	sinP = sin(pitch);
	
	r[0] = cosP;	/*r[1]=*/		r[2] = sinP;
	/*r[3]=*/		r[4] = 1;		/*r[5]=*/
	r[6]= -sinP;	/*r[7]=*/		r[8] = cosP;
}

void RotateMat_heading(double heading, double *r)
{
	memset( r, 0, 9*sizeof(double));
	
	double cosH, sinH;
	
	cosH = cos(heading);	sinH = sin(heading);
	
	r[0] = cosH;	r[1] = -sinH;	/*r[2]=*/
	r[3] = sinH;	r[4] = cosH;	/*r[5]=*/
	/*r[6] =;		r[7] =;*/		r[8] = 1;
}

//Rn=R(H)*R(P)*R(R)
void RotateMat_rph(double roll, double pitch, double heading, double *r)
{
	double cosR,sinR;
	double cosP,sinP;
	double cosH,sinH;
	
	cosR = cos(roll);		sinR = sin(roll);
	cosP = cos(pitch);		sinP = sin(pitch);
	cosH = cos(heading);	sinH = sin(heading);
	
	*r++ =  cosH*cosP;	*r++ = -sinH*cosR + cosH*sinP*sinR;	*r++ = sinH*sinR + cosH*sinP*cosR;
	*r++ =  sinH*cosP;	*r++ = cosH*cosR + sinH*sinP*sinR;	*r++ = sinH*sinP*cosR - cosH*sinR;
	*r++ =  -sinP;		*r++ = cosP*sinR;					*r   = cosP*cosR;
}


// void RotateMat_TPlane2WGS84_NED(double lat0, double lon0, double *R)
// {
// 	double cosB, sinB;	//纬度
// 	double cosL, sinL;	//经度
// 	
// 	cosB=cos(lat0);		sinB=sin(lat0);
// 	cosL=cos(lon0);		sinL=sin(lon0);
// 	
// 	R[0]=-cosL*sinB;	R[1]=-sinL;		R[2]=-cosL*cosB;
// 	R[3]=-sinL*sinB;	R[4]=cosL;		R[5]=-sinL*cosB;
// 	R[6]=cosB;			R[7]=0.0e0;		R[8]=-sinB;
// }

// void RotateMat_WGS842TPlane_ENU1(double lat0, double lon0, double *R)
// {//逆矩阵再旋转
// 	double cosB, sinB;	//纬度
// 	double cosL, sinL;	//经度
// 	double RT[9];
// 	
// 	cosB=cos(lat0);		sinB=sin(lat0);
// 	cosL=cos(lon0);		sinL=sin(lon0);
// 	
// 	RT[0]=-cosL*sinB;	RT[3]=-sinL;	RT[6]=-cosL*cosB;
// 	RT[1]=-sinL*sinB;	RT[4]=cosL;		RT[7]=-sinL*cosB;
// 	RT[2]=cosB;			RT[5]=0.0e0;	RT[8]=-sinB;
// 
// 	//		|0  1  0|   |-1  0  0|
// 	//	R=	|1  0  0| * |0   1  0| * RT
// 	//		|0  0  1|   |0  0  -1|
// 	R[0]=-sinL;			R[1]=cosL;			R[2]=0.0e0;
// 	R[3]=-cosL*sinB;	R[4]=-sinL*sinB;	R[5]=cosB;
// 	R[6]=cosL*cosB;		R[7]=sinL*cosB;		R[8]=sinB;
// }

void RotateMat_NED2ECEF(double lat0, double lon0, double *R)
{
 	double cosB, sinB;	//纬度
	double cosL, sinL;	//经度
	
	cosB=cos(lat0);		sinB=sin(lat0);
	cosL=cos(lon0);		sinL=sin(lon0);

	R[0]=-cosL*sinB;	R[1]=-sinL;		R[2]=-cosL*cosB;
	R[3]=-sinL*sinB;	R[4]=cosL;		R[5]=-sinL*cosB;
	R[6]=cosB;			R[7]=0;			R[8]=-sinB;
}

void RotateMat_ENU2ECEF(double lat0, double lon0, double *R)
{
	double cosB, sinB;	//纬度
	double cosL, sinL;	//经度
	
	cosB=cos(lat0);		sinB=sin(lat0);
	cosL=cos(lon0);		sinL=sin(lon0);
	
	R[0]=-sinL;		R[1]=-sinB*cosL;	R[2]=cosB*cosL;
	R[3]=cosL;		R[4]=-sinB*sinL;	R[5]=cosB*sinL;
	R[6]=0;			R[7]=cosB;			R[8]=sinB;
}

void RotateMat_ECEF2ENU(double lat0, double lon0, double *R)
{
	double cosB, sinB;	//纬度
	double cosL, sinL;	//经度
	
	cosB=cos(lat0);		sinB=sin(lat0);
	cosL=cos(lon0);		sinL=sin(lon0);
	
	R[0]=-sinL;			R[1]=cosL;			R[2]=0;
	R[3]=-sinB*cosL;	R[4]=-sinB*sinL;	R[5]=cosB;
	R[6]=cosB*cosL;		R[7]=cosB*sinL;		R[8]=sinB;
}

void RotateMat_ECEF2NED(double lat0, double lon0, double *R)
{
	double cosB, sinB;	//纬度
	double cosL, sinL;	//经度
	
	cosB=cos(lat0);		sinB=sin(lat0);
	cosL=cos(lon0);		sinL=sin(lon0);
	
	R[0]=-cosL*sinB;	R[1]=-sinL*sinB;	R[2]=cosB;
	R[3]=-sinL;			R[4]=cosL;			R[5]=0;
	R[6]=-cosL*cosB;	R[7]=-sinL*cosB;	R[8]=-sinB;
}