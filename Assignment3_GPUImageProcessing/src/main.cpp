#include "image_io.hpp"
#include "processing.hpp"
#include <iostream>
#include <chrono>  // Asegúrate de incluir esto

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

    // Gaussian blur in CPU with 3x3 kernel - MEDIR TIEMPO AQUÍ
    auto t0 = std::chrono::high_resolution_clock::now();
    auto blur3 = gaussian_blur_cpu(gray, img.width, img.height, 1);
    auto t1 = std::chrono::high_resolution_clock::now();
    double t_cpu = std::chrono::duration<double, std::milli>(t1 - t0).count();  // <--- AÑADIR ESTA LÍNEA
    
    save_image(std::string(argv[2]) + "/blur_cpu_3x3.png", blur3.data(), img.width, img.height, 1);

    // Optional: test with 5x5 kernel
    auto blur5 = gaussian_blur_cpu(gray, img.width, img.height, 2);
    save_image(std::string(argv[2]) + "/blur_cpu_5x5.png", blur5.data(), img.width, img.height, 1);

    // Gaussian blur in GPU with 3x3 kernel
    auto t2 = std::chrono::high_resolution_clock::now();
    auto blur_gpu = gaussian_blur_gpu(gray, img.width, img.height, 1);
    auto t3 = std::chrono::high_resolution_clock::now();
    double t_gpu = std::chrono::duration<double, std::milli>(t3 - t2).count();
    save_image(std::string(argv[2]) + "/blur_gpu.png", blur_gpu.data(), img.width, img.height, 1);

    // Performance report
    std::cout << "\n===== Performance Report =====\n";
    std::cout << "T_CPU:   " << t_cpu << " ms\n";
    std::cout << "T_GPU:   " << t_gpu << " ms\n";
    std::cout << "Speedup: " << t_cpu / t_gpu << "x\n";
    std::cout << "==================================\n";

    free_image(img);
    return 0;
}