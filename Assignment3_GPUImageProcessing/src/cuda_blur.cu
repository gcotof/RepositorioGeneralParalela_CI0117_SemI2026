#include "cuda_blur.hpp"
#include <iostream>
#include <chrono>
#include <cuda_runtime.h>

static const float KERNEL_3x3[3][3] = {
    {1/16.f, 2/16.f, 1/16.f},
    {2/16.f, 4/16.f, 2/16.f},
    {1/16.f, 2/16.f, 1/16.f}
};

static const float KERNEL_5x5[5][5] = {
    {1/256.f, 4/256.f,  6/256.f, 4/256.f, 1/256.f},
    {4/256.f, 16/256.f, 24/256.f, 16/256.f, 4/256.f},
    {6/256.f, 24/256.f, 36/256.f, 24/256.f, 6/256.f},
    {4/256.f, 16/256.f, 24/256.f, 16/256.f, 4/256.f},
    {1/256.f, 4/256.f, 6/256.f, 4/256.f, 1/256.f}
};

_global_ void blur_kernel(const unsigned char* in, unsigned char* out, const float* k, int width, int height, int radius, int ksize) {
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
