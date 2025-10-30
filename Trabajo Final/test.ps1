# Script de prueba completo para FileUtil (PowerShell)
# Este script prueba todas las funcionalidades del programa

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "TEST COMPLETO DE FILEUTIL" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$script:ErrorCount = 0
$script:TestCount = 0
$script:PassCount = 0

# Función para ejecutar un test
function Test-Command {
    param(
        [string]$TestName,
        [string]$Command,
        [scriptblock]$Verification
    )
    
    $script:TestCount++
    Write-Host "[$script:TestCount] TEST: $TestName" -ForegroundColor Yellow
    
    try {
        if ($Command) {
            Invoke-Expression $Command 2>&1 | Out-Null
        }
        
        if ($Verification) {
            $result = & $Verification
            if ($result) {
                Write-Host "[OK] $TestName - Exitoso" -ForegroundColor Green
                $script:PassCount++
                return $true
            } else {
                Write-Host "[ERROR] $TestName - Fallo en verificacion" -ForegroundColor Red
                $script:ErrorCount++
                return $false
            }
        } else {
            Write-Host "[OK] $TestName - Exitoso" -ForegroundColor Green
            $script:PassCount++
            return $true
        }
    } catch {
        Write-Host "[ERROR] $TestName - Error: $_" -ForegroundColor Red
        $script:ErrorCount++
        return $false
    }
}

# Preparar entorno de prueba
Write-Host "[PREPARACION] Preparando entorno de prueba..." -ForegroundColor Cyan

if (Test-Path "test_output") {
    Remove-Item -Recurse -Force "test_output"
}
if (Test-Path "test_dir") {
    Remove-Item -Recurse -Force "test_dir"
}

New-Item -ItemType Directory -Path "test_output" | Out-Null
New-Item -ItemType Directory -Path "test_dir" | Out-Null
New-Item -ItemType Directory -Path "test_dir\subdir" | Out-Null

# Crear archivos de prueba
Write-Host "[PREPARACION] Creando archivos de prueba..." -ForegroundColor Cyan

Set-Content -Path "test_dir\archivo1.txt" -Value "Este es un archivo de texto de prueba para compresion."
Set-Content -Path "test_dir\archivo2.txt" -Value "Contenido repetitivo: ABC ABC ABC ABC ABC ABC ABC"
Set-Content -Path "test_dir\archivo3.txt" -Value "Archivo con muchos espacios:   a   b   c   d   e   f   g   h   i   j"
Set-Content -Path "test_dir\subdir\archivo4.txt" -Value "Archivo en subdirectorio"

# Crear un archivo más grande para mejor prueba de compresión
$largeContent = "A" * 1000 + "B" * 500 + "C" * 200
Set-Content -Path "test_dir\archivo_grande.txt" -Value $largeContent

Write-Host ""

# Test 1: Compresión de archivo simple
Test-Command -TestName "Compresion de archivo simple" `
    -Command ".\fileutil.exe -c -i test_dir\archivo1.txt -o test_output\archivo1.huf" `
    -Verification { Test-Path "test_output\archivo1.huf" }

# Test 2: Descompresión de archivo
Test-Command -TestName "Descompresion de archivo" `
    -Command ".\fileutil.exe -d -i test_output\archivo1.huf -o test_output\archivo1_decomp.txt" `
    -Verification {
        if (Test-Path "test_output\archivo1_decomp.txt") {
            $original = Get-Content "test_dir\archivo1.txt" -Raw
            $decompressed = Get-Content "test_output\archivo1_decomp.txt" -Raw
            return $original -eq $decompressed
        }
        return $false
    }

# Test 3: Encriptación de archivo
Test-Command -TestName "Encriptacion de archivo" `
    -Command ".\fileutil.exe -e -i test_dir\archivo2.txt -o test_output\archivo2.enc -k 'clave_secreta_123'" `
    -Verification { Test-Path "test_output\archivo2.enc" }

# Test 4: Desencriptación de archivo
Test-Command -TestName "Desencriptacion de archivo" `
    -Command ".\fileutil.exe -u -i test_output\archivo2.enc -o test_output\archivo2_decrypt.txt -k 'clave_secreta_123'" `
    -Verification {
        if (Test-Path "test_output\archivo2_decrypt.txt") {
            $original = Get-Content "test_dir\archivo2.txt" -Raw
            $decrypted = Get-Content "test_output\archivo2_decrypt.txt" -Raw
            return $original -eq $decrypted
        }
        return $false
    }

# Test 5: Compresión y encriptación combinadas
Test-Command -TestName "Compresion y encriptacion combinadas" `
    -Command ".\fileutil.exe -ce -i test_dir\archivo3.txt -o test_output\archivo3.huf.enc -k 'mi_clave_secreta'" `
    -Verification { Test-Path "test_output\archivo3.huf.enc" }

# Test 6: Desencriptación y descompresión combinadas
Test-Command -TestName "Desencriptacion y descompresion combinadas" `
    -Command ".\fileutil.exe -du -i test_output\archivo3.huf.enc -o test_output\archivo3_restored.txt -k 'mi_clave_secreta'" `
    -Verification {
        if (Test-Path "test_output\archivo3_restored.txt") {
            $original = Get-Content "test_dir\archivo3.txt" -Raw
            $restored = Get-Content "test_output\archivo3_restored.txt" -Raw
            return $original -eq $restored
        }
        return $false
    }

# Test 7: Procesamiento de directorio completo (concurrencia)
Test-Command -TestName "Procesamiento de directorio completo (concurrencia)" `
    -Command ".\fileutil.exe -ce -i test_dir -o test_output\test_dir_compressed -k 'clave_directorio'" `
    -Verification { Test-Path "test_output\test_dir_compressed\archivo1.txt.huf.enc" }

# Test 8: Verificar que los archivos procesados del directorio se pueden restaurar
Test-Command -TestName "Restauracion de archivos de directorio" `
    -Command ".\fileutil.exe -du -i test_output\test_dir_compressed\archivo1.txt.huf.enc -o test_output\archivo1_from_dir.txt -k 'clave_directorio'" `
    -Verification {
        if (Test-Path "test_output\archivo1_from_dir.txt") {
            $original = Get-Content "test_dir\archivo1.txt" -Raw
            $restored = Get-Content "test_output\archivo1_from_dir.txt" -Raw
            return $original -eq $restored
        }
        return $false
    }

# Test 9: Archivo vacío
Test-Command -TestName "Archivo vacio" `
    -Command {
        New-Item -ItemType File -Path "test_dir\archivo_vacio.txt" -Force | Out-Null
        .\fileutil.exe -c -i test_dir\archivo_vacio.txt -o test_output\archivo_vacio.huf
        .\fileutil.exe -d -i test_output\archivo_vacio.huf -o test_output\archivo_vacio_restored.txt
    } `
    -Verification {
        Test-Path "test_output\archivo_vacio_restored.txt"
    }

# Test 10: Verificar que archivos encriptados son diferentes a los originales
Test-Command -TestName "Verificacion de seguridad (archivo encriptado diferente)" `
    -Command {
        # Este test ya se ejecutó antes, solo verificamos
    } `
    -Verification {
        if ((Test-Path "test_output\archivo2.enc") -and (Test-Path "test_dir\archivo2.txt")) {
            $originalBytes = [System.IO.File]::ReadAllBytes("test_dir\archivo2.txt")
            $encryptedBytes = [System.IO.File]::ReadAllBytes("test_output\archivo2.enc")
            if ($originalBytes.Length -ne $encryptedBytes.Length) {
                return $true
            }
            for ($i = 0; $i -lt $originalBytes.Length; $i++) {
                if ($originalBytes[$i] -ne $encryptedBytes[$i]) {
                    return $true
                }
            }
            return $false
        }
        return $false
    }

# Resumen de resultados
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "RESUMEN DE RESULTADOS" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Pruebas ejecutadas: $script:TestCount" -ForegroundColor White
Write-Host "Pruebas exitosas: $script:PassCount" -ForegroundColor Green
Write-Host "Pruebas fallidas: $script:ErrorCount" -ForegroundColor $(if ($script:ErrorCount -eq 0) { "Green" } else { "Red" })
Write-Host ""

if ($script:ErrorCount -eq 0) {
    Write-Host "[SUCCESS] Todas las pruebas pasaron exitosamente!" -ForegroundColor Green
    Write-Host ""
    Write-Host "Archivos de prueba creados en: test_output\" -ForegroundColor Cyan
    Write-Host "Puedes revisar los archivos generados para verificar los resultados." -ForegroundColor Cyan
} else {
    Write-Host "[FAILURE] Algunas pruebas fallaron. Revisa los mensajes de error arriba." -ForegroundColor Red
}

Write-Host ""
Write-Host "Archivos de prueba preservados en: test_output\ y test_dir\" -ForegroundColor Yellow
Write-Host "Para limpiar, ejecuta: Remove-Item -Recurse -Force test_output, test_dir" -ForegroundColor Yellow
Write-Host ""