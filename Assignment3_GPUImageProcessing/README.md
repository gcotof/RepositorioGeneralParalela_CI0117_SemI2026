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
| `output/blur_cpu.png` | Gaussian blur applied on the CPU |
| `output/blur_gpu.png` | Gaussian blur applied on the GPU (OpenACC) |

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

The default kernel is **3×3** (radius 1). To use a **5×5** kernel (radius 2), modify the call in `main.cpp`:

```cpp
// Change radius=1 to radius=2
auto blur_cpu = gaussian_blur_cpu(gray, img.width, img.height, 2);
auto blur_gpu = gaussian_blur_gpu(gray, img.width, img.height, 2);
```
