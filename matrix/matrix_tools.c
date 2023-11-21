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
   float m;

   for(i=0; i<rows; i++) 
   for(j=0; j<cols; j++) 
   {
     outputMatrix[i][j]=inputMatrix[j][i];
   }
}   

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
  
   for (i=0; i<rows; i++) 
   for (j=0; j<cols; j++) 
   {
      for (k=0; k<rows; k++)
      {
         cElement = inputMatrix_1[i][k] * inputMatrix_2[k][j];
         sum = sum + cElement;
      }
     
      outputMatrix[i][j] = sum;
      sum = 0; /* Not necessary */
      
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

   for (rowIndex = 0; rowIndex < rows; rowIndex++)
   for (colIndex = 0; colIndex < cols; colIndex++)
   {
     matrixInt[rowIndex][colIndex] =(int) matrixFloat[rowIndex][colIndex];
   }
}

void MTOOLS_matrixCopyInt1(int** inputMatrix, int** outputMatrix, int rows,int cols, int zeroRowsInput, int zeroColsInput)
{

  int rowIndex, colIndex;

  for (rowIndex = 0; rowIndex < rows; rowIndex++)
  for (colIndex = 0; colIndex < cols; colIndex++)
  {
     outputMatrix[rowIndex][colIndex] = inputMatrix[zeroRowsInput + rowIndex][colIndex + zeroColsInput];
  }
}

void MTOOLS_matrixCopyInt2(int** inputMatrix,int** outputMatrix,int rows,int cols,int zeroRowsOutput,int zeroColsOutput)
{

  int rowIndex, colIndex;

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
