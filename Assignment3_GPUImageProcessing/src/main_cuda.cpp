#include "image_io.hpp"
#include "processing.hpp"
#include "cuda_blur.hpp"
#include <iostream>
#include <chrono>
using namespace std;

int main (int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Uso: " << argv[0] << " <imagen_entrada> <carpeta_salida>\n";
        return 1;
    }
    Image img = load_image(argv[1]);
    cout << "Imagen cargada: " << img.width << "x" << img.height << " (" << img.channels << " canales)" << endl;
    auto gray = to_grayscale(img.data, img.width, img.height, img.channels);
    save_image(string(argv[2]) + "/gray.png", gray.data(), img.width, img.height, 1);
   
    return 0;
}