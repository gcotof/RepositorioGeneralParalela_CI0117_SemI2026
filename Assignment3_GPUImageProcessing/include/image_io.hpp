#pragma once
#include <string>

struct Image {
    unsigned char* data;  // pixels in RGB or RGBA format.
    int width, height, channels;
};

Image load_image(const std::string& path);
void  save_image(const std::string& path, const unsigned char* data,
                 int width, int height, int channels);
void  free_image(Image& img);