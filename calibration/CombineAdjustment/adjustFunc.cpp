#include "StdAfx.h"
#include "CombineAdjustment/BA_def.h"
#include "CombineAdjustment/adjustFunc.h"
#include "Calib_Camera.h"
#include "dpsMatrix.h"
#include "orsMath/orsIMatrixService.h"

void calcImgProj(/*double *calibParam, */double t[3], double angle[3], double objpt[3], double imgpt[2], void *adata)
{
	double uvw[3];
	double Rmis[9], Rimu[9], R[9];
	struct imgBA_globs *gl;

	gl=(struct imgBA_globs *)adata;

	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

	assert(matrixService.get());

	matrixService->RotateMatrix_fwk(angle[0], angle[1], angle[2], R);

	// 	matrix_ident(3, Rmis);
	// 
	// 	matrix_product(3, 3, 3, 3, Rimu, Rmis, R);

	double a1,a2,a3,b1,b2,b3,c1,c2,c3;

	a1 = R[0];	a2 = R[1];	a3 = R[2];
	b1 = R[3];	b2 = R[4];	b3 = R[5];
	c1 = R[6];	c2 = R[7];	c3 = R[8];

	// 	double vec1[3], vec2[3];
	// 
	// 	matrix_product(3, 3, 3, 1, Rimu, uvw, vec1);

	double vec2[3];
	vec2[0]=objpt[0]-t[0];
	vec2[1]=objpt[1]-t[1];
	vec2[2]=objpt[2]-t[2];

	double _X;
	double _Y;
	double _Z;

	double xp, yp;
	//	double x, y, deltax, deltay;
	double x0, y0, f, k1, k2, k3, p1, p2, _b1, _b2;
	double dx, dy, dr;
	double r2;

	x0 = gl->camInfo->m_x0;
	y0 = gl->camInfo->m_y0;
	f = gl->camInfo->m_f;
	// 	k1 = camInfo.k1;
	// 	k2 = camInfo.k2;
	// 	k3 = camInfo.k3;
	// 	p1 = camInfo.p1;
	// 	p2 = camInfo.p2;
	// 	_b1 = camInfo._b1;
	// 	_b2 = camInfo._b2;

	_X = a1 * vec2[0] + b1 * vec2[1] + c1 * vec2[2];
	_Y = a2 * vec2[0] + b2 * vec2[1] + c2 * vec2[2];
	_Z = a3 * vec2[0] + b3 * vec2[1] + c3 * vec2[2];

	xp = - f * _X / _Z;
	yp = - f * _Y / _Z;

	// 	r2 = xp*xp + yp*yp;
	// 
	// 	dr = ( k1+ ( k2 + k3*r2)*r2 )*r2;
	// 	dx = xp*dr;
	// 	dy = yp*dr;
	// 
	// 	dx += p1*( r2 + 2*xp*xp ) + 2*p2*xp*yp;
	// 	dy += 2*p1*xp*yp + p2*( r2 + 2*yp*yp );
	// 
	// 	dx=_b1 * xp + _b2 * yp;
	// 
	// 	imgpt[0] = xp + x0 + dx;	//校正前的像平面坐标
	// 	imgpt[1] = yp + y0 + dy;

	imgpt[0] = xp;
	imgpt[1] = yp;
}

void calcImgProjJacKRTS(double t[3], double qr0[3], double imgpt[2], double objpt[3], 
	double jacmRT[2][6], double jacmS[2][3], void *adata)
{
	double phi, omega, kappa;
	double a1,a2,a3;
	double b1,b2,b3;
	double c1,c2,c3;

	double X,Y,Z;
	double Xs,Ys,Zs;
	double _X,_Y,_Z;

	double x, y;
	double x0, y0, f;

	struct imgBA_globs *gl;
	gl=(struct imgBA_globs *)adata;

	x0=gl->camInfo->m_x0;
	y0=gl->camInfo->m_y0;
	f=gl->camInfo->m_f;
	
	phi = qr0[0];
	omega = qr0[1];
	kappa = qr0[2];

	X = objpt[0]; 	Y = objpt[1]; 	Z = objpt[2];
	Xs = t[0]; 		Ys = t[1];		Zs = t[2];

	a1 = cos(phi)*cos(kappa)-sin(phi)*sin(omega)*sin(kappa);
	a2 = -cos(phi)*sin(kappa)-sin(phi)*sin(omega)*cos(kappa);
	a3 = -sin(phi)*cos(omega);
	b1 = cos(omega)*sin(kappa);
	b2 = cos(omega)*cos(kappa);
	b3 = -sin(omega);
	c1 = sin(phi)*cos(kappa)+cos(phi)*sin(omega)*sin(kappa);
	c2 = -sin(phi)*sin(kappa)+cos(phi)*sin(omega)*cos(kappa);
	c3 = cos(phi)*cos(omega); 

	_X = a1 * (X - Xs) + b1 * (Y - Ys) + c1 * (Z - Zs);
	_Y = a2 * (X - Xs) + b2 * (Y - Ys) + c2 * (Z - Zs);
	_Z = a3 * (X - Xs) + b3 * (Y - Ys) + c3 * (Z - Zs);

	/*x = - f * _X / _Z;
	y = - f * _Y / _Z;*/

	x = imgpt[0]/* - x0*/;
	y = imgpt[1]/* - y0*/;

	jacmRT[0][0] = 1 / _Z * (a1 * f + a3 * x);
	jacmRT[0][1] = 1 / _Z * (b1 * f + b3 * x);
	jacmRT[0][2] = 1 / _Z * (c1 * f + c3 * x);

	jacmRT[0][3] = y * sin(omega) - (x / f * (x * cos(kappa) - y * sin(kappa)) + f * cos(kappa)) * cos(omega);
	jacmRT[0][4] = - f * sin(kappa) - x / f * (x * sin(kappa) + y * cos(kappa));
	jacmRT[0][5] = y;

	jacmRT[1][0] = 1 / _Z * (a2 * f + a3 * y);
	jacmRT[1][1] = 1 / _Z * (b2 * f + b3 * y);
	jacmRT[1][2] = 1 / _Z * (c2 * f + c3 * y);

	jacmRT[1][3] = - x * sin(omega) - (y / f * (x * cos(kappa) - y * sin(kappa)) - f * sin(kappa)) * cos(omega);
	jacmRT[1][4] = - f * cos(kappa) - y / f * (x * sin(kappa) + y * cos(kappa));
	jacmRT[1][5] = - x;

	jacmS[0][0] = - jacmRT[0][0];
	jacmS[0][1] = - jacmRT[0][1];
	jacmS[0][2] = - jacmRT[0][2];

	jacmS[1][0] = - jacmRT[1][0];
	jacmS[1][1] = - jacmRT[1][1];
	jacmS[1][2] = - jacmRT[1][2];
}

