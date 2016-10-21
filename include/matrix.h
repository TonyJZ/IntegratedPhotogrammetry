/* 
 *  Copyright (c) 2008  Noah Snavely (snavely (at) cs.washington.edu)
 *    and the University of Washington
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

/* matrix.h */
/* Various linear algebra routines */

#ifndef __matrix_h__
#define __matrix_h__


#ifdef MATRIX_EXPORTS
#define _matrix_dll_ __declspec(dllexport)
#else
#define _matrix_dll_ __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Fill a given matrix with an n x n identity matrix */
void _matrix_dll_ matrix_ident(int n, double *A);

/* Fill a given matrix with an m x n matrix of zeroes */
void _matrix_dll_ matrix_zeroes(int m, int n, double *A);
    
/* Transpose the m x n matrix A and put the result in the n x m matrix AT */
void _matrix_dll_ matrix_transpose(int m, int n, double *A, double *AT);

/* Compute the matrix product R = AB */
void _matrix_dll_ matrix_product(int Am, int An, int Bm, int Bn, 
                    const double *A, const double *B, double *R);

void _matrix_dll_ matrix_product_old(int Am, int An, int Bm, int Bn, 
                        const double *A, const double *B, double *R);
void _matrix_dll_ matrix_transpose_product_old(int Am, int An, int Bm, int Bn, 
                                  double *A, double *B, double *R);
void _matrix_dll_ matrix_transpose_product2_old(int Am, int An, int Bm, int Bn, 
                                   double *A, double *B, double *R);

void _matrix_dll_ matrix_product_ipp(int Am, int An, int Bn, 
                        const double *A, const double *B, double *R);
void _matrix_dll_ matrix_transpose_product_ipp(int Am, int An, int Bn, 
                                  const double *A, const double *B, double *R);
void _matrix_dll_ matrix_transpose_product2_ipp(int Am, int An, int Bm, 
                                   const double *A, const double *B, 
                                   double *R);
void _matrix_dll_ matrix_array_product_ipp(int count, int Am, int An, int Bn,
                              const double *A, const double *B, double *R);

void _matrix_dll_ matrix_product33(double *A, double *B, double *R);
_matrix_dll_ void matrix_product121(double *A, double *b, double *r);
_matrix_dll_ void matrix_product131(double *A, double *b, double *r);
_matrix_dll_ void matrix_product331(double *A, double *b, double *r);
_matrix_dll_ void matrix_product341(double *A, double *b, double *r);    
_matrix_dll_ void matrix_product44(double *A, double *B, double *R);
_matrix_dll_ void matrix_product441(double *A, double *b, double *r);
    
/* Compute the power of a matrix */
_matrix_dll_ void matrix_power(int n, double *A, int pow, double *R);

/* Compute the matrix product R = A B^T */
_matrix_dll_ void matrix_transpose_product2(int Am, int An, int Bm, int Bn, double *A, double *B, double *R);

/* Compute the matrix sum R = A + B */
_matrix_dll_ void matrix_sum(int Am, int An, int Bm, int Bn, 
                double *A, double *B, double *R);

/* Compute the matrix difference R = A - B */
_matrix_dll_ void matrix_diff(int Am, int An, int Bm, int Bn, double *A, double *B, double *R);

/* Compute the determinant of a 3x3 matrix */
_matrix_dll_ double matrix_determinant3(double *A);

/* Compute the matrix product R = A^T B */
_matrix_dll_ void matrix_transpose_product(int Am, int An, int Bm, int Bn, double *A, double *B, double *R);

/* Compute (transpose of) LU decomposition of A */
_matrix_dll_ void matrix_lu(int n, double *A, double *LU, int *ipiv);
_matrix_dll_ void matrix_lu_no_transpose(int n, double *A, double *LU, int *ipiv);
    
/* Solve a system of equations using a precomputed LU decomposition */
_matrix_dll_ void matrix_solve_lu(int n, double *LU, int *ipiv, double *b, double *x);

/* Invert the n-by-n matrix A, storing the result in Ainv */
_matrix_dll_ void matrix_invert(int n, double *A, double *Ainv);
_matrix_dll_ void matrix_invert_inplace(int n, double *A);
    
/* Get the norm of the matrix */
_matrix_dll_ double matrix_norm(int m, int n, double *A);

/* Get the [squared] norm of the matrix */
_matrix_dll_ double matrix_normsq(int m, int n, double *A);

/* Scale a matrix by a scalar */
_matrix_dll_ void matrix_scale(int m, int n, double *A, double s, double *R);

/* Print the given m x n matrix */
_matrix_dll_ void matrix_print(int m, int n, double *A);

