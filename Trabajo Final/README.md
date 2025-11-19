# Proyecto Final de Sistemas Operativos - Utilidad de Compresión y Encriptación

**Estudiante:** Mateo Lopera Ortiz  
**Materia:** Sistemas Operativos

---

## 1. Introducción
Este proyecto consiste en el desarrollo de una herramienta de línea de comandos (CLI) en C++ que permite comprimir, descomprimir, encriptar y desencriptar archivos y directorios. El objetivo principal es aplicar conceptos de bajo nivel como llamadas al sistema (Windows API), gestión de memoria y concurrencia para procesar múltiples archivos de manera eficiente.

La herramienta, llamada `so_final`, fue construida desde cero sin utilizar librerías externas para los algoritmos de compresión o encriptación, cumpliendo con los requisitos del reto.

## 2. Diseño de la Solución
La arquitectura del software es modular, separando claramente las responsabilidades:

- **Main**: Maneja la interacción con el usuario (CLI) y orquesta el flujo de trabajo.
- **FileManager**: Encapsula las llamadas al sistema de Windows (`CreateFile`, `ReadFile`, `WriteFile`, `FindFirstFile`) para interactuar con el disco.
- **Concurrency**: Gestiona la creación y sincronización de hilos (`CreateThread`, `WaitForMultipleObjects`) para procesar archivos en paralelo.
- **Algorithms**: Contiene la lógica pura de compresión (RLE) y encriptación (Vigenère).

### Flujo de Datos
1. El usuario ejecuta el comando con los parámetros deseados.
2. El programa identifica si la entrada es un archivo o un directorio.
3. Si es un directorio, se listan todos los archivos.
4. Para cada archivo, se lanza un hilo independiente.
5. Cada hilo lee el archivo, aplica las transformaciones (Compresión -> Encriptación o viceversa) y escribe el resultado.

## 3. Justificación de Algoritmos

### Compresión: Run-Length Encoding (RLE)
Elegí **RLE** por su simplicidad y eficiencia en datos con alta redundancia consecutiva (como imágenes simples o logs repetitivos).
- **Ventajas**: Muy rápido de implementar y ejecutar (O(n)). No requiere diccionarios complejos en memoria.
- **Desventajas**: No comprime bien archivos con alta entropía (texto natural variado).
- **Por qué RLE**: Dado el tiempo y el enfoque en la arquitectura de sistemas operativos (llamadas al sistema y concurrencia), RLE permite demostrar la manipulación de buffers byte a byte sin la complejidad de LZW o Huffman, cumpliendo el requisito de "algoritmo propio".

### Encriptación: Cifrado Vigenère
Implementé **Vigenère**, un cifrado polialfabético.
- **Ventajas**: Más seguro que un cifrado César simple, ya que la clave altera el desplazamiento en cada byte.
- **Desventajas**: Vulnerable al criptoanálisis moderno si la clave es corta.
- **Por qué Vigenère**: Es un algoritmo clásico que permite entender los fundamentos de la criptografía simétrica (operaciones a nivel de byte con una clave) sin la complejidad matemática de AES. Es suficiente para demostrar la protección de datos en este contexto académico.

## 4. Estrategia de Concurrencia
Para maximizar el uso de la CPU, implementé un modelo de **hilo por archivo**.
- Utilizo `CreateThread` de la API de Windows para lanzar una tarea por cada archivo encontrado en el directorio de entrada.
- El hilo principal espera a que todos los hilos terminen usando `WaitForMultipleObjects`.
- Esto permite que, mientras un hilo está bloqueado esperando I/O de disco, otro hilo pueda estar usando la CPU para comprimir o encriptar, mejorando significativamente el rendimiento en operaciones por lotes.

## 5. Guía de Uso

### Requisitos
- Compilador `g++` (MinGW/MSYS2).
- Herramienta `make`.

### Compilación
Abrir la terminal en la carpeta del proyecto y ejecutar:
```bash
make
```
Esto generará el ejecutable `so_final.exe`.

### Ejecución
La sintaxis general es:
```bash
./so_final.exe -[operaciones] -i [entrada] -o [salida] -k [clave]
```

**Ejemplos:**

1. **Comprimir y Encriptar una carpeta:**
   ```bash
   ./so_final.exe -ce -i "./datos_origen" -o "./datos_seguros" -k "MiClaveSecreta"
   ```

2. **Desencriptar y Descomprimir:**
   ```bash
   ./so_final.exe -ud -i "./datos_seguros" -o "./datos_restaurados" -k "MiClaveSecreta"
   ```

## 6. Caso de Uso Válido: "SecureLog Archiver"

**Escenario:** Una empresa de servidores web genera gigabytes de logs de acceso diariamente (`access.log`, `error.log`). Estos logs contienen texto muy repetitivo (IPs, fechas, códigos de error) y a veces información sensible de usuarios.

**Problema:**
- El almacenamiento de texto plano es costoso e ineficiente.
- Guardar logs con datos de usuarios sin encriptar viola normativas de privacidad.

**Solución:**
El administrador del sistema utiliza nuestra herramienta `so_final` en un script nocturno (cron job).
- **Compresión (RLE):** Reduce drásticamente el tamaño de los logs debido a las largas secuencias de caracteres repetidos (espacios, ceros, fechas).
- **Encriptación (Vigenère):** Ofusca el contenido para que, si alguien roba el disco de backups, no pueda leer los datos de los usuarios sin la clave.
- **Concurrencia:** Procesa los cientos de archivos de log de diferentes servidores virtuales simultáneamente, reduciendo la ventana de tiempo de backup de horas a minutos.

---
