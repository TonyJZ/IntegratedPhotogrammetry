/*
-------------------------------------------------------------------------------------

   Filename:           GMatrix.cpp

   Date started:       11/09/1999 (month/day/year)
   Date last modified: 03/24/2001
   Written by:         Glenn MacGougan [gdmacgou@hotmail.com]

   Purpose of file:    This file contains the implementaton code for a GMatrix class

   Acknowledgement:    This GMatrix library is based on Bruce Chaplin's GMatrix
                       library (Fall 1997) supplied for use by geomatics students.

   Terms of use:       This GMatrix library is freeware. Please acknowledge this
                       author in any published or produced work.

   Last revision:      a. Updated Print functions for cleaner output
                       b. Fixed a very uncommon ReadFromFile() bug
                       c. Updated some ReadFromFile Stuff                      
                       d. Added AddRow()
                          Added AddColumn()
                          Added InsertSubMatrix()
                          Added ExtractSubMatrix()
                       e. (March14,2001) 
                          changed member variable notation to 'm_'
                          Added RobustInverse() 
                                Factorize(), 
                                SolveByGaussianElimination()
                          Added LUFactorization()
                                

--------------------------------------------------------------------------------------
*/


#include "stdafx.h"

#define USING_MFC

#ifdef USING_MFC      
   #include <afxwin.h> // for CString & AfxMessageBox   
#endif


#include <stdlib.h>
#include <stdio.h>   
#include <string.h>
#include <ctype.h>
#include <conio.h>

#include <math.h>
//#include <iomanip.h>   
#include <fstream>


#include "GMatrix.h"


const long kMaxReadFromFileBuffer = 8192;

/////////////////////////////////////////////////////////////////////////////////
//           IMPLEMENTATION FOR GMatrix CLASS
//
//

// default constructor
//##ModelId=3C7561880011
GMatrix::GMatrix()
: m_rows(0), 
  m_cols(0),
  m_data(NULL)
{    
}


// destructor
//##ModelId=3C756188031E
GMatrix::~GMatrix()
{ 
   if( m_data )
      matDealloc(m_data); 
}


// vector style constructor
//##ModelId=3C75618800DC
GMatrix::GMatrix( int nrows )
: m_rows(nrows), 
  m_cols(1)
{ 
   matAlloc( m_rows, m_cols ); 
}



// GMatrix style constructor
//##ModelId=3C7561880198
GMatrix::GMatrix( int nrows, int ncols )
: m_rows(nrows), 
  m_cols(ncols)
{ 
   matAlloc( m_rows, m_cols ); 
}



// copy constructor
//##ModelId=3C7561880253
GMatrix::GMatrix( const GMatrix& mat )
: m_rows(mat.m_rows), 
  m_cols(mat.m_cols)
{
   if( mat.m_rows <= 0 || mat.m_cols <= 0 )
   { 
      m_rows = 0; 
      m_cols = 0;   
   }

   matAlloc( m_rows, m_cols );
   deepCopy( mat );
}





// assignment operator (constructor)
//##ModelId=3C75618F02EF
GMatrix& GMatrix::operator= (const GMatrix& mat)
{
   // trap assignment to self
   if( this == &mat )
      return *this;

   if( m_rows != mat.m_rows || m_cols != mat.m_cols )
   {
      matDealloc( m_data );
      m_rows = mat.m_rows;
      m_cols = mat.m_cols;
      matAlloc( m_rows, m_cols );
   }
   deepCopy( mat );
   return *this;
}



// private helper function to allocate memory using double ptrs
// based on GMatrix.c (Terry Labach & co.)
//##ModelId=3C756190032E
void GMatrix::matAlloc( const int nrows, const int ncols )
{
   int i = 0;

   // don't need to allocate memory
   if( nrows <= 0 || ncols <= 0)
   {
      m_rows = 0; 
      m_cols = 0;
      m_data = NULL;
      return;
   }

   // 64K is maximum under DOS and Turbo C
#if defined(__TURBOC__) || defined(MSDOS)
   long size = (long)nrows * ncols * sizeof (double);
   if( size > 65520L )
      MatrixError( "matAlloc", "Memory allocation greater than 64K" );
#endif

   m_rows = nrows;
   m_cols = ncols;

   // GMatrix is a pointer to pointer to type double
   // it points to the first element in an array of ptrs to types double
   m_data = new double* [m_rows];
   if( m_data == NULL)
      MatrixError( "matAlloc", "Memory allocation error");

   // GMatrix[0] is a ptr to double
   // it is the first element in array of ptrs to double
   // it points to the first double in the allocated memory
   //  内存是连续的
   m_data[0] = new double [m_rows * m_cols];
   if( m_data[0] == NULL)
      MatrixError( "matAlloc", "Memory allocation error");

   for( i = 1; i < m_rows; i++ )
      m_data[i] = m_data[0] + i * m_cols;

   Zero(); // set elements to zero
   return;
}



// private helper function to deallocate the GMatrix memory
// based on GMatrix.c by Terry Labach & co.
// WARNING: DO NOT CHANGE THE ORDER OF THE DELETE STATEMENTS!
//##ModelId=3C7561910001
void GMatrix::matDealloc( double** data )
{
   if( data )
   {
      delete [] data[0];
      delete [] data;
   }
   data = NULL;
}



// perform a deep copy of a GMatrix
// ie. copy element-for-element
//##ModelId=3C75619100AD
void GMatrix::deepCopy( const GMatrix& mat )
{
   int i = 0,
       j = 0;

   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         m_data[i][j] = mat[i][j];
}



// output an error to user, release memory and exit
// static function
//##ModelId=3C75619000AD
void GMatrix::MatrixError( const char* error )
{
   MatrixError( "", error );
}

// output an error to user and exit
// note that memory is not released
// static function
//##ModelId=3C756190014A
void GMatrix::MatrixError( const char* function, const char* error )
{
#ifdef USING_MFC
	ASSERT(false);
	CString errMsg = "GMatrix::";
	errMsg += function;
	errMsg += "() - ";
	errMsg += error;
	AfxMessageBox(errMsg);   
#endif
	
#ifndef USING_MFC
	char errmsg[2000]; // a large buffer
	sprintf( errmsg, "GMatrix::%s() Error: %s", function, error );   
	cerr << errmsg << endl;
	cerr << "Program Terminated. Press a key:";
	cerr.flush();
	getch();
#endif

   exit(1);
}

// output a message to user 
// static function
//##ModelId=3C75619001E6
void GMatrix::MatrixMessage( const char* function, const char* message )
{
#ifdef USING_MFC
   CString msgStr = "GMatrix::";
   msgStr += function;
   msgStr += "() - ";
   msgStr += message;
   AfxMessageBox(msgStr);
#endif
#ifndef USING_MFC
   char msg[2000]; // a large buffer
   sprintf( msg, "GMatrix::%s() - %s", function, message );   
   cerr << msg << endl;   
#endif   
}



//##ModelId=3C7561900282
void GMatrix::CheckIndex( int index, int maxIndex )
{
   if( maxIndex == 0 )
   {
      GMatrix::MatrixError( "CheckIndex", "Cannot index GMatrix with zero rows and/or cols" );      
   }

   if( index < 0 || index >= maxIndex )
   {
      char buffer[900];
      sprintf( buffer, "Array index, %d, out of bounds 0:%d", index, maxIndex );
      GMatrix::MatrixError( "CheckIndex", buffer );      
   }
   return;
}



// remove the GMatrix from memory
//##ModelId=3C756189012A
void GMatrix::Clear()
{
   matDealloc( m_data );
   m_data = NULL;
   m_rows = 0;
   m_cols = 0;
}


// set all GMatrix elements to zero
//##ModelId=3C75618901D6
void GMatrix::Zero()
{
	memset( m_data[0], 0, m_rows*m_cols*sizeof(double) );
	
	return;
}



// set all column elements to zero
//##ModelId=3C7561890273
void GMatrix::ZeroCol( const int col )
{
   int i = 0;

   if( m_cols == 0 || col < 0 || col >= m_cols )
      MatrixError( "ZeroCol", "Invalid column specified" );

   for( i = 0; i < m_rows; i++ )
      m_data[i][col] = 0.0;

   return;
}


// set all row elements to zero
//##ModelId=3C756189030F
void GMatrix::ZeroRow( const int row )
{
   int i = 0;
          
   if( m_rows == 0 || row < 0 || row >= m_rows )
      MatrixError( "ZeroRow", "Invalid column specified" );

   for( i = 0; i < m_cols; i++ )
      m_data[row][i] = 0.0;

   return;
}



// resets the GMatrix to an identity
//##ModelId=3C75618A01F6

void GMatrix::Identity(void)
{
   int i = 0,
       j = 0;

   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "I", "Identity called on GMatrix with zero rows and/or columns" );

   // reset to identity
   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         m_data[i][j] = (i == j) ? (1.0) : (0.0);
}



// return the transpose of a GMatrix
//##ModelId=3C75618F009E
GMatrix GMatrix::T() const
{
   int i = 0,
       j = 0;

   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "T", "Transpose called on GMatrix with zero rows and/or columns" );

   GMatrix temp(m_cols, m_rows); // store result
   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         temp[j][i] = m_data[i][j];

   return temp; // return a copy
}



// fills the GMatrix with the given value
//##ModelId=3C75618A0011
void GMatrix::Fill( const double value )
{
   int i = 0,
       j = 0;

   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "Fill", "called on GMatrix with zero rows and/or columns" );

   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         m_data[i][j] = value;

   return;
}



// fills the given row GMatrix with the given value
//##ModelId=3C75618A00AD
void GMatrix::FillRow( const int row, const double value )
{
   int j = 0;

   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "FillRow", "called on GMatrix with zero rows and/or columns" );

   if( row < 0 || row >=m_rows )
      MatrixError( "FillRow", "called with a row value invalid!" );

   for( j = 0; j < m_cols; j++ )
      m_data[row][j] = value;

   return;
}



// fills the given column GMatrix with the given value
//##ModelId=3C75618A014A
void GMatrix::FillColumn( const int col, const double value )
{
   int i = 0;

   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "FillColumn", "called on GMatrix with zero rows and/or columns" );

   if( col < 0 || col >= m_cols )
      MatrixError( "fillColumn", "called with the inputed column value invalid!" );

   for( i = 0; i < m_rows; i++ )
      m_data[i][col] = value;

   return;
}



// inverse of a symmetric positive-definite GMatrix by Cholesky decomp.
// WARNING: GMatrix must be symmetric +ve definite
//
// Original Author:   Bryan Townsend
// Modifications:     Darren Cosandier
//                    Terry Labach
//                    Grant Chevallier
//                    Glenn MacGougan
//
//   Any of the following errors cause a program exit:
//   o   GMatrix not positive definite, now RobustInverse is called
//   o   Singularity
//   o   Can't take square root of a negative number, now RobustInverse is called
//   o   Can't divide by zero, now RobustInverse is called
//
//##ModelId=3C75618E037C
GMatrix GMatrix::Inv() const
{
   const double EPSILON = 1.0E-20;

   int i = 0,
       j = 0, 
       k = 0;

   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "Inv", "Inverse called on GMatrix with zero rows and/or columns" );

   if( m_rows != m_cols )
      MatrixError( "Inv", "Non-square GMatrix" );

   // temp object to hold the result
   GMatrix A( m_rows, m_cols ); // store result
   A.deepCopy( *this );

   // check for positive definiteness
   bool isPositiveDefinite = true;
   for( i = 0; i < m_rows; i++ )
   {
      if( A[i][i] < 0.0 )
      {
         isPositiveDefinite = false;
         break;
      }
         
      if( fabs( A[i][i] ) < EPSILON )
      {
         isPositiveDefinite = false;
         break;
      }
   }
   if( !isPositiveDefinite )
   {
      // use Robust Inversion
      return RobustInverse();
   } 

   // Perform Choleski decomposition
   for( j = 0; j < m_rows; j++ )
   {
      for( k = 0; k < j; k++ )
         A[j][j] -= A[j][k] * A[j][k];

      if( A[j][j] < 0.0 )
      {
         // use Robust Inversion
         return RobustInverse();
      }  

      A[j][j] = sqrt (A[j][j]);

      for( i = j + 1; i < m_rows; i++ )
      {
         for( k = 0; k < j; k++ )
            A[i][j] -= A[i][k] * A[j][k];

         if( fabs(A[j][j]) < EPSILON )
         {
            // use Robust Inversion
            return RobustInverse();
         }              

         A[i][j] /= A[j][j];
      }
   }

   // inversion of lower triangular GMatrix
   for( j = 0; j < m_rows; j++ )
   {
      A[j][j] = 1.0 / A[j][j];

      for( i = j + 1; i < m_rows; i++ )
      {
         A[i][j] *= -A[j][j] / A[i][i];

         for( k = j + 1; k < i; k++ )
            A[i][j] -= A[i][k] * A[k][j] / A[i][i];
      }
   }

   // construction of lower triangular inverse GMatrix
   for( j = 0; j < m_rows; j++ )
   {
      for( i = j; i < m_rows; i++ )
      {
         A[i][j] *= A[i][i];

         for( k = i + 1; k < m_rows; k++ )
            A[i][j] += A[k][i] * A[k][j];
      }
   }

   // fill upper diagonal
   for( i = 1; i < m_rows; i++ )
   {
      for( j = 0; j < i; j++ )
         A[j][i] = A[i][j];
   }
   return A; // return a copy
}



