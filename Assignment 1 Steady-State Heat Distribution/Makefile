CXX = g++
CXXFLAGS = -O2 -fopenmp
SRCS = main.cpp grid.cpp output.cpp solver.cpp parallel.cpp
TARGET = heat

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET) *.vtk
