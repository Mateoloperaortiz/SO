# Makefile para compilación del proyecto FileUtil
# Compilador: g++ (GCC)
# Sistema: Windows

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -pthread
TARGET = fileutil.exe
SOURCES = main.cpp file_manager.cpp compressor.cpp encryptor.cpp thread_manager.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = file_manager.h compressor.h encryptor.h thread_manager.h

# Regla principal
all: $(TARGET)

# Compilar ejecutable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Compilacion exitosa: $(TARGET)"

# Compilar archivos objeto
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Archivos eliminados"

# Recompilar desde cero
rebuild: clean all

# Ayuda
help:
	@echo "Makefile para FileUtil"
	@echo ""
	@echo "Comandos disponibles:"
	@echo "  make          - Compilar el proyecto"
	@echo "  make clean    - Eliminar archivos compilados"
	@echo "  make rebuild  - Recompilar desde cero"
	@echo "  make help     - Mostrar esta ayuda"

.PHONY: all clean rebuild help
