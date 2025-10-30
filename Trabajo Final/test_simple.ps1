# Script de prueba rápida y simple para FileUtil
# Ejecuta pruebas básicas y muestra resultados

Write-Host "Ejecutando pruebas basicas de FileUtil..." -ForegroundColor Cyan
Write-Host ""

# Verificar que el ejecutable existe
if (-not (Test-Path ".\fileutil.exe")) {
    Write-Host "ERROR: fileutil.exe no encontrado en el directorio actual" -ForegroundColor Red
    Write-Host "Ejecuta este script desde el directorio donde compilaste el proyecto" -ForegroundColor Yellow
    exit 1
}

# Crear archivo de prueba
$testFile = "test_file.txt"
$testContent = "Este es un archivo de prueba con contenido repetitivo: ABC ABC ABC ABC ABC ABC"
Set-Content -Path $testFile -Value $testContent

Write-Host "Archivo de prueba creado: $testFile" -ForegroundColor Green

# Test 1: Compresión
Write-Host "`n[TEST 1] Comprimiendo archivo..." -ForegroundColor Yellow
.\fileutil.exe -c -i $testFile -o "test_file.huf" 2>&1 | Out-Null
if (Test-Path "test_file.huf") {
    Write-Host "[OK] Archivo comprimido creado" -ForegroundColor Green
} else {
    Write-Host "[FAIL] Error al comprimir" -ForegroundColor Red
    exit 1
}

# Test 2: Descompresión
Write-Host "[TEST 2] Descomprimiendo archivo..." -ForegroundColor Yellow
.\fileutil.exe -d -i "test_file.huf" -o "test_file_decomp.txt" 2>&1 | Out-Null
if (Test-Path "test_file_decomp.txt") {
    $original = Get-Content $testFile -Raw
    $decompressed = Get-Content "test_file_decomp.txt" -Raw
    if ($original -eq $decompressed) {
        Write-Host "[OK] Descompresion exitosa, contenido identico" -ForegroundColor Green
    } else {
        Write-Host "[FAIL] Contenido no coincide" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "[FAIL] Error al descomprimir" -ForegroundColor Red
    exit 1
}

# Test 3: Encriptación
Write-Host "[TEST 3] Encriptando archivo..." -ForegroundColor Yellow
.\fileutil.exe -e -i $testFile -o "test_file.enc" -k "clave123" 2>&1 | Out-Null
if (Test-Path "test_file.enc") {
    Write-Host "[OK] Archivo encriptado creado" -ForegroundColor Green
} else {
    Write-Host "[FAIL] Error al encriptar" -ForegroundColor Red
    exit 1
}

# Test 4: Desencriptación
Write-Host "[TEST 4] Desencriptando archivo..." -ForegroundColor Yellow
.\fileutil.exe -u -i "test_file.enc" -o "test_file_decrypt.txt" -k "clave123" 2>&1 | Out-Null
if (Test-Path "test_file_decrypt.txt") {
    $original = Get-Content $testFile -Raw
    $decrypted = Get-Content "test_file_decrypt.txt" -Raw
    if ($original -eq $decrypted) {
        Write-Host "[OK] Desencriptacion exitosa, contenido identico" -ForegroundColor Green
    } else {
        Write-Host "[FAIL] Contenido no coincide" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "[FAIL] Error al desencriptar" -ForegroundColor Red
    exit 1
}

# Test 5: Compresión + Encriptación
Write-Host "[TEST 5] Comprimiendo y encriptando..." -ForegroundColor Yellow
.\fileutil.exe -ce -i $testFile -o "test_file.huf.enc" -k "clave123" 2>&1 | Out-Null
if (Test-Path "test_file.huf.enc") {
    Write-Host "[OK] Archivo comprimido y encriptado creado" -ForegroundColor Green
} else {
    Write-Host "[FAIL] Error al comprimir y encriptar" -ForegroundColor Red
    exit 1
}

# Test 6: Desencriptación + Descompresión
Write-Host "[TEST 6] Desencriptando y descomprimiendo..." -ForegroundColor Yellow
.\fileutil.exe -du -i "test_file.huf.enc" -o "test_file_restored.txt" -k "clave123" 2>&1 | Out-Null
if (Test-Path "test_file_restored.txt") {
    $original = Get-Content $testFile -Raw
    $restored = Get-Content "test_file_restored.txt" -Raw
    if ($original -eq $restored) {
        Write-Host "[OK] Restauracion exitosa, contenido identico" -ForegroundColor Green
    } else {
        Write-Host "[FAIL] Contenido no coincide" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "[FAIL] Error al restaurar" -ForegroundColor Red
    exit 1
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "[SUCCESS] Todas las pruebas basicas pasaron!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "`nArchivos de prueba creados:" -ForegroundColor Yellow
Get-ChildItem -Filter "test_file*" | ForEach-Object { Write-Host "  - $($_.Name)" -ForegroundColor White }
Write-Host "`nPara limpiar archivos de prueba, ejecuta:" -ForegroundColor Yellow
Write-Host "  Remove-Item test_file*" -ForegroundColor White
Write-Host ""
