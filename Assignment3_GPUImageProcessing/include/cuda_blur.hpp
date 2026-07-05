#pragma once
#include <vector>
using namespace std;

// GPU version using explicit CUDA (no OpenACC). Same signature as the
// CPU/OpenACC versions. Default radius=1 selects the 3x3 kernel.
vector<unsigned char> gaussian_blur_cuda(const vector<unsigned char>& gray, int width, int height, int radius = 1);