// same as above, but operates on the original GMatrix
//##ModelId=3C75618A0292
void GMatrix::InvInPlace()
{
    const double EPSILON = 1.0E-20;

   int i = 0,
       j = 0, 
       k = 0;

   if( m_rows == 0 || m_cols == 0)
      MatrixError( "InvInPlace", "called on GMatrix with zero rows and/or columns" );

   if( m_rows != m_cols)
      MatrixError( "InvInPlace", "Non-square GMatrix");

   // check for positive definiteness
   bool isPositiveDefinite = true;
   for( i = 0; i < m_rows; i++ )
   {
      if( m_data[i][i] < 0.0 )
      {
         isPositiveDefinite = false;
         break;
      }
         
      if( fabs( m_data[i][i] ) < EPSILON )
      {
         isPositiveDefinite = false;
         break;
      }
   }
   if( !isPositiveDefinite )
   {
      // use Robust Inversion
      *this = RobustInverse();
      return;
   } 


   // Perform Choleski decomposition
   for( j = 0; j < m_rows; j++ )
   {
      for( k = 0; k < j; k++ )
         m_data[j][j] -= m_data[j][k] * m_data[j][k];

      if( m_data[j][j] < 0.0 )
      {
         // use Robust Inversion
         *this = RobustInverse();
         return;
      }  

      m_data[j][j] = sqrt (m_data[j][j]);

      for( i = j + 1; i < m_rows; i++ )
      {
         for( k = 0; k < j; k++ )
            m_data[i][j] -= m_data[i][k] * m_data[j][k];

         if( fabs(m_data[j][j]) < EPSILON )
         {
            // use Robust Inversion
            *this = RobustInverse();
            return;
         }  

         m_data[i][j] /= m_data[j][j];
      }
   }

   // inversion of lower triangular GMatrix
   for( j = 0; j < m_rows; j++ )
   {
      m_data[j][j] = 1.0 / m_data[j][j];

      for( i = j + 1; i < m_rows; i++ )
      {
         m_data[i][j] *= -m_data[j][j] / m_data[i][i];

         for( k = j + 1; k < i; k++ )
            m_data[i][j] -= m_data[i][k] * m_data[k][j] / m_data[i][i];
      }
   }

   // construction of lower triangular inverse GMatrix
   for( j = 0; j < m_rows; j++ )
   {
      for( i = j; i < m_rows; i++ )
      {
         m_data[i][j] *= m_data[i][i];

         for( k = i + 1; k < m_rows; k++ )
            m_data[i][j] += m_data[k][i] * m_data[k][j];
      }
   }

   // fill upper diagonal
   for( i = 1; i < m_rows; i++ )
   {
      for( j = 0; j < i; j++ )
         m_data[j][i] = m_data[i][j];
   }

}

// returns the inverse of a diagonal GMatrix
//##ModelId=3C75618E0273
GMatrix GMatrix::DiagInv() const
{
   int i = 0;

   // temp object to hold the result
   GMatrix A( m_rows, m_cols ); // store result
   A.deepCopy( *this );

   // check it's square
   if( !A.isSquare() )
      MatrixError( "DiagInv", "called on non square GMatrix" );

   // invert the GMatrix
   for( i = 0; i < m_rows; i++ )
      A[i][i] = 1.0 / A[i][i];

   return A;
}

// returns the value of the largest absolute element end reference to its index
//##ModelId=3C75618B02E0
double GMatrix::MaxAbs( int &row, int &col ) const
{
   int i = 0,
       j = 0;

   if( m_rows == 0 || m_cols == 0 )
   MatrixError( "MaxAbs", "called on GMatrix with zero rows and/or columns" );

   double max = m_data[0][0];
   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         if( fabs( m_data[i][j] ) > max )
         {
            max = fabs( m_data[i][j] );
            row = i;
            col = j;
         }
      }
   }
   return max;
}



// returns the value of the maximum element end reference to its index
//##ModelId=3C75618B037C
double GMatrix::Max( int &row, int &col ) const
{
   int i = 0,
       j = 0;

   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "Max", "called on GMatrix with zero rows and/or columns" );

   double max = m_data[0][0];

   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         if(  m_data[i][j] > max )
         {            
            max = m_data[i][j];
            row = i;
            col = j;
         }
      }
   }
   return max;
}



// returns the value of the minimum element end reference to its index
//##ModelId=3C75618C0021
double GMatrix::Min(int &row, int &col) const
{
   int i = 0,
       j = 0;

   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "Min", "called on GMatrix with zero rows and/or columns" );

   double min = m_data[0][0];

   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         if( m_data[i][j] < min )
         {
            min = m_data[i][j];
            row = i;
            col = j;
         }
      }
   }
   return min;
}

// returns the value of the largest absolute element
//##ModelId=3C75618C009E
double GMatrix::MaxAbsVal() const
{
   int i = 0,
       j = 0;

   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "MaxAbsVal", "called on GMatrix with zero rows and/or columns" );

   double max = m_data[0][0];

   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         if( fabs( m_data[i][j] ) > max )
         {
            max = fabs( m_data[i][j] );
         }
      }
   }

   return max;
}



// returns the value of the smallest absolute element
//##ModelId=3C75618C011B
double GMatrix::MinAbsVal() const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "MinAbsVal", "called on GMatrix with zero rows and/or columns" );

   int i = 0,
       j = 0;

   double min = m_data[0][0];

   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         if( fabs( m_data[i][j] ) < min )
         {
            min = fabs( m_data[i][j] );
         }
      }
   }

   return min;
}



//##ModelId=3C75618C0198
double GMatrix::MaxVal() const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "MaxVal", "called on GMatrix with zero rows and/or columns" );

   int i = 0,
       j = 0;

   double max = m_data[0][0];

   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         if( m_data[i][j] > max )
         {
            max = m_data[i][j];
         }
      }
   }

   return max;
}



// returns the maximum element in the specified column
//##ModelId=3C75618C0234
double GMatrix::MaxColVal( const int col ) const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "MaxColVal", "called on GMatrix with zero rows and/or columns" );

   int i = 0;

   double max = m_data[0][col];

   for( i = 0; i < m_rows; i++ )
   {
      if( m_data[i][col] > max )
      {
         max = m_data[i][col];
      }
   }

   return max;
}



//##ModelId=3C75618C02B1
double GMatrix::MinVal() const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "MinVal", "called on GMatrix with zero rows and/or columns" );

   int i = 0,
       j = 0;

   double min = m_data[0][0];

   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         if( m_data[i][j] < min )
         {
            min = m_data[i][j];
         }
      }
   }

   return min;
}



// returns the minimum element in the specified column
//##ModelId=3C75618C034D
double GMatrix::MinColVal( const int col ) const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "MinColVal", "called on GMatrix with zero rows and/or columns" );

   int i = 0;

   double min = m_data[0][col];

   for( i = 0; i < m_rows; i++ )   
   {
      if( m_data[i][col] < min )
      {
         min = m_data[i][col];
      }
   }

   return min;
}



// returns the range of the m_data ie: Range = MaxVal - MinVal
//##ModelId=3C75618C03CA
double GMatrix::Range() const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "Range", "called on GMatrix with zero rows and/or columns" );

   GMatrix temp;
   temp = (*this);
   return( fabs( temp.MaxVal() - temp.MinVal() ) );
}



// returns the sum of all the elements
//##ModelId=3C75618D005F
double GMatrix::Sum() const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "Sum", "called on GMatrix with zero rows and/or columns");

   int i = 0,
       j = 0;

   double sum = 0;

   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         sum += m_data[i][j];
      }
   }
   return sum;
}



// returns the sum of one column
//##ModelId=3C75618D00DC
double GMatrix::ColumnSum( const int col ) const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "ColumnSum", "called on GMatrix with zero rows and/or columns" );

   if( col > m_cols )
      MatrixError( "ColumnSum", "called using an column value too large" );

   int i = 0;   

   double sum = 0;

   for( i = 0; i < m_rows; i++ )
      sum += m_data[i][col];

   return sum;
}



// returns the sum of one row
//##ModelId=3C75618D014A
double GMatrix::RowSum( const int row ) const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "RowSum", "called on GMatrix with zero rows and/or columns" );

   if( row > m_rows )
      MatrixError( "RowSum", "called using an row value too large" );

   int j = 0;

   double sum = 0.0;

   for( j = 0; j < m_cols; j++ )
      sum += m_data[row][j];

   return sum;
}


// returns the norm of a column vector
//##ModelId=3C75618D01B7
double GMatrix::ColumnNorm( const int col ) const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "ColumnNorm", "called on GMatrix with zero rows and/or columns" );

   if( col < 0 || col >= m_cols )
      MatrixError( "ColumnNorm", "called with invalid column" );
   
   int i = 0;       

   double result = 0.0;

   for( i = 0; i < m_rows; i++ )
      result += m_data[i][col]*m_data[i][col];

   result = sqrt(result);
   return result;
}



// returns the sum of squares of the GMatrix
//##ModelId=3C75618D0244
double GMatrix::SS() const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "SS", "called on GMatrix with zero rows and/or columns" );

   double result = 0.0;
   
   int i = 0,
       j = 0;   

   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         result += m_data[i][j]*m_data[i][j];
   
   return result;
}



// returns a sample mean of every element in the GMatrix
//##ModelId=3C75618D02B1
double GMatrix::Mean() const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "Mean", "called on GMatrix with zero rows and/or columns" );
      
   const double n = (double)(m_rows*m_cols);
   
   double sum     = 0.0,
          meanval = 0.0;

   int i = 0,
       j = 0;
   
   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         sum += m_data[i][j];

   meanval = sum / n;
   return meanval;
}



// returns a sample variance
//##ModelId=3C75618D030F
double GMatrix::Var() const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "Var", "called on GMatrix with zero rows and/or columns" );
   if( m_cols == 1 && m_rows == 1 )
      return 0.0;

   const double n = (double)(m_rows*m_cols);

   double sumx2 = 0.0,
          sumx  = 0.0,
          var   = 0.0;
   
   int i = 0,
       j = 0;
   
   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         sumx += m_data[i][j];
         sumx2 += m_data[i][j]*m_data[i][j];
      }
   }
   var = (n*sumx2 - sumx*sumx) / (n*(n-1));
   return var;
}



// returns a sample standard deviation
//##ModelId=3C75618D037C
double GMatrix::Stdev() const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "Stdev", "called on GMatrix with zero rows and/or columns" );
   if( m_cols == 1 && m_rows == 1 )
      return 0.0;

   double std = 0.0;
   std = sqrt(Var());
   return std;
}



// returns the sample RMS value
//##ModelId=3C75618D03DA
double GMatrix::RMS() const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "RMS", "called on GMatrix with zero rows and/or columns" );
   
   double sum_sqrs  = 0.0, 
          mean_sqrs = 0.0, 
          rms       = 0.0;

   const double n = (double)(m_rows*m_cols);

   int i = 0,
       j = 0;   

   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         sum_sqrs += m_data[i][j]*m_data[i][j];

   mean_sqrs = sum_sqrs / (n);

   rms = sqrt(mean_sqrs);
   return rms;
}



