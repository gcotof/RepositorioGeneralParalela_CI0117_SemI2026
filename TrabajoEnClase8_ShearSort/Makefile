CXX = g++
CXXFLAGS = -fopenmp -O2

TARGET = shear
SRC = shear.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

