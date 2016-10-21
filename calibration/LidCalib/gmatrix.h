/*
-------------------------------------------------------------------------------------

   Filename:           GMatrix.h

   Date started:       11/09/1999 (month/day/year)
   Date last modified: 03/24/2001
   Written by:         Glenn MacGougan [gdmacgou@hotmail.com]

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

#ifndef MATRIX_GDM_H
#define MATRIX_GDM_H


#ifdef _WINDOWS // This is defined by the Compiler if the program is running in Windows mode
   #define USING_MFC // Needed for GMatrix Class to Tell it to use MFC error output
   #include <afxwin.h>
#endif // you can override this by defining USING_MFC on your own

#ifndef USING_MFC
   #define MATRIX_MESSAGES // turn on messaging for ReadFromFile and some Print functions
#endif

#include <stdio.h>

#include <fstream>   
using namespace std;

// #ifdef _MATH_DLL
// #define MATH_DLL_API __declspec(dllexport)
// #else
// #define MATH_DLL_API __declspec(dllimport)
// #endif


//##ModelId=3C7561880002
class GMatrix
{

public: // Constructors / Destructor

	//##ModelId=3C7561880011
   GMatrix();                                        // default constructor (no data allocated yet)
	//##ModelId=3C75618800DC
   GMatrix(int nrows);                               // vector style constructor == (nrows,1)
	//##ModelId=3C7561880198
   GMatrix(int nrows, int ncols);                    // GMatrix style constructor (nrow,ncols)
	//##ModelId=3C7561880253
   GMatrix(const GMatrix& mat);                       // copy constructor
	//##ModelId=3C756188031E
   virtual ~GMatrix();                               // destructor

public: // High Level Operations

   // plot the GMatrix to a bitmap file - in its infancy
	//##ModelId=3C75618803DA
   bool Plot( const char* FileName, const int col1, const int col2 );

   // input GMatrix from a file
   // 'w' indicates whitespace delimited, ',' indicated comma delimted, etc   
	//##ModelId=3C7561890002
   void ReadFromFile(const char *infile_name, const char delimiter); 

   
   // alter the GMatrix so that it contains only data within the specified time window
	//##ModelId=3C7561890030
   void TimeWindow( const int    TimeField, 
                    const double StartTime, 
                    const double Duration );
   
   // alter the GMatrix so that it contains only data within the specified time window but allows time rollovers
	//##ModelId=3C7561890040
   void TimeWindow( const int    TimeField, 
                    const double StartTime, 
                    const double Duration, 
                    const double Rollover, 
                    const double RollAllowance );
   
public: // GMatrix Qualifiers

	//##ModelId=3C756189005F
   bool isConformal(const GMatrix& mat) const;          // Is the GMatrix mat conformal for multiplication (*this * mat)
	//##ModelId=3C75618900AD
   bool isSameSize(const GMatrix& mat) const;           // Is this GMatrix the same size as mat
	//##ModelId=3C75618900FC
   bool isSquare() const  { return m_rows == m_cols; } // is this a square GMatrix?
	//##ModelId=3C756189010B
   int  nCols()    const  { return m_cols;           } // return no. of cols
	//##ModelId=3C756189010D
   int  nElems()   const  { return m_rows * m_cols;  } // return total no. of elements
	//##ModelId=3C756189011C
   int  nRows()    const  { return m_rows;           } // return no. of rows   
      
 

public: // GMatrix Mainpulation

	//##ModelId=3C756189012A
   void Clear();                                       // remove the GMatrix from memory   
	//##ModelId=3C75618901D6
   void Zero();                                        // zero all elements   
	//##ModelId=3C7561890273
   void ZeroCol(const int col);                        // zero all elements in a specified column
	//##ModelId=3C756189030F
   void ZeroRow(const int row);                        // zero all elements in a specified row
	//##ModelId=3C75618903AB
   void Concatonate(const GMatrix& mat);                // A becomes A|B   
	//##ModelId=3C75618A0011
   void Fill(const double value);                      // fills the GMatrix with the given value
	//##ModelId=3C75618A00AD
   void FillRow(const int row,const double value);     // fills the GMatrix row with the given value
	//##ModelId=3C75618A014A
   void FillColumn(const int col,const double value);  // fills the GMatrix column with the given value
	//##ModelId=3C75618A01F6
   void Identity(void);                                // reset the GMatrix to an identity
	//##ModelId=3C75618A0292
   void InvInPlace();                                  // Inverts the GMatrix
	//##ModelId=3C75618B0011
   void Redim(const int newRows, const int newCols);   // redimension GMatrix
	//##ModelId=3C75618B005F
   void RemoveColumn(const int col);                   // removes specified column
	//##ModelId=3C75618B00BD
   void RemoveRow(const int row);                      // removes specified row
	//##ModelId=3C75618B011B
   void Round(const double precision_);                // round the GMatrix to the specified precision 
                                                       // (eg 18.33348 rounded with precision=0.0001 
                                                       // becomes 18.3335)
	//##ModelId=3C75618B0169
   void AddColumn(const GMatrix& mat );                 // Concatonates a Column to the end GMatrix
	//##ModelId=3C75618B01B7
   void AddRow(const GMatrix& mat);                     // Concatonates a Row to the end GMatrix
   
	//##ModelId=3C75618B01F6
   void InsertSubMatrix( const GMatrix& mat,
                         const int row,
                         const int col );              // Inserts a submatrix starting at indices (row,col)
   
	//##ModelId=3C75618B0234
   void Sort();                                        // sort the GMatrix on a column by column basis 
                                                       // in ascending order using a quicksort algorith

	//##ModelId=3C75618B0292
   void SortByColumn(const int col);                   // sorts the GMatrix's rows using a quicksort algorithm 
                                                       // based on the values in one column
   
public: // Statistics

	//##ModelId=3C75618B02E0
   double MaxAbs(int &row, int &col) const;         // returns the value of the largest absolute element and reference to its index
	//##ModelId=3C75618B037C
   double Max(int &row, int &col) const;            // returns the index of the maximum element in the GMatrix and reference to its index
	//##ModelId=3C75618C0021
   double Min(int &row, int &col) const;            // returns the index of the minimum element in the GMatrix and reference to its index 
   
	//##ModelId=3C75618C009E
   double MaxAbsVal() const;                         // returns the value of the largest absolute element   
	//##ModelId=3C75618C011B
   double MinAbsVal() const;                         // returns the value of the smallest absolute element
	//##ModelId=3C75618C0198
   double MaxVal() const;                            // returns the maximum element in the GMatrix
	//##ModelId=3C75618C0234
   double MaxColVal(const int col) const;            // returns the maximum element in the specified column
	//##ModelId=3C75618C02B1
   double MinVal() const;                            // returns the minimum element in the GMatrix
	//##ModelId=3C75618C034D
   double MinColVal(const int col) const;            // returns the minimum element in the specified column
	//##ModelId=3C75618C03CA
   double Range() const;                             // returns the range of the data ie: Range = MaxVal - MinVal
	//##ModelId=3C75618D005F
   double Sum() const;                               // returns the sum of all values in the GMatrix
	//##ModelId=3C75618D00DC
   double ColumnSum(const int col) const;            // returns the sum of one column
	//##ModelId=3C75618D014A
   double RowSum(const int row) const;               // returns the sum of one row
	//##ModelId=3C75618D01B7
   double ColumnNorm(const int col) const;           // returns the norm of a column
	//##ModelId=3C75618D0244
   double SS() const;                                // returns the sum of squares of the GMatrix
	//##ModelId=3C75618D02B1
   double Mean() const;                              // returns a sample mean for the entire GMatrix
	//##ModelId=3C75618D030F
   double Var() const;                               // returns a sample variance for the entire GMatrix
	//##ModelId=3C75618D037C
   double Stdev() const;                             // returns the sample standard deviation  for the entire GMatrix
	//##ModelId=3C75618D03DA
   double RMS() const;                               // returns the sample RMS value - only works on a (n,1) GMatrix
	//##ModelId=3C75618E006F
   double Skew() const;                              // returns the skewness - only works on a (n,1) GMatrix
	//##ModelId=3C75618E00CD
   double Kurt() const;                              // returns the kurtosis - only works on a (n,1) GMatrix   
	//##ModelId=3C75618E012A
   double Trace() const;                             // returns the trace of a square (only) GMatrix
   
public: // Operations that return Matrices

	//##ModelId=3C75618E0188
   GMatrix Abs() const;                                // returns the absolute GMatrix ie. all positive
	//##ModelId=3C75618E01D6
   GMatrix ATA() const;                                // returns the multiplication of the GMatrix transpose by itself: ATA
                                                      // useful for least squares problems (without weight GMatrix)
   
	//##ModelId=3C75618E0224
   GMatrix Column(const int col) const;                // returns the column specified by the index
	//##ModelId=3C75618E0273
   GMatrix DiagInv() const;                            // returns the inverse of a diagonal GMatrix
	//##ModelId=3C75618E02F0
   GMatrix DotMulitply( const GMatrix& mat) const;      // A .* B (element multiplication)
	//##ModelId=3C75618E033E
   GMatrix DotDivision( const GMatrix& mat) const;      // A ./ B (element division)      
	//##ModelId=3C75618E037C
   GMatrix Inv() const;                                // returns the inverse
	//##ModelId=3C75618F0021
   GMatrix RobustInverse() const;                      // perfroms an inverse by Gaussian Elimination methods
	//##ModelId=3C75618F0050
   GMatrix Row(const int row) const;                   // returns the row specified by the index
	//##ModelId=3C75618F009E
   GMatrix T() const;                                  // returns the transpose   

	//##ModelId=3C75618F013A
   GMatrix ExtractSubMatrix( const int row,
                            const int col,
                            const int numrows,
                            const int numcols );      // Extract a submatrix starting at indices (row,col) with size numrows,numcols

	//##ModelId=3C75618F0188
   void LUFactorization( bool &IsFullRank,            // Perform a lower/upper factorization of the GMatrix 
                         GMatrix &L,                   // returns IsFullRank == true if L and U are set
                         GMatrix &U );                 // this GMatrix must be dimension (n,n)


  
public: // Output operations
   
   // GMatrix output with default precision and width delimited by whitespace
	//##ModelId=3C75618F01C7
   void Print(ostream& strm, const int precision = 3, const int width = 12) const;
	//##ModelId=3C75618F01F5
   void Print(FILE* outFile, const int precision = 3, const int width = 12) const;
   
   // GMatrix output with default precision and width delimited by the specified delimiter
	//##ModelId=3C75618F0224
//    void Print(ostream& strm, const int precision = 3, const char delimiter = ',') const;
// 	//##ModelId=3C75618F0253
//    void Print(FILE* out,     const int precision = 3, const char delimiter = ',') const;

	//##ModelId=3C75618F0282
   void Print( const char* outfile, const int precision, const char delimiter = ',') const;
	//##ModelId=3C75618F02B1
   void Print( const char* outfile, const int precision, const int width) const;
   
   // output GMatrix to stream: cout << A; OR: file << A;
	//##ModelId=3C756191033F
   friend ostream& operator<< (ostream& strm, const GMatrix& mat);
   
public: // Operator Overloads

	//##ModelId=3C75618F02EF
   GMatrix& operator=(const GMatrix& mat);             // assignment operator

   // add, subtract, multiply or divide by a scalar (shorthand notation: A *= 5)
	//##ModelId=3C75618F038C
   void operator+= (int    sclr) { (*this)+=(double)sclr; }
	//##ModelId=3C75618F038E
   void operator+= (float  sclr) { (*this)+=(double)sclr; }
	//##ModelId=3C75618F039B
   void operator+= (double sclr);   
	//##ModelId=3C75618F03BB
   void operator-= (int    sclr) { (*this)+=-1.0*(double)sclr; }
	//##ModelId=3C75618F03BD
   void operator-= (float  sclr) { (*this)+=-1.0*(double)sclr; }
	//##ModelId=3C75618F03CA
   void operator-= (double sclr) { (*this)+=-1.0*(double)sclr; }      
	//##ModelId=3C75618F03CC
   void operator*= (int    sclr) { (*this)*=(double)sclr; }      
	//##ModelId=3C75618F03DA
   void operator*= (float  sclr) { (*this)*=(double)sclr; }
	//##ModelId=3C75618F03DC
   void operator*= (double sclr);
	//##ModelId=3C7561900011
   void operator/= (int    sclr) { (*this)/=(double)sclr; }
	//##ModelId=3C7561900013
   void operator/= (float  sclr) { (*this)/=(double)sclr; }
	//##ModelId=3C7561900021
   void operator/= (double sclr);

   // add or subtract a GMatrix (shorthand notation: A += B)
	//##ModelId=3C7561900040
   void operator+= (const GMatrix& mat);
	//##ModelId=3C7561900050
   void operator-= (const GMatrix& mat);

   // postfix ++ operator overload
   // add +1.0 to all elements and returns GMatrix values after the increment, e.g. GMatrix B = A++
	//##ModelId=3C756191034D
   friend const GMatrix operator++ (GMatrix& mat, int);

   // postfix -- operator overload
   // subtract 1.0 to all elements and returns GMatrix values after the increment, e.g. GMatrix B = A--
	//##ModelId=3C7561910351
   friend const GMatrix operator-- (GMatrix& mat, int);

   // equality operator
	//##ModelId=3C756191035E
   friend bool operator== (const GMatrix& mat1, const GMatrix& mat2);

   // multiply, add or subtract two matrices
	//##ModelId=3C7561910362
   friend const GMatrix operator* (const GMatrix& mat1, const GMatrix& mat2);
	//##ModelId=3C756191036F
   friend const GMatrix operator+ (const GMatrix& mat1, const GMatrix& mat2);
	//##ModelId=3C756191037C
   friend const GMatrix operator- (const GMatrix& mat1, const GMatrix& mat2);
   
   // raise all GMatrix elements to the power sclr_
	//##ModelId=3C7561910380
   friend const GMatrix operator^ (const GMatrix& mat, int sclr)     { return mat^( (double)sclr ); }
	//##ModelId=3C756191038E
   friend const GMatrix operator^ (const GMatrix& mat, float sclr)   { return mat^( (double)sclr ); }
	//##ModelId=3C7561910392
   friend const GMatrix operator^ (const GMatrix& mat, double sclr);

   // add to a GMatrix by a scalar variable: ie. A = 2.0 + B and B + 2.0 (adds to 2.0 to all elements)
	//##ModelId=3C756191039E
   friend const GMatrix operator+ (const GMatrix& mat, int    sclr)  { return ((double)sclr) + mat; }
	//##ModelId=3C75619103AB
   friend const GMatrix operator+ (const GMatrix& mat, float  sclr)  { return ((double)sclr) + mat; }
	//##ModelId=3C75619103AF
   friend const GMatrix operator+ (const GMatrix& mat, double sclr)  { return sclr + mat;           }
	//##ModelId=3C75619103BC
   friend const GMatrix operator+ (int    sclr, const GMatrix& mat)  { return ((double)sclr) + mat; }
	//##ModelId=3C75619103C0
   friend const GMatrix operator+ (float  sclr, const GMatrix& mat)  { return ((double)sclr) + mat; }
	//##ModelId=3C75619103CC
   friend const GMatrix operator+ (double sclr, const GMatrix& mat);

   // subtract from a GMatrix by a scalar variable: ie. A = B - 2.0
	//##ModelId=3C75619103D0
   friend const GMatrix operator- (const GMatrix& mat, int    sclr)  { return mat + (-1.0*(double)sclr); }
	//##ModelId=3C75619103DD
   friend const GMatrix operator- (const GMatrix& mat, float  sclr)  { return mat + (-1.0*(double)sclr); }
	//##ModelId=3C7561920001
   friend const GMatrix operator- (const GMatrix& mat, double sclr)  { return mat + (-1.0*sclr);         }

   // subtract from a GMatrix by a scalar variable: ie. A = 2.0 - B == -B + 2.0
	//##ModelId=3C7561920005
   friend const GMatrix operator- (int    sclr, const GMatrix& mat)  { return mat - ((double)sclr); }
	//##ModelId=3C7561920011
   friend const GMatrix operator- (float  sclr, const GMatrix& mat)  { return mat - ((double)sclr); }
	//##ModelId=3C7561920015
   friend const GMatrix operator- (double sclr, const GMatrix& mat);

   // multiply GMatrix by a scalar variable: A = 2.0 * B and B * 2.0   
	//##ModelId=3C7561920022
   friend const GMatrix operator* (const GMatrix& mat, int    sclr)  { return (((double)sclr) * mat); }
	//##ModelId=3C7561920026
   friend const GMatrix operator* (const GMatrix& mat, float  sclr)  { return (((double)sclr) * mat); }
	//##ModelId=3C7561920033
   friend const GMatrix operator* (const GMatrix& mat, double sclr)  { return (sclr * mat);           }
	//##ModelId=3C7561920037
   friend const GMatrix operator* (int    sclr, const GMatrix& mat)  { return (((double)sclr) * mat); }
	//##ModelId=3C7561920043
   friend const GMatrix operator* (float  sclr, const GMatrix& mat)  { return (((double)sclr) * mat); }   
	//##ModelId=3C7561920050
   friend const GMatrix operator* (double sclr, const GMatrix& mat);

   // divide GMatrix by a scalar variable: A = B / 2.0
	//##ModelId=3C7561920054
   friend const GMatrix operator/ (const GMatrix& mat, int    sclr)  { return mat / ((double)sclr); }
	//##ModelId=3C756192005F
   friend const GMatrix operator/ (const GMatrix& mat, float  sclr)  { return mat / ((double)sclr); }
	//##ModelId=3C7561920063
   friend const GMatrix operator/ (const GMatrix& mat, double sclr);

   // divide GMatrix by a scalar variable: A = 2.0 / B
	//##ModelId=3C756192006F
   friend const GMatrix operator/ (int    sclr, const GMatrix& mat)  { return ((double)sclr) / mat; }
	//##ModelId=3C7561920073
   friend const GMatrix operator/ (float  sclr, const GMatrix& mat)  { return ((double)sclr) / mat; }
	//##ModelId=3C756192007F
   friend const GMatrix operator/ (double sclr, const GMatrix& mat);


   

public: // GMatrix usage an an array A[][]
   
   // nested class for each row of the GMatrix
	//##ModelId=3C75619200AD
   class RowArray
   {
   public:
      // set an element value (allows write access)
		//##ModelId=3C75619200BD
      double& operator [] (int col)
      { 
         CheckIndex( col, ncol );
         return rp[col]; 
      }
      
      // retrieve an element value (read-only)
		//##ModelId=3C75619200BF
      const double& operator [] (int col) const
      { 
         CheckIndex( col, ncol ); 
         return rp[col]; 
      }

      friend class GMatrix;
      
   private:
      // constructor
		//##ModelId=3C75619200CE
      RowArray( double* row, int ncol_ ) 
      : rp( row ), 
        ncol( ncol_ ) 
      {}

		//##ModelId=3C75619200D1
      double *rp; // row pointer
		//##ModelId=3C75619200DC
      int ncol;   // no. of columns
   };
   
	//##ModelId=3C756190006F
   const RowArray operator[] (int row) const;        // retrieve an element value (read-only)
	//##ModelId=3C756190007E
   RowArray operator[] (int row);                    // set an element value (allows write access)

   
public: // friend classes

   friend class RowArray;
   friend class MatrixPlot;
   

public: // static functions

   // function to round a double value
	//##ModelId=3C7561900080
   static double RoundVal(double value, double resolution);

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
   static void TimeMatchFields( GMatrix &A, 
                                const int FieldA, // zero based column index
                                GMatrix &B, 
                                const int FieldB, // zero based column index
                                const double precision, 
                                const double Rollover = 604800.0,
                                const double RollAllowance = 14400 );
   
   // display error and exit
	//##ModelId=3C75619000AD
   static void MatrixError( const char* error );
	//##ModelId=3C756190014A
   static void MatrixError( const char* function, const char* error );

   // display a message, (does not stop execution)
	//##ModelId=3C75619001E6
   static void MatrixMessage( const char* function, const char* message );

private: 
   // private static function for bounds checking
	//##ModelId=3C7561900282
   static void CheckIndex(int index, int maxIndex);

private:
   // private helper functions
	//##ModelId=3C756190032E
   void matAlloc( const int nrows, const int ncols );  // allocate memory
	//##ModelId=3C7561910001
   void matDealloc( double** data );                   // deallocate memory
	//##ModelId=3C75619100AD
   void deepCopy( const GMatrix& mat );                 // copy element-by-element

	//##ModelId=3C7561910169
   void factorize( bool &IsFullRank, const int n, int* index, GMatrix &A ) const;
	//##ModelId=3C7561910224
   void solveByGaussianElimination( const GMatrix &b, 
                                    GMatrix &X, 
                                    const GMatrix &FactorizedA, 
                                    int *index ) const;

	//##ModelId=3C7561910263
   int advanceBuffer( char *linemark, const char delimiter );

   // prints the valud into ValueBuffer with width_and precision_
	//##ModelId=3C7561910272
   void valueToBuffer( const double value, 
                       char *ValueBuffer, 
                       const int width, 
                       const int precision ) const;

   
   // functions used in the Sort() fuction
   // just sorts a single column GMatrix (n,1)
	//##ModelId=3C75619102C1
   void quickSortMat(GMatrix &a, int start, int end);
	//##ModelId=3C75619102D0
   void swapMat_doubles(double& i, double& j);
	//##ModelId=3C75619102E0
   int  partitionMat(GMatrix &a, int start, int end);
   
   // functions used in the SortByColumn() function
   // sorts the column GMatrix (n,1) and an index GMatrix
	//##ModelId=3C75619102EF
   void quickSortMat(GMatrix &a, GMatrix &index, int start, int end);
	//##ModelId=3C756191030F
   void swapMat_doubles(double& i, double& j, double &index_a, double &index_b);
	//##ModelId=3C756191031E
   int  partitionMat(GMatrix &a, GMatrix &index, int start, int end);   
   

private:
   // member variables
	//##ModelId=3C756191032E
   int m_rows;        // no. of rows
	//##ModelId=3C756191032F
   int m_cols;        // no. of cols
	//##ModelId=3C756191033E
   double** m_data;   // the GMatrix data

};


//##ModelId=3C756192014A
class MatrixPlot
{
public: // constructor / destructor
	//##ModelId=3C756192014B
   MatrixPlot();
	//##ModelId=3C7561920159
   virtual ~MatrixPlot();

	//##ModelId=3C756192015B
   void Plot( const GMatrix &X, const GMatrix &Y );

	//##ModelId=3C756192016A
   bool SaveToFile( const char *FileName );


private: // types
	//##ModelId=3C75619201E6
   typedef unsigned char byte;
	//##ModelId=3C75619201F5
   typedef unsigned short word;
	//##ModelId=3C7561920205
   typedef unsigned long dword;

	//##ModelId=3C7561920244
   enum eNumber
   {
		//##ModelId=3C7561920246
      kZero=0,
		//##ModelId=3C7561920247
      kOne,
		//##ModelId=3C7561920253
      kTwo,
		//##ModelId=3C7561920254
      kThree,
		//##ModelId=3C7561920255
      kFour, 
		//##ModelId=3C7561920256
      kFive,
		//##ModelId=3C7561920263
      kSix,
		//##ModelId=3C7561920264
      kSeven,
		//##ModelId=3C7561920265
      kEight,
		//##ModelId=3C7561920266
      kNine,
		//##ModelId=3C7561920272
      kDot,
		//##ModelId=3C7561920273
      kMinus,
		//##ModelId=3C7561920274
      kExp
   };
   

private: // drawing functions    
	//##ModelId=3C756192016C
   void DrawLine( byte color, const int x, const int y, const int to_x, const int to_y );
	//##ModelId=3C756192017D
   void DrawValue( const double value, const int left, const int top );
	//##ModelId=3C756192018A
   void DrawNumber( eNumber Number, const int x, const int y );
	//##ModelId=3C756192018E
   void DrawPoint( byte color, const int x, const int y );

private: // member vars     

	//##ModelId=3C756192019C
   GMatrix PlotData;      

	//##ModelId=3C75619201A7
   const int kImageHeight;
	//##ModelId=3C75619201A8
   const int kImageWidth;
	//##ModelId=3C75619201A9
   const int kPlotHeight;
	//##ModelId=3C75619201B7
   const int kPlotWidth;
	//##ModelId=3C75619201B9
   const byte kWhite;
	//##ModelId=3C75619201C8
   const byte kBlack; 
	//##ModelId=3C75619201CD
   const byte kBlue;

	//##ModelId=3C7561920292
   struct stBitmapFileHeader
   {
		//##ModelId=3C75619202A2
      word  Type;         // must be BM
		//##ModelId=3C75619202A7
      dword Size;         // size in bytes of the file
		//##ModelId=3C75619202B2
      word  Reserved1;    // 0
		//##ModelId=3C75619202C2
      word  Reserved2;    // 0
		//##ModelId=3C75619202C7
      dword Offsetbits;   // offset in bytes from BitmapFileHeader to bitmap bits
   } BitmapFileHeader;

	//##ModelId=3C75619202E0
   struct stRGB
   {
		//##ModelId=3C75619202F1
      byte Blue;
		//##ModelId=3C7561920300
      byte Green;
		//##ModelId=3C7561920305
      byte Red;
		//##ModelId=3C7561920310
      byte Reserved;
   };

	//##ModelId=3C756192032E
   struct stColorTable
   {
		//##ModelId=3C7561920331
      stRGB RGBWhite;
		//##ModelId=3C7561920340
      stRGB RGBBlack;
		//##ModelId=3C756192034E
      stRGB RGBBlue;      
   };   
   
	//##ModelId=3C756192037C
   struct stBitmapInfoHeader
   {
		//##ModelId=3C756192038E
      dword      Size;
		//##ModelId=3C756192039B
      long       Width;
		//##ModelId=3C756192039C
      long       Height;
		//##ModelId=3C756192039E
      word       Planes;
		//##ModelId=3C75619203AC
      word       BitCount;
		//##ModelId=3C75619203BC
      dword      Compression;
		//##ModelId=3C75619203C1
      dword      SizeImage;
		//##ModelId=3C75619203CA
      long       XPelsPerMeter;
		//##ModelId=3C75619203CB
      long       YPelsPerMeter;
		//##ModelId=3C75619203DB
      dword      ClrUsed;
		//##ModelId=3C7561930002
      dword      ClrImportant;
		//##ModelId=3C7561930007
      stColorTable ColorTable;
   } BitmapInfoHeader;

}; // end MatrixPlot class



#endif //MATRIX_GDM_H
