#pragma once
#include <vector>
using namespace std;

// Versión GPU con CUDA 
vector<unsigned char> gaussian_blur_cuda(const vector<unsigned char>& gray, int width, int height, int radius = 1);