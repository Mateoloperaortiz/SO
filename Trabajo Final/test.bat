@echo off
REM Script de prueba completo para FileUtil
REM Este script prueba todas las funcionalidades del programa

echo ========================================
echo TEST COMPLETO DE FILEUTIL
echo ========================================
echo.

REM Colores y configuración
setlocal enabledelayedexpansion
set ERROR=0
set TEST_COUNT=0
set PASS_COUNT=0

REM Crear directorios de prueba
echo [1/10] Preparando entorno de prueba...
if exist test_output rmdir /s /q test_output
if exist test_dir rmdir /s /q test_dir
mkdir test_output
mkdir test_dir
mkdir test_dir\subdir

REM Crear archivos de prueba
echo [2/10] Creando archivos de prueba...
echo "Este es un archivo de texto de prueba para compresion." > test_dir\archivo1.txt
echo "Contenido repetitivo: ABC ABC ABC ABC ABC ABC ABC" > test_dir\archivo2.txt
echo "Archivo con muchos espacios:   a   b   c   d   e   f   g   h   i   j" > test_dir\archivo3.txt
echo "Archivo en subdirectorio" > test_dir\subdir\archivo4.txt

REM Función para verificar si un archivo existe
set /a TEST_COUNT+=1
echo.
echo [3/10] TEST: Compresion de archivo simple
.\fileutil.exe -c -i test_dir\archivo1.txt -o test_output\archivo1.huf
if exist test_output\archivo1.huf (
    echo [OK] Archivo comprimido creado exitosamente
    set /a PASS_COUNT+=1
) else (
    echo [ERROR] No se creo el archivo comprimido
    set /a ERROR+=1
)

set /a TEST_COUNT+=1
echo.
echo [4/10] TEST: Descompresion de archivo
.\fileutil.exe -d -i test_output\archivo1.huf -o test_output\archivo1_decomp.txt
if exist test_output\archivo1_decomp.txt (
    echo [OK] Archivo descomprimido creado exitosamente
    REM Verificar que el contenido sea igual
    fc /b test_dir\archivo1.txt test_output\archivo1_decomp.txt >nul
    if !errorlevel! equ 0 (
        echo [OK] Contenido del archivo descomprimido es identico al original
        set /a PASS_COUNT+=1
    ) else (
        echo [ERROR] El contenido del archivo descomprimido no coincide
        set /a ERROR+=1
    )
) else (
    echo [ERROR] No se creo el archivo descomprimido
    set /a ERROR+=1
)

set /a TEST_COUNT+=1
echo.
echo [5/10] TEST: Encriptacion de archivo
.\fileutil.exe -e -i test_dir\archivo2.txt -o test_output\archivo2.enc -k "clave_secreta_123"
if exist test_output\archivo2.enc (
    echo [OK] Archivo encriptado creado exitosamente
    set /a PASS_COUNT+=1
) else (
    echo [ERROR] No se creo el archivo encriptado
    set /a ERROR+=1
)

set /a TEST_COUNT+=1
echo.
echo [6/10] TEST: Desencriptacion de archivo
.\fileutil.exe -u -i test_output\archivo2.enc -o test_output\archivo2_decrypt.txt -k "clave_secreta_123"
if exist test_output\archivo2_decrypt.txt (
    echo [OK] Archivo desencriptado creado exitosamente
    REM Verificar que el contenido sea igual
    fc /b test_dir\archivo2.txt test_output\archivo2_decrypt.txt >nul
    if !errorlevel! equ 0 (
        echo [OK] Contenido del archivo desencriptado es identico al original
        set /a PASS_COUNT+=1
    ) else (
        echo [ERROR] El contenido del archivo desencriptado no coincide
        set /a ERROR+=1
    )
) else (
    echo [ERROR] No se creo el archivo desencriptado
    set /a ERROR+=1
)

set /a TEST_COUNT+=1
echo.
echo [7/10] TEST: Compresion y encriptacion combinadas
.\fileutil.exe -ce -i test_dir\archivo3.txt -o test_output\archivo3.huf.enc -k "mi_clave_secreta"
if exist test_output\archivo3.huf.enc (
    echo [OK] Archivo comprimido y encriptado creado exitosamente
    set /a PASS_COUNT+=1
) else (
    echo [ERROR] No se creo el archivo comprimido y encriptado
    set /a ERROR+=1
)

set /a TEST_COUNT+=1
echo.
echo [8/10] TEST: Desencriptacion y descompresion combinadas
.\fileutil.exe -du -i test_output\archivo3.huf.enc -o test_output\archivo3_restored.txt -k "mi_clave_secreta"
if exist test_output\archivo3_restored.txt (
    echo [OK] Archivo desencriptado y descomprimido creado exitosamente
    REM Verificar que el contenido sea igual
    fc /b test_dir\archivo3.txt test_output\archivo3_restored.txt >nul
    if !errorlevel! equ 0 (
        echo [OK] Contenido del archivo restaurado es identico al original
        set /a PASS_COUNT+=1
    ) else (
        echo [ERROR] El contenido del archivo restaurado no coincide
        set /a ERROR+=1
    )
) else (
    echo [ERROR] No se creo el archivo restaurado
    set /a ERROR+=1
)

set /a TEST_COUNT+=1
echo.
echo [9/10] TEST: Procesamiento de directorio completo (concurrencia)
if exist test_output\test_dir_compressed rmdir /s /q test_output\test_dir_compressed
mkdir test_output\test_dir_compressed
.\fileutil.exe -ce -i test_dir -o test_output\test_dir_compressed -k "clave_directorio"
if exist test_output\test_dir_compressed\archivo1.txt.huf.enc (
    echo [OK] Archivos del directorio procesados exitosamente
    set /a PASS_COUNT+=1
) else (
    echo [ERROR] No se procesaron los archivos del directorio
    set /a ERROR+=1
)

set /a TEST_COUNT+=1
echo.
echo [10/10] TEST: Archivo vacio
echo. > test_dir\archivo_vacio.txt
.\fileutil.exe -c -i test_dir\archivo_vacio.txt -o test_output\archivo_vacio.huf
if exist test_output\archivo_vacio.huf (
    echo [OK] Archivo vacio comprimido exitosamente
    .\fileutil.exe -d -i test_output\archivo_vacio.huf -o test_output\archivo_vacio_restored.txt
    if exist test_output\archivo_vacio_restored.txt (
        echo [OK] Archivo vacio descomprimido exitosamente
        set /a PASS_COUNT+=1
    ) else (
        echo [ERROR] No se pudo descomprimir archivo vacio
        set /a ERROR+=1
    )
) else (
    echo [ERROR] No se comprimio el archivo vacio
    set /a ERROR+=1
)

REM Resumen de resultados
echo.
echo ========================================
echo RESUMEN DE RESULTADOS
echo ========================================
echo Pruebas ejecutadas: %TEST_COUNT%
echo Pruebas exitosas: %PASS_COUNT%
echo Pruebas fallidas: %ERROR%
echo.

if %ERROR% equ 0 (
    echo [SUCCESS] Todas las pruebas pasaron exitosamente!
    echo.
    echo Archivos de prueba creados en: test_output\
    echo Puedes revisar los archivos generados para verificar los resultados.
) else (
    echo [FAILURE] Algunas pruebas fallaron. Revisa los mensajes de error arriba.
)

echo.
echo Presiona cualquier tecla para limpiar archivos de prueba...
pause >nul

REM Limpiar archivos de prueba (opcional)
REM echo Limpiando archivos de prueba...
REM if exist test_output rmdir /s /q test_output
REM if exist test_dir rmdir /s /q test_dir

endlocal