#ifndef CUDA_DCT_H
#define CUDA_DCT_H

#ifdef __cplusplus
extern "C" {
#endif

int CUDA_multiplyMatrixSquare(const float *a, const float *b, float *out, int n);
int CUDA_forwardDctPicture(const int *input, int *output, int rows, int cols);
int CUDA_inverseDctPicture(const int *input, int *output, int rows, int cols);

#ifdef __cplusplus
}
#endif

#endif
