#ifndef _MATRIX_TOOLS_H_
#define _MATRIX_TOOLS_H_


/**
 * \file     matrix_tools.h
 * \brief    matrix tools
 * \author   sonia.zaibi\@enit.rnu.edu
 * \version  0.8
 * \date     first release 
 * \date     modification  mohamet.jaafar\@gmail.com 2008
 *
 * This file describe public PGM Picture reader / writer structure and functions.
 *
 * ----------------------------------------
 * Copyright (C) 2004 Mohamed JAAFAR
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
/**
 * \defgroup JPEG JPEG
 * \{ */

/**
 * \defgroup JPEG_CODEC JPEG_CODEC
 * \{ */


/* =================================80======================================= */
/*                               INCLUDES                                     */
/* =================================80======================================= */
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "dctlib.h"
#include "error.h"
/* =================================80======================================= */
/*                               DEFINES                                      */
/* =================================80======================================= */


/* =================================80======================================= */
/*                               FUNCTIONS                                    */
/* =================================80======================================= */

/**
 *
 * \brief   allocates  2D Matrix.
 *
 * \param   int nb_rows                               : number of rows.
 * \param   int nb_cols                               : number of colums.
 * \return  address in memory of the allocated matrix
 * \author  sonia.zaibi\@enit.rnu.edu
 * \date    
 */
 
int** MTOOLS_matrixAllocInt_f(int nb_rows,int nb_cols, int *** matrix);

/**
 *
 * \brief   deallocates  2D Matrix.
 *
 * \param   int** matrix                              : address in memory of the allocated matrix.
 * \return  void
 * \author  sonia.zaibi\@enit.rnu.edu
 * \date    
 */
 
void MTOOLS_matrixFreeInt_f(int** matrix);

/**
 *
 * \brief   allocates  2D Matrix.
 *
 * \param   int nb_rows                               : number of rows.
 * \param   int nb_cols                               : number of colums.
 * \return  address in memory of the allocated matrix
 * \author  sonia.zaibi\@enit.rnu.edu
 * \date    
 */
 
float** MTOOLS_matrixAllocFloat_f(int nb_rows,int nb_cols, float *** matrix);

/**
 *
 * \brief   deallocates  2D Matrix.
 *
 * \param   float** matrix                              : address in memory of the allocated matrix.
 * \return  void
 * \author  sonia.zaibi\@enit.rnu.edu
 * \date    
 */
 
void MTOOLS_matrixFreeFloat_f(float** matrix);
/**
 *
 * \brief   shows  2D Matrix.
 *
 * \param   int** matrix                              : matrix to show.
 * \param   int col                                   : number of columns.
 * \param   int row                                   : matrix of rows.
 * \param   unsigned int                              : the width of the line to show on the screen.
 * \return  void
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */
 
void MTOOLS_matrixShowInt_f(int **matrix,int rows,int cols, unsigned int lineWidth);

/**
 *
 * \brief   shows  2D Matrix.
 *
 * \param   float** matrix                            : matrix to show.
 * \param   int col                                   : number of columns.
 * \param   int row                                   : matrix of rows.
 * \param   unsigned int                              : the width of the line to show on the screen.
 * \return  void
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */
 
void MTOOLS_matrixShowFloat_f(float **matrix,int rows,int cols, unsigned int lineWidth);


/**
 *
 * \brief   transpose Mtrix
 *
 * \param   float** inputMatrix                       : input matrix.
 * \param   float** outputMatrix                      : output matrix.
 * \param   int col                                   : number of columns.
 * \param   int row                                   : matrix of rows.
 * \return  void
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */

void MTOOLS_matrixTransposer(float **inputMatrix,float **outputMatrix,int rows,int cols);


/**
 *
 * \brief   transpose Matrix
 *
 * \param   float** inputMatrix_1                     : input matrix 1.
 * \param   float** inputMatrix_2                     : input matrix 2.
 * \param   float** outputMatrix                      : output matrix.
 * \param   int col                                   : number of columns.
 * \param   int row                                   : matrix of rows.
 * \return  void
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */

void MTOOLS_multiplyMatrix(float **inputMatrix_1,float **inputMatrix_2,float **outputMatrix,int rows,int cols);


/**
 *
 * \brief   transpose Matrix
 *
 * \param   float** inputMatrix_1                     : input matrix 1.
 * \param   float** inputMatrix_2                     : input matrix 2.
 * \param   float** outputMatrix                      : output matrix.
 * \param   int col                                   : number of columns.
 * \param   int row                                   : matrix of rows.
 * \return  void
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */
 
void MTOOLS_multiplyMatrixZeros(float **inputMatrix_1,float **inputMatrix_2,float **outputMatrix,int rows,int cols, int zeroRow, int zeroCol);


/**
 *
 * \brief   converts Matrix from int to float
 *
 * \param   int** matrixInt                       : input matrix.
 * \param   float** matrixFloat                   : output matrix.
 * \param   int cols                              : number of columns.
 * \param   int rows                              : matrix of rows.
 * \return  void
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */

void MTOOLS_matrixConvInt2Float(int** matrixInt,float** matrixFloat,int rows,int cols);

/**
 *
 * \brief   converts Matrix from float to int
 *
 * \param   int** matrixFloat                     : input matrix.
 * \param   float** matrixInt                     : output matrix.
 * \param   int cols                              : number of columns.
 * \param   int rows                              : matrix of rows.
 * \return  void
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */
 
void MTOOLS_matrixConvFloat2Int(float** matrixFloat,int** matrixInt,int rows,int cols);


/**
 *
 * \brief   transpose Mtrix
 *
 * \param   int** inputMatrix                         : input matrix.
 * \param   int** outputMatrix                        : output matrix.
 * \param   int cols                                  : number of columns.
 * \param   int rows                                  : matrix of rows.
 * \param   int zeroCols                              : zero of columns(input).
 * \param   int zeroRow                               : zero of rows(input).
 * \return  void
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */
void MTOOLS_matrixCopyInt1(int** inputMatrix,int** outputMatrix,int rows,int cols,int zeroRowsInput,int zeroColsOutput);


/**
 *
 * \brief   transpose Matrix
 *
 * \param   int** inputMatrix                         : input matrix.
 * \param   int** outputMatrix                        : output matrix.
 * \param   int cols                                  : number of columns.
 * \param   int rows                                  : matrix of rows.
 * \param   int zeroCols                              : zero of columns(output).
 * \param   int zeroRow                               : zero of rows(output).
 * \return  void
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */
 
void MTOOLS_matrixCopyInt2(int** inputMatrix,int** outputMatrix,int rows,int cols,int zeroRowsOutput,int zeroColsOutput);

/**
 *
 * \brief   zigzag matrix collector
 *
 * \param   int** inputMatrix                         : input matrix.
 * \param   int matrixWidth                           : matrix width.
 * \param   int* outputStream                         : output tabular.
 * \return  void
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */
 
void MTOOLS_zigzagMatrixCollector(int** inputMatrix, int matrixWidth, int* outputStream);

// JPEG_CODEC
/** \} */
// JPEG
/** \} */

#endif // < _MATRIX_TOOLS_H_
