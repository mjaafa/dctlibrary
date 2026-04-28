#include "cuda_dct.h"

#include <cuda_runtime.h>
#include <math_constants.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCT_BLOCK 8

namespace {

#define CUDA_CHECK(call) do { \
    cudaError_t _err = (call); \
    if (_err != cudaSuccess) { \
        fprintf(stderr, "CUDA error %s at %s:%d\n", cudaGetErrorString(_err), __FILE__, __LINE__); \
        goto done; \
    } \
} while (0)

__device__ __forceinline__ float dctCoeff(int u, int x)
{
    const float alpha = (u == 0) ? rsqrtf(2.0f) : 1.0f;
    return 0.5f * alpha * cosf(((2.0f * x + 1.0f) * u * CUDART_PI_F) / 16.0f);
}

/* Forward DCT for one 8×8 block.
   Shared memory usage:
     pixels[8][8]  – pixel data loaded once; eliminates 64× redundant global reads per thread.
     coeff[8][8]   – coeff[k][n] = dctCoeff(k,n); eliminates repeated cosf() calls. */
__global__ void forwardDctKernel(const int* __restrict__ input,
                                  int* __restrict__ output, int rows, int cols)
{
    __shared__ float pixels[DCT_BLOCK][DCT_BLOCK];
    __shared__ float coeff[DCT_BLOCK][DCT_BLOCK];

    int u = threadIdx.y;
    int v = threadIdx.x;
    int rowBase = blockIdx.y * DCT_BLOCK;
    int colBase = blockIdx.x * DCT_BLOCK;

    pixels[u][v] = (rowBase + u < rows && colBase + v < cols)
                 ? (float)input[(rowBase + u) * cols + (colBase + v)]
                 : 0.0f;
    coeff[u][v] = dctCoeff(u, v);
    __syncthreads();

    float sum = 0.0f;
    #pragma unroll
    for (int x = 0; x < DCT_BLOCK; ++x) {
        float rowSum = 0.0f;
        #pragma unroll
        for (int y = 0; y < DCT_BLOCK; ++y)
            rowSum += coeff[v][y] * pixels[x][y];
        sum += coeff[u][x] * rowSum;
    }

    int q = 1 + u + v;
    if (rowBase + u < rows && colBase + v < cols)
        output[(rowBase + u) * cols + (colBase + v)] = (int)(sum / (float)q);
}

/* Inverse DCT for one 8×8 block.
   Same shared memory strategy: load the quantized block and cosine table once,
   then compute entirely from shared memory. */
__global__ void inverseDctKernel(const int* __restrict__ input,
                                  int* __restrict__ output, int rows, int cols)
{
    __shared__ float dctBlock[DCT_BLOCK][DCT_BLOCK];
    __shared__ float coeff[DCT_BLOCK][DCT_BLOCK];

    int x = threadIdx.y;
    int y = threadIdx.x;
    int rowBase = blockIdx.y * DCT_BLOCK;
    int colBase = blockIdx.x * DCT_BLOCK;

    /* Dequantize while loading. */
    if (rowBase + x < rows && colBase + y < cols) {
        int q = 1 + x + y;
        dctBlock[x][y] = (float)(input[(rowBase + x) * cols + (colBase + y)] * q);
    } else {
        dctBlock[x][y] = 0.0f;
    }
    coeff[x][y] = dctCoeff(x, y);
    __syncthreads();

    float sum = 0.0f;
    #pragma unroll
    for (int u = 0; u < DCT_BLOCK; ++u) {
        float inner = 0.0f;
        #pragma unroll
        for (int v = 0; v < DCT_BLOCK; ++v)
            inner += coeff[v][y] * dctBlock[u][v];
        sum += coeff[u][x] * inner;
    }

    sum = fmaxf(0.0f, fminf(255.0f, sum));
    if (rowBase + x < rows && colBase + y < cols)
        output[(rowBase + x) * cols + (colBase + y)] = (int)sum;
}

int doPictureTransform(const int* hostInput, int* hostOutput, int rows, int cols, bool inverse)
{
    if ((rows % DCT_BLOCK) != 0 || (cols % DCT_BLOCK) != 0) {
        fprintf(stderr, "CUDA DCT requires image dimensions to be multiples of 8.\n");
        return -1;
    }

    const size_t bytes = (size_t)rows * (size_t)cols * sizeof(int);
    int *dInput = nullptr, *dOutput = nullptr;
    int rc = -1;

    CUDA_CHECK(cudaMalloc(&dInput, bytes));
    CUDA_CHECK(cudaMalloc(&dOutput, bytes));
    CUDA_CHECK(cudaMemcpy(dInput, hostInput, bytes, cudaMemcpyHostToDevice));

    {
        dim3 grid((unsigned)(cols / DCT_BLOCK), (unsigned)(rows / DCT_BLOCK), 1U);
        dim3 block(DCT_BLOCK, DCT_BLOCK, 1U);
        if (inverse)
            inverseDctKernel<<<grid, block>>>(dInput, dOutput, rows, cols);
        else
            forwardDctKernel<<<grid, block>>>(dInput, dOutput, rows, cols);
    }
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());
    CUDA_CHECK(cudaMemcpy(hostOutput, dOutput, bytes, cudaMemcpyDeviceToHost));
    rc = 0;

done:
    cudaFree(dInput);
    cudaFree(dOutput);
    return rc;
}

} // namespace

extern "C" int CUDA_forwardDctPicture(const int *input, int *output, int rows, int cols)
{
    return doPictureTransform(input, output, rows, cols, false);
}

extern "C" int CUDA_inverseDctPicture(const int *input, int *output, int rows, int cols)
{
    return doPictureTransform(input, output, rows, cols, true);
}
