#include "cuda_matrix_tools.h"

#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>

namespace {

#define TILE_SIZE 16

/* Tiled matrix multiply using shared memory.
   Each TILE_SIZE×TILE_SIZE block cooperatively loads one tile of A and one tile of B
   before computing the partial dot products, replacing O(n) global reads per thread
   with O(n/TILE_SIZE) global reads. */
__global__ void matmulSquareKernel(const float* __restrict__ a,
                                    const float* __restrict__ b,
                                    float* __restrict__ c, int n)
{
    __shared__ float tileA[TILE_SIZE][TILE_SIZE];
    __shared__ float tileB[TILE_SIZE][TILE_SIZE];

    int row = blockIdx.y * TILE_SIZE + threadIdx.y;
    int col = blockIdx.x * TILE_SIZE + threadIdx.x;
    float sum = 0.0f;

    int numTiles = (n + TILE_SIZE - 1) / TILE_SIZE;
    for (int t = 0; t < numTiles; ++t) {
        int aCol = t * TILE_SIZE + threadIdx.x;
        int bRow = t * TILE_SIZE + threadIdx.y;
        tileA[threadIdx.y][threadIdx.x] = (row < n && aCol < n) ? a[row * n + aCol] : 0.0f;
        tileB[threadIdx.y][threadIdx.x] = (bRow < n && col < n) ? b[bRow * n + col] : 0.0f;
        __syncthreads();

        #pragma unroll
        for (int k = 0; k < TILE_SIZE; ++k)
            sum += tileA[threadIdx.y][k] * tileB[k][threadIdx.x];
        __syncthreads();
    }

    if (row < n && col < n)
        c[row * n + col] = sum;
}

/* Transpose using shared memory with +1 column padding to avoid bank conflicts.
   Loads a tile with coalesced reads, then writes the transposed tile with coalesced
   writes. Without shared memory the output writes are strided (uncoalesced). */
__global__ void transposeFloatKernel(const float* __restrict__ input,
                                      float* __restrict__ output, int rows, int cols)
{
    __shared__ float tile[TILE_SIZE][TILE_SIZE + 1];

    int x = blockIdx.x * TILE_SIZE + threadIdx.x;
    int y = blockIdx.y * TILE_SIZE + threadIdx.y;

    if (y < rows && x < cols)
        tile[threadIdx.y][threadIdx.x] = input[y * cols + x];
    __syncthreads();

    /* Swap block indices so output writes are coalesced. */
    x = blockIdx.y * TILE_SIZE + threadIdx.x;
    y = blockIdx.x * TILE_SIZE + threadIdx.y;

    if (y < cols && x < rows)
        output[y * rows + x] = tile[threadIdx.x][threadIdx.y];
}

__global__ void intToFloatKernel(const int* __restrict__ input,
                                  float* __restrict__ output, int count)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < count)
        output[idx] = (float)input[idx];
}

__global__ void floatToIntKernel(const float* __restrict__ input,
                                  int* __restrict__ output, int count)
{
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < count)
        output[idx] = (int)input[idx];
}

__global__ void copyInt1Kernel(const int* __restrict__ input, int inputStride,
                                int* __restrict__ output, int rows, int cols,
                                int zeroRowsInput, int zeroColsInput)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows && col < cols)
        output[row * cols + col] = input[(zeroRowsInput + row) * inputStride + zeroColsInput + col];
}

__global__ void copyInt2Kernel(const int* __restrict__ input,
                                int* __restrict__ output, int outputStride,
                                int rows, int cols, int zeroRowsOutput, int zeroColsOutput)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < rows && col < cols)
        output[(zeroRowsOutput + row) * outputStride + zeroColsOutput + col] = input[row * cols + col];
}

int validateCount(int rows, int cols)
{
    if (rows <= 0 || cols <= 0)
        return -1;
    return rows * cols;
}

static void logCudaError(cudaError_t err, const char *file, int line)
{
    fprintf(stderr, "CUDA MatrixTools error %s at %s:%d\n",
            cudaGetErrorString(err), file, line);
}

#define CUDA_CHK(call) do { \
    cudaError_t _e = (call); \
    if (_e != cudaSuccess) { logCudaError(_e, __FILE__, __LINE__); goto done; } \
} while (0)

} // namespace