// returns the skewness - only works on a (n,1) GMatrix
//##ModelId=3C75618E006F
double GMatrix::Skew() const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "Skew", "called on GMatrix with zero rows and/or columns" );
   if( m_cols != 1 )
      MatrixError( "Skew", "called on GMatrix more than one column" );
   if( m_rows < 3 )
      MatrixError( "Skew", "called on GMatrix with less than three elements" );

   double skew    = 0.0, 
          sum     = 0.0, 
          meanval = 0.0, 
          stdev   = 0.0;
   
   const double n = (double)m_rows;

   int i = 0;

   meanval = Mean();
   stdev = Stdev();

   for( i = 0; i < m_rows; i++ )
      sum += pow( ((m_data[i][0] - meanval)/stdev), 3);

   skew = n*sum / ((n-1)*(n-2));

   return skew;
}



// returns the kurtosis - only works on a (n,1) GMatrix
//##ModelId=3C75618E00CD
double GMatrix::Kurt() const
{
   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "Kurt", "called on GMatrix with zero rows and/or columns" );
   if( m_cols != 1 )
      MatrixError( "Kurt", "called on GMatrix with more than one column" );
   if( m_rows < 4 )
      MatrixError( "Kurt", "called on GMatrix with less than four elements" );

   int i = 0;
   
   double kurt    = 0.0, 
          sum     = 0.0, 
          meanval = 0.0, 
          stdev   = 0.0;
   
   const double n = (double)m_rows;

   meanval = Mean();
   stdev = Stdev();

   for( i = 0; i < m_rows; i++ )
      sum += pow( ((m_data[i][0] - meanval)/stdev), 4);   

   kurt = n*(n+1)*sum / ((n-1)*(n-2)*(n-3)) - 3*(n-1)*(n-1) / ((n-2)*(n-3));
   return kurt;
}

// returns the trace of a square (only) GMatrix
//##ModelId=3C75618E012A
double GMatrix::Trace() const
{  
   int i = 0;
   double trace = 0.0;
   
   if( !isSquare() )
      MatrixError( "Trace", "called on non-square GMatrix" );

   for( i = 0; i < m_rows; i++ )   
      trace+=m_data[i][i];

   return trace;
}

// removes specified column
//##ModelId=3C75618B005F
void GMatrix::RemoveColumn( const int col )
{
   int i = 0,
       j = 0;

   if( m_rows <= 0 || m_cols < 2 || col >= m_cols )
      MatrixError( "RemoveColumn", "invalid call");

   GMatrix Copy;   
   Copy = *this;

   Copy.Redim( m_rows, m_cols-1 );

   for( j = 0; j<m_cols; j++ )
   {     
      for( i = 0; i<m_rows; i++ )
      {
         if( j == col )
         {
            if( j == m_cols - 1 )
               continue;
            else
               Copy[i][j] = m_data[i][j+1];
         }
         else if( j > col )
         {
            Copy[i][j-1] = m_data[i][j];
         }
         else     
         {
            Copy[i][j] = m_data[i][j];
         }
      }
   }
   Redim( m_rows, m_cols-1 );
   deepCopy(Copy);
}




// removes specified row
//##ModelId=3C75618B00BD
void GMatrix::RemoveRow( const int row )
{
   if( m_rows <=0 || m_rows < 2 || row >= m_rows )
      MatrixError( "RemoveRow", "invalid call" );

   int i = 0,
       j = 0;

   GMatrix Copy;   
   Copy = *this;

   Copy.Redim( m_rows-1, m_cols );

   for( i = 0; i<m_rows; i++ )
   {     
      for( j = 0; j<m_cols; j++ )
      {
         if( i == row )
         {
            if( i == m_rows - 1 )
               continue;
            else
               Copy[i][j] = m_data[i+1][j];
         }
         else if( i > row )
         {
            Copy[i-1][j] = m_data[i][j];
         }
         else     
         {
            Copy[i][j] = m_data[i][j];
         }
      }
   }
   Redim( m_rows-1, m_cols );
   deepCopy(Copy);
}

            







// redimension the GMatrix (larger or smaller)
// if new dimensions are larger, GMatrix is padded with zeroes
// else rows/cols are truncated
//##ModelId=3C75618B0011
void GMatrix::Redim( const int newRows, const int newCols )
{
   if( newRows < 0 || newCols < 0 )
      MatrixError("Redim","newRows or newCols less than zero");

   int i    = 0,
       j    = 0,
       rows = 0,
       cols = 0;

   if( m_rows != newRows || m_cols != newCols )
   {
      // determine overlapping elements for copying
      rows = m_rows < newRows ? m_rows : newRows;
      cols = m_cols < newCols ? m_cols : newCols;

      double** oldData = m_data; // store ptr to current m_data
      matAlloc( newRows, newCols ); // alloc new memory

      // only need to copy across elements if GMatrix is not 0 x 0
      if( rows != 0 && cols != 0 )
      {
         for( i = 0; i < rows; i++ )
            for( j = 0; j < cols; j++ )
               m_data[i][j] = oldData[i][j]; // copy elements
      }
      matDealloc( oldData ); // dealloc the old m_data
   }
}



// sort the GMatrix using a quicksort algorithm on a column by column basis
//##ModelId=3C75618B0234
void GMatrix::Sort()
{
   int i = 0,
       j = 0;

   for( i = 0; i < m_cols; i++ )
   {
      GMatrix Col( m_rows );      
      Col = this->Column(i);
      
      quickSortMat( Col, 0, Col.nRows()-1 );
      
      for( j = 0; j < m_rows; j++ )
         m_data[j][i]=Col[j][0];
   }
}



// sorts the GMatrix's rows using a quicksort algorithm based on the values in one column
//##ModelId=3C75618B0292
void GMatrix::SortByColumn( const int col )
{
   if( col >= m_cols )
      MatrixError( "SortByColumn", "called with an invalid column index" );

   int i = 0,
       j = 0;

   GMatrix Col(m_rows);
   Col = this->Column(col);
   
   GMatrix Index(m_rows);
   for( i = 0; i < m_rows; i++ )
      Index[i][0] = i;

   quickSortMat( Col, Index, 0, Col.nRows()-1 );

   GMatrix Temp( m_rows, m_cols );
   
   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         Temp[i][j] = m_data[i][j];
   
   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         m_data[i][j] = Temp[ ((int)(Index[i][0])) ][j];
}



// round the GMatrix to the specified precision (eg 18.33348 rounded with precision=0.0001 becomes 18.3335)
//##ModelId=3C75618B011B
void GMatrix::Round( const double precision )
{
   int i = 0,
       j = 0;

   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         m_data[i][j] = RoundVal( m_data[i][j], precision );
}



// returns the multiplication of the GMatrix transpose by itself: ATA
// useful for least squares problems
//##ModelId=3C75618E01D6
GMatrix GMatrix::ATA() const
{
   int i = 0,
       j = 0,
       k = 0;

   // allocate GMatrix to store result
   GMatrix result( m_cols, m_cols );

   // perform the multiplication
   for( i = 0; i < m_cols; i++ )
       for( j = 0; j < m_cols; j++ )
         for( k = 0; k < m_rows; k++ )
             result[i][j] += m_data[k][i] * m_data[k][j];

   return result;
}



// return the column specified by the index
//##ModelId=3C75618E0224
GMatrix GMatrix::Column( const int col ) const
{
   if( col >= m_cols )
      MatrixError( "Column", "called with an invalid column index" );

   int j = 0;

   GMatrix result(m_rows);
   
   for( j = 0; j < m_rows; j++ )
      result[j][0] = m_data[j][col];
   
   return result;
}



//return the row specified by the index
//##ModelId=3C75618F0050
GMatrix GMatrix::Row( const int row ) const
{
   if( row >= m_rows )
      MatrixError( "Row", "called with an invalid row index" );

   int j = 0;

   GMatrix result(1,m_cols);
   
   for( j = 0; j < m_cols; j++ )
      result[0][j] = m_data[row][j];
   
   return result;
}



//returns the absolute GMatrix ie. all positive
//##ModelId=3C75618E0188
GMatrix GMatrix::Abs() const
{
   int i = 0,
       j = 0;

   GMatrix result( m_rows, m_cols );

   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         result[i][j] = fabs(m_data[i][j]);

   return result;      
}


// size comparison
//##ModelId=3C75618900AD
bool GMatrix::isSameSize( const GMatrix& mat ) const
{
   if( m_rows == mat.nRows() && m_cols == mat.nCols() )
      return true;
   else
      return false;
}

// conformal comparison
//##ModelId=3C756189005F
bool GMatrix::isConformal( const GMatrix& mat ) const
{
   if( m_cols == mat.nRows() )
      return true;
   else
      return false;
}


// A .* B (element multiplication)
//##ModelId=3C75618E02F0
GMatrix GMatrix::DotMulitply( const GMatrix& mat ) const
{
   if( !isSameSize(mat) )
      MatrixError( "DotMultiply", "called with different sized GMatrix" );

   int i = 0,
       j = 0;

   GMatrix Result( m_rows, m_cols );

   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         Result[i][j] = m_data[i][j] * mat[i][j];
      }
   }
   return Result;
}  



// A ./ B (element division)
//##ModelId=3C75618E033E
GMatrix GMatrix::DotDivision( const GMatrix& mat ) const
{
   if( !isSameSize(mat) )
      MatrixError( "DotDivision", "called with different sized GMatrix" );

   int i = 0,
       j = 0;

   GMatrix Result( m_rows, m_cols );

   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         if( mat[i][j] == 0.0 )
            Result[i][j] = m_data[i][j] / 1e-99; // don't allow divide by zero
         else
            Result[i][j] = m_data[i][j] / mat[i][j];
      }
   }
   return Result;
}  



// A becomes A|B   
//##ModelId=3C75618903AB
void GMatrix::Concatonate( const GMatrix& mat )
{
   if( m_rows != mat.nRows() )
      MatrixError( "Concatonate", "unequal row sizes" );

   if( m_rows == 0 || m_cols == 0 )
      MatrixError( "Concatonate", "GMatrix with zero rows and/or columns" );

   int i = 0,
       j = 0;
  
   const int col = m_cols;
   Redim( m_rows, m_cols+mat.nCols() );

   for( i = 0; i < mat.nRows(); i++ )
   {
      for( j = 0; j < mat.nCols(); j++ )
      {
         m_data[i][col+j] = mat[i][j];
      }
   }
}



// Concatonates a Column to the end GMatrix
//##ModelId=3C75618B0169
void GMatrix::AddColumn( const GMatrix& mat )
{
   if( m_rows != mat.nRows() )
      MatrixError( "AddColumn", "unequal row sizes" );

   if( mat.nCols() != 1 )
      MatrixError( "AddColumn", "invalid number of columns in input GMatrix" );

   int i = 0,
       j = 0;

   const int col = m_cols;
   Redim( m_rows, m_cols+1 );

   for( i = 0; i < mat.nRows(); i++ )
   {
      m_data[i][col] = mat[i][0];
   }
}



// Adds a Row to the end GMatrix
//##ModelId=3C75618B01B7
void GMatrix::AddRow( const GMatrix& mat )
{
   if( m_cols != mat.nCols() )
      MatrixError( "AddRow", "unequal row sizes" );

   if( mat.nRows() != 1 )
      MatrixError( "AddRow", "invalid number of columns in input GMatrix" );

   int i = 0;

   const int row = m_rows;
   Redim( m_rows+1, m_cols );

   for( i = 0; i < mat.nCols(); i++ )
   {
      m_data[row][i] = mat[0][i];      
   }
}



// Inserts a submatrix starting at indices (row,col)
//##ModelId=3C75618B01F6
void GMatrix::InsertSubMatrix( const GMatrix& mat,
                              const int row,
                              const int col )
{
   int i = 0,
       j = 0;

   // check that the submatrix doesn't exceed the bounds of the GMatrix
   if( row + mat.nRows() > m_rows )
      MatrixError( "InsertSubMatrix", "submatrix size exceeds the number of rows in the GMatrix" );

   if( col + mat.nCols() > m_cols )
      MatrixError( "InsertSubMatrix", "submatrix size exceeds the number of columns in the GMatrix");

   // insert the submatrix
   for( i = row; i < row+mat.nRows(); i++ )
   {
      for( j = col; j < col+mat.nCols(); j++ )
      {
         m_data[i][j] = mat[ i-row ][ j-col ];
      }
   }

}




