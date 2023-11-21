/*****************************************************************************
 * main.c:
 *****************************************************************************
 * Copyright (C) 2004 Mohamed JAAFAR
 * $Id$
 *
 * Authors: Mohamed Jaafar <mohamet.jaafar@gmail.com>
 *          
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
 *****************************************************************************
 *
 *                               motherTASK(main)
 *                                       |
 *                                       |
 *                                       |>   dctTASK
 *                                       |
 *                                       |
 *                                       |>   idctTASK   
 *                                       |
 *                                       |
 *                                       |>   PGM PIC from dct
 *                                       |
 *                                       |>   PGM PIC from idct
 *                                       |
 *                                       |
 *                                       |END
 *
 *
/* =================================80======================================= */
/*                               INCLUDES                                     */
/* =================================80======================================= */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "dctlib.h"
#include "error.h"
#ifdef _USE_PGM
#include "pgm_reader.h"
#endif /*_USE_PGM*/
#ifdef _USE_MATRIX_TOOLS
#include "matrix_tools.h"
#endif /*_USE_MATRIX_TOOLS*/
#ifdef _USE_JOBS
#include "job_tools.h"
#endif /*_USE_JOBS*/
#ifdef _USE_DCT
#include "dct_tools.h"
#endif /*_USE_DCT*/
/* =================================80======================================= */
/*                               MACRO's                                      */
/* =================================80======================================= */

/* =================================80======================================= */
/*                               TYPEDEF & STRUCTURES & ENUMS                 */
/* =================================80======================================= */

/* =================================80======================================= */
/*                               VARIABLES                                    */
/* =================================80======================================= */
#ifdef _USE_JOBS
JOB_msgQueue_ts dctMsgQueue = { 
	.trigger = PTHREAD_COND_INITIALIZER,
       	.locker   = PTHREAD_MUTEX_INITIALIZER
}; 
JOB_msgQueue_ts iDctMsgQueue = {
	.trigger = PTHREAD_COND_INITIALIZER,
       	.locker   = PTHREAD_MUTEX_INITIALIZER
}; 
#endif /*_USE_JOBS*/
/* =================================80======================================= */
/*                               FUNCTIONS                                    */
/* =================================80======================================= */

int main(int argc, char** argv)
{

   int ret;
   int **pictureMatrix = NULL;
   int col;
   int row;

#ifdef _USE_PGM   
   char picturePath[PGM_MAX_PATH_LENGTH];
   
   if(picturePath,argv[1] != NULL)
   {   
      sprintf(picturePath,argv[1]);
   }
   else
   {
      sprintf(picturePath,"lenna.pgm");
   }
      
   pictureMatrix = (int**)PGM_readPicture(picturePath,&row, &col);
#endif /*_USE_PGM*/

#if (defined _USE_DCT) && (defined _USE_MATRIX_TOOLS)
#ifdef _USE_JOBS   
   int z_ret;
   
   /* DCT */
   
   int** ffDct = NULL;
   MTOOLS_matrixAllocInt_f(row, col,&ffDct);
   
   pthread_t forwardDctTaskId;
   int taskPriority = DCT_FFDCT_PRIORITY;
   DCT_data_ts forwardDctTaskData;
   
   /* dct msg queue creation*/
   z_ret = JTOOLS_msgQueueInit(&dctMsgQueue);
   
   z_ret = JTOOLS_createJob(&forwardDctTaskId, taskPriority , (void*)&DCT_forwardDct_f,
                                (void*)&forwardDctTaskData);
   if (0 != z_ret)
   {
       printf(" ERROR when creating job !! %d \n", z_ret);
       return -1;
   }
      
   int blockIndexI = 0;
   int blockIndexJ = 0;
   
   forwardDctTaskData.inputPictureMatrix  = pictureMatrix;
   forwardDctTaskData.outputPictureMatrix = ffDct;
   forwardDctTaskData.col                 = row;
   forwardDctTaskData.row                 = col;
   
   for (blockIndexI = 0; blockIndexI<(row/DCT_8_X_8_BLOCK); blockIndexI++)
   for (blockIndexJ = 0; blockIndexJ<(col/DCT_8_X_8_BLOCK); blockIndexJ++) 
   {
      JTOOLS_msgQueuePush(&dctMsgQueue, (long) DCT_JOB_COMPUTE, (long) blockIndexI, (long) blockIndexJ);
      //DCT_forwardDct_f(&forwardDctTaskData);
   usleep(100);
   }
   
   JTOOLS_msgQueuePush(&dctMsgQueue, (long) JOB_EXIT, (long) blockIndexI, (long) blockIndexJ);
   JTOOLS_exitJob(&forwardDctTaskId);
   /* IDCT */
#if 1 
   int** iDct = NULL;
   MTOOLS_matrixAllocInt_f(row, col, &iDct);
   
   pthread_t iDctTaskId;
   
   DCT_data_ts iDctTaskData;
   
   taskPriority = DCT_IDCT_PRIORITY;
   /* idct msg queue creation*/
   z_ret = JTOOLS_msgQueueInit(&iDctMsgQueue);
   
   z_ret = JTOOLS_createJob(&iDctTaskId, taskPriority, (void*)&DCT_iDct_f,
                            (void*)&iDctTaskData);
   
   iDctTaskData.inputPictureMatrix  = forwardDctTaskData.outputPictureMatrix;
   iDctTaskData.outputPictureMatrix = iDct;
   iDctTaskData.col                 = row;
   iDctTaskData.row                 = col;
   
   for (blockIndexI = 0; blockIndexI<(row/DCT_8_X_8_BLOCK); blockIndexI++)
   for (blockIndexJ = 0; blockIndexJ<(col/DCT_8_X_8_BLOCK); blockIndexJ++) 
   {
      JTOOLS_msgQueuePush(&iDctMsgQueue, (long) DCT_JOB_COMPUTE, (long) blockIndexI, (long) blockIndexJ);
      //DCT_iDct_f(&forwardDctTaskData);
	      
   usleep(100);
   }
         
  
   JTOOLS_msgQueuePush(&iDctMsgQueue, (long) JOB_EXIT, (long) blockIndexI, (long) blockIndexJ);   
   JTOOLS_exitJob(&iDctTaskId);
#endif   

#endif /*_USE_JOBS*/   
#ifdef _USE_PGM

#ifdef _USE_MATRIX_TOOLS
   int **zigzag8X8 = NULL;
   MTOOLS_matrixAllocInt_f((DCT_8_X_8_BLOCK + DCT_8_X_8_BLOCK),(DCT_8_X_8_BLOCK + DCT_8_X_8_BLOCK), &zigzag8X8);
   
   MTOOLS_zigzagMatrixCollector(zigzag8X8, DCT_8_X_8_BLOCK, NULL);
   
   MTOOLS_matrixShowInt_f(zigzag8X8, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
#endif /*_USE_MATRIX_TOOLS*/
   
   sprintf(picturePath,"lenna_dct.pgm");
   PGM_writePicture(picturePath, forwardDctTaskData.outputPictureMatrix, row, col);
   
   sprintf(picturePath,"lenna_idct.pgm");
   PGM_writePicture(picturePath, iDctTaskData.outputPictureMatrix, row, col);
#endif /*_USE_PGM*/
   MTOOLS_matrixFreeInt_f(zigzag8X8); // TBC : FREE IS NOT TOO SECURED
#endif /* (defined _USE_DCT) && (defined _USE_MATRIX_TOOLS) */

#ifdef _USE_MATRIX_TOOLS
//   MTOOLS_matrixFreeInt_f(pictureMatrix); // TBC : FREE IS NOT TOO SECURED
#endif /*_USE_MATRIX_TOOLS*/

   return OK;
}
