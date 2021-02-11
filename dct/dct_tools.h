#ifndef _DCT_TOOLS_H_
#define _DCT_TOOLS_H_

/**
 * \file     dct_tools.h
 * \brief    dct / idct
 * \author   mohamet.jaafar\@gmail.com
 * \version  0.8
 * \date     first release 
 * \date     modification  mohamet.jaafar\@gmail.com 2008
 *
 * This file describe public DCT /IDCT structure and functions.
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
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "dctlib.h"
/* =================================80======================================= */
/*                               DEFINES                                      */
/* =================================80======================================= */

/** \brief  dct macroblock 8*8                                                */
#define DCT_8_X_8_BLOCK            (8)
/** \brief  dct quality                                                       */
#define DCT_QUALITY                (1)
/** \brief  first pixel in the 8*8 quality                                    */
#define DCT_FIRST_PIX_QUALITY      (1)
/** \brief  coefficients definition (dct matrix)                              */
/** \brief  first coefficient                                                 */
#define DCT_1__SQRT2              (0.70710678)   //(0.7071067811865475727)
/** \brief  second coefficient                                                */
#define DCT_PI__16                (0.98078528)   //(0.9807852804032304306)
/** \brief  third coefficient                                                 */
#define DCT_2PI__16               (0.92387953)   //(0.9238795325112867385)
/** \brief  forth coefficient                                                 */
#define DCT_3PI__16               (0.83146961)   //(0.8314696123025452357)
/** \brief  fifth coefficient                                                 */
#define DCT_4PI__16               (0.70710678)   //(0.7071067811865475727)
/** \brief  sixth coefficient                                                 */
#define DCT_5PI__16               (0.55557023)   //(0.5555702330196022887)
/** \brief  seventh coefficient                                               */
#define DCT_6PI__16               (0.38268343)   //(0.3826834323650898373)
/** \brief  eigth coefficient                                                 */
#define DCT_7PI__16               (0.19509032)   //(0.1950903220161283314)
#ifdef _USE_JOBS
/** \brief  dct task priority                                                 */
#define DCT_FFDCT_PRIORITY        (99)
/** \brief  idct task priority                                                 */
#define DCT_IDCT_PRIORITY         (99)
#endif /*_USE_JOBS*/
/* =================================80======================================= */
/*                               TYPEDEF                                      */
/* =================================80======================================= */

/**
* \brief  DCT FF structure definition.
*/
typedef struct DCT_data
{
   int** inputPictureMatrix;           /* input matrix */
   int** outputPictureMatrix;          /* output matrix */
   int col;                            /* number of columns*/
   int row;                            /* number of rows*/
}DCT_data_ts;

/** \brief  flag for testing idct                                             */
//#define TEST_IDENTITY           (0)
/** \brief  flag for testing block 8x8 processing                             */
//#define BLOCK_GO                  (1)   
/* =================================80======================================= */
/*                               FUNCTIONS                                    */
/* =================================80======================================= */

/**
 *
 * \brief   forward dct.
 *
 * \param   void *params                        : argument to pass (DCT_data_ts).
 * \return  dct job id
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */

void * DCT_forwardDct_f(void* params);

/**
 *
 * \brief   initialize the quantum matrix DCT_8_X_8_BLOCK.
 *
 * \param   int **quantumMatrix                 : the quantum matrix.
 * \return  
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */

void DCT_quantumMatrixInit_f(int** quantumMatrix);


/**
 *
 * \brief   initialize the dct matrix.
 *
 * \param   int **dctMatrix                 : the dct matrix.
 * \return  
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */

void DCT_transfomMatrixInit_f(float **dctMatrix);      


/**
 *
 * \brief   quantify Matrix DCT_8_X_8_BLOCK.
 *
 * \param   int** quantifiedMatrix                    : output matrix.
 * \param   float** inputMatrix                       : input matrix.
 * \param   int** quantumMatrix                       : quantum matrix.
 * \return  void
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */

void DCT_quantifyMatrix_f(int** quantifiedMatrix, float** inputMatrix, int** quantumMatrix);              


/**
 *
 * \brief   dequantify Matrix DCT_8_X_8_BLOCK.
 *
 * \param   int** dequantifiedMatrix                    : output matrix.
 * \param   float** inputMatrix                         : input matrix.
 * \param   int** quantumMatrix                         : quantum matrix.
 * \return  void
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */

void DCT_dequantifyMatrix_f(float** dequantifiedMatrix, int** inputMatrix, int** quantumMatrix);

/**
 *
 * \brief   inverse dct.
 *
 * \param   void *params                        : argument to pass (DCT_data_ts).
 * \return  dct job id
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */

void * DCT_iDct_f(void* params);

// JPEG_CODEC
/** \} */
// JPEG
/** \} */

#endif /*_DCT_TOOLS_H_*/