// Extract a submatrix starting at indices (row,col) with size numrows,numcols
//##ModelId=3C75618F013A
GMatrix GMatrix::ExtractSubMatrix( const int row,
                                 const int col,
                                 const int numrows,
                                 const int numcols )
{
   int i = 0,
       j = 0;

   // check that the submatrix doesn't exceed the bounds of the GMatrix
   if( row + numrows > m_rows )
      MatrixError( "ExtractSubMatrix", "submatrix size exceeds the number of rows in the GMatrix");

   if( col + numcols > m_cols )
      MatrixError( "ExtractSubMatrix", "submatrix size exceeds the number of columns in the GMatrix");

   GMatrix result(numrows,numcols);

   for( i = row; i < row+numrows; i++ )
   {
      for( j = col; j < col+numcols; j++ )
      {
         result[ i-row ][ j-col ] = m_data[i][j];

      }
   }

   return result;
}


// Perform a lower/upper factorization of the GMatrix 
// returns IsFullRank == true if L and U are set
//##ModelId=3C75618F0188
void GMatrix::LUFactorization( bool &IsFullRank, GMatrix &L, GMatrix &U )
{
   // factorization by naive gaussian elimination
   // Reference  
   // Chaney, Ward & David Kincaid, "Numerical Mathematics and Computing, 3rd Edition", Cole
   //         Publishing Co., 1994, Belmont, CA
   // (ENGG 407 Textbook,  p.237)

   if( !isSquare() )
      MatrixError( "LUFactorization", "GMatrix not square" );

   // redimension arrays appropriately
   if( L.nRows() != m_rows || L.nCols() != m_cols )
      L.Redim( m_rows, m_cols );
   if( U.nRows() != m_rows || U.nCols() != m_cols )
      U.Redim( m_rows, m_cols );

   const int n = m_rows;

   // make U a copy of the GMatrix
   U = *this;

   int *index;
   index = new int[n];
   if( !index )
      MatrixError( "LUFactorization", "Memory allocation failure" );

   double *scale;
   scale = new double[n];
   if( !scale )
   {
      if( index )
         delete index;
      MatrixError( "LUFactorization", "Memory allocation failure" );
   }

   int i     = 0,
       j     = 0,
       k     = 0,
       tempi = 0;

   double r     = 0.0,
          rmax  = 0.0,
          smax  = 0.0,
          xmult = 0.0,
          tempd = 0.0;

   // First Loop
   // initial form of the indexArray is determined
   // the scale factor array scaleArray is also determined
   for( i = 0; i < n; i++ )
   {
      index[i] = i;
      smax = 0;
      for( j = 0; j < n; j++ )
      {
         tempd = fabs( U[i][j] );
         if( fabs(smax) < tempd )
         {
            smax = tempd;
         }
      }
      scale[i] = smax;
   }

   // Second Loop
   // perform gaussian elimination to form the Lower and Upper matrices
   for( k = 0; k < n - 1; k++ )   
   {
      // select the pivot row, j, based on rmax, the first occurance of the largest ratio
      rmax = 0;
      for( i = k; i < n; i++ )
      {  
         r = fabs( U[index[i]][k] ) / scale[index[i]];

         if( r > rmax )
         {
            rmax = r;
            j = i;
         }
      }
      
      for( i = k + 1; i < n; i++ )      
      {
         xmult = U[index[i]][k] / U[index[k]][k];         
         L[index[i]][k] = xmult;    

         bool isFullRank = false;
         for( j = k + 1; j < n; j++ )         
         {
            U[index[i]][j] = U[index[i]][j] - xmult*U[index[k]][j];

            // if the upper GMatrix every has all zeros in one row, no solution is available
            if( U[index[i]][j] != 0 )
               isFullRank = true;
         }
         if( !isFullRank )
         {
            IsFullRank = false;
            L.Zero();
            U.Zero();
            return;
         }
         U[index[i]][k] = 0.0;
      }      
   }

   delete index;
   delete scale;

   // place one's along the diagonal
   for( i = 0; i < n; i++ )
      L[i][i] = 1.0;

   IsFullRank = true;

   // complete
}



//##ModelId=3C7561910169
void GMatrix::factorize( bool &IsFullRank, const int n, int* index, GMatrix &A ) const
{
   // first perform factorization by gaussian elimination with scaled parital pivoting
   // Reference  
   // Chaney, Ward & David Kincaid, "Numerical Mathematics and Computing, 3rd Edition", Cole
   //         Publishing Co., 1994, Belmont, CA
   // (ENGG 407 Textbook,  p.237)

   if( !isSquare() )
      MatrixError( "Factorize", "GMatrix not square" );

   double *scale;
   scale = new double[n];
   if( !scale )
   {
      if( index )
         delete index;
      MatrixError( "Factorize", "Memory allocation failure" );
   }
  
   int i     = 0,
       j     = 0,
       k     = 0,
       tempi = 0;

   double r     = 0.0,
          rmax  = 0.0,
          smax  = 0.0,
          xmult = 0.0,
          tempd = 0.0;

   // First Loop
   // initial form of the indexArray is determined
   // the scale factor array scaleArray is also determined
   for( i = 0; i < n; i++ )
   {
      index[i] = i;
      smax = 0;
      for( j = 0; j < n; j++ )
      {
         tempd = fabs( A[i][j] );
         if( fabs(smax) < tempd )
         {
            smax = tempd;
         }
      }
      scale[i] = smax;
   }

   // Second Loop
   // perform gaussian elimination to form the Lower and Upper matrices
   for( k = 0; k < n - 1; k++ )   
   {
      // select the pivot row, j, based on rmax, the first occurance of the largest ratio
      rmax = 0;
      for( i = k; i < n; i++ )
      {  
         r = fabs( A[index[i]][k] ) / scale[index[i]];

         if( r > rmax )
         {
            rmax = r;
            j = i;
         }
      }

      tempi    = index[j];
      index[j] = index[k];
      index[k] = tempi;
      
      for( i = k + 1; i < n; i++ )
      {
         xmult = A[index[i]][k] / A[index[k]][k];
         A[index[i]][k] = xmult;
         
         bool isFullRank = false;
         for( j = k + 1; j < n; j++ )
         {
            A[index[i]][j] = A[index[i]][j] - xmult*A[index[k]][j];

            // if the upper GMatrix every has all zeros in one row, no solution is available
            if( A[index[i]][j] != 0 )
               isFullRank = true;
         }
         if( !isFullRank )
         {
            IsFullRank = false;
            return;
         }         
      } 
   }

   delete scale;

   IsFullRank = true;

}


// Solve Ax=b
// FactorizedA is obtained from "void GMatrix::Factorize( const int n, int* index, GMatrix &A )"
//##ModelId=3C7561910224
void GMatrix::solveByGaussianElimination( const GMatrix &b, 
                                         GMatrix &X, 
                                         const GMatrix &FactorizedA, 
                                         int *index ) const
{
   int i = 0,
       j = 0,
       k = 0;

   const GMatrix &A = FactorizedA;

   // make B a copy of b
   GMatrix B = b;

   double sum = 0.0;

   const int n = m_rows;

   
   for( k = 0; k < n - 1; k++ )
   {
      for( i = k + 1; i < n; i++ )
      {
         B[index[i]][0] = B[index[i]][0] - A[index[i]][k]*B[index[k]][0];
      }
   }
   X[n-1][0] = B[index[n-1]][0] / A[index[n-1]][n-1];
   
   for( i = n - 2; i >= 0; i-- )
   {
      sum = B[index[i]][0];
      for( j = i + 1; j < n; j++ )
      {
         sum = sum - A[index[i]][j]*X[j][0];
      }
      X[i][0] = sum / A[index[i]][i];
   }
}



//##ModelId=3C75618F0021
GMatrix GMatrix::RobustInverse() const
{
   const int n = m_rows;

   int i = 0;

   if( !isSquare() )
      MatrixError( "RobustInverse", "GMatrix not square" );

   if( n == 0 )
      MatrixError( "RobustInverse", "zero rows/cols GMatrix");

   int *index;
   index = new int[n];
   if( !index )
      MatrixError( "RobustInverse", "Memory allocation failure");

   // make A a copy of this GMatrix
   GMatrix A = *this;

   bool isFullRank = false;

   // need to get the index and Factorized A
   factorize( isFullRank, n, index, A );

   if( !isFullRank )
   {
      if( index )
         delete index;
      MatrixError( "RobustInverse", "GMatrix is not full rank for inverse" );
   }

   GMatrix ColumnI(n,1);
   GMatrix X(n,1);
   GMatrix Inverse(n,n);

   for( i = 0; i < n; i++ )
   {
      if( i != 0 )
         ColumnI[i-1][0] = 0.0;

      ColumnI[i][0] = 1.0;
      
      solveByGaussianElimination( ColumnI, X, A, index );

      Inverse.InsertSubMatrix( X, 0, i );
   }

   delete index;

   return Inverse;

}








   

   

// print the GMatrix values to a specified filename
//##ModelId=3C75618F0282
// void GMatrix::Print( const char* outfile, const int precision, const char delimiter ) const
// {
//    // generate warning if GMatrix is 0 x 0
//    if( m_cols == 0 || m_rows == 0 )
//    {
//       MatrixMessage( "Print" , "printing GMatrix with zero rows and/or columns" ); 
//    }
// 
//    ofstream out;
// 
//    out.open( outfile );
// 
//    if( out.fail() )
//    {
//       MatrixError( "Print", "unable to open output file" );
//    }
// 
//    Print( out, precision, delimiter );
// 
//    out.close();
// 
// #ifdef MATRIX_MESSAGES
//    char buf[400];
//    sprintf( buf, "%6d rows by %6d columns printed to %s", m_rows, m_cols, outfile );
//    MatrixMessage( "Print", buf );
// #endif
// }


// print the GMatrix values to a specified filename
//##ModelId=3C75618F02B1
void GMatrix::Print( const char* outfile, const int precision, const int width ) const
{
   // generate warning if GMatrix is 0 x 0
   if( m_cols == 0 || m_rows == 0 )
   {
      MatrixMessage( "Print" , "printing GMatrix with zero rows and/or columns" ); 
   }

   ofstream out;

   out.open( outfile );

   if( out.fail() )
   {
      MatrixError( "Print", "unable to open output file" );
   }

   Print( out, precision, width );

   out.close();

#ifdef MATRIX_MESSAGES
   char buf[400];
   sprintf( buf, "%6d rows by %6d columns printed to %s", m_rows, m_cols, outfile );
   MatrixMessage( "Print", buf );
#endif
}



//##ModelId=3C7561910272
void GMatrix::valueToBuffer( const double value, 
                            char *ValueBuffer, 
                            const int width, 
                            const int precision ) const
{
   char format[9];
   format[0] = '\0';   
   
   int k      = 0,
       p      = 0,
       length = 0;
   
   if( (fabs(value) > 1.0e+6) || (fabs(value) < 1.0e-6 && value != 0.0) )
   {
      sprintf( format, "%%%d.%de", width, precision );
   }
   else
   {
      sprintf( format, "%%%d.%dlf", width, precision );      
   }
   
   ValueBuffer[0] = '\0';
   sprintf( ValueBuffer, format, value );

   length = strlen( ValueBuffer );


   // trim right zeros
   k = length-1;
   while( k >= 0 && ValueBuffer[k] == '0' )
   {
      ValueBuffer[k] = ' ';
      k--;
   }
   if(ValueBuffer[k] == '.')
   {
      ValueBuffer[k] = ' ';
      k--;
   }
   // now right shift the buffer (right align buffer)
   p = length - 1;
   if( p != k )
   {
      while( k >= 0 )
      {
         if( ValueBuffer[k] == ' ' )
            break;
         ValueBuffer[p] = ValueBuffer[k];            
         ValueBuffer[k] = ' ';
         p--;
         k--;
      }
   }
}





// print the GMatrix values to a specified stream
// pass cout to print to screen
//##ModelId=3C75618F01C7
void GMatrix::Print( ostream& strm, const int precision, const int width ) const
{
   // generate warning if GMatrix is 0 x 0
   if( m_cols == 0 || m_rows == 0 )
   {
      MatrixMessage( "Print" , "printing GMatrix with zero rows and/or columns" ); 
   }

   double value = 0;
   int i = 0,
       j = 0;       

   char ValueBuffer[100];
   ValueBuffer[0] = '\0';
   
   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         value = m_data[i][j];

         valueToBuffer( value, ValueBuffer, width, precision );         
         
         strm << ValueBuffer;
      }
      strm << endl;
   }
}



