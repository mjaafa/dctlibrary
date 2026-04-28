/**
 * \file pgm_reader.c
 * \brief pgm picture read write file
 * \author sonia.zaibi\@enit.rnu.edu
 * \version 0.8
 * \date first release 
 * \date modification  mohamet.jaafar\@gmail.com 2008
 *
 * This file describe an pgm picture read / write.
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
 */

/* =================================80======================================= */
/*                               INCLUDES                                     */
/* =================================80======================================= */

#include "matrix_tools.h"
#ifdef USE_CUDA_MATRIX_TOOLS
#include "cuda_matrix_tools.h"
#endif
#ifdef USE_CUDA_MATRIX_TOOLS
static int MTOOLS_matrixTransposerCuda(float **inputMatrix,float **outputMatrix,int rows,int cols);
#endif
/* =================================80======================================= */
/*                               MACRO's                                      */
/* =================================80======================================= */

/* =================================80======================================= */
/*                               TYPEDEF & STRUCTURES & ENUMS                 */
/* =================================80======================================= */

/* =================================80======================================= */
/*                               VARIABLES                                    */
/* =================================80======================================= */

/* =================================80======================================= */
/*                               FUNCTIONS                                    */
/* =================================80======================================= */

int** MTOOLS_matrixAllocInt_f(int nb_rows,int nb_cols, int *** matrix)
{
   int row;
   
   *matrix = (int**)malloc(sizeof(int *)*nb_rows);
   for (row = 0; row < nb_rows; row++)
   {
      (*matrix)[row] = (int *)malloc(sizeof(int)*(nb_cols));
   }
     
}

void MTOOLS_matrixFreeInt_f(int** matrix)
{
 
   if (matrix != NULL)
   {
      free(matrix);
   }
   else
      return;
}

float** MTOOLS_matrixAllocFloat_f(int nb_rows,int nb_cols, float ***matrix)
{
   int row;
   
   *matrix = (float**)malloc(sizeof(float *)*nb_rows);
   for (row = 0; row < nb_rows; row++)
   {
      (*matrix)[row] = (float *)malloc(sizeof(float)*(nb_cols));
   }
     
}

void MTOOLS_matrixFreeFloat_f(float** matrix)
{
      
   if (matrix != NULL)
   {
      free(matrix);
   }
   else
      return;
}

void MTOOLS_matrixShowInt_f(int **matrix,int rows,int cols, unsigned int lineWidth)
{
   int i,j;
   
   if((matrix == NULL) || (lineWidth < 1))
      return;
   
   printf("\n");
   for (i=0;i<rows;i++) 
   for (j=0;j<cols;j++) 
   {
      printf("%d ",matrix[i][j]);
      if (j == (lineWidth - 1)) printf("\n");
   }
}

void MTOOLS_matrixShowFloat_f(float **matrix,int rows,int cols, unsigned int lineWidth)
{
   int i,j;
   
   if((matrix == NULL) || (lineWidth < 1))
      return;
   
   printf("\n");
   for (i=0;i<rows;i++) 
   for (j=0;j<cols;j++) 
   {
      printf("%f ",matrix[i][j]);
      if (j == (lineWidth - 1)) printf("\n");
   }
}

void MTOOLS_matrixTransposer(float **inputMatrix,float **outputMatrix,int rows,int cols)
{
   int i,j;

#ifdef USE_CUDA_MATRIX_TOOLS
   if (MTOOLS_matrixTransposerCuda(inputMatrix, outputMatrix, rows, cols) == OK)
   {
      return;
   }
#endif

   for(i=0; i<rows; i++) 
   for(j=0; j<cols; j++) 
   {
     outputMatrix[i][j]=inputMatrix[j][i];
   }
}   

#ifdef USE_CUDA_MATRIX_TOOLS
static int MTOOLS_countElements(int rows, int cols, size_t *count)
{
   if ((rows <= 0) || (cols <= 0) || (count == NULL))
   {
      return ERROR_1;
   }

   *count = (size_t)rows * (size_t)cols;
   return OK;
}

static void MTOOLS_flattenFloatMatrix(float **matrix, float *buffer, int rows, int cols)
{
   int rowIndex;
   int colIndex;
   for (rowIndex = 0; rowIndex < rows; ++rowIndex)
   for (colIndex = 0; colIndex < cols; ++colIndex)
   {
      buffer[rowIndex * cols + colIndex] = matrix[rowIndex][colIndex];
   }
}

