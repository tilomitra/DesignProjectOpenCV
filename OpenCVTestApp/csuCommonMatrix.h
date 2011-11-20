/*
Copyright (c) 2003 Colorado State University

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom
the Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef MATRIX_INCLUDED
#define MATRIX_INCLUDED

#include "csuCommon.h"

/*! \brief Basic structure used to represent a matrix */
typedef struct {
    int row_dim;  /* Number of rows    */
    int col_dim;  /* Number of columns */
    FTYPE *data;    /* Data stored in standard form: column major */
    FTYPE **cols;
}
matrix;

typedef matrix* Matrix;

/*! \brief creates and allocates memory for a matrix */
Matrix makeMatrix(int row_dim, int col_dim);

/*! \brief frees the area allocated to a matrix      */
void freeMatrix(Matrix A);

/*! \brief creates, allocates, and initailizes memory for a matrix */
Matrix makeZeroMatrix(int row_dim, int col_dim);

/*! \brief creates and allocates memory for an identity matrix */
Matrix makeIdentityMatrix(int dim);           /* creates and allocates memory for an identity matrix */

/*! \brief ME (Matrix Element) is used to access a matrix element:
*           implements dope vector expansion
*           Remember that a matrix is stored by columns.
*
*  There are two versions of ME avaible and they are toggled with the
*  range check definition below (MATRIX_RANGE_CHECK).  When MATRIX_RANGE_CHECK
*  is enabled it will cause a function to be called that will check that the
*  element to be accessed is in the correct range (determined by ->row_dim
*  and ->col_dim) if it is not a nice error message will be printed indicating
*  file, function and line number where the error occurred and the program will
*  exit.  Range checking is great for debugging however it is also rather expensive
*  (function call and compares) especially within the nested loops of a matrix multiply.
*
*     Matrix A; index i,j
*/

/* #define MATRIX_RANGE_CHECK */
#ifdef MATRIX_RANGE_CHECK
#define  ME( mat , i , j )  ( ( rangeCheck(mat,i,j,__FILE__,__FUNCTION__,__LINE__,#mat))->cols[j][i] )
#else
#define  ME( mat , i , j )  ( (mat)->cols[j][i] )
#endif

/*! \brief fucntion that confirms an element is within the matrix. \sa ME */
Matrix rangeCheck(Matrix mat, int i, int j, const char* file_name, const char* func_name, int line_num, const char* mat_name);

/*! \brief creates a new matrix that is the product of A and B */
Matrix multiplyMatrix (const Matrix A, const Matrix B);
/*! \brief creates a new matrix that is the product of (A transpose) and B */
Matrix transposeMultiplyMatrixL (const Matrix A, const Matrix B);
/*! \brief creates a new matrix that is the product of A and (B transpose) */
Matrix transposeMultiplyMatrixR (const Matrix A, const Matrix B);

/*! \brief A - B */
Matrix subtractMatrix (const Matrix A, const Matrix B);
/*! \brief A + B */
Matrix addMatrix (const Matrix A, const Matrix B);
/*! \brief A += B */
void addMatrixEquals ( Matrix A, const Matrix B);

/*! \brief creates a new matrix that is the transpose of A     */
Matrix transposeMatrix(const Matrix A);

/*! \brief invert a matrix */
Matrix invertRREF(Matrix);

/*! \brief solve a least squares problem - This function returns the solution vector a */
Matrix leastSquares(Matrix A, Matrix b);
/*! \brief solve a weighted least squares problem - This function returns the solution vector a */
Matrix weightedLeastSquares(Matrix A, Matrix b, Matrix W);

/*! \brief creates a matrix of consisting of columns col1 to col2 of matrix mat */
Matrix matrixCols(const Matrix mat, int col1, int col2);

/*! \brief alocates and fills a duplicate matrix */
Matrix duplicateMatrix(const Matrix mat);

typedef enum {
  octaveFormat = 0x00,
  matlabFormat = 0x01,
  appendToFile = 0x02,

  formatMask   = 0x01
} MatrixSaveMode;

/*! \brief set to zero for octive format otherwize use matlab */
extern MatrixSaveMode asciiFormat;

/*! \brief Macro used to save a matrix */
#define  SAVE_MATRIX( mat )  saveMatrixAscii(NULL, #mat, mat, asciiFormat)

/*! \brief saves a matrix to an octive file */
void saveMatrixAscii(const char *fname, const char* label, const Matrix m, MatrixSaveMode format);

/*! \brief return a matrix of random elements (from 0.0 to 1.0) */
Matrix makeRandomMatrix (int row_dim, int col_dim);

/*! \brief return the sum of the squares of the matrix elements */
FTYPE matrixSumOfSquares (Matrix mat);

/*! \brief simil fucntion that will print a matrix stdout. Used for debuging purposes.  */
void printMatrix(const char* label, Matrix m);

#endif