// print the GMatrix values to a specified stream
// pass cout to print to screen
//##ModelId=3C75618F01F5
void GMatrix::Print( FILE* outFile, const int precision, const int width ) const
{
   // generate warning if GMatrix is 0 x 0
   if( m_cols == 0 || m_rows == 0 )
   {
      MatrixMessage( "Print" , "printing GMatrix with zero rows and/or columns" ); 
   }

   if( outFile == NULL )
   {
      MatrixError( "Print", "invalid FILE*" );
   }


   double value = 0;
   int i = 0,
       j = 0;       

   char ValueBuffer[100];
   ValueBuffer[0] = '\0';
   
   for( i = 0; i < m_rows; i++ )
   {
      for( j = 0; j < m_cols; j++ )
      {
         value = m_data[i][j];

         valueToBuffer( value, ValueBuffer, width, precision );         

         fprintf( outFile, ValueBuffer );
      }
      fprintf( outFile, "\n" );
      
   }
}




// print the GMatrix values to a specified stream with a specified delimiter
// pass cout to print to screen
//##ModelId=3C75618F0224
// void GMatrix::Print( ostream& strm, const int precision, const char delimiter ) const
// {
//    // generate warning if GMatrix is 0 x 0
//    if( m_cols == 0 || m_rows == 0 )
//    {
//       MatrixMessage( "Print" , "printing GMatrix with zero rows and/or columns" ); 
//    }
// 
//    double value = 0;
//    int i      = 0,
//        j      = 0,
//        k      = 0,
//        length = 0;
// 
//    char format[9];
//    format[0] = '\0';   
// 
//    char ValueBuffer[100];
//    ValueBuffer[0] = '\0';
//    
//    for( i = 0; i < m_rows; i++ )
//    {
//       for( j = 0; j < m_cols; j++ )
//       {
//          value = m_data[i][j];
//          
//          format[0] = '\0';
//          if( (fabs(value) > 1.0e+5) || (fabs(value) < 1.0e-5 && value != 0.0) )
//          {
//             sprintf( format, "%%.%de", precision );
//          }
//          else
//          {
//             sprintf( format, "%%.%dlf", precision );      
//          }
// 
//          ValueBuffer[0] = '\0';
//          sprintf( ValueBuffer, format, value );
// 
//          length = strlen( ValueBuffer );
// 
// 
//          // trim right zeros
//          k = length-1;
//          while( k >= 0 && ValueBuffer[k] == '0' )
//          {
//             ValueBuffer[k] = '\0';
//             k--;
//          }
//          if(ValueBuffer[k] == '.')
//             ValueBuffer[k] = '\0';
// 
//          if( j != m_cols-1 )
//             strm << ValueBuffer << delimiter;
//          else
//             strm << ValueBuffer;
//       }
//       strm << endl;
//    }
// }




// print the GMatrix values to a specified stream with a specified delimiter
// pass cout to print to screen
//##ModelId=3C75618F0253
// void GMatrix::Print(FILE* outFile, const int precision, const char delimiter) const
// {
//    // generate warning if GMatrix is 0 x 0
//    if( m_cols == 0 || m_rows == 0)
//    {
//       MatrixMessage( "Print" , "printing GMatrix with zero rows and/or columns" ); 
//    }
// 
//    if( outFile == NULL )
//    {
//       MatrixError( "Print", "invalid FILE*" );
//    }
// 
// 
//    double value = 0;
//    int i      = 0,
//        j      = 0,
//        k      = 0,
//        length = 0;
// 
// 
//    char format[9];
//    format[0] = '\0';  
//    
//    char ValueBuffer[100];
//    ValueBuffer[0] = '\0';
//    
//    for( i = 0; i < m_rows; i++ )
//    {
//       for( j = 0; j < m_cols; j++ )
//       {
//          value = m_data[i][j];
//          
//          format[0] = '\0';
//          if( (fabs(value) > 1.0e+5) || (fabs(value) < 1.0e-5 && value != 0.0) )
//          {
//             sprintf( format, "%%.%de", precision );
//          }
//          else
//          {
//             sprintf( format, "%%.%dlf", precision );      
//          }
// 
//          ValueBuffer[0] = '\0';
//          sprintf( ValueBuffer, format, value );
// 
//          length = strlen( ValueBuffer );
// 
// 
//          // trim right zeros
//          k = length-1;
//          while( k>=0 && ValueBuffer[k] == '0' )
//          {
//             ValueBuffer[k] = '\0';
//             k--;
//          }
//          if(ValueBuffer[k] == '.')
//             ValueBuffer[k] = '\0';
// 
//          if( j != m_cols-1 )
//          {
//             fprintf( outFile, ValueBuffer );
//             fprintf( outFile, "%c", delimiter );
//          }
//          else
//          {
//             fprintf( outFile, ValueBuffer );
//             fprintf( outFile, "\n" );
//          }
//       }      
//    }
// }



// Reads in the GMatrix m_data from the specified file using the indicated delimiter
// ReadFromFile is 'read smart' (it determines the size of the input GMatrix on its own)
// The number of columns are first determined then all m_data is read into linked lists
// untill end of file is reached. Data is then stored in the GMatrix.
//##ModelId=3C7561890002
void GMatrix::ReadFromFile( const char *infile_name, const char delimiter )
{
   int ret_val      = 0,
       i            = 0,
       j            = 0,
       w            = 0,
       num_rows     = 0,
       num_cols     = 0,
       time_to_bail = 0,
       line_length  = 0,
       scount       = 0,
       itemp        = 0,
       nelem        = 0;

   double dum_double = 0.0,
          dtemp      = 0.0;
   
   char linebuf[kMaxReadFromFileBuffer];
   char *linemark;

   FILE *infile = NULL;  

   // create a linked list
   struct stListElem
   {
      double value;
      stListElem *next;
   };
   stListElem *L = NULL;
   stListElem *nL = NULL;
   stListElem head;
   head.next = NULL;
   head.value = 0.0;   
   
   // open the input file
   infile = fopen( infile_name, "r" );
   if( infile == NULL )
   {
      sprintf( linebuf, "Unable to open input file: %s.", infile_name );
      MatrixError( "ReadFromFile", linebuf );
   }
   
   // get first line of m_data
   fgets( linebuf, kMaxReadFromFileBuffer-1, infile );
   if( feof(infile) )
   {
      sprintf( linebuf, "No Data in GMatrix input file: %s.", infile_name );
      MatrixError( "ReadFromFile", linebuf );
   }

   linemark = linebuf;
   line_length = strlen(linebuf);
   
   // advance linemark to the start of the input m_data 
   // if whitespace present before first value
   for( i = 0; i < line_length; i++ )
   {
      if( isspace(linemark[i]) )         
         continue;
      else 
         break;
   }
   linemark+=i;

   // determine the number of columns in the GMatrix
   while( !time_to_bail )
   {
      ret_val = sscanf( linemark+scount,"%lf",&dum_double );
      if( ret_val != 1 )
         break;
      num_cols++;
      itemp = advanceBuffer( linemark+scount, delimiter );
      scount += itemp;
      if( itemp == 0 )
         time_to_bail=1;
      if( scount > line_length )
         time_to_bail=1;
   }

   // check num_cols
   if( num_cols <= 0 )   
      MatrixError( "ReadFromFile", "No data columns found." );
   
   // reset the infile pointer
   rewind(infile);
      
   // read the m_data into L until eof is reached
   L = &head;
   nelem = 0;
   // two cases - 1. for whitespace, 2. other delimiters

   //case 1
   if( delimiter == 'w' ) // use super fast routine
   {
      while( !feof(infile) )
      {
         if( fscanf(infile,"%lf",&dum_double) != 1)
            break;
         
         // store the value in the list
         L->value = dum_double;

         // allocate new list element
         nL = new stListElem;
         if( !nL )
            MatrixError( "ReadFromFile", "Memory allocation error" );
         
         nL->value = 0.0;
         nL->next = NULL;
         
         // store address in previous list element pointer
         L->next = nL;
         L = nL;

         nelem++;
      }
   }   
   //case 2
   else while( !feof(infile) ) 
   {
      // get the next line from the input file
      linebuf[0] = '\0';      
      fgets( linebuf, kMaxReadFromFileBuffer-1, infile );

      if( feof(infile) )
         break;
      
      // initialize next loop parameters
      linemark = linebuf;
      line_length = strlen(linebuf);      
      time_to_bail = 0;
      scount = 0;
      
      // loop through each line of m_data extracting the m_data
      while( !time_to_bail )
      {
         // get the next value
         ret_val = sscanf( linemark+scount,"%lf",&dum_double );
         if( ret_val != 1 )
            break;

         // store the value in the list
         L->value = dum_double;
         
         // advance mark in string to next element         
         itemp = advanceBuffer( linemark+scount, delimiter );
         scount += itemp;
         if( itemp == 0 )
            time_to_bail=1;
         if( scount > line_length )
            time_to_bail=1;
         
         // allocate new list element
         nL = new stListElem;
         if( !nL )
            MatrixError( "ReadFromFile", "Memory allocation error" );
         
         nL->value = 0.0;
         nL->next = NULL;
         
         // store address in previous list element pointer
         L->next = nL;
         L = nL;

         nelem++;
      }
   }
   L->next = NULL; // tells us when list stops

   fclose(infile);

   // nelem should be exactly divisible by num_cols
   if( (nelem % num_cols) != 0 )
      MatrixError( "ReadFromFile", "Invalid number of elements found - not rectangular" );

   num_rows = nelem / num_cols;

   // set up the GMatrix
   matDealloc(m_data);
   m_rows = num_rows;
   m_cols = num_cols;
   matAlloc(m_rows,m_cols);

   // scroll thru the list extracting the m_data
   L = &head;
   for( i = 0; i < num_rows; i++ )
   {
      for( j = 0; j < num_cols; j++ )
      {
         m_data[i][j] = L->value;
         L = L->next;
      }
   }

   // delete the list
   L = head.next;
   while( L!=NULL )
   {
      nL = L->next;
      delete L;
      L = nL;
   }     

   //indicate success if not using MFC
#ifdef MATRIX_MESSAGES
   sprintf( linebuf, "%6d rows by %6d columns read from %s", m_rows, m_cols, infile_name );
   MatrixMessage( "ReadFromFile", linebuf );
#endif

}



// add to GMatrix by a scalar (shorthand notation: A += 5.0)
//##ModelId=3C75618F039B
void GMatrix::operator+= (double sclr)
{
   int i = 0,
       j = 0;

   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         m_data[i][j] += (double)sclr;
}

// multiply GMatrix by a scalar (shorthand notation: A *= 5.0)
//##ModelId=3C75618F03DC
void GMatrix::operator*= (double sclr)
{
   int i = 0,
       j = 0;

   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         m_data[i][j] *= (double)sclr;
}


// divide GMatrix by a scalar (shorthand notation: A /= 5.0)
//##ModelId=3C7561900021
void GMatrix::operator/= (double sclr)
{
   if( sclr == 0.0 )
      MatrixError( "/=", "Attempt to divide by zero" );

   int i = 0,
       j = 0;

   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         m_data[i][j] /= (double)sclr;
}


// A += B, where A is a GMatrix, and B is a GMatrix compatible for addition
//##ModelId=3C7561900040
void GMatrix::operator+= (const GMatrix& mat)
{
   if(     m_rows == 0 ||     m_cols == 0 ||
       mat.m_rows == 0 || mat.m_cols == 0 )
      MatrixError( "+=" , "GMatrix with zero rows and/or columns" );

   if( mat.m_rows != m_rows || mat.m_cols != m_cols )
      MatrixError( "+=", "Matrices not conformal for addition" );

   int i = 0,
       j = 0;

   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         m_data[i][j] += mat[i][j];
}



