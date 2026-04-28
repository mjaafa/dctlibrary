#ifndef CUDA_MATRIX_TOOLS_H
#define CUDA_MATRIX_TOOLS_H

#ifdef __cplusplus
extern "C" {
#endif

int CUDA_matrixMultiplyFloatSquare(const float *a, const float *b, float *out, int n);
int CUDA_matrixTransposeFloat(const float *input, float *output, int rows, int cols);
int CUDA_matrixIntToFloat(const int *input, float *output, int rows, int cols);
int CUDA_matrixFloatToInt(const float *input, int *output, int rows, int cols);
int CUDA_matrixCopyInt1(const int *input, int input_stride, int *output,
                        int rows, int cols, int zero_rows_input, int zero_cols_input);
int CUDA_matrixCopyInt2(const int *input, int *output, int output_stride,
                        int rows, int cols, int zero_rows_output, int zero_cols_output);

/* Backward-compatible name used by older CUDA DCT/MatrixTools code. */
int CUDA_multiplyMatrixSquare(const float *a, const float *b, float *out, int n);

#ifdef __cplusplus
}
#endif

#endif /* CUDA_MATRIX_TOOLS_H */