/* Read a matrix from a file */
_matrix_dll_ void matrix_read_file(int m, int n, double *matrix, char *fname);

/* Write a matrix to a file */
_matrix_dll_ void matrix_write_file(int m, int n, double *matrix, char *fname);

/* Return the product x**T A x */
_matrix_dll_ double matrix_double_product(int n, double *A, double *x);

/* Compute the cross product of two 3 x 1 vectors */
_matrix_dll_ void matrix_cross(const double *u, const double *v, double *w);
_matrix_dll_ void matrix_cross4(const double *u, const double *v, const double *w, 
		   double *x);
    
/* Create the 3x3 cross product matrix from a 3-vector */
_matrix_dll_ void matrix_cross_matrix(double *v, double *v_cross);

/* Convert a rotation matrix to axis and angle representation */
_matrix_dll_ void matrix_to_axis_angle(double *R, double *axis, double *angle);
_matrix_dll_ void axis_angle_to_matrix(double *axis, double angle, double *R);
_matrix_dll_ void axis_angle_to_matrix4(double *axis, double angle, double *R);

/* Convert a matrix to a normalize quaternion */
_matrix_dll_ void matrix_to_quaternion(double *R, double *q);
/* Convert a normalized quaternion to a matrix */
_matrix_dll_ void quaternion_to_matrix(double *q, double *R);
    
/* Decompose a square matrix into an orthogonal matrix and a symmetric
 * positive semidefinite matrix */
_matrix_dll_ void matrix_polar_decomposition(int n, double *A, double *Q, double *S);
    
/* Driver for the minpack function lmdif, which uses
 * Levenberg-Marquardt for non-linear least squares minimization */
_matrix_dll_ void lmdif_driver(void *fcn, int m, int n, double *xvec, double tol);
_matrix_dll_ void lmdif_driver2(void *fcn, int m, int n, double *xvec, double tol);
_matrix_dll_ void lmdif_driver3(void *fcn, int m, int n, double *xvec, double tol,
                   int maxfev, double *H);

/* Driver for the lapack function dgelss, which finds x to minimize
 * norm(b - A * x) */
_matrix_dll_ void dgelss_driver(double *A, double *b, double *x, int m, int n, int nrhs);
void _matrix_dll_ dgelsy_driver(double *A, double *b, double *x, int m, int n, int nrhs);

/* Version of above where matrix is already in column-major order */
_matrix_dll_ void dgelsy_driver_transpose(double *A, double *b, double *x, 
			     int m, int n, int nrhs);

/* Solve an n x n system */
_matrix_dll_ void dgesv_driver(int n, double *A, double *b, double *x);
    
/* n: the order of matrix A
 * A: matrix for which the eigenvectors/values are to be computed
 * evec: output array containing the eigenvectors
 * eval: output array containing the eigenvalues
 *
 * Note: Assumes the results are real! */
_matrix_dll_ int dgeev_driver(int n, double *A, double *evec, double *eval);

/* Compute singular value decomposition of an m x n matrix A */
_matrix_dll_ int dgesvd_driver(int m, int n, double *A, double *U, double *S, double *VT);
/* Compute singular value decomposition of an m x n matrix A 
 * (only compute S and VT) */
_matrix_dll_ int dgesvd_driver_vt(int m, int n, double *A, double *S, double *VT);

/* Compute Cholesky decomposition of an nxn matrix */
_matrix_dll_ void dpotrf_driver(int n, double *A, double *U);

/* Compute a QR factorization of an m by n matrix A */
_matrix_dll_ void dgeqrf_driver(int m, int n, double *A, double *Q, double *R);

/* Compute an RQ factorization of an m by n matrix A */
_matrix_dll_ void dgerqf_driver(int m, int n, double *A, double *R, double *Q);

/* Find the unit vector that minimizes ||Ax|| */
_matrix_dll_ void matrix_minimum_unit_norm_solution(int m, int n, double *A, double *x);

_matrix_dll_ void cblas_dgemm_driver_x(int m1, int n12, int k2,
                          int as, int bs, int cs,
                          double *a, double *b, double *c);

_matrix_dll_ void cblas_dgemm_driver(int m1, int n12, int k2, 
                        double *a, double *b, double *c);
    
_matrix_dll_ void cblas_dgemm_driver_transpose(int m1, int n12, int k2, 
                                  double *a, double *b, double *c);
    
_matrix_dll_ void cblas_dgemm_driver_transpose2(int m1, int n12, int k2, 
                                   double *a, double *b, double *c);

_matrix_dll_ void slerp(double *v1, double *v2, double t, double *v3);
    
#ifdef __cplusplus
}
#endif

#endif /* __matrix_h__ */
