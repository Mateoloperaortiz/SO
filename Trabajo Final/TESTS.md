# Scripts de Prueba para FileUtil

Este directorio contiene scripts de prueba completos para verificar todas las funcionalidades de FileUtil.

## Scripts Disponibles

### 1. `test_simple.ps1` (Recomendado para inicio rápido)
Script PowerShell simple y rápido que ejecuta pruebas básicas esenciales.

**Uso:**
```powershell
.\test_simple.ps1
```

**Pruebas que ejecuta:**
- Compresión de archivo
- Descompresión de archivo
- Encriptación de archivo
- Desencriptación de archivo
- Compresión + Encriptación combinadas
- Desencriptación + Descompresión combinadas

**Tiempo estimado:** ~5 segundos

### 2. `test.ps1` (Prueba completa)
Script PowerShell completo con verificaciones exhaustivas.

**Uso:**
```powershell
.\test.ps1
```

**Pruebas que ejecuta:**
- Todas las pruebas del script simple
- Procesamiento de directorio completo (concurrencia)
- Restauración de archivos desde directorio
- Manejo de archivos vacíos
- Verificación de seguridad (archivos encriptados diferentes)

**Tiempo estimado:** ~10-15 segundos

### 3. `test.bat` (Para CMD)
Script batch compatible con Command Prompt de Windows.

**Uso:**
```cmd
test.bat
```

**Pruebas que ejecuta:**
- Similar a test.ps1 pero en formato batch
- Compatible con CMD tradicional

**Tiempo estimado:** ~10-15 segundos

## Requisitos Previos

1. El ejecutable `fileutil.exe` debe estar en el directorio actual
2. Ejecutar los scripts desde el directorio raíz del proyecto

## Estructura de Pruebas

Todos los scripts crean los siguientes directorios:
- `test_dir/` - Directorio con archivos de prueba originales
- `test_output/` - Directorio con archivos procesados (comprimidos/encriptados)

## Ejecución de Pruebas

### Opción 1: PowerShell (Recomendado)
```powershell
# Prueba rápida
.\test_simple.ps1

# Prueba completa
.\test.ps1
```

### Opción 2: Command Prompt
```cmd
test.bat
```

### Opción 3: Ejecutar desde otro directorio
```powershell
cd "C:\Users\mateo\Documents\GitHub\SO\Trabajo Final"
.\test_simple.ps1
```

## Resultados Esperados

Si todas las pruebas pasan, verás:
```
========================================
RESUMEN DE RESULTADOS
========================================
Pruebas ejecutadas: 10
Pruebas exitosas: 10
Pruebas fallidas: 0

[SUCCESS] Todas las pruebas pasaron exitosamente!
```

## Limpieza de Archivos de Prueba

Después de ejecutar las pruebas, puedes limpiar los archivos generados:

**PowerShell:**
```powershell
Remove-Item -Recurse -Force test_output, test_dir
Remove-Item test_file*
```

**CMD:**
```cmd
rmdir /s /q test_output
rmdir /s /q test_dir
del test_file*
```

## Solución de Problemas

### Error: "fileutil.exe no encontrado"
- Asegúrate de estar en el directorio correcto
- Verifica que `fileutil.exe` existe ejecutando: `dir fileutil.exe`

### Error: "No se puede ejecutar scripts"
Si PowerShell muestra un error de política de ejecución:
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### Error: "Archivo no encontrado"
- Verifica que los scripts estén en el mismo directorio que `fileutil.exe`
- Ejecuta los scripts desde el directorio raíz del proyecto

## Personalización

Puedes modificar los scripts para:
- Cambiar las rutas de prueba
- Agregar más archivos de prueba
- Modificar las claves de encriptación
- Cambiar los nombres de archivos de salida

## Notas

- Los scripts preservan los archivos de prueba después de ejecutarse para que puedas revisarlos manualmente
- Los archivos comprimidos deberían ser más pequeños que los originales (excepto archivos muy pequeños)
- Los archivos encriptados deberían ser diferentes a los originales
- Los archivos restaurados deberían ser idénticos a los originales