// A -= B, where A is a GMatrix, and B is a GMatrix compatible for subtraction
//##ModelId=3C7561900050
void GMatrix::operator-= (const GMatrix& mat)
{
   if(     m_rows == 0 ||     m_cols == 0 ||
       mat.m_rows == 0 || mat.m_cols == 0 )
      GMatrix::MatrixError ( "-=", "Subtraction called on GMatrix with zero rows and/or columns" );

   if( mat.m_rows != m_rows || mat.m_cols != m_cols )
      GMatrix::MatrixError( "-=", "Matrices not conformal for subtraction");

   int i = 0,
       j = 0;

   for( i = 0; i < m_rows; i++ )
      for( j = 0; j < m_cols; j++ )
         m_data[i][j] -= mat[i][j];
}



/////////////////////////////////////////////////////////////////////////////////
///                   Friend functions                                          ///
/////////////////////////////////////////////////////////////////////////////////

// postfix ++ operator overload
// add +1.0 to all elements and returns GMatrix values after the increment, e.g. GMatrix B = A++   
/* friend */
const GMatrix operator++ (GMatrix& mat, int)
{
   int i = 0,
       j = 0;

   for( i = 0; i < mat.m_rows; i++ )
      for( j = 0; j < mat.m_cols; j++ )
         mat[i][j] += 1.0;
   
   return mat;
}


// postfix -- operator overload
// subtract 1.0 to all elements and returns GMatrix values after the increment, e.g. GMatrix B = A--   
/* friend */
const GMatrix operator-- (GMatrix& mat, int)
{
   int i = 0,
       j = 0;

   for( i = 0; i < mat.m_rows; i++ )
      for( j = 0; j < mat.m_cols; j++ )
         mat[i][j] -= 1.0;
   
   return mat;
}



// GMatrix eqaulity:  B == C
/* friend */ 
bool operator== (const GMatrix& mat1, const GMatrix& mat2)
{
   if( mat1.m_cols == 0 || mat1.m_rows == 0 ||
       mat2.m_cols == 0 || mat2.m_rows == 0 )
   {
      GMatrix::MatrixError( "==", "GMatrix with zero rows and/or columns" );
   }

   if( !mat1.isSameSize(mat2) )
   {
      return false;      
   }

   int i = 0,
       j = 0;
   
   bool result = true;
   for( i = 0; i < mat1.m_rows; i++ )
   {
      for( j = 0; j < mat1.m_cols; j++ )
      {
         if( mat1[i][j] != mat2[i][j] )
            result = false;
      }
   }

   return result;
}


// GMatrix multiplication: A = B * C
/* friend */ 
const GMatrix operator* (const GMatrix& mat1, const GMatrix& mat2)
{
   if( mat1.m_cols == 0 || mat1.m_rows == 0 ||
       mat2.m_cols == 0 || mat2.m_rows == 0 )
   {
      GMatrix::MatrixError( "*" , "Multiplication called on GMatrix with zero rows and/or columns" );
   }

   if( mat1.m_cols != mat2.m_rows )
      GMatrix::MatrixError( "*", "Matrices not conformal for multiplication" );

   int i = 0,
       j = 0,
       k = 0;
   
   GMatrix temp (mat1.m_rows, mat2.m_cols);

   for( i = 0; i < mat1.m_rows; i++ )
   {
      for( j = 0; j < mat2.m_cols; j++ )
      {
         for( k = 0;k < mat1.m_cols; k++)
         {
            temp[i][j] += mat1[i][k] * mat2[k][j];
         }
      }
   }

   return temp; // return a copy
}



// GMatrix addition: A = B + C
/* friend */ 
const GMatrix operator+ (const GMatrix& mat1, const GMatrix& mat2)
{
   if( mat1.m_cols == 0 || mat1.m_rows == 0 ||
       mat2.m_cols == 0 || mat2.m_rows == 0 )
   {
      GMatrix::MatrixError( "+", "Addition called on GMatrix with zero rows and/or columns" );
   }

   if( mat1.m_cols != mat2.m_cols || mat1.m_rows != mat2.m_rows )
      GMatrix::MatrixError( "+", "Matrices not conformal for addition" );

   GMatrix temp( mat1.m_rows, mat1.m_cols );

   int i = 0,
       j = 0;

   for( i = 0; i < mat1.m_rows; i++ )
   {
      for( j = 0; j < mat1.m_cols; j++ )
      {
         temp[i][j] = mat1[i][j] + mat2[i][j];
      }
   }

   return temp; // return a copy
}



// GMatrix subtraction: A = B - C
/* friend */ 
const GMatrix operator- (const GMatrix& mat1, const GMatrix& mat2)
{
   if( mat1.m_cols == 0 || mat1.m_rows == 0 ||
       mat2.m_cols == 0 || mat2.m_rows == 0 )
   {
      GMatrix::MatrixError( "-", "Subtraction called on GMatrix with zero rows and/or columns" );
   }

   if( mat1.m_cols != mat2.m_cols || mat1.m_rows != mat2.m_rows )
      GMatrix::MatrixError( "-", "Matrices not conformal for subtraction" );

   GMatrix temp(mat1.m_rows, mat1.m_cols);

  int i = 0,
      j = 0;

   for( i = 0; i < mat1.m_rows; i++ )
   {
      for( j = 0; j < mat1.m_cols; j++ )
      {
         temp[i][j] = mat1[i][j] - mat2[i][j];
      }
   }

   return temp; // return a copy
}


// raise each element to a power
/* friend */ 
const GMatrix operator^ (const GMatrix& mat, double sclr)
{
   if( mat.m_cols == 0 || mat.m_rows == 0 )      
   {
      GMatrix::MatrixError( "^", "GMatrix with zero rows and/or columns");
   }

   GMatrix result( mat.m_rows, mat.m_cols );

   int i = 0,
       j = 0;

   for( i = 0; i < mat.m_rows; i++ )
   {
      for( j = 0; j < mat.m_cols; j++ )
      {
         result[i][j] = pow( (mat[i][j]), sclr );
      }
   }

   return result; // return a copy
}

/* friend */ 
const GMatrix operator+ (double sclr, const GMatrix& mat)
{
   if( mat.m_rows == 0 || mat.m_cols == 0 )
      GMatrix::MatrixError( "+", "GMatrix with zero rows and/or columns");

   GMatrix temp( mat.m_rows, mat.m_cols );

   int i = 0,
       j = 0;

   for( i = 0; i < mat.m_rows; i++ )
   {
      for( j = 0; j < mat.m_cols; j++ )
      {
         temp[i][j] = mat[i][j] + (double)sclr;
      }
   }

   return temp; // return a copy
}

/* friend */ 
const GMatrix operator- (double sclr, const GMatrix& mat)
{
   if( mat.m_rows == 0 || mat.m_cols == 0 )
      GMatrix::MatrixError( "+", "GMatrix with zero rows and/or columns");

   GMatrix temp( mat.m_rows, mat.m_cols );

   int i = 0,
       j = 0;

   for( i = 0; i < mat.m_rows; i++ )
   {
      for( j = 0; j < mat.m_cols; j++ )
      {
         temp[i][j] = (double)sclr - mat[i][j];
      }
   }

   return temp; // return a copy
}


/* friend */ 
const GMatrix operator* (double sclr, const GMatrix& mat)
{
   if( mat.m_rows == 0 || mat.m_cols == 0 )
      GMatrix::MatrixError( "*", "GMatrix with zero rows and/or columns");

   GMatrix temp( mat.m_rows, mat.m_cols );

   int i = 0,
       j = 0;

   for( i = 0; i < mat.m_rows; i++ )
   {
      for( j = 0; j < mat.m_cols; j++ )
      {
         temp[i][j] = mat[i][j] * (double)sclr;
      }
   }

   return temp; // return a copy
}

/* friend */ 
const GMatrix operator/ (const GMatrix& mat, double sclr)
{
   if( mat.m_rows == 0 || mat.m_cols == 0 )
      GMatrix::MatrixError( "/", "GMatrix with zero rows and/or columns");

   if( sclr == 0 )
      GMatrix::MatrixError( "/", "attempt to divide by zero: mat[i][j]/sclr" );

   GMatrix temp( mat.m_rows, mat.m_cols );

   int i = 0,
       j = 0;

   for( i = 0; i < mat.m_rows; i++ )
   {
      for( j = 0; j < mat.m_cols; j++ )
      {
         temp[i][j] = mat[i][j] / (double)sclr;
      }
   }

   return temp; // return a copy
}

/* friend */ 
const GMatrix operator/ (double sclr, const GMatrix& mat)
{
   if( mat.m_rows == 0 || mat.m_cols == 0 )
      GMatrix::MatrixError( "/", "GMatrix with zero rows and/or columns");

   GMatrix temp( mat.m_rows, mat.m_cols );

   int i = 0,
       j = 0;

   for( i = 0; i < mat.m_rows; i++ )
   {
      for( j = 0; j < mat.m_cols; j++ )
      {
         if( mat[i][j] == 0 )
            GMatrix::MatrixError( "/", "attempt to divide by zero: sclr/mat[i][j]" );

         temp[i][j] = (double)sclr / mat[i][j];
      }
   }

   return temp; // return a copy
}



// output of a GMatrix: cout << A;
/* friend */ 
ostream& operator<< (ostream& strm, const GMatrix& mat)
{
   // generate warning if GMatrix is 0 x 0
   if( mat.m_cols == 0 || mat.m_rows == 0 )
   {
      GMatrix::MatrixMessage( "<<", "attempt to print GMatrix with zero rows/cols" );
      return strm;
   }

   mat.Print( strm, strm.precision(), strm.width() );
   
   return strm;
}



//##ModelId=3C7561910263
int GMatrix::advanceBuffer( char *linemark, const char delimiter )
{
   int at_next_field=0;
   char c;
   int i = 0;
   double ret_val=0;
   int scount=0;
   int length=strlen(linemark);
   
   //advance by the delimiter to the start of the next field
   //read over first field to the start of the first digit in the next field   
   while( !at_next_field )
   {
      ret_val = sscanf( linemark+scount,"%c", &c );
      if( ret_val != 1 )
         return 0;
      scount++;
      if( scount > length )
         MatrixError( "advanceBuffer", "check for possible invalid delimiter" );
      if( delimiter == 'w' ) // delimited by whitespace
      {
         if( isspace(c) )
         {            
            do
            {
               ret_val = sscanf( linemark+scount,"%c", &c );
               if( ret_val != 1 )
                  return 0;      
               scount++;
            }while( isspace(c) );
            scount--;
            at_next_field=1;            
         }         
      }
      else if( c == delimiter )
      {         
         at_next_field=1;       
      }      
   }
   return(scount);   
}


///////////////////////////////////////////////////////////////////////////
// The following the functions are used in a recursive quicksort 
// algorith for double arrays
//
//

// The normal quicksort function
//##ModelId=3C75619102C1
void GMatrix::quickSortMat(GMatrix &a, int start, int end)
{
   if( start < end)
   {
      int split = partitionMat(a, start, end);
      quickSortMat(a, start, split);
      quickSortMat(a, split + 1, end);
   }
   return;
}

// swap two doubles i and j
//##ModelId=3C75619102D0
void GMatrix::swapMat_doubles(double& i, double& j)
{
   double temp = i;
   i = j;
   j = temp;
   return;
}

// partition the GMatrix
//##ModelId=3C75619102E0
int GMatrix::partitionMat(GMatrix &a, int start, int end)
{
   int right = end + 1;
   int left = start -1;
   double  pivot = a[start][0];
   while (right > left)
   {
      do { left++; } while (a[left][0] < pivot);
      do { right--; } while (a[right][0] > pivot);
      swapMat_doubles(a[left][0], a[right][0]);
   }
   swapMat_doubles(a[left][0], a[right][0]);
   return right;
}

// quicksort that also returns a sorted indexing GMatrix
//##ModelId=3C75619102EF
void GMatrix::quickSortMat(GMatrix &a, GMatrix &index, int start, int end)
{
   if( start < end)
   {
      int split = partitionMat(a, index, start, end);
      quickSortMat(a, index, start, split);
      quickSortMat(a, index, split + 1, end);
   }
   return;
}

// swap the doubles
//##ModelId=3C756191030F
void GMatrix::swapMat_doubles(double& i, double& j, double &index_a, double &index_b)
{
   double temp = i;
   double temp_ind = index_a;
   i = j;
   index_a = index_b;
   j = temp;
   index_b = temp_ind;
   return;
}

