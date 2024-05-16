/**
 * \file     dct_tools.c
 * \brief    dct / idct
 * \author   mohamet.jaafar\@gmail.com
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
#include "dct_tools.h"
#ifdef _USE_JOBS
#include "job_tools.h"
#endif /*_USE_JOBS*/
#include <CL/cl.h>
#include "matrix_tools.h"
/* =================================80======================================= */
/*                               MACRO's                                      */
/* =================================80======================================= */

/* =================================80======================================= */
/*                               TYPEDEF & STRUCTURES & ENUMS                 */
/* =================================80======================================= */
extern MTOOLS_OpenCL_context_t openCLConext;
/* =================================80======================================= */
/*                               VARIABLES                                    */
/* =================================80======================================= */
#ifdef _USE_JOBS
extern JOB_msgQueue_ts dctMsgQueue; // TBC : for test only
extern JOB_msgQueue_ts iDctMsgQueue;
#endif /*_USE_JOBS*/
/* =================================80======================================= */
/*                               FUNCTIONS                                    */
/* =================================80======================================= */

void * DCT_forwardDct_f(void* params)
{
   JOB_msg_ts msg;
   int blockIndexI;
   int blockIndexJ;
   int job;

   cl_mem inputMatrixBuffer_1;
   cl_mem inputMatrixBuffer_2;
   cl_mem outputMatrixBuffer;
   
   /* prepare C matrix of the DCT */
   float** cMatrix  = NULL;
   MTOOLS_matrixAllocFloat_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, &cMatrix);
   DCT_transfomMatrixInit_f(cMatrix);
   
   /* prepare Ctr matrix of the DCT */
   float** ctrMatrix  = NULL;
   MTOOLS_matrixAllocFloat_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, &ctrMatrix);
   MTOOLS_matrixTransposer((int**)cMatrix,(int**)ctrMatrix, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
 
   /* prepare quantum matrix */
   int** quantumMatrix  = NULL;
   MTOOLS_matrixAllocInt_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, &quantumMatrix);
   DCT_quantumMatrixInit_f(quantumMatrix);
   
   int **block8X8;// = NULL;
   MTOOLS_matrixAllocInt_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, &block8X8);
   
   float **block8X8f;// = NULL;
   MTOOLS_matrixAllocFloat_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, &block8X8f);
   
   float **blockDct8X8Step1; // DCT
   MTOOLS_matrixAllocFloat_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK,&blockDct8X8Step1);
   
   float **blockDct8X8Step2; // DCT
   MTOOLS_matrixAllocFloat_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK,&blockDct8X8Step2);
   
   int** blockDct8X8;// = NULL;
   MTOOLS_matrixAllocInt_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK,&blockDct8X8);
   
   MTOOLS_matrixShowFloat_f(cMatrix,DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
   //OpenCL
  // MTOOLS_initOpenCL();
  

   while(1)
   {
      JTOOLS_msgQueueWait(&dctMsgQueue, &msg);
      DCT_data_ts* data = (DCT_data_ts*)params;
      
      job         = msg.msgId;
      blockIndexI = (int)msg.data1;
      blockIndexJ = (int)msg.data2;
     //printf(" I = %d ; J = %d \n", blockIndexI, blockIndexJ); 
	 //printf("JOB = %d " , job);
            
      switch(job)
      {
         case DCT_JOB_COMPUTE :
         {
#ifdef _USE_MATRIX_TOOLS 

            /*if(data->inputPictureMatrix == NULL)
            return;*/
            MTOOLS_matrixCopyInt1(data->inputPictureMatrix, block8X8,DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK,
                                  (blockIndexI*DCT_8_X_8_BLOCK), (blockIndexJ*DCT_8_X_8_BLOCK));
                                   
            MTOOLS_matrixConvInt2Float(block8X8, block8X8f, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
#if 1            
cl_int clError;
inputMatrixBuffer_1 = clCreateBuffer(openCLConext.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * DCT_8_X_8_BLOCK * DCT_8_X_8_BLOCK, data->inputPictureMatrix, &clError);
inputMatrixBuffer_2 = clCreateBuffer(openCLConext.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * DCT_8_X_8_BLOCK * DCT_8_X_8_BLOCK, cMatrix, &clError);
outputMatrixBuffer = clCreateBuffer(openCLConext.context, CL_MEM_WRITE_ONLY, sizeof(float) * DCT_8_X_8_BLOCK * DCT_8_X_8_BLOCK, NULL, &clError);

// Check for buffer creation errors
if (!inputMatrixBuffer_1 || !inputMatrixBuffer_2 || !outputMatrixBuffer || clError != CL_SUCCESS) {
    printf("Error: Failed to create OpenCL buffers. Error code: %d\n", clError);
    // Handle error (e.g., return from function or exit program)
}

// Set kernel arguments
clSetKernelArg(openCLConext.kernelOperations[MTOOLS_KERNEL_MULTIPLY_MATRIX], 0, sizeof(cl_mem), (void*)&inputMatrixBuffer_1);
clSetKernelArg(openCLConext.kernelOperations[MTOOLS_KERNEL_MULTIPLY_MATRIX], 1, sizeof(cl_mem), (void*)&inputMatrixBuffer_2);
clSetKernelArg(openCLConext.kernelOperations[MTOOLS_KERNEL_MULTIPLY_MATRIX], 2, sizeof(cl_mem), (void*)&outputMatrixBuffer);
clSetKernelArg(openCLConext.kernelOperations[MTOOLS_KERNEL_MULTIPLY_MATRIX], 3, sizeof(int), (void*)DCT_8_X_8_BLOCK);
clSetKernelArg(openCLConext.kernelOperations[MTOOLS_KERNEL_MULTIPLY_MATRIX], 4, sizeof(int), (void*)DCT_8_X_8_BLOCK);

// Execute the kernel
size_t globalItemSize = DCT_8_X_8_BLOCK;
cl_int kernelError = clEnqueueNDRangeKernel(openCLConext.queue, openCLConext.kernelOperations[MTOOLS_KERNEL_MULTIPLY_MATRIX], 2, NULL, &globalItemSize, NULL, 0, NULL, NULL);

// Check for kernel execution errors
if (kernelError != CL_SUCCESS) {
    printf("Error: Failed to execute OpenCL kernel. Error code: %d\n", kernelError);
    // Handle error (e.g., return from function or exit program)
}

// Read the result from the device
cl_int readBufferError = clEnqueueReadBuffer(openCLConext.queue, outputMatrixBuffer, CL_TRUE, 0, sizeof(float) * DCT_8_X_8_BLOCK * DCT_8_X_8_BLOCK, blockDct8X8Step1, 0, NULL, NULL);

// Check for buffer read errors
if (readBufferError != CL_SUCCESS) {
    printf("Error: Failed to read buffer from device. Error code: %d\n", readBufferError);
    // Handle error (e.g., return from function or exit program)
}

// Cleanup OpenCL buffers
clReleaseMemObject(inputMatrixBuffer_1);
clReleaseMemObject(inputMatrixBuffer_2);
clReleaseMemObject(outputMatrixBuffer);
#endif

            //MTOOLS_multiplyMatrix((float**)block8X8f, cMatrix, blockDct8X8Step1, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
            //MTOOLS_matrixShowFloat_f(blockDct8X8Step1,DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
           
            MTOOLS_multiplyMatrix(ctrMatrix, blockDct8X8Step1, blockDct8X8Step2, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
      
            DCT_quantifyMatrix_f(blockDct8X8, blockDct8X8Step2, quantumMatrix);
                       
            MTOOLS_matrixCopyInt2(blockDct8X8,data->outputPictureMatrix, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK,
                                       (blockIndexI * DCT_8_X_8_BLOCK), (blockIndexJ * DCT_8_X_8_BLOCK));
     //       MTOOLS_matrixShowInt_f(blockDct8X8,DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
            
#endif /*_USE_MATRIX_TOOLS*/
         break;
         }
         
         case JOB_EXIT:
         {
   //         MTOOLS_matrixFreeInt_f(quantumMatrix); // TBC : FREE IS NOT TOO SECURED
     ///       MTOOLS_matrixFreeInt_f(block8X8); // TBC : FREE IS NOT TOO SECURED
        //    MTOOLS_matrixFreeFloat_f(block8X8f); // TBC : FREE IS NOT TOO SECURED
   //         MTOOLS_matrixFreeFloat_f(cMatrix); // TBC : FREE IS NOT TOO SECURED
     //       MTOOLS_matrixFreeFloat_f(ctrMatrix); // TBC : FREE IS NOT TOO SECURED
      //      MTOOLS_matrixFreeFloat_f(blockDct8X8Step1); // TBC : FREE IS NOT TOO SECURED
      //      MTOOLS_matrixFreeFloat_f(blockDct8X8Step2); // TBC : FREE IS NOT TOO SECURED
      //      MTOOLS_matrixFreeInt_f(blockDct8X8); // TBC : FREE IS NOT TOO SECURED
            break;
         }
         default:
         {
            break;
         }
      }
   }
#ifdef _USE_JOB
   return (DCT_JOB_ID);
#endif /* _USE_JOB */
}

void * DCT_iDct_f(void* params)
{
   JOB_msg_ts msg;
   int blockIndexI;
   int blockIndexJ;
   int job;
   
   /* prepare C matrix of the DCT */
   float** cMatrix;
   MTOOLS_matrixAllocFloat_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK,&cMatrix);
   DCT_transfomMatrixInit_f(cMatrix);
   MTOOLS_matrixShowFloat_f(cMatrix,DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
   
   /* prepare Ctr matrix of the DCT */
   float** ctrMatrix  = NULL;
   MTOOLS_matrixAllocFloat_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, &ctrMatrix);
   MTOOLS_matrixTransposer((int**)cMatrix,(int**)ctrMatrix, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
 
   /* prepare quantum matrix */
   int** quantumMatrix  = NULL;
   MTOOLS_matrixAllocInt_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK,&quantumMatrix);
   DCT_quantumMatrixInit_f(quantumMatrix);
   
   int **block8X8 = NULL;
   MTOOLS_matrixAllocInt_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, &block8X8);
   
   float **block8X8f = NULL;
   MTOOLS_matrixAllocFloat_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, &block8X8f);
   
   float **blockDct8X8f = NULL;
   MTOOLS_matrixAllocFloat_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK,&blockDct8X8f);
   
   float **blockDct8X8Step1; // DCT
   MTOOLS_matrixAllocFloat_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, &blockDct8X8Step1);
   
   float **blockDct8X8Step2; // DCT
   MTOOLS_matrixAllocFloat_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, &blockDct8X8Step2);
   
   int** blockDct8X8  = NULL;
   MTOOLS_matrixAllocInt_f(DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, &blockDct8X8);
   
   while(1)
   {
      JTOOLS_msgQueueWait(&iDctMsgQueue, &msg);
      DCT_data_ts* data = (DCT_data_ts*)params;
      
      job         = msg.msgId;
      blockIndexI = (int)msg.data1;
      blockIndexJ = (int)msg.data2;
    // printf(" I = %d ; J = %d \n", blockIndexI, blockIndexJ); 
	 //printf("JOB = %d " , job);
            
      switch(job)
      {
         case DCT_JOB_COMPUTE :
         {
#ifdef _USE_MATRIX_TOOLS 
   /*         if(data->inputPictureMatrix == NULL)
            return;*/
            MTOOLS_matrixCopyInt1(data->inputPictureMatrix, blockDct8X8,DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK,
                                  blockIndexI*DCT_8_X_8_BLOCK, blockIndexJ*DCT_8_X_8_BLOCK);
                                   
            DCT_dequantifyMatrix_f(block8X8f, blockDct8X8, quantumMatrix);
                                                        
            MTOOLS_multiplyMatrix(block8X8f, ctrMatrix, blockDct8X8Step2, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
            
            MTOOLS_multiplyMatrix(cMatrix, blockDct8X8Step2, blockDct8X8Step1, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
                                 
            MTOOLS_matrixConvFloat2Int(blockDct8X8Step1, block8X8, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);           
            
            MTOOLS_matrixCopyInt2(block8X8,data->outputPictureMatrix, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK,
                                       (blockIndexI * DCT_8_X_8_BLOCK), (blockIndexJ * DCT_8_X_8_BLOCK));
      //      MTOOLS_matrixShowInt_f(blockDct8X8,DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK, DCT_8_X_8_BLOCK);
            
#endif /*_USE_MATRIX_TOOLS*/
         break;
         }
         
         case JOB_EXIT:
         {
     //       MTOOLS_matrixFreeInt_f(quantumMatrix); // TBC : FREE IS NOT TOO SECURED
       //     MTOOLS_matrixFreeInt_f(block8X8); // TBC : FREE IS NOT TOO SECURED
         //   MTOOLS_matrixFreeFloat_f(block8X8f); // TBC : FREE IS NOT TOO SECURED
 //           MTOOLS_matrixFreeFloat_f(blockDct8X8f); // TBC : FREE IS NOT TOO SECURED
   //         MTOOLS_matrixFreeFloat_f(cMatrix); // TBC : FREE IS NOT TOO SECURED
     //       MTOOLS_matrixFreeFloat_f(ctrMatrix); // TBC : FREE IS NOT TOO SECURED
  //          MTOOLS_matrixFreeFloat_f(blockDct8X8Step1); // TBC : FREE IS NOT TOO SECURED
    //        MTOOLS_matrixFreeFloat_f(blockDct8X8Step2); // TBC : FREE IS NOT TOO SECURED
      //      MTOOLS_matrixFreeInt_f(blockDct8X8); // TBC : FREE IS NOT TOO SECURED
            break;
         }
         default:
         {
            break;
         }
      }
   }
#ifdef _USE_JOB
   return (DCT_JOB_ID);
#endif /* _USE_JOB */
}

void DCT_transfomMatrixInit_f(float **dctMatrix)
{
   int rowIndex,colIndex,phasisU,phasisV;
   float cosinusValues[DCT_8_X_8_BLOCK];

   cosinusValues[0] = DCT_1__SQRT2;
   cosinusValues[1] = DCT_PI__16;
   cosinusValues[2] = DCT_2PI__16;
   cosinusValues[3] = DCT_3PI__16;
   cosinusValues[4] = DCT_4PI__16;
   cosinusValues[5] = DCT_5PI__16;
   cosinusValues[6] = DCT_6PI__16;
   cosinusValues[7] = DCT_7PI__16;
   
   for(rowIndex=0; rowIndex<DCT_8_X_8_BLOCK; rowIndex++) 
   for(colIndex=0; colIndex<DCT_8_X_8_BLOCK; colIndex++) 
   {

      phasisU = ((2*colIndex+1)*rowIndex);
      phasisV = phasisU / DCT_8_X_8_BLOCK;
      
      switch (phasisV % 4)
      {
         case 0:
         {
            dctMatrix[rowIndex][colIndex]=*(cosinusValues + (phasisU % DCT_8_X_8_BLOCK)) / 2;
            break;
         }
      
         case 1:
         {
            dctMatrix[rowIndex][colIndex]=-(*(cosinusValues + DCT_8_X_8_BLOCK - (phasisU%DCT_8_X_8_BLOCK))) / 2;
            break;
         }
      
         case 2: 
         {
            dctMatrix[rowIndex][colIndex]=-(*(cosinusValues + phasisU % DCT_8_X_8_BLOCK)) / 2;
            break;
         }
      
         case 3:
         {
            dctMatrix[rowIndex][colIndex]=*(cosinusValues + DCT_8_X_8_BLOCK - (phasisU % DCT_8_X_8_BLOCK)) / 2;
            break;
         }
      }
   }
}

void DCT_quantumMatrixInit_f(int** quantumMatrix)
{
   int rowIndex;
   int colIndex;
   
   if (quantumMatrix == NULL)
      return;
      
   for (rowIndex=0; rowIndex< DCT_8_X_8_BLOCK; rowIndex++)
   for (colIndex=0; colIndex< DCT_8_X_8_BLOCK; colIndex++) 
   {
        quantumMatrix[rowIndex][colIndex] = DCT_FIRST_PIX_QUALITY + (rowIndex + colIndex) * DCT_QUALITY;
         
   }
}

void DCT_quantifyMatrix_f(int** quantifiedMatrix, float** inputMatrix, int** quantumMatrix)
{
   int rowIndex;
   int colIndex;

   for (rowIndex=0; rowIndex< DCT_8_X_8_BLOCK; rowIndex++)
   for (colIndex=0; colIndex< DCT_8_X_8_BLOCK; colIndex++)
   {
      quantifiedMatrix[rowIndex][colIndex]=(int)((inputMatrix[rowIndex][colIndex])/quantumMatrix[rowIndex][colIndex]);
   }
}

void DCT_dequantifyMatrix_f(float** dequantifiedMatrix, int** inputMatrix, int** quantumMatrix)
{
   int rowIndex;
   int colIndex;
   
   for (rowIndex=0; rowIndex< DCT_8_X_8_BLOCK; rowIndex++)
   for (colIndex=0; colIndex< DCT_8_X_8_BLOCK; colIndex++)
   {
      dequantifiedMatrix[rowIndex][colIndex]=(float)((inputMatrix[rowIndex][colIndex])*quantumMatrix[rowIndex][colIndex]);
   }
}


