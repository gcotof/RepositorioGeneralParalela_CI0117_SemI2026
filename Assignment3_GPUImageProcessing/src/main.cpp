#include "image_io.hpp"
#include "processing.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <imagen_entrada> <carpeta_salida>\n";
        return 1;
    }

    // Read image
    Image img = load_image(argv[1]);
    std::cout << "Imagen cargada: " << img.width << "x" << img.height
              << " (" << img.channels << " canales)\n";

    // Grayscale conversion
    auto gray = to_grayscale(img.data, img.width, img.height, img.channels);
    save_image(std::string(argv[2]) + "/gray.png", gray.data(), img.width, img.height, 1);

    // Gaussian blur in CPU with 3x3 kernel
    auto blur3 = gaussian_blur_cpu(gray, img.width, img.height, 1);
    save_image(std::string(argv[2]) + "/blur_cpu_3x3.png", blur3.data(), img.width, img.height, 1);

    // Optional: test with 5x5 kernel
    auto blur5 = gaussian_blur_cpu(gray, img.width, img.height, 2);
    save_image(std::string(argv[2]) + "/blur_cpu_5x5.png", blur5.data(), img.width, img.height, 1);

    free_image(img);
    return 0;
}