#pragma once
#include <vector>

// Converts RGB/RGBA image to grayscale (1 channel)
// Input:  RGB pixels, width, height, channels (3 or 4)
// Output: vector with one value per pixel [0-255]
std::vector<unsigned char> to_grayscale(const unsigned char* data, int width, int height, int channels);