//partition the Matrices
//##ModelId=3C756191031E
int GMatrix::partitionMat(GMatrix &a, GMatrix &index, int start, int end)
{
   int right = end + 1;
   int left = start -1;
   double  pivot = a[start][0];
   while (right > left)
   {
      do { left++; } while (a[left][0] < pivot);
      do { right--; } while (a[right][0] > pivot);
      swapMat_doubles(a[left][0], a[right][0], index[left][0], index[right][0]);
   }
   swapMat_doubles(a[left][0], a[right][0], index[left][0], index[right][0]);
   return right;
}
//
//
///////////////////////////////////////////////////////////////////////////


// a rounding function used as a subroutine
//##ModelId=3C7561900080
double GMatrix::RoundVal(double value, double resolution)
{
   int power=0;
   double temp=0.0;   
   double intpart=0.0;
   double dummy;

   value = modf( value, &intpart );

   if( resolution != 0.0 )
   {
      while(resolution<1.0)
      {
         resolution*=10.0;
         power++;
      }
   }
   
   value *= pow(10.0,power);
   
   temp = modf( value, &dummy );
   temp*=10;
   temp = abs( (long int)temp );
   value = (long int)value;
   if( temp > 4 && temp < 10)
   {
      if( intpart < 0)
         value--;
      else
         value++;
   }
   
   value *= pow(10.0, -(power));

   return (value+intpart);
}



// Function to alter the GMatrix so that it contains only m_data within the specified time window
// only works on a time by row basis
//##ModelId=3C7561890030
void GMatrix::TimeWindow( const int TimeField, const double StartTime, const double Duration )
{
   TimeWindow( TimeField, StartTime, Duration, 1.0e99, 0.0 );
}

// Function alter the GMatrix so that it contains only m_data within the specified time 
// window but allows time rollovers
// only works on a time by row basis
//##ModelId=3C7561890040
void GMatrix::TimeWindow( const int TimeField, 
                         const double StartTime, 
                         const double Duration, 
                         const double Rollover, 
                         const double RollAllowance )
{
   int Index=0;
   double PriorTime = -999999.99;
   const int field = TimeField;

   int i = 0,
       j = 0;


   for( i = 0; i < nRows(); i++ )
   {
      // within range
      if( (m_data[i][field] >= StartTime) && (m_data[i][field] <= (StartTime+Duration)) )
      {
         for( j = 0; j < nCols(); j++ )
         {
            m_data[Index][j] = m_data[i][j];            
         }
         Index++;
      }
      else
      {
         // has rollover occured
         if(   (PriorTime      <    Rollover)
            && (PriorTime      >    Rollover-RollAllowance) 
            && (m_data[i][field] <    RollAllowance)
            && (PriorTime      !=   -999999.99) )
         {
            // rollover has occurred
            for( j = 0; j < nCols(); j++ )
            {
               m_data[Index][j] = m_data[i][j];            
            }
            Index++;
         }         
      }
      PriorTime = m_data[i][field];
   }

   Redim( Index, nCols() );
}


// Match Matrices by time defined via Columns (that can rollover) 
//    precision 0   = whole number
//    precision 0.1 = match to nearest tenth
//    etc.
// Rollovers
//     GPS time: Rollover = 604800.0 and RollAllowance = 14400.0
//     Hours   : Rollover = 12.0     and RollAllowance = 1.0 
//     Minutes : Rollover = 60.0     and RollAllowance = 2.0 
// Function returns adjusted Time Matched Matrices
//##ModelId=3C7561900091
void GMatrix::TimeMatchFields( GMatrix &A, 
                              const int FieldA, 
                              GMatrix &B, 
                              const int FieldB,
                              const double precision,
                              const double Rollover,
                              const double RollAllowance )
{
   int Index        = 0,
       IndexA       = 0,
       IndexB       = 0,
       row          = 0,
       col          = 0;       

   double field1    = 0,
          field2    = 0;          

   const GMatrix FixedA = A; 
   const GMatrix FixedB = B; 

   const int nRowsA = A.nRows();
   const int nRowsB = B.nRows();

   IndexA = 0;
   IndexB = 0;
   while( IndexA < nRowsA && IndexB < nRowsB ) 
   {
      field1 = FixedA[IndexA][FieldA];
      field1 = GMatrix::RoundVal(field1,precision);

      field2 = FixedB[IndexB][FieldB];
      field2 = GMatrix::RoundVal(field2,precision);
      
      if( (field1 == -99999999.0) || (field2 == -99999999.0) )
         break;

      if( field1 == field2 )
      {
         // copy row to A         
         for( col = 0; col < A.nCols(); col++ )
            A[Index][col] = FixedA[IndexA][col];

         // copy row to B      
         for( col = 0; col < B.nCols(); col++ )
            B[Index][col] = FixedB[IndexB][col];

         Index++;
         IndexA++;
         IndexB++;
      }	  
      else if( field1 < field2 )
      {
         if( field1 >= 0.0 && field1 <= RollAllowance && field2 > Rollover - RollAllowance )
            IndexB++; // Rollover occurred in A so advance B
         else
            IndexA++; 
      }
      else if( field1 > field2 )
      {
         if( field2 >= 0.0 && field2 <= RollAllowance && field1 > Rollover - RollAllowance )
            IndexA++; // Rollover occurred in B so advance A
         else
            IndexB++;
      }      
   }
   A.Redim(Index,A.nCols());
   B.Redim(Index,B.nCols());
}










/////////////////////////////////////////////////////////////////////////////////
//                        ROWARRAY CLASS STUFF
//
//
// retrieve an element value (read-only)
//##ModelId=3C756190006F
const GMatrix::RowArray GMatrix::operator[] (int row) const
{
   CheckIndex( row, m_rows );
   return RowArray( m_data[row], m_cols );
}


// set an element value (allows write access)
//##ModelId=3C756190007E
GMatrix::RowArray GMatrix::operator[] (int row)
{
   CheckIndex( row, m_rows );
   return RowArray( m_data[row], m_cols );
}
//
//
/////////////////////////////////////////////////////////////////////////////////

























//##ModelId=3C756192014B
MatrixPlot::MatrixPlot()
: kImageHeight(450),
  kImageWidth(600),
  kPlotHeight(340),
  kPlotWidth(430),
  kWhite(0),
  kBlack(1),
  kBlue(2)
{
   BitmapFileHeader.Type = 0x4D42; //BM
   BitmapFileHeader.Size = (kImageWidth*kImageHeight + sizeof(stBitmapFileHeader) + sizeof(BitmapInfoHeader));
   BitmapFileHeader.Reserved1 = 0;
   BitmapFileHeader.Reserved2 = 0;
   BitmapFileHeader.Offsetbits = sizeof(BitmapInfoHeader);

   BitmapInfoHeader.Size           = sizeof(BitmapInfoHeader) - sizeof(stColorTable);
   BitmapInfoHeader.Width          = kImageWidth;
   BitmapInfoHeader.Height         = kImageHeight;
   BitmapInfoHeader.Planes         = 1;
   BitmapInfoHeader.BitCount       = 8;
   BitmapInfoHeader.Compression    = 0;
   BitmapInfoHeader.SizeImage      = kImageWidth*kImageHeight;
   BitmapInfoHeader.XPelsPerMeter  = 800;
   BitmapInfoHeader.YPelsPerMeter  = 600;
   BitmapInfoHeader.ClrUsed        = 3;
   BitmapInfoHeader.ClrImportant   = 3;
   BitmapInfoHeader.ColorTable.RGBWhite.Blue = 255;   
   BitmapInfoHeader.ColorTable.RGBWhite.Green = 255;
   BitmapInfoHeader.ColorTable.RGBWhite.Red = 255;
   BitmapInfoHeader.ColorTable.RGBWhite.Reserved = 0;
   BitmapInfoHeader.ColorTable.RGBBlack.Blue = 0;
   BitmapInfoHeader.ColorTable.RGBBlack.Green = 0;
   BitmapInfoHeader.ColorTable.RGBBlack.Red = 0;
   BitmapInfoHeader.ColorTable.RGBBlack.Reserved = 0;   
   BitmapInfoHeader.ColorTable.RGBBlue.Blue = 255;
   BitmapInfoHeader.ColorTable.RGBBlue.Green = 0;
   BitmapInfoHeader.ColorTable.RGBBlue.Red = 0;
   BitmapInfoHeader.ColorTable.RGBBlue.Reserved = 0;   

   PlotData.Redim(kImageHeight,kImageWidth);
   PlotData.Fill(kWhite);
}

// destructor
//##ModelId=3C7561920159
MatrixPlot::~MatrixPlot ()
{

}




//##ModelId=3C756192015B
void MatrixPlot::Plot( const GMatrix &X,
                       const GMatrix &Y )
{
   int i = 0,
       x = 0,
       y = 0;

   const int kPlotWidth  = 430;
   const int kPlotHeight = 340;
   
   const int kStartX   = 100;
   const int kFinishX  = 530;
   const int kStartY   = 40;
   const int kFinishY  = 380;

   // first draw the box   
   DrawLine( kBlack, kStartX,  kStartY,  kFinishX, kStartY);  //bottom
   DrawLine( kBlack, kStartX,  kStartY,  kStartX,  kFinishY); //left side
   DrawLine( kBlack, kStartX,  kFinishY, kFinishX, kFinishY); //top
   DrawLine( kBlack, kFinishX, kStartY,  kFinishX, kFinishY); //right side

   const double kMinX = X.MinVal();
   const double kMaxX = X.MaxVal();
   const double kMinY = Y.MinVal();
   const double kMaxY = Y.MaxVal();

   double value = 0.0;
   // determine the mapping scale factors
   value = X.Range();
   if( value == 0.0 )
      value = 0.1;
   const double kRangeX  = value;
   const double kOnePercentRangeX = kRangeX / 100.0;

   value = Y.Range();
   if( value == 0.0 )
      value = 0.1;   
   const double kRangeY  = value;
   const double kOnePercentRangeY= kRangeY / 100.0;
   const double kScaleX  = kPlotWidth  / kRangeX;
   const double kScaleY  = kPlotHeight / kRangeY;
   const double kSpacingX = kRangeX / 5.0;
   const double kSpacingY = kRangeY / 8.0;

      // draw the tick marks along the x-axis
   const int kNumTicksX = (int) (kRangeX / kSpacingX);
   if( kNumTicksX == 0 )
   {
      GMatrix::MatrixError("GMatrix::Plot() Bad Spacing-X Parameter");
   }

   const int kTickSizeX = kPlotWidth / kNumTicksX;
   
   x = kStartX + kTickSizeX;
   while( x < (kPlotWidth + kStartX) )
   {
      DrawLine( kBlack, x, kStartY,  x, kStartY  + 4 ); // bottom ticks
      DrawLine( kBlack, x, kFinishY, x, kFinishY - 4 ); // top ticks
      x += kTickSizeX;
   }

   // draw the tick marks along the y-axis
   const int kNumTicksY = (int) (kRangeY / kSpacingY);
   if( kNumTicksY == 0 )
   {
      GMatrix::MatrixError("GMatrix::Plot() Bad Spacing-Y Parameter");
   }

   const int kTickSizeY = kPlotHeight / kNumTicksY;
   
   y = kStartY + kTickSizeY;
   while( y < (kPlotHeight + kStartY) )
   {
      DrawLine( kBlack, kStartX,  y, kStartX  + 4, y );
      DrawLine( kBlack, kFinishX - 4, y, kFinishX, y );
      y += kTickSizeY;
   }


   // now the hard part
   // draw the x-labels
   x = kStartX - 48;
   value = kMinX;
   while( x < (kPlotWidth + kStartX) && !(value > kMaxX + kOnePercentRangeX) )
   {
      DrawValue( value, x - 10, kStartY - 5 );
      value += kSpacingX;
      x += kTickSizeX;      
   }
   // draw the y-labels   
   y = kStartY + 6;
   value = kMinY;
   while( y < (kPlotWidth + kStartY) && !(value > kMaxY + kOnePercentRangeY) )
   {
      DrawValue( value, kStartX - 80, y);
      value += kSpacingY;
      y += kTickSizeY;
   }


   // now plot the PlotData
   for( i = 0; i < X.nRows()-1; i++ )
   {
      int x1 = (int)((X[i][0]-kMinX)*kScaleX) + kStartX;
      int y1 = (int)((Y[i][0]-kMinY)*kScaleY) + kStartY;
      int x2 = (int)((X[i+1][0]-kMinX)*kScaleX) + kStartX;
      int y2 = (int)((Y[i+1][0]-kMinY)*kScaleY) + kStartY;
      DrawPoint( kBlue, x1, y1 );
      DrawLine(
                kBlue,
                x1, 
                y1, 
                x2, 
                y2 );
   }
}


//##ModelId=3C756192016A
bool MatrixPlot::SaveToFile( const char *FileName )
{
   int i = 0,
       x = 0,
       y = 0;

   // now output the PlotData to file
   FILE* BmpFile = NULL;

   BmpFile = fopen(FileName,"w");
   if( !BmpFile )
      return false;

   // write the header
   fwrite( &BitmapFileHeader, sizeof(BitmapFileHeader), 1, BmpFile );
   fwrite( &BitmapInfoHeader, sizeof(BitmapInfoHeader), 1, BmpFile );
   
   // write the PlotData
   y = kImageHeight;
   byte abyte = kWhite;
   for( i = 0; i < kImageHeight; i++ )
   {
      y--;
      for( x = 0; x < kImageWidth; x++ )
      {
         abyte = (byte)(PlotData[i][x]);
         fwrite( &(abyte), sizeof(byte), 1, BmpFile );
      }
   }
 
   fclose(BmpFile);
   return true;
}




// plot these columns in a CBitmap
//##ModelId=3C75618803DA
bool GMatrix::Plot( const char* FileName, 
                   const int col1,                                      
                   const int col2 )
{     
   int i = 0,
       x = 0,
       y = 0;

  GMatrix X = (*this).Column(col1);
  GMatrix Y = (*this).Column(col2);

  MatrixPlot P;
  P.Plot( X, Y );

  return P.SaveToFile( FileName );
}



//##ModelId=3C756192017D
void MatrixPlot::DrawValue( const double value, const int left, const int top )
{
   char ValueBuffer[20];
   ValueBuffer[0] = '\0';
   
   if( fabs(value) > 1.0e+5 || (fabs(value) < 1.0e-5 && value != 0.0) )
   {      
      sprintf( ValueBuffer, "%13.3e", value );
   }
   else
   {      
      sprintf( ValueBuffer, "%13.3lf", value );
   }  
   
   const int kTextWidth = 6*13;
   
   if( (left + kTextWidth) >= kImageWidth )
      return;
   if( top >= kImageHeight )
      return;
  
   const int length = strlen(ValueBuffer);

   int offset = 0,
       i      = 0;

   const byte kBlack = 0;

   // trim right zeros
   i = length-1;
   while( i>=0 && ValueBuffer[i] == '0' )
   {
      ValueBuffer[i] = '\0';
      i--;
   }
   if(ValueBuffer[i] == '.')
      ValueBuffer[i] = '\0';


   offset = 0;
   i = 0;
   while( i >= 0 && ValueBuffer[i] != '\0' )
   {
      switch( ValueBuffer[i] )
      {
         case 'e': DrawNumber( kExp,   left + offset, top ); break;
         case '-': DrawNumber( kMinus, left + offset, top ); break;
         case '.': DrawNumber( kDot,   left + offset, top ); break;
         case '1': DrawNumber( kOne,   left + offset, top ); break;
         case '2': DrawNumber( kTwo,   left + offset, top ); break;
         case '3': DrawNumber( kThree, left + offset, top ); break;
         case '4': DrawNumber( kFour,  left + offset, top ); break;
         case '5': DrawNumber( kFive,  left + offset, top ); break;
         case '6': DrawNumber( kSix,   left + offset, top ); break;
         case '7': DrawNumber( kSeven, left + offset, top ); break;
         case '8': DrawNumber( kEight, left + offset, top ); break;
         case '9': DrawNumber( kNine,  left + offset, top ); break;
         case '0': DrawNumber( kZero,  left + offset, top ); break;
         default: break;
      }
      i++;
      offset +=6;               
   }
}



//##ModelId=3C756192018E
void MatrixPlot::DrawPoint( byte color, const int x, const int y )
{
   if( y < 3 )
      return;

   if( x + 3 >= kImageWidth )
      return;

   byte Point[3][3] =
   {{ 1,1,1 },
   {  1,1,1 },
   {  1,1,1 },};


   for( int i = 0; i < 3; i++ )
   {
      for( int j = 0; j < 3; j++ )
      {
         PlotData[y-i+1][x+j-1] = (Point[i][j]) * color;
      }
   }
}


      
//##ModelId=3C756192018A
void MatrixPlot::DrawNumber( eNumber Number, const int x, const int y )
{
   
   if( y < 7 )
      return;

   if( x + 6 >= kImageWidth )
      return;

   byte Exp[7][6] =
   {{ 0,0,0,0,0,0 },
   { 0,0,0,0,0,0 },
   { 0,1,1,1,0,0 },
   { 1,0,0,0,1,0 },
   { 1,1,1,1,0,0 },
   { 1,0,0,0,0,0 },
   { 0,1,1,1,0,0 },};

   byte Dot[7][6] =
   {{ 0,0,0,0,0,0 },
   { 0,0,0,0,0,0 },
   { 0,0,0,0,0,0 },
   { 0,0,0,0,0,0 },
   { 0,0,0,0,0,0 },
   { 0,0,1,1,0,0 },
   { 0,0,1,1,0,0 },};

   byte Minus[7][6] =
   {{ 0,0,0,0,0,0 },
   { 0,0,0,0,0,0 },
   { 0,1,1,1,1,0 },
   { 0,0,0,0,0,0 },
   { 0,0,0,0,0,0 },
   { 0,0,0,0,0,0 },
   { 0,0,0,0,0,0 },};

   byte Zero[7][6] = 
   {{ 0,1,1,1,0,0 },
   { 1,0,0,0,1,0 },
   { 1,0,0,1,1,0 },
   { 1,0,1,0,1,0 },
   { 1,1,0,0,1,0 },
   { 1,0,0,0,1,0 },
   { 0,1,1,1,0,0 }};

   byte One[7][6] =
   {{ 0,0,1,0,0,0 },
   { 0,1,1,0,0,0 },
   { 0,0,1,0,0,0 },
   { 0,0,1,0,0,0 },
   { 0,0,1,0,0,0 },
   { 0,0,1,0,0,0 },
   { 0,1,1,1,0,0 }};

   byte Two[7][6] =
   {{ 0,1,1,1,0,0 },
   { 1,0,0,0,1,0 },
   { 0,0,0,0,1,0 },
   { 0,0,0,1,0,0 },
   { 0,0,1,0,0,0 },
   { 0,1,0,0,0,0 },
   { 1,1,1,1,1,0 }};

   byte Three[7][6] =
   {{ 1,1,1,1,1,0 },
   { 0,0,0,1,0,0 },
   { 0,0,1,0,0,0 },
   { 0,0,0,1,0,0 },
   { 0,0,0,0,1,0 },
   { 1,0,0,0,1,0 },
   { 0,1,1,1,0,0 }};

   byte Four[7][6] =
   {{ 0,0,0,1,0,0 },
   { 0,0,1,1,0,0 },
   { 0,1,0,1,0,0 },
   { 1,0,0,1,0,0 },
   { 1,1,1,1,1,0 },
   { 0,0,0,1,0,0 },
   { 0,0,0,1,0,0 }};

   byte Five[7][6] =
   {{ 1,1,1,1,1,0 },
   { 1,0,0,0,0,0 },
   { 1,1,1,1,0,0 },
   { 0,0,0,0,1,0 },
   { 0,0,0,0,1,0 },
   { 1,0,0,0,1,0 },
   { 0,1,1,1,0,0 }};

   byte Six[7][6] = 
   {{ 0,0,1,1,0,0 },
   { 0,1,0,0,0,0 },
   { 1,0,0,0,0,0 },
   { 1,1,1,1,0,0 },
   { 1,0,0,0,1,0 },
   { 1,0,0,0,1,0 },
   { 0,1,1,1,0,0 }};

   byte Seven[7][6] =
   {{ 1,1,1,1,1,0 },
   { 0,0,0,0,1,0 },
   { 0,0,0,1,0,0 },
   { 0,0,1,0,0,0 },
   { 0,0,1,0,0,0 },
   { 0,0,1,0,0,0 },
   { 0,0,1,0,0,0 }};

   byte Eight[7][6] =
   {{ 0,1,1,1,0,0 },
   { 1,0,0,0,1,0 },
   { 1,0,0,0,1,0 },
   { 0,1,1,1,0,0 },
   { 1,0,0,0,1,0 },
   { 1,0,0,0,1,0 },
   { 0,1,1,1,0,0 }};

   byte Nine[7][6] =
   {{ 0,1,1,1,0,0 },
   { 1,0,0,0,1,0 },
   { 1,0,0,0,1,0 },
   { 0,1,1,1,1,0 },
   { 0,0,0,0,1,0 },
   { 0,0,0,1,0,0 },
   { 0,1,1,0,0,0 }};

   for( int i = 0; i < 7; i++ )
   {
      for( int j = 0; j < 6; j++ )
      {
         switch( Number )
         {
            case kZero:  PlotData[y-i][x+j] = (Zero[i][j])  * kBlack; break;
            case kOne:   PlotData[y-i][x+j] = (One[i][j])   * kBlack; break;
            case kTwo:   PlotData[y-i][x+j] = (Two[i][j])   * kBlack; break;
            case kThree: PlotData[y-i][x+j] = (Three[i][j]) * kBlack; break;
            case kFour:  PlotData[y-i][x+j] = (Four[i][j])  * kBlack; break;
            case kFive:  PlotData[y-i][x+j] = (Five[i][j])  * kBlack; break;
            case kSix:   PlotData[y-i][x+j] = (Six[i][j])   * kBlack; break;
            case kSeven: PlotData[y-i][x+j] = (Seven[i][j]) * kBlack; break;
            case kEight: PlotData[y-i][x+j] = (Eight[i][j]) * kBlack; break;
            case kNine:  PlotData[y-i][x+j] = (Nine[i][j])  * kBlack; break;            
            case kDot:   PlotData[y-i][x+j] = (Dot[i][j])   * kBlack; break;            
            case kMinus: PlotData[y-i][x+j] = (Minus[i][j]) * kBlack; break;
            case kExp:   PlotData[y-i][x+j] = (Exp[i][j])   * kBlack; break;            
            default: break;
         }
      }
   }
}




//##ModelId=3C756192016C
void MatrixPlot::DrawLine( byte color, const int x, const int y, const int to_x, const int to_y )
{
   int row      = 0,
       row_last = 0,
       halfway  = 0,
       col      = 0;

   double m = 0.0,
          b = 0.0;


   if( x >= kImageWidth || to_x >= kImageWidth )
      return;
   if( y >= kImageHeight || to_y >= kImageHeight )
      return;

   
   
   // y = mx + b
   if( to_x - x == 0 ) // m = 0
   {
      for( row=y; row<=to_y; row++ )
      {
         PlotData[row][x] = color;
      }
      return;
   }

   if( to_x > x )
   {
      m = (double)(to_y - y) / (double)(to_x - x);
      b = y - m*x;

      col = x;
      row_last = (int)(m*col + b);      
      for( col=x; col<=to_x; col++ )
      {
         row = (int)(m*col + b);

         halfway = abs(row_last - row) / 2;
         while( abs(row_last - row) > 1 ) // must draw a vertical line
         {
            if( row_last > row )
               row_last--;
            else
               row_last++;

            // draw vertical halfway up col-1 and halfway up col
            if( abs(row_last - row) > halfway )
               PlotData[row_last][col-1] = color;
            else
               PlotData[row_last][col] = color;

         }

         PlotData[row][col] = color;
         row_last = row;
      }
   }
   else
   {
      m = (double)(y - to_y) / (double)(x - to_x);
      b = y - m*x;

      col = to_x;
      row_last = (int)(m*col + b);      
      for( col=to_x; col<=x; col++ )
      {
         row = (int)(m*col + b);

         halfway = abs(row_last - row) / 2;
         while( abs(row_last - row) > 1 ) // must draw a vertical line
         {
            if( row_last > row )
               row_last--;
            else
               row_last++;

            // draw vertical halfway up col-1 and halfway up col
            if( abs(row_last - row) > halfway )
               PlotData[row_last][col-1] = color;
            else
               PlotData[row_last][col] = color;

         }

         PlotData[row][col] = color;
         row_last = row;
      }
   }
}
