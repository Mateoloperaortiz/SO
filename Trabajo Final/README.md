# FileUtil - Utilidad de Compresión y Encriptación

Utilidad de línea de comandos en C++ para comprimir/descomprimir y encriptar/desencriptar archivos o directorios completos de manera eficiente usando procesamiento concurrente.

## Características

- **Compresión sin pérdida**: Algoritmo Huffman implementado desde cero
- **Encriptación simétrica**: Algoritmo Vigenère mejorado con XOR
- **Procesamiento concurrente**: Múltiples archivos procesados en paralelo usando threads
- **Llamadas directas al sistema**: Usa Windows API (CreateFile, ReadFile, WriteFile) en lugar de abstracciones estándar
- **Sin dependencias externas**: Todos los algoritmos implementados manualmente

## Requisitos

- Compilador C++11 compatible (GCC, MinGW, MSVC)
- Sistema operativo Windows (usa Windows API)
- Make (opcional, para usar el Makefile)

## Compilación

### Usando Makefile:

```bash
make
```

### Compilación manual:

```bash
g++ -std=c++11 -Wall -Wextra -O2 -pthread -o fileutil.exe main.cpp file_manager.cpp compressor.cpp encryptor.cpp thread_manager.cpp
```

### Ejecutar el programa:

En PowerShell (Windows):
```bash
.\fileutil.exe --help
.\fileutil.exe -c -i archivo.txt -o archivo.huf
```

En Command Prompt (CMD):
```bash
fileutil.exe --help
fileutil.exe -c -i archivo.txt -o archivo.huf
```

## Uso

### Sintaxis básica:

```
fileutil [operaciones] [opciones]
```

### Operaciones:

- `-c` : Comprimir
- `-d` : Descomprimir
- `-e` : Encriptar
- `-u` : Desencriptar

Las operaciones se pueden combinar (ej: `-ce` para comprimir y luego encriptar).

### Opciones:

- `--comp-alg [algoritmo]` : Algoritmo de compresión (actualmente solo "huffman")
- `--enc-alg [algoritmo]` : Algoritmo de encriptación (actualmente solo "vigenere")
- `-i <ruta>` : Ruta de entrada (archivo o directorio)
- `-o <ruta>` : Ruta de salida (archivo o directorio)
- `-k <clave>` : Clave secreta para encriptación/desencriptación (requerida para -e/-u)
- `-h, --help` : Mostrar ayuda

### Ejemplos:

**Nota**: En PowerShell (Windows), usa `.\fileutil.exe` en lugar de `fileutil.exe` para ejecutar desde el directorio actual.

```bash
# Comprimir un archivo
.\fileutil.exe -c -i documento.txt -o documento.huf

# Comprimir y encriptar un archivo
.\fileutil.exe -ce -i documento.txt -o documento.huf.enc -k mi_clave_secreta

# Desencriptar y descomprimir
.\fileutil.exe -du -i documento.huf.enc -o documento.txt -k mi_clave_secreta

# Procesar un directorio completo
.\fileutil.exe -ce -i carpeta/ -o carpeta_comprimida/ -k mi_clave

# Solo encriptar
.\fileutil.exe -e -i archivo.txt -o archivo.enc -k clave123

# Solo descomprimir
.\fileutil.exe -d -i archivo.huf -o archivo.txt
```

## Algoritmos Implementados

### Compresión: Huffman

**Justificación:**
- Algoritmo sin pérdida ampliamente conocido y eficiente
- Produce buena compresión para archivos con patrones repetitivos
- Complejidad: O(n log n) para construcción del árbol, O(n) para codificación
- Implementación relativamente simple comparada con otros algoritmos avanzados

**Ventajas:**
- Sin pérdida de información
- Eficiente para datos con distribución no uniforme
- Genera códigos de longitud variable optimizados

**Desventajas:**
- Requiere dos pasadas sobre los datos (construcción de árbol + codificación)
- Puede no ser óptimo para archivos pequeños
- El árbol debe almacenarse junto con los datos comprimidos

### Encriptación: Vigenère Mejorado

**Justificación:**
- Algoritmo simétrico clásico mejorado con operaciones XOR
- Más seguro que el Vigenère original gracias a la combinación con XOR
- Implementación sencilla pero efectiva para el propósito educativo
- Permite claves de longitud variable

**Ventajas:**
- Implementación relativamente simple
- Operaciones rápidas (XOR y suma modular)
- Resistente a análisis de frecuencia simple (gracias a XOR)

**Desventajas:**
- Menos seguro que algoritmos modernos como AES
- Vulnerable a ataques de texto conocido si la clave es débil
- La seguridad depende de la fortaleza de la clave

## Arquitectura

```
fileutil.exe
├── main.cpp              - Punto de entrada y parsing de argumentos
├── file_manager.cpp      - Manejo de archivos con Windows API
├── compressor.cpp        - Implementación de Huffman
├── encryptor.cpp         - Implementación de Vigenère
└── thread_manager.cpp    - Gestión de threads para concurrencia
```

## Estrategia de Concurrencia

El programa utiliza threads de C++11 (`std::thread`) para procesar múltiples archivos en paralelo:

1. **Detección de directorio**: Si la entrada es un directorio, se obtienen todos los archivos recursivamente
2. **Creación de threads**: Se crea un thread por cada archivo a procesar
3. **Sincronización**: Se usa `std::mutex` para proteger operaciones de salida compartidas
4. **Espera de finalización**: Todos los threads se esperan antes de terminar el programa

**Ventajas:**
- Mejor aprovechamiento de sistemas multinúcleo
- Reducción significativa del tiempo total de procesamiento
- Escalable con el número de archivos

**Consideraciones:**
- El número máximo de threads depende del sistema operativo
- Para archivos grandes, puede requerir más memoria
- La sincronización añade una pequeña sobrecarga

## Estructura del Proyecto

```
.
├── main.cpp
├── file_manager.h
├── file_manager.cpp
├── compressor.h
├── compressor.cpp
├── encryptor.h
├── encryptor.cpp
├── thread_manager.h
├── thread_manager.cpp
├── Makefile
├── README.md
├── TESTS.md
├── CASO_DE_USO.md
├── INSTRUCCIONES.md
├── test.bat
├── test.ps1
└── test_simple.ps1
```

## Pruebas

El proyecto incluye scripts de prueba completos para verificar todas las funcionalidades:

```bash
# Prueba rápida (PowerShell)
.\test_simple.ps1

# Prueba completa (PowerShell)
.\test.ps1

# Prueba completa (CMD)
test.bat
```

Ver `TESTS.md` para más información sobre los scripts de prueba.

## Limitaciones Conocidas

1. Solo funciona en Windows (usa Windows API específica)
2. Los algoritmos implementados son versiones educativas (no usar para datos críticos)
3. La gestión de memoria para archivos muy grandes puede mejorar
4. No hay validación exhaustiva de integridad de archivos comprimidos

## Notas de Desarrollo

- El código está comentado en español según los requisitos del proyecto
- Se utilizan llamadas directas al sistema operativo (Windows API)
- Todos los algoritmos están implementados desde cero sin librerías externas
- El código sigue principios de programación orientada a objetos

## Licencia

Este proyecto es parte de un trabajo académico.

## Autor

Desarrollado como trabajo final de Sistemas Operativos.
