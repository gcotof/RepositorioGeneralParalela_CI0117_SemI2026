#include "cuda_blur.hpp"
#include <iostream>
#include <chrono>
#include <cuda_runtime.h>
using namespace std;

// ---------------------------------------------------------------------------
// cuda_blur.cu
//
// Gaussian Blur GPU kernel using explicit CUDA (no OpenACC).
//
// blur_kernel()        — one thread per output pixel. Same convolution and
//                        clamp border handling as the CPU/OpenACC versions.
//
// gaussian_blur_cuda() — host-side driver: allocates device memory, copies
//                        input/kernel to device, launches blur_kernel, copies
//                        result back. Explicit memory management (no unified
//                        memory) with a single round-trip transfer.
// ---------------------------------------------------------------------------

// CUDA error-checking macro: prints the real driver message on failure
// instead of continuing silently with undefined device state.
#define CUDA_CHECK(call) \
    do { \
        cudaError_t err = (call); \
        if (err != cudaSuccess) \
            std::cerr << "Error CUDA en " << __FILE__ << ":" << __LINE__ << " -> " << cudaGetErrorString(err) << std::endl; \
    } while (0)

// Normalized 3x3 Gaussian kernel (radius 1)
static const float KERNEL_3x3[3][3] = {
    {1/16.f, 2/16.f, 1/16.f},
    {2/16.f, 4/16.f, 2/16.f},
    {1/16.f, 2/16.f, 1/16.f}
};
// Normalized 5x5 Gaussian kernel (radius 2)
static const float KERNEL_5x5[5][5] = {
    {1/256.f, 4/256.f,  6/256.f, 4/256.f, 1/256.f},
    {4/256.f, 16/256.f, 24/256.f, 16/256.f, 4/256.f},
    {6/256.f, 24/256.f, 36/256.f, 24/256.f, 6/256.f},
    {4/256.f, 16/256.f, 24/256.f, 16/256.f, 4/256.f},
    {1/256.f, 4/256.f, 6/256.f, 4/256.f, 1/256.f}
};
// ---------------------------------------------------------------------------
// blur_kernel: one thread computes one output pixel (in, out same layout
// as the CPU version). Border handling via clamp (same as CPU/OpenACC).
// ---------------------------------------------------------------------------
__global__ void blur_kernel(const unsigned char* in, unsigned char* out, const float* k, int width, int height, int radius, int ksize) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= width || y >= height) return;
    float suma = 0.0f;
    for (int ky = -radius; ky <= radius; ky++) {
        for (int kx = -radius; kx <= radius; kx++) {
            int yy = min(max(y + ky, 0), height - 1);
            int xx = min(max(x + kx, 0), width - 1);
            float peso = k[(ky + radius) * ksize + (kx + radius)];
            suma += peso * (float)in[yy * width + xx];
        }
    }
    out[y * width + x] = (unsigned char)suma;
}
// ---------------------------------------------------------------------------
// gaussian_blur_cuda: host driver. Explicit device memory (no cudaMallocManaged),
// single host->device transfer, one kernel launch, single device->host transfer.
// ---------------------------------------------------------------------------
vector<unsigned char> gaussian_blur_cuda(const vector<unsigned char>& gray, int width, int height, int radius) {
    int total = width * height;
    vector<unsigned char> blur(total);
    int ksize = (radius == 1) ? 3 : 5;
    int klen = ksize * ksize;
    vector<float> kernel(klen);
    if (radius == 1) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++)
                kernel[i * 3 + j] = KERNEL_3x3[i][j];
        }
    } else {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++)
                kernel[i * 5 + j] = KERNEL_5x5[i][j];
        }
    }
    unsigned char *d_in, *d_out;
    float *d_kernel;
    CUDA_CHECK(cudaMalloc(&d_in, total * sizeof(unsigned char)));
    CUDA_CHECK(cudaMalloc(&d_out, total * sizeof(unsigned char)));
    CUDA_CHECK(cudaMalloc(&d_kernel, klen * sizeof(float)));
    
    auto start = chrono::high_resolution_clock::now();
    CUDA_CHECK(cudaMemcpy(d_in, gray.data(), total * sizeof(unsigned char), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(d_kernel, kernel.data(), klen * sizeof(float), cudaMemcpyHostToDevice));

    dim3 blockDim(16, 16);
    dim3 gridDim((width  + blockDim.x - 1) / blockDim.x, (height + blockDim.y - 1) / blockDim.y);
    blur_kernel<<<gridDim, blockDim>>>(d_in, d_out, d_kernel, width, height, radius, ksize);
    CUDA_CHECK(cudaGetLastError()); // error de LANZAMIENTO del kernel
    CUDA_CHECK(cudaDeviceSynchronize()); // error de EJECUCION del kernel

    CUDA_CHECK(cudaMemcpy(blur.data(), d_out, total * sizeof(unsigned char), cudaMemcpyDeviceToHost));

    auto end = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double,milli>(end - start).count();
    cout << "[CUDA] Tiempo blur: " << ms << " ms" << endl;
    cudaFree(d_in);
    cudaFree(d_out);
    cudaFree(d_kernel);

    return blur;
}
