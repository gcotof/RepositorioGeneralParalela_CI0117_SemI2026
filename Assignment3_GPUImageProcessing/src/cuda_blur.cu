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

