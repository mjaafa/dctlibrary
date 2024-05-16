___kernel void MTOOLS_matrixTransposer(__global float **inputMatrix,__global float **outputMatrix, int rows, int cols)
{
   int i,j;
   float m;

   for(i=0; i<rows; i++) 
   for(j=0; j<cols; j++) 
   {
     outputMatrix[i][j]=inputMatrix[j][i];
   }
}   

__kernel void MTOOLS_multiplyMatrix(__global float **inputMatrix_1,__global float **inputMatrix_2,__global float **outputMatrix,__global int rows,__global int cols)
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

__kernel void MTOOLS_multiplyMatrixZeros(__global float **inputMatrix_1,__global float **inputMatrix_2,__global float **outputMatrix,__global int rows,__global int cols, __global int zeroRow, __global int zeroCol)
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

__kernel void MTOOLS_matrixConvint2float(__global int** matrixint,__global float** matrixfloat,__global int rows,__global int cols)
{
   int rowIndex;
   int colIndex;

   for (rowIndex = 0; rowIndex < rows; rowIndex++)
   for (colIndex = 0; colIndex < cols; colIndex++)
   {
      matrixfloat[rowIndex][colIndex] =(float) matrixint[rowIndex][colIndex];
      //pr__global intf("\n[%d][%d]__global float = %d => __global int = %d \n", rowIndex, colIndex, matrix__global float[rowIndex][colIndex], matrix__global int[rowIndex][colIndex]);
   }
}

__kernel void MTOOLS_matrixConvfloat2int(__global float** matrixfloat,__global int** matrixint,__global int rows,__global int cols)
{
   int rowIndex;
   int colIndex;

   for (rowIndex = 0; rowIndex < rows; rowIndex++)
   for (colIndex = 0; colIndex < cols; colIndex++)
   {
     matrixint[rowIndex][colIndex] =(int) matrixfloat[rowIndex][colIndex];
   }
}

__kernel void MTOOLS_zigzagMatrixCollector(__global int** inputMatrix, __global int matrixWidth,__global int* outputStream)
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

   __global int stop = (matrixWidth-2);
   __global int far  = 1;
   
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