static void MTOOLS_unflattenFloatMatrix(const float *buffer, float **matrix, int rows, int cols)
{
   int rowIndex;
   int colIndex;
   for (rowIndex = 0; rowIndex < rows; ++rowIndex)
   for (colIndex = 0; colIndex < cols; ++colIndex)
   {
      matrix[rowIndex][colIndex] = buffer[rowIndex * cols + colIndex];
   }
}

static void MTOOLS_flattenIntMatrix(int **matrix, int *buffer, int rows, int cols)
{
   int rowIndex;
   int colIndex;
   for (rowIndex = 0; rowIndex < rows; ++rowIndex)
   for (colIndex = 0; colIndex < cols; ++colIndex)
   {
      buffer[rowIndex * cols + colIndex] = matrix[rowIndex][colIndex];
   }
}

static void MTOOLS_unflattenIntMatrix(const int *buffer, int **matrix, int rows, int cols)
{
   int rowIndex;
   int colIndex;
   for (rowIndex = 0; rowIndex < rows; ++rowIndex)
   for (colIndex = 0; colIndex < cols; ++colIndex)
   {
      matrix[rowIndex][colIndex] = buffer[rowIndex * cols + colIndex];
   }
}

static int MTOOLS_matrixTransposerCuda(float **inputMatrix,float **outputMatrix,int rows,int cols)
{
   float *hostInput;
   float *hostOutput;
   size_t count;
   int ret;

   if ((inputMatrix == NULL) || (outputMatrix == NULL) || (MTOOLS_countElements(rows, cols, &count) != OK))
   {
      return ERROR_1;
   }

   hostInput = (float*)malloc(count * sizeof(float));
   hostOutput = (float*)malloc(count * sizeof(float));
   if ((hostInput == NULL) || (hostOutput == NULL))
   {
      free(hostInput);
      free(hostOutput);
      return ERROR_1;
   }

   MTOOLS_flattenFloatMatrix(inputMatrix, hostInput, rows, cols);
   ret = CUDA_matrixTransposeFloat(hostInput, hostOutput, rows, cols);
   if (ret == OK)
   {
      MTOOLS_unflattenFloatMatrix(hostOutput, outputMatrix, cols, rows);
   }

   free(hostInput);
   free(hostOutput);
   return ret;
}

int MTOOLS_multiplyMatrixCuda(float **inputMatrix_1,float **inputMatrix_2,float **outputMatrix,int rows,int cols)
{
   float *hostInput1;
   float *hostInput2;
   float *hostOutput;
   size_t count;
   int ret;

   if ((rows != cols) || (inputMatrix_1 == NULL) || (inputMatrix_2 == NULL) || (outputMatrix == NULL) ||
       (MTOOLS_countElements(rows, cols, &count) != OK))
   {
      return ERROR_1;
   }

   hostInput1 = (float*)malloc(count * sizeof(float));
   hostInput2 = (float*)malloc(count * sizeof(float));
   hostOutput = (float*)malloc(count * sizeof(float));
   if ((hostInput1 == NULL) || (hostInput2 == NULL) || (hostOutput == NULL))
   {
      free(hostInput1);
      free(hostInput2);
      free(hostOutput);
      return ERROR_1;
   }

   MTOOLS_flattenFloatMatrix(inputMatrix_1, hostInput1, rows, cols);
   MTOOLS_flattenFloatMatrix(inputMatrix_2, hostInput2, rows, cols);
   ret = CUDA_matrixMultiplyFloatSquare(hostInput1, hostInput2, hostOutput, rows);
   if (ret == OK)
   {
      MTOOLS_unflattenFloatMatrix(hostOutput, outputMatrix, rows, cols);
   }

   free(hostInput1);
   free(hostInput2);
   free(hostOutput);
   return ret;
}

static int MTOOLS_multiplyMatrixZerosCuda(float **inputMatrix_1,float **inputMatrix_2,float **outputMatrix,
                                          int rows,int cols, int zeroRow, int zeroCol)
{
   float *hostInput1;
   float *hostInput2;
   float *hostOutput;
   size_t count;
   int i;
   int j;
   int ret;

   if ((rows != cols) || (inputMatrix_1 == NULL) || (inputMatrix_2 == NULL) || (outputMatrix == NULL) ||
       (zeroRow < 0) || (zeroCol < 0) || (MTOOLS_countElements(rows, cols, &count) != OK))
   {
      return ERROR_1;
   }

   hostInput1 = (float*)malloc(count * sizeof(float));
   hostInput2 = (float*)malloc(count * sizeof(float));
   hostOutput = (float*)malloc(count * sizeof(float));
   if ((hostInput1 == NULL) || (hostInput2 == NULL) || (hostOutput == NULL))
   {
      free(hostInput1);
      free(hostInput2);
      free(hostOutput);
      return ERROR_1;
   }

   for (i = 0; i < rows; ++i)
   for (j = 0; j < cols; ++j)
   {
      hostInput1[i * cols + j] = inputMatrix_1[zeroRow + i][zeroCol + j];
   }
   MTOOLS_flattenFloatMatrix(inputMatrix_2, hostInput2, rows, cols);

   ret = CUDA_matrixMultiplyFloatSquare(hostInput1, hostInput2, hostOutput, rows);
   if (ret == OK)
   {
      MTOOLS_unflattenFloatMatrix(hostOutput, outputMatrix, rows, cols);
   }

   free(hostInput1);
   free(hostInput2);
   free(hostOutput);
   return ret;
}

static int MTOOLS_matrixConvInt2FloatCuda(int** matrixInt,float** matrixFloat,int rows,int cols)
{
   int *hostInput;
   float *hostOutput;
   size_t count;
   int ret;

   if ((matrixInt == NULL) || (matrixFloat == NULL) || (MTOOLS_countElements(rows, cols, &count) != OK))
   {
      return ERROR_1;
   }

   hostInput = (int*)malloc(count * sizeof(int));
   hostOutput = (float*)malloc(count * sizeof(float));
   if ((hostInput == NULL) || (hostOutput == NULL))
   {
      free(hostInput);
      free(hostOutput);
      return ERROR_1;
   }

   MTOOLS_flattenIntMatrix(matrixInt, hostInput, rows, cols);
   ret = CUDA_matrixIntToFloat(hostInput, hostOutput, rows, cols);
   if (ret == OK)
   {
      MTOOLS_unflattenFloatMatrix(hostOutput, matrixFloat, rows, cols);
   }

   free(hostInput);
   free(hostOutput);
   return ret;
}

static int MTOOLS_matrixConvFloat2IntCuda(float** matrixFloat,int** matrixInt,int rows,int cols)
{
   float *hostInput;
   int *hostOutput;
   size_t count;
   int ret;

   if ((matrixFloat == NULL) || (matrixInt == NULL) || (MTOOLS_countElements(rows, cols, &count) != OK))
   {
      return ERROR_1;
   }

   hostInput = (float*)malloc(count * sizeof(float));
   hostOutput = (int*)malloc(count * sizeof(int));
   if ((hostInput == NULL) || (hostOutput == NULL))
   {
      free(hostInput);
      free(hostOutput);
      return ERROR_1;
   }

   MTOOLS_flattenFloatMatrix(matrixFloat, hostInput, rows, cols);
   ret = CUDA_matrixFloatToInt(hostInput, hostOutput, rows, cols);
   if (ret == OK)
   {
      MTOOLS_unflattenIntMatrix(hostOutput, matrixInt, rows, cols);
   }

   free(hostInput);
   free(hostOutput);
   return ret;
}

static int MTOOLS_matrixCopyInt1Cuda(int** inputMatrix,int** outputMatrix,int rows,int cols,int zeroRowsInput,int zeroColsInput)
{
   int *hostInput;
   int *hostOutput;
   int inputRows;
   int inputStride;
   size_t inputCount;
   size_t outputCount;
   int ret;

   if ((inputMatrix == NULL) || (outputMatrix == NULL) || (rows <= 0) || (cols <= 0) ||
       (zeroRowsInput < 0) || (zeroColsInput < 0))
   {
      return ERROR_1;
   }

   inputRows = zeroRowsInput + rows;
   inputStride = zeroColsInput + cols;
   inputCount = (size_t)inputRows * (size_t)inputStride;
   outputCount = (size_t)rows * (size_t)cols;

   hostInput = (int*)malloc(inputCount * sizeof(int));
   hostOutput = (int*)malloc(outputCount * sizeof(int));
   if ((hostInput == NULL) || (hostOutput == NULL))
   {
      free(hostInput);
      free(hostOutput);
      return ERROR_1;
   }

   MTOOLS_flattenIntMatrix(inputMatrix, hostInput, inputRows, inputStride);
   ret = CUDA_matrixCopyInt1(hostInput, inputStride, hostOutput, rows, cols, zeroRowsInput, zeroColsInput);
   if (ret == OK)
   {
      MTOOLS_unflattenIntMatrix(hostOutput, outputMatrix, rows, cols);
   }

   free(hostInput);
   free(hostOutput);
   return ret;
}

static int MTOOLS_matrixCopyInt2Cuda(int** inputMatrix,int** outputMatrix,int rows,int cols,int zeroRowsOutput,int zeroColsOutput)
{
   int *hostInput;
   int *hostOutput;
   int outputRows;
   int outputStride;
   size_t inputCount;
   size_t outputCount;
   int ret;

   if ((inputMatrix == NULL) || (outputMatrix == NULL) || (rows <= 0) || (cols <= 0) ||
       (zeroRowsOutput < 0) || (zeroColsOutput < 0))
   {
      return ERROR_1;
   }

   outputRows = zeroRowsOutput + rows;
   outputStride = zeroColsOutput + cols;
   inputCount = (size_t)rows * (size_t)cols;
   outputCount = (size_t)outputRows * (size_t)outputStride;

   hostInput = (int*)malloc(inputCount * sizeof(int));
   hostOutput = (int*)malloc(outputCount * sizeof(int));
   if ((hostInput == NULL) || (hostOutput == NULL))
   {
      free(hostInput);
      free(hostOutput);
      return ERROR_1;
   }

   MTOOLS_flattenIntMatrix(inputMatrix, hostInput, rows, cols);
   MTOOLS_flattenIntMatrix(outputMatrix, hostOutput, outputRows, outputStride);
   ret = CUDA_matrixCopyInt2(hostInput, hostOutput, outputStride, rows, cols, zeroRowsOutput, zeroColsOutput);
   if (ret == OK)
   {
      MTOOLS_unflattenIntMatrix(hostOutput, outputMatrix, outputRows, outputStride);
   }

   free(hostInput);
   free(hostOutput);
   return ret;
}
#endif

void MTOOLS_multiplyMatrix(float **inputMatrix_1,float **inputMatrix_2,float **outputMatrix,int rows,int cols)
{
   int i,j,k;
   float sum = 0;
   float cElement = 0;

   /* check matrix compatibility*/
   if(rows != cols)
   {
      return;
   }

#ifdef USE_CUDA_MATRIX_TOOLS
   if (MTOOLS_multiplyMatrixCuda(inputMatrix_1, inputMatrix_2, outputMatrix, rows, cols) == OK)
   {
      return;
   }
#endif

   for (i=0; i<rows; i++)
   for (j=0; j<cols; j++)
   {
      for (k=0; k<rows; k++)
      {
         cElement = inputMatrix_1[i][k] * inputMatrix_2[k][j];
         sum = sum + cElement;
      }

      outputMatrix[i][j] = sum;
      sum = 0;
   }
}

void MTOOLS_multiplyMatrixZeros(float **inputMatrix_1,float **inputMatrix_2,float **outputMatrix,int rows,int cols, int zeroRow, int zeroCol)
{

   int i,j,k;
   float sum = 0;
   float cElement = 0;
   
   /* check matrix compatibility*/
   if(rows != cols)
   {
      return;
   }   

#ifdef USE_CUDA_MATRIX_TOOLS
   if (MTOOLS_multiplyMatrixZerosCuda(inputMatrix_1, inputMatrix_2, outputMatrix, rows, cols, zeroRow, zeroCol) == OK)
   {
      return;
   }
#endif
  
   for (i=0; i<rows; i++) 
   for (j=0; j<cols; j++) 
   {
      for (k=0; k<rows; k++)
      {
         cElement = inputMatrix_1[zeroRow + i][k + zeroCol] * inputMatrix_2[k][j];
         sum = sum + cElement;
      }
     
      outputMatrix[i][j] = sum;
      sum = 0; /* Not necessary */
      
   }
}

void MTOOLS_matrixConvInt2Float(int** matrixInt,float** matrixFloat,int rows,int cols)
{
   int rowIndex;
   int colIndex;

#ifdef USE_CUDA_MATRIX_TOOLS
   if (MTOOLS_matrixConvInt2FloatCuda(matrixInt, matrixFloat, rows, cols) == OK)
   {
      return;
   }
#endif


   for (rowIndex = 0; rowIndex < rows; rowIndex++)
   for (colIndex = 0; colIndex < cols; colIndex++)
   {
      matrixFloat[rowIndex][colIndex] =(float) matrixInt[rowIndex][colIndex];
      //printf("\n[%d][%d]float = %d => Int = %d \n", rowIndex, colIndex, matrixFloat[rowIndex][colIndex], matrixInt[rowIndex][colIndex]);
   }
}

