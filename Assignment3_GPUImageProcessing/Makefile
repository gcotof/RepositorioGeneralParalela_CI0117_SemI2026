CXX     = g++
CXXFLAGS = -O2 -Iinclude -std=c++17

SRC = src/stb_impl.cpp src/image_io.cpp src/processing.cpp src/main.cpp
OBJ = $(SRC:.cpp=.o)
BIN = gaussian_blur

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(BIN) output/*.png output/*.jpg