# Assignment 3 — Gaussian Blur with OpenACC
**Course:** CI-0117 Parallel and Concurrent Programming

GPU-accelerated image processing pipeline: converts an image to grayscale and applies a Gaussian Blur filter using OpenACC.

---

## Project Structure

```text
proyecto/
├── include/
│   ├── stb_image.h
│   ├── stb_image_write.h
│   ├── image_io.hpp
│   └── processing.hpp
├── src/
│   ├── stb_impl.cpp
│   ├── image_io.cpp
│   ├── processing.cpp
│   └── main.cpp
├── images/          ← input images
├── output/          ← generated images
└── Makefile
```

---

## Dependencies

| Environment | Required Compiler |
|---|---|
| Local (without GPU) | `g++` with C++17 support |
| Kabré — Nukwa node | `nvc++` from the NVIDIA HPC SDK |

`stb_image` and `stb_image_write` are included in `include/` — no installation is required.

---

## Compilation

### Local (`g++`, without OpenACC)

The `#pragma acc` directives are ignored; this build is useful for verifying the algorithm's logic.

```bash
make
```

### Kabré — Nukwa node (`nvc++`, with OpenACC)

```bash
make acc
```

The `-Minfo=accel` flag prints which loops were parallelized on the GPU.

---

## Running

```bash
# Local build
./gaussian_blur <input_image> <output_folder>

# OpenACC build (Kabré)
./gaussian_blur_acc <input_image> <output_folder>
```

### Example

```bash
./gaussian_blur images/foto.jpg output/
```

Supported input formats: `.jpg`, `.jpeg`, `.png`, `.bmp`

---

## Generated Outputs

| File | Description |
|---|---|
| `output/gray.png` | Grayscale image |
| `output/blur_cpu_3x3.png` | Gaussian blur applied on the CPU (3x3 kernel) |
| `output/blur_gpu_5x5.png` | Gaussian blur applied on the GPU (5x5 kernel) |
| `output/blur_gpu.png` | Gaussian blur applied on the GPU (OpenACC, 3x3 kernel) |


---

## Performance Report

When executed, the program automatically prints:

```text
===== Performance Report =====
T_CPU:   142.3 ms
T_GPU:   18.7 ms
Speedup: 7.6x
==============================
```

The actual execution times on Kabré (Nukwa node) are documented in the PDF report.

---

## Filter Parameters

By default, the program runs the CPU blur with **both** a 3×3 kernel (radius 1) and a 5×5 kernel (radius 2), saving each result separately (`blur_cpu_3x3.png` and `blur_cpu_5x5.png`).

The GPU (OpenACC) blur only runs with the **3×3** kernel by default. To use a 5×5 kernel on the GPU instead, modify the call in `main.cpp`:

```cpp
// Change radius=1 to radius=2
auto blur_gpu = gaussian_blur_gpu(gray, img.width, img.height, 2);
```
