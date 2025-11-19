CXX = g++
CXXFLAGS = -Wall -std=c++17 -static-libgcc -static-libstdc++
TARGET = so_final.exe
SRCS = main.cpp FileManager.cpp Concurrency.cpp Compression.cpp Encryption.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
