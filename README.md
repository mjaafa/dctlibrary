# dctlibrary

DCT image processing library with optional CUDA GPU acceleration.

MIT licence.

---

## Requirements

- CMake 3.20+
- A C99-capable compiler (GCC, Clang, or MSVC)
- CUDA Toolkit 11+ (only for CUDA builds)

---

## Building

### CPU-only (no CUDA)

```bash
cmake -S . -B build
cmake --build build --config Release
```

The executable lands at `build/dctlib` (Linux/macOS) or `build\Release\dctlib.exe` (Windows).

### Enable optional features

| Feature | CMake flag | Default |
|---|---|---|
| PGM image I/O | `-D_USE_PGM=ON` | ON |
| Matrix tools API | `-D_USE_MATRIX_TOOLS=ON` | ON |
| DCT API | `-D_USE_DCT=ON` | ON |
| Job/threading system | `-D_USE_JOBS=ON` | OFF |
| CUDA matrix acceleration | `-DBUILD_CUDA_MATRIX_TOOLS=ON` | OFF |
| CUDA DCT kernels | `-DBUILD_CUDA_DCT=ON` | OFF |

Example — CPU-only build with the threading system enabled:

```bash
cmake -S . -B build -D_USE_JOBS=ON
cmake --build build --config Release
```

---

## CUDA MatrixTools build

MatrixTools can now be built with CUDA independently from the full CUDA DCT path:

```bash
cmake -S . -B build-cuda -DBUILD_CUDA_MATRIX_TOOLS=ON
cmake --build build-cuda --config Release
```

To enable the CUDA DCT picture kernels too, use:

```bash
cmake -S . -B build-cuda -DBUILD_CUDA_DCT=ON
cmake --build build-cuda --config Release
```

`BUILD_CUDA_DCT=ON` automatically enables `BUILD_CUDA_MATRIX_TOOLS=ON` for compatibility with older build commands.

On Windows with Visual Studio and CUDA 12.x, for example:

```powershell
cmake -S . -B build-cuda -G "Visual Studio 17 2022" -A x64 -DBUILD_CUDA_MATRIX_TOOLS=ON
cmake --build build-cuda --config Release
```

The C MatrixTools APIs still keep CPU fallback code. When CUDA is enabled, these functions try CUDA first and fall back to the original CPU implementation if the CUDA call fails:

- `MTOOLS_matrixTransposer`
- `MTOOLS_multiplyMatrix`
- `MTOOLS_multiplyMatrixZeros`
- `MTOOLS_matrixConvInt2Float`
- `MTOOLS_matrixConvFloat2Int`
- `MTOOLS_matrixCopyInt1`
- `MTOOLS_matrixCopyInt2`
