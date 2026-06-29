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