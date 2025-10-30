# Instrucciones de Compilación y Ejecución

## Requisitos Previos

1. **Compilador C++**: Necesitas tener instalado un compilador compatible con C++11:
   - MinGW-w64 (recomendado para Windows)
   - Visual Studio con herramientas de C++
   - TDM-GCC

2. **Make** (opcional): Para usar el Makefile proporcionado:
   - MinGW incluye `make`
   - O usa `mingw32-make` si está disponible

## Compilación

### Opción 1: Usando Makefile

```bash
make
```

Esto generará el ejecutable `fileutil.exe`.

### Opción 2: Compilación Manual

```bash
g++ -std=c++11 -Wall -Wextra -O2 -pthread -o fileutil.exe main.cpp file_manager.cpp compressor.cpp encryptor.cpp thread_manager.cpp
```

### Opción 3: Compilación con Visual Studio

```bash
cl /EHsc /std:c++11 /O2 main.cpp file_manager.cpp compressor.cpp encryptor.cpp thread_manager.cpp /Fe:fileutil.exe
```

## Pruebas Básicas

**Nota**: En PowerShell, usa `.\fileutil.exe` en lugar de `fileutil.exe`. En CMD puedes usar `fileutil.exe` directamente.

### 1. Crear archivo de prueba

```bash
echo "Este es un archivo de prueba para comprimir y encriptar" > test.txt
```

### 2. Comprimir archivo

```bash
.\fileutil.exe -c -i test.txt -o test.huf
```

### 3. Descomprimir archivo

```bash
.\fileutil.exe -d -i test.huf -o test_decompressed.txt
```

### 4. Encriptar archivo

```bash
.\fileutil.exe -e -i test.txt -o test.enc -k mi_clave_secreta
```

### 5. Desencriptar archivo

```bash
.\fileutil.exe -u -i test.enc -o test_decrypted.txt -k mi_clave_secreta
```

### 6. Comprimir y encriptar

```bash
.\fileutil.exe -ce -i test.txt -o test.huf.enc -k mi_clave_secreta
```

### 7. Desencriptar y descomprimir

```bash
.\fileutil.exe -du -i test.huf.enc -o test_restored.txt -k mi_clave_secreta
```

### 8. Procesar directorio completo

```bash
# Crear directorio de prueba
mkdir test_dir
copy test.txt test_dir\test1.txt
copy test.txt test_dir\test2.txt
copy test.txt test_dir\test3.txt

# Comprimir y encriptar todos los archivos
fileutil.exe -ce -i test_dir -o test_dir_compressed -k mi_clave

# Verificar que se crearon los archivos comprimidos y encriptados
dir test_dir_compressed
```

## Verificación

Después de cada operación, puedes verificar que los archivos fueron procesados correctamente:

```bash
# Comparar archivos originales y descomprimidos
fc test.txt test_decompressed.txt

# Verificar que los archivos encriptados son diferentes
fc test.txt test.enc
```

## Solución de Problemas

### Error: "No se pudo abrir el archivo"
- Verifica que la ruta del archivo sea correcta
- Asegúrate de tener permisos de lectura/escritura
- En Windows, usa rutas con barras invertidas o comillas si hay espacios

### Error: "Se requiere -k para operaciones de encriptación"
- Agrega `-k tu_clave` cuando uses `-e` o `-u`

### Error de compilación: "thread: No such file or directory"
- Asegúrate de usar C++11 o superior
- Verifica que el compilador soporte threads (`-pthread` en GCC)

### Archivos muy grandes
- El programa puede usar bastante memoria para archivos grandes
- Considera procesar archivos individualmente en lugar de directorios completos

## Ejemplos Avanzados

### Procesar solo archivos específicos

```bash
# Comprimir múltiples archivos individualmente
fileutil.exe -c -i documento1.pdf -o documento1.huf
fileutil.exe -c -i documento2.pdf -o documento2.huf
fileutil.exe -c -i documento3.pdf -o documento3.huf
```

### Script de respaldo automatizado

Crea un archivo `backup.bat`:

```batch
@echo off
set KEY=clave_secreta_2024
set SOURCE=C:\Documentos\Importantes
set DEST=C:\Backup\Encriptado

fileutil.exe -ce -i "%SOURCE%" -o "%DEST%" -k "%KEY%"
echo Backup completado
```

Ejecuta con:
```bash
backup.bat
```
