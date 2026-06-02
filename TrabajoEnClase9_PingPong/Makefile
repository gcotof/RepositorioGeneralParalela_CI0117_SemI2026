CXX = mpic++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17

TARGETS = pingpong ringu colectivas_allreduce

all: $(TARGETS)

pingpong: pingpong.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

ringu: ringu.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

allreduce: colectivas_allreduce.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)