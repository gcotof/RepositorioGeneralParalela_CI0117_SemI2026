CXX = mpic++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17

TARGETS = pingpong ringu colectivas_allreduce colectivas_reduce

all: $(TARGETS)

pingpong: pingpong.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

ringu: ringu.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

colectivas_allreduce: colectivas_allreduce.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

colectivas_reduce: colectivas_reduce.cpp	
	$(CXX) $(CXXFLAGS) -o $@ $

clean:
	rm -f $(TARGETS)