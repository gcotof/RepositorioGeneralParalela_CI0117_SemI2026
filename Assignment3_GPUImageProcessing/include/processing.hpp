#pragma once
#include <vector>

// Converts RGB/RGBA image to grayscale (1 channel)
// Input:  RGB pixels, width, height, channels (3 or 4)
// Output: vector with one value per pixel [0-255]
std::vector<unsigned char> to_grayscale(const unsigned char* data, int width, int height, int channels);

std::vector<unsigned char> gaussian_blur_cpu(const std::vector<unsigned char>& gray, int width, int height, int radius = 1);

// GPU version with OpenACC — works with raw pointers internally
std::vector<unsigned char> gaussian_blur_gpu(const std::vector<unsigned char>& gray, int width, int height, int radius = 1);