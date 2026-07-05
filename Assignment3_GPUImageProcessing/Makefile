# Compilador normal (fases 1-4)
CXX      = g++
CXXFLAGS = -O2 -Iinclude -std=c++17

# Compilador OpenACC (fase 5, usar en Kabré)
ACCCXX      = nvc++
ACCCXXFLAGS = -O2 -Iinclude -std=c++17 -acc -Minfo=accel -gpu=cc70

SRC = src/stb_impl.cpp src/image_io.cpp src/processing.cpp src/main.cpp
BIN = gaussian_blur

all: $(BIN)

# Build sin GPU (para desarrollo local)
$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build con OpenACC (para Kabré)
acc: $(SRC)
	$(ACCCXX) $(ACCCXXFLAGS) -o $(BIN)_acc $^

clean:
	rm -f $(BIN) $(BIN)_acc output/*.png output/*.jpg

# Build con CUDA
NVCC      = nvcc
NVCCFLAGS = -O2 -Iinclude -std=c++17 -arch=sm_70

cuda: src/stb_impl.cpp src/image_io.cpp src/processing.cpp src/cuda_blur.cu src/main_cuda.cpp
	$(NVCC) $(NVCCFLAGS) -o gaussian_blur_cuda $^