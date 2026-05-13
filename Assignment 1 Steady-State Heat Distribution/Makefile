CXX     = g++
CXXFLAGS = -O3 -Wall -Wextra -std=c++17 -fopenmp
SRCS    = main.cpp grid.cpp solver.cpp parallel.cpp output.cpp
TARGET  = heat

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

run-parallel: $(TARGET)
	./$(TARGET) --parallel --threads 4

clean:
	rm -f $(TARGET) *.vtk *.csv

.PHONY: run run-parallel clean