void MTOOLS_matrixConvFloat2Int(float** matrixFloat,int** matrixInt,int rows,int cols)
{
   int rowIndex;
   int colIndex;

#ifdef USE_CUDA_MATRIX_TOOLS
   if (MTOOLS_matrixConvFloat2IntCuda(matrixFloat, matrixInt, rows, cols) == OK)
   {
      return;
   }
#endif


   for (rowIndex = 0; rowIndex < rows; rowIndex++)
   for (colIndex = 0; colIndex < cols; colIndex++)
   {
     matrixInt[rowIndex][colIndex] =(int) matrixFloat[rowIndex][colIndex];
   }
}

void MTOOLS_matrixCopyInt1(int** inputMatrix, int** outputMatrix, int rows,int cols, int zeroRowsInput, int zeroColsInput)
{

  int rowIndex, colIndex;

#ifdef USE_CUDA_MATRIX_TOOLS
  if (MTOOLS_matrixCopyInt1Cuda(inputMatrix, outputMatrix, rows, cols, zeroRowsInput, zeroColsInput) == OK)
  {
     return;
  }
#endif


  for (rowIndex = 0; rowIndex < rows; rowIndex++)
  for (colIndex = 0; colIndex < cols; colIndex++)
  {
     outputMatrix[rowIndex][colIndex] = inputMatrix[zeroRowsInput + rowIndex][colIndex + zeroColsInput];
  }
}

void MTOOLS_matrixCopyInt2(int** inputMatrix,int** outputMatrix,int rows,int cols,int zeroRowsOutput,int zeroColsOutput)
{

  int rowIndex, colIndex;

#ifdef USE_CUDA_MATRIX_TOOLS
  if (MTOOLS_matrixCopyInt2Cuda(inputMatrix, outputMatrix, rows, cols, zeroRowsOutput, zeroColsOutput) == OK)
  {
     return;
  }
#endif


  for (rowIndex = 0; rowIndex < rows; rowIndex++)
  for (colIndex = 0; colIndex < cols; colIndex++)
  {
     outputMatrix[zeroRowsOutput + rowIndex][colIndex + zeroColsOutput] = inputMatrix[rowIndex][colIndex];
  }
  
  
}

void MTOOLS_zigzagMatrixCollector(int** inputMatrix, int matrixWidth,int* outputStream)
{
   int state = 1;
   int i,j,k;
   int cycle;
   int streamIndex = 1;

   /* Process the 1st part of the matrix */

   for(k=0; k<matrixWidth; k++)
   {
      cycle = 1;
      if((state == 0) && (cycle == 1))
      {
         for (i=0; i<k+1; i++)
         {
            //outputStream[streamIndex] = inputMatrix[i][k-i];streamIndex++;
            inputMatrix[i][k-i] = streamIndex;
            streamIndex++;
         }
         
         state = 1;
         cycle = 0;
      }
     
      if((state == 1) && (cycle == 1))
      {
         for(j=0; j<k+1; j++)
         {
           //outputStream[streamIndex] = inputMatrix[k-j][j];streamIndex++;
           inputMatrix[k-j][j] = streamIndex;
           streamIndex++;
         }
         
         state = 0;
         cycle = 0;
      }
   }

   int stop = (matrixWidth-2);
   int far  = 1;
   
   /* Process the 2nd part of the matrix */
   
   do
   {
      cycle = 1;
      
      if((state == 0) && (cycle == 1))
      {
         for(i=0; i<stop+1; i++)
         {
            //outputStream[streamIndex] = inputMatrix[stop-i+far][i+far];streamIndex++;
            inputMatrix[stop-i+far][i+far] = streamIndex;
            streamIndex++;
         }
         
         state = 1;
         cycle = 0;
         far++;
      }

      if((state == 1) && (cycle == 1))
      {
         for(j=0; j<stop+1; j++)
         {
            //outputStream[streamIndex] = inputMatrix[stop-j+far][j+far];streamIndex++;
            inputMatrix[stop-j+far][j+far] = streamIndex;
            streamIndex++;
         }
         
         state = 0;
         cycle = 0;
         far++;
      }
      
      stop = stop-1; 
   }while(stop>-1);
}
