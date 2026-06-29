#include "image_io.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
#include <stdexcept>
#include <string>

Image load_image(const std::string& path) {
    Image img;
    // stbi_load returns pixels in heap; channels = real channels of the file
    img.data = stbi_load(path.c_str(), &img.width, &img.height, &img.channels, 0);
    if (!img.data)
        throw std::runtime_error("No se pudo cargar la imagen: " + path);
    return img;
}

void save_image(const std::string& path, const unsigned char* data,
                int width, int height, int channels) {
    // Detects format by extension
    std::string ext = path.substr(path.rfind('.'));
    int ok = 0;
    if      (ext == ".png") ok = stbi_write_png(path.c_str(), width, height, channels, data, width * channels);
    else if (ext == ".jpg" || ext == ".jpeg") ok = stbi_write_jpg(path.c_str(), width, height, channels, data, 90);
    else if (ext == ".bmp") ok = stbi_write_bmp(path.c_str(), width, height, channels, data);
    else throw std::runtime_error("Format not supported: " + ext);

    if (!ok) throw std::runtime_error("Could not save image: " + path);
}

void free_image(Image& img) {
    stbi_image_free(img.data);
    img.data = nullptr;
}