#include "image_io.hpp"
#include "processing.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <imagen_entrada> <imagen_salida>\n";
        return 1;
    }

    // Reads image
    Image img = load_image(argv[1]);
    std::cout << "Imagen cargada: " << img.width << "x" << img.height
              << " (" << img.channels << " canales)\n";

    // Convert to grayscale
    auto gray = to_grayscale(img.data, img.width, img.height, img.channels);

    // Save (1 channel → PNG handles it well)
    save_image(argv[2], gray.data(), img.width, img.height, 1);
    std::cout << "Imagen en grises guardada en: " << argv[2] << "\n";

    free_image(img);
    return 0;
}