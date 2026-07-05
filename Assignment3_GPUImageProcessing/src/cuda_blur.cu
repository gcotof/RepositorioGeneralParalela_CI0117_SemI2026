#include "cuda_blur.hpp"
#include <iostream>
#include <chrono>
#include <cuda_runtime.h>

#define CUDA_CHECK(call)                                                     
    do {                                                                     
        cudaError_t err = (call);                                           
        if (err != cudaSuccess)                                           
            cerr << "Error CUDA en " << __FILE__ << ":" << __LINE__  << " -> " << cudaGetErrorString(err) << endl;                                                                     
    } while (0)

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
    cudaMalloc(&d_in, total * sizeof(unsigned char));
    cudaMalloc(&d_out, total * sizeof(unsigned char));
    cudaMalloc(&d_kernel, klen * sizeof(float));
    
    auto start = chrono::high_resolution_clock::now();
    cudaMemcpy(d_in, gray.data(),total * sizeof(unsigned char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_kernel, kernel.data(), klen * sizeof(float), cudaMemcpyHostToDevice);

    dim3 blockDim(16, 16);
    dim3 gridDim((width  + blockDim.x - 1) / blockDim.x, (height + blockDim.y - 1) / blockDim.y);
    blur_kernel<<<gridDim, blockDim>>>(d_in, d_out, d_kernel, width, height, radius, ksize);
    CUDA_CHECK(cudaGetLastError()); // error de LANZAMIENTO del kernel
    CUDA_CHECK(cudaDeviceSynchronize()); // error de EJECUCION del kernel

    CUDA_CHECK(cudaMemcpy(blur.data(), d_out, total * sizeof(unsigned char), cudaMemcpyDeviceToHost));

    auto end = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double,milli>(end - start).count();
    cout << "[CUDA] Tiempo blur: " << ms << " ms" << endl;
    cudaFree(d_in);s
    cudaFree(d_out);
    cudaFree(d_kernel);

    return blur;
}
