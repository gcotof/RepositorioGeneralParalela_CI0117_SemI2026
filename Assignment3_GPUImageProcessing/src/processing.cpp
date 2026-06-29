#include "processing.hpp"
#include <iostream>

std::vector<unsigned char> to_grayscale(const unsigned char* data, int width, int height, int channels) {
    int total = width * height;
    std::vector<unsigned char> gray(total);

    for (int i = 0; i < total; i++) {
        int r = data[i * channels + 0];
        int g = data[i * channels + 1];
        int b = data[i * channels + 2];
        // Standard luminance formula (ITU-R BT.601)
        gray[i] = static_cast<unsigned char>(0.299f * r + 0.587f * g + 0.114f * b);
    }

    return gray;
}

// Precomputed normalized kernels
// 3x3: sum = 16, each value divided by 16
static const float KERNEL_3x3[3][3] = {
    {1/16.f, 2/16.f, 1/16.f},
    {2/16.f, 4/16.f, 2/16.f},
    {1/16.f, 2/16.f, 1/16.f}
};

// 5x5: sum = 256, each value divided by 256
static const float KERNEL_5x5[5][5] = {
    {1/256.f,  4/256.f,  6/256.f,  4/256.f, 1/256.f},
    {4/256.f, 16/256.f, 24/256.f, 16/256.f, 4/256.f},
    {6/256.f, 24/256.f, 36/256.f, 24/256.f, 6/256.f},
    {4/256.f, 16/256.f, 24/256.f, 16/256.f, 4/256.f},
    {1/256.f,  4/256.f,  6/256.f,  4/256.f, 1/256.f}
};

std::vector<unsigned char> gaussian_blur_cpu(const std::vector<unsigned char>& gray,
                                              int width, int height, int radius) {
    std::vector<unsigned char> blur(width * height);

    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float suma = 0.0f;

            for (int ky = -radius; ky <= radius; ky++) {
                for (int kx = -radius; kx <= radius; kx++) {
                    // Clamp: if the index goes out of bounds, use the nearest valid pixel
                    int yy = std::min(std::max(y + ky, 0), height - 1);
                    int xx = std::min(std::max(x + kx, 0), width  - 1);

                    float peso = (radius == 1)
                        ? KERNEL_3x3[ky + radius][kx + radius]
                        : KERNEL_5x5[ky + radius][kx + radius];

                    suma += peso * gray[yy * width + xx];
                }
            }

            blur[y * width + x] = static_cast<unsigned char>(suma);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "[CPU] Tiempo blur: " << ms << " ms\n";

    return blur;
}


std::vector<unsigned char> gaussian_blur_gpu(const std::vector<unsigned char>& gray,
                                              int width, int height, int radius) {
    int total = width * height;
    std::vector<unsigned char> blur(total);

    // Flatten the kernel to a 1D array to pass it to the GPU
    // OpenACC cannot access static 2D arrays outside their scope easily
    int ksize = (radius == 1) ? 3 : 5;
    int klen  = ksize * ksize;
    std::vector<float> kernel(klen);

    if (radius == 1) {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                kernel[i * 3 + j] = KERNEL_3x3[i][j];
    } else {
        for (int i = 0; i < 5; i++)
            for (int j = 0; j < 5; j++)
                kernel[i * 5 + j] = KERNEL_5x5[i][j];
    }

    // Raw pointers for acc directives.
    const unsigned char* in  = gray.data();
          unsigned char* out = blur.data();
    const float*         k   = kernel.data();

    auto start = std::chrono::high_resolution_clock::now();

    // Transfer data to GPU, execute kernel, copy result back
    #pragma acc data copyin(in[0:total], k[0:klen]) copyout(out[0:total])
    {
        #pragma acc parallel loop collapse(2) present(in, out, k)
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float suma = 0.0f;

                for (int ky = -radius; ky <= radius; ky++) {
                    for (int kx = -radius; kx <= radius; kx++) {
                        // Border clamp
                        int yy = (y + ky < 0) ? 0 : (y + ky >= height ? height - 1 : y + ky);
                        int xx = (x + kx < 0) ? 0 : (x + kx >= width  ? width  - 1 : x + kx);

                        float peso = k[(ky + radius) * ksize + (kx + radius)];
                        suma += peso * (float)in[yy * width + xx];
                    }
                }

                out[y * width + x] = (unsigned char)suma;
            }
        }
    } // <- The automatic copyout happens here.

    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "[GPU] Tiempo blur: " << ms << " ms\n";

    return blur;
}