#include "processing.hpp"

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