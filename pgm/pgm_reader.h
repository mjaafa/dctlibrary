#ifndef _PGM_READER_H_
#define _PGM_READER_H_

/**
 * \file     pgm_reader.h
 * \brief    pgm picture reader / writer
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
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "dctlib.h"
#include "error.h"
/* =================================80======================================= */
/*                               DEFINES                                      */
/* =================================80======================================= */

/** \brief  Max path length                                                   */
#define PGM_MAX_PATH_LENGTH (100)

/* =================================80======================================= */
/*                               FUNCTIONS                                    */
/* =================================80======================================= */

/**
 *
 * \brief   read pgm picture.
 *
 * \param   char *pathPicture                            : path of the pgm picture to read.
 * \param   int *row                                     : output number of rows.
 * \param   int *col                                     : output number of columns. 
 * \return  pointer on the allocated matrix in the heap
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */


int PGM_readPicture(char* pathPicture, int *row, int *col);

/**
 *
 * \brief   write pgm picture.
 *
 * \param   char *pathPicture                            : path of the pgm picture to write.
 * \param   int **pictureMatrix                          : matrix holding pixel values.
 * \param   int row                                      : input number of rows.
 * \param   int col                                      : input number of columns. 
 * \return  error code
 * \author  mohamet.jaafar\@gmail.com
 * \date    
 */
                    
       

int PGM_writePicture(char* pathPicture, int **pictureMatrix, int row, int col);


// JPEG_CODEC
/** \} */
// JPEG
/** \} */

#endif // < _PGM_READER_H_