extern "C" int CUDA_matrixMultiplyFloatSquare(const float *a, const float *b, float *out, int n)
{
    if (n <= 0 || !a || !b || !out)
        return -1;

    const size_t bytes = (size_t)n * (size_t)n * sizeof(float);
    float *dA = NULL, *dB = NULL, *dOut = NULL;
    int rc = -1;

    CUDA_CHK(cudaMalloc(&dA, bytes));
    CUDA_CHK(cudaMalloc(&dB, bytes));
    CUDA_CHK(cudaMalloc(&dOut, bytes));
    CUDA_CHK(cudaMemcpy(dA, a, bytes, cudaMemcpyHostToDevice));
    CUDA_CHK(cudaMemcpy(dB, b, bytes, cudaMemcpyHostToDevice));

    {
        dim3 block(TILE_SIZE, TILE_SIZE);
        dim3 grid((n + TILE_SIZE - 1) / TILE_SIZE, (n + TILE_SIZE - 1) / TILE_SIZE);
        matmulSquareKernel<<<grid, block>>>(dA, dB, dOut, n);
    }
    CUDA_CHK(cudaGetLastError());
    CUDA_CHK(cudaDeviceSynchronize());
    CUDA_CHK(cudaMemcpy(out, dOut, bytes, cudaMemcpyDeviceToHost));
    rc = 0;

done:
    cudaFree(dA); cudaFree(dB); cudaFree(dOut);
    return rc;
}

extern "C" int CUDA_matrixTransposeFloat(const float *input, float *output, int rows, int cols)
{
    int count = validateCount(rows, cols);
    if (count < 0 || !input || !output)
        return -1;

    const size_t bytes = (size_t)count * sizeof(float);
    float *dInput = NULL, *dOutput = NULL;
    int rc = -1;

    CUDA_CHK(cudaMalloc(&dInput, bytes));
    CUDA_CHK(cudaMalloc(&dOutput, bytes));
    CUDA_CHK(cudaMemcpy(dInput, input, bytes, cudaMemcpyHostToDevice));

    {
        dim3 block(TILE_SIZE, TILE_SIZE);
        dim3 grid((cols + TILE_SIZE - 1) / TILE_SIZE, (rows + TILE_SIZE - 1) / TILE_SIZE);
        transposeFloatKernel<<<grid, block>>>(dInput, dOutput, rows, cols);
    }
    CUDA_CHK(cudaGetLastError());
    CUDA_CHK(cudaDeviceSynchronize());
    CUDA_CHK(cudaMemcpy(output, dOutput, bytes, cudaMemcpyDeviceToHost));
    rc = 0;

done:
    cudaFree(dInput); cudaFree(dOutput);
    return rc;
}

extern "C" int CUDA_matrixIntToFloat(const int *input, float *output, int rows, int cols)
{
    int count = validateCount(rows, cols);
    if (count < 0 || !input || !output)
        return -1;

    const size_t inBytes = (size_t)count * sizeof(int);
    const size_t outBytes = (size_t)count * sizeof(float);
    int *dInput = NULL;
    float *dOutput = NULL;
    int rc = -1;

    CUDA_CHK(cudaMalloc(&dInput, inBytes));
    CUDA_CHK(cudaMalloc(&dOutput, outBytes));
    CUDA_CHK(cudaMemcpy(dInput, input, inBytes, cudaMemcpyHostToDevice));

    {
        int block = 256;
        int grid = (count + block - 1) / block;
        intToFloatKernel<<<grid, block>>>(dInput, dOutput, count);
    }
    CUDA_CHK(cudaGetLastError());
    CUDA_CHK(cudaDeviceSynchronize());
    CUDA_CHK(cudaMemcpy(output, dOutput, outBytes, cudaMemcpyDeviceToHost));
    rc = 0;

done:
    cudaFree(dInput); cudaFree(dOutput);
    return rc;
}

extern "C" int CUDA_matrixFloatToInt(const float *input, int *output, int rows, int cols)
{
    int count = validateCount(rows, cols);
    if (count < 0 || !input || !output)
        return -1;

    const size_t inBytes = (size_t)count * sizeof(float);
    const size_t outBytes = (size_t)count * sizeof(int);
    float *dInput = NULL;
    int *dOutput = NULL;
    int rc = -1;

    CUDA_CHK(cudaMalloc(&dInput, inBytes));
    CUDA_CHK(cudaMalloc(&dOutput, outBytes));
    CUDA_CHK(cudaMemcpy(dInput, input, inBytes, cudaMemcpyHostToDevice));

    {
        int block = 256;
        int grid = (count + block - 1) / block;
        floatToIntKernel<<<grid, block>>>(dInput, dOutput, count);
    }
    CUDA_CHK(cudaGetLastError());
    CUDA_CHK(cudaDeviceSynchronize());
    CUDA_CHK(cudaMemcpy(output, dOutput, outBytes, cudaMemcpyDeviceToHost));
    rc = 0;

done:
    cudaFree(dInput); cudaFree(dOutput);
    return rc;
}

extern "C" int CUDA_matrixCopyInt1(const int *input, int input_stride, int *output,
                                    int rows, int cols, int zero_rows_input, int zero_cols_input)
{
    int outCount = validateCount(rows, cols);
    if (outCount < 0 || !input || !output || input_stride <= 0 ||
        zero_rows_input < 0 || zero_cols_input < 0)
        return -1;

    const int inputRows = zero_rows_input + rows;
    const size_t inputBytes = (size_t)inputRows * (size_t)input_stride * sizeof(int);
    const size_t outputBytes = (size_t)outCount * sizeof(int);
    int *dInput = NULL, *dOutput = NULL;
    int rc = -1;

    CUDA_CHK(cudaMalloc(&dInput, inputBytes));
    CUDA_CHK(cudaMalloc(&dOutput, outputBytes));
    CUDA_CHK(cudaMemcpy(dInput, input, inputBytes, cudaMemcpyHostToDevice));

    {
        dim3 block(TILE_SIZE, TILE_SIZE);
        dim3 grid((cols + TILE_SIZE - 1) / TILE_SIZE, (rows + TILE_SIZE - 1) / TILE_SIZE);
        copyInt1Kernel<<<grid, block>>>(dInput, input_stride, dOutput, rows, cols,
                                        zero_rows_input, zero_cols_input);
    }
    CUDA_CHK(cudaGetLastError());
    CUDA_CHK(cudaDeviceSynchronize());
    CUDA_CHK(cudaMemcpy(output, dOutput, outputBytes, cudaMemcpyDeviceToHost));
    rc = 0;

done:
    cudaFree(dInput); cudaFree(dOutput);
    return rc;
}

extern "C" int CUDA_matrixCopyInt2(const int *input, int *output, int output_stride,
                                    int rows, int cols, int zero_rows_output, int zero_cols_output)
{
    int inputCount = validateCount(rows, cols);
    if (inputCount < 0 || !input || !output || output_stride <= 0 ||
        zero_rows_output < 0 || zero_cols_output < 0)
        return -1;

    const int outputRows = zero_rows_output + rows;
    const size_t inputBytes = (size_t)inputCount * sizeof(int);
    const size_t outputBytes = (size_t)outputRows * (size_t)output_stride * sizeof(int);
    int *dInput = NULL, *dOutput = NULL;
    int rc = -1;

    CUDA_CHK(cudaMalloc(&dInput, inputBytes));
    CUDA_CHK(cudaMalloc(&dOutput, outputBytes));
    CUDA_CHK(cudaMemcpy(dInput, input, inputBytes, cudaMemcpyHostToDevice));
    CUDA_CHK(cudaMemcpy(dOutput, output, outputBytes, cudaMemcpyHostToDevice));

    {
        dim3 block(TILE_SIZE, TILE_SIZE);
        dim3 grid((cols + TILE_SIZE - 1) / TILE_SIZE, (rows + TILE_SIZE - 1) / TILE_SIZE);
        copyInt2Kernel<<<grid, block>>>(dInput, dOutput, output_stride, rows, cols,
                                        zero_rows_output, zero_cols_output);
    }
    CUDA_CHK(cudaGetLastError());
    CUDA_CHK(cudaDeviceSynchronize());
    CUDA_CHK(cudaMemcpy(output, dOutput, outputBytes, cudaMemcpyDeviceToHost));
    rc = 0;

done:
    cudaFree(dInput); cudaFree(dOutput);
    return rc;
}

extern "C" int CUDA_multiplyMatrixSquare(const float *a, const float *b, float *out, int n)
{
    return CUDA_matrixMultiplyFloatSquare(a, b, out, n);
}
