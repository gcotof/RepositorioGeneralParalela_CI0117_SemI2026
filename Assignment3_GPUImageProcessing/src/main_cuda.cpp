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
   
    // Blur CPU 
    auto t0 = chrono::high_resolution_clock::now();
    auto blur3 = gaussian_blur_cpu(gray, img.width, img.height, 1);
    auto t1 = chrono::high_resolution_clock::now();
    double t_cpu = chrono::duration<double, std::milli>(t1 - t0).count();
    save_image(string(argv[2]) + "/blur_cpu_3x3.png", blur3.data(), img.width, img.height, 1);
    // Blur GPU con CUDA 
    auto t2 = chrono::high_resolution_clock::now();
    auto blur_cuda = gaussian_blur_cuda(gray, img.width, img.height, 1);
    auto t3 = chrono::high_resolution_clock::now();
    double t_gpu = chrono::duration<double, std::milli>(t3 - t2).count();
    save_image(string(argv[2]) + "/blur_cuda.png", blur_cuda.data(), img.width, img.height, 1);

    cout << "\n===== Performance Report (CUDA) =====\n";
    cout << "T_CPU:   " << t_cpu << " ms\n";
    cout << "T_GPU:   " << t_gpu << " ms\n";
    cout << "Speedup: " << t_cpu / t_gpu << "x\n";
    cout << "======================================\n";
    free_image(img);

    return 0;
}