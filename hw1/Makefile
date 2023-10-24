CXX = g++
CXXFLAGS = -std=c++11
SRC = QuineMcCluskey.cpp classtype.h column.cpp Petrick.cpp 312511052.cpp
TARGET = 312511052.o

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

.PHONY: clean

clean:
	rm -f $(TARGET)
