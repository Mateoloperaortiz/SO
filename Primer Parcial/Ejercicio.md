# Sistemas Operativos **Manejo de Grandes Volúmenes de Datos en Linux con C/C++**

*2025B*

# Objetivo

Desarrollar un programa en C/C++ (Linux) que procese grandes volúmenes de datos generados por el Generador de Datos, midiendo tiempo/memoria y comparando:

* Valores vs. apuntadores  
* Struct (C) vs. Class (C++)

# Datos Clave

Cada registro incluye (como mínimo):

* Nombre completo  
* Fecha de nacimiento (para cálculo de edad)  
* Lugar de residencia (ciudad en Colombia)  
* Patrimonio - Deudas  
* Documento de identidad  
* Calendario de declaración de renta (A/B/C):  
  * Asignado según los dos últimos dígitos del documento  
  * Siguiendo el Calendario Tributario DIAN 2025: 
    * `Grupo A`: Dígitos 00-39  
    * `Grupo B`: Dígitos 40-79  
    * `Grupo C`: Dígitos 80-99

Aquí tienes el calendario tributario de la DIAN para 2025 en formato Markdown.

### **Calendario tributario 2025**

**Plazos para el cumplimiento de las obligaciones tributarias administradas por la Unidad Administrativa Especial Dirección de Impuestos y Aduanas Nacionales - DIAN**

---

### **Renta Personas Naturales**

| **Hasta agosto** | 12 | 13 | 14 | 15 | 19 | 20 | 21 | 22 | 25 | 26 | 27 | 28 | 29 | Últimos dígitos del NIT |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| *Declaración y pago* | 01-02 | 03-04 | 05-06 | 07-08 | 09-10 | 11-12 | 13-14 | 15-16 | 17-18 | 19-20 | 21-22 | 23-24 | 25-26 | |
| **Hasta septiembre** | 1 | 2 | 3 | 4 | 5 | 8 | 9 | 10 | 11 | 12 | 15 | |
| | 27-28 | 29-30 | 31-32 | 33-34 | 35-36 | 37-38 | 39-40 | 41-42 | 43-44 | 45-46 | 47-48 | |
| | 16 | 17 | 18 | 19 | 22 | 23 | 24 | 25 | 26 | | | | |
| | 49-50 | 51-52 | 53-54 | 55-56 | 57-58 | 59-60 | 61-62 | 63-64 | 65-66 | | | | |
| **Hasta octubre** | 1 | 2 | 3 | 6 | 7 | 8 | 9 | 10 | 14 | 15 | 16 | 17 | |
| *Declaración y pago* | 67-68 | 69-70 | 71-72 | 73-74 | 75-76 | 77-78 | 79-80 | 81-82 | 83-84 | 85-86 | 87-88 | 89-90 | |
| | 20 | 21 | 22 | 23 | 24 | | | | | | | | |
| | 91-92 | 93-94 | 95-96 | 97-98 | 99-00 | | | | | | | | |

# Preguntas Obligatorias

1. 👵 Persona más longeva:  
   * En todo el país  
   * Por cada ciudad  
2. 💰 Persona con mayor patrimonio:  
   * En el país  
   * Por ciudad  
   * Por grupo de declaración (A/B/C)  
3. 📅 Declarantes de renta:  
   * Listar y contar personas por calendario (puede usar el calendario Grupo A, Grupo B y Grupo C en lugar del calendarios tributario de la DIAN)  
   * Validar asignación según terminación del documento

## Tres Preguntas Adicionales

*(Diseñadas por el equipo de trabajo)*  
*Ejemplos:*

1. Ciudades con patrimonio promedio más alto  
2. Porcentaje de personas \> 80 años por calendario

# Requerimientos Técnicos

* Comparaciones críticas:  
  * Rendimiento: Valores vs. Apuntadores  
  * Eficiencia: Struct (C) vs. Class (C++)  
* Métricas obligatorias:  
  * Tiempo de ejecución  
  * Uso de memoria (RAM)  
* Optimización:  
  * Algoritmos eficientes para \>1 millón de registros

# Entrega

* 📦 Qué entregar:

| Componente | Formato |
| :---- | :---- |
| Código | C/C++ (Linux) |
| Análisis | PDF con resultados y métricas |
| Explicación | Video (5 min máximo) |

* ⏰ Plazo: 1 semana (buzón de Interactiva)  
* 👥 Equipos:  
  * Todos deben subir la misma entrega  
  * Nota 0.0 si algún miembro no publica

# Rúbrica Simplificada

| Aspecto | Puntos | Criterios Clave |
| :---- | :---- | :---- |
| Funcionalidad | 1.0 | Respuestas correctas \+ Preguntas adicionales relevantes |
| Eficiencia | 1.5 | Análisis valores/apuntadores \+ Struct/Class \+ Métricas |
| Calidad de Código | 1.0 | Legibilidad \+ Documentación |
| Claridad | 0.5 | Informe y video comprensibles |
| Pensamiento Crítico | 1.0 | Profundidad en 4 preguntas técnicas |

#

# 4 Preguntas de Pensamiento Crítico

1. Memoria: ¿Por qué usar *apuntadores* reduce 75% de memoria con 10M registros?  
2. Datos: Si el calendario depende de dígitos del documento, ¿cómo optimizar búsquedas por grupo?  
3. Localidad: ¿Cómo afecta el acceso a memoria al usar *array de structs* vs. *vector de clases*?  
4. Escalabilidad (Consulta): Si los datos exceden la RAM, ¿cómo usar `mmap` o memoria virtual?

## Metodología de implementación y medición

* **Variantes medidas**:
  * `generador/ConMedidor/medida_estructura/` (Struct/C)
  * `generador/ConMedidor/medida_clases/` (Class/C++)
* **Métricas**: `Monitor` mide tiempo (ms) y memoria (KB); permite mostrar resumen y exportar CSV (`estadisticas.csv`).
* **Carga de datos**: Generador sintético realista (nombres, ciudades de Colombia, fechas, ingresos/patrimonio, deudas, declarantes). Calendario A/B/C según últimos 2 dígitos del ID: A=00–39, B=40–79, C=80–99.
* **Flujo de prueba**: Menú 0→4→5→7→8→9. Opción 6 (listados) se evita en N grande por I/O masivo.

### Notas específicas de macOS

* **Memoria (RSS)**: `Monitor::obtener_memoria()` usa `mach_task_basic_info` vía `task_info(mach_task_self(), ...)` para leer `resident_size` (bytes) y lo convierte a KB dividiendo por 1024. En Linux se usa `/proc/self/statm` multiplicando páginas residentes por `sysconf(_SC_PAGESIZE)/1024`.
* **Tiempo**: en ambos SO se usa `std::chrono::high_resolution_clock`. En la variante de clases se reportan milisegundos con decimales; en estructuras se hace `duration_cast<milliseconds>` (enteros). Las diferencias de resolución no afectan la comparación relativa.
* **Unidades**: todos los reportes de memoria están normalizados en KB y los tiempos en ms para facilitar la comparación entre SO.

### Cómo reproducir (ejemplos)

```bash
# Struct (C)
cd generador/ConMedidor/medida_estructura
make -B
printf "0\n1000000\n4\n5\n7\n8\n9\n" | ./programa

# Clases (C++)
cd generador/ConMedidor/medida_clases
make -B
printf "0\n1000000\n4\n5\n7\n8\n9\n" | ./programa
```

## Resultados de benchmark (1,000,000 registros)

  Rutas de salida: `medida_estructura/estadisticas.csv` y `medida_clases/estadisticas.csv`.

* **Clases**
  * Crear datos: 145.566 ms, 148,496 KB
  * Análisis longevidad: 410.487 ms, 256 KB
  * Análisis patrimonio: 31.6232 ms, 32 KB
  * Suma aprox.: 587.68 ms | Pico en 'Crear datos': 148,496 KB

* **Estructuras**
  * Crear datos: 838.000 ms, 148,496 KB
  * Análisis longevidad: 595.000 ms, 240 KB
  * Análisis patrimonio: 268.000 ms, 64 KB
  * Suma aprox.: 1701.00 ms | Pico en 'Crear datos': 148,496 KB

### Observaciones

* Generación: Clases ≈5.75× más rápido (−82.6%). Memoria pico idéntica (vector con mismos campos).
* Longevidad: Clases ≈1.45× más rápido (−31%).
* Patrimonio: Clases ≈8.48× más rápido (−88%). Menor memoria transitoria.

## Cobertura de las Preguntas Obligatorias (en el programa)

Las opciones del menú resuelven las preguntas:

* **[4] Persona más longeva**: global y por ciudad.
  Nota: Se imprime el resultado en consola (solo resumen en N grande).
* **[5] Mayor patrimonio**: global, por ciudad y por grupo (A/B/C) con formato monetario fijo (2 decimales).
  Nota: Se usa fixed con 2 decimales para evitar notación científica.
* **[6] Declarantes por calendario**: listar/contar por A/B/C y validar IDs.
  Nota: en N grande, evitar listados extensos por I/O.

## Comparaciones requeridas

* **Struct (C) vs. Class (C++)**: ver resultados de benchmark arriba. En estas tareas, la variante con clases fue consistentemente más rápida con igual memoria pico del dataset.
* **Valores vs. Apuntadores**:
  * **Estado actual**: El almacenamiento base en ambas variantes es `std::vector<Persona>` (por valor). Varias utilidades retornan `const Persona*` para evitar copias, pero no se mantiene un `vector<Persona*>` dedicado.
  * **Metodología sugerida de comparación**: Duplicar pruebas con una versión que almacene `std::vector<std::unique_ptr<Persona>>` o `std::vector<Persona*>` (con ownership claro) y medir: (a) tiempo de generación/recorrido, (b) huella de memoria, (c) impacto en locality y branch prediction. Considerar over‑head de asignaciones en heap y punteros (8 bytes c/u). Esperable: menos copias intermedias, pero no necesariamente menor memoria total si los objetos son únicos; sí puede mejorar ciertos patrones de paso por referencia.

## Tres Preguntas Adicionales (propuestas) y cómo resolverlas

1) **Ciudades con patrimonio promedio más alto**  
   * Agregado: `prom[c] = sumaPatrimonio[c] / cnt[c]` usando `unordered_map<string, pair<double,int>>`.  
   * Complejidad: O(N).  
   * Salida: Top‑5 ciudades con promedio y tamaño de muestra.

2) **Porcentaje de personas > 80 años por calendario (A/B/C)**  
   * Cómputo: contar por grupo (A/B/C) y mayores de 80 con `calcularEdad()`.  
   * Métrica: `pct = mayores80[g] / total[g] * 100`.  
   * Complejidad: O(N).

3) **Top‑5 ciudades por ratio declarantes/total**  
   * Cómputo: `ratio[c] = declarantes[c] / total[c]`.  
   * Complejidad: O(N) + ordenamiento O(C log C).  
   * Salida: ciudad, ratio, declarantes, total.

> Nota: Estas métricas se integran naturalmente al menú como nuevas sub‑opciones de análisis con `Monitor` y exporte CSV.

## Conclusiones

* **Rendimiento**: En generación y análisis medidos, la variante con clases supera a la de estructuras manteniendo igual memoria pico del dataset.
* **Localidad y acceso**: Recorridos lineales sobre `std::vector` favorecen cache; evitar copias mediante referencias/`const Persona*` reduce overhead.
* **Formato de salida**: Monetario en punto fijo (2 decimales) evita notación científica, facilitando lectura y comparación.
* **Escalabilidad**: En ≥1M, evitar listados masivos; centrarse en agregados O(N) y exporte CSV para análisis offline.

## Respuestas a las 4 Preguntas de Pensamiento Crítico

1) **Memoria y apuntadores**: Los punteros evitan copiar objetos grandes al pasar/retornar, reduciendo memoria transitoria y tráfico de cache. Un ahorro "75%" depende del patrón (tamaño del objeto vs. número de copias/redundancias). Si se comparte una sola instancia referenciada desde múltiples estructuras, el ahorro es significativo; si cada objeto es único, el beneficio principal es evitar copias temporales más que disminuir la huella base.
2) **Búsquedas por grupo (A/B/C)**: Mapear A/B/C en un índice O(1) usando `grupoCalendarioPorID()` y contenedores `unordered_map<char, ...>` o buckets (3 contadores/vectores). Para consultas, almacenar offsets o listas por grupo al generar.
3) **Localidad (AoS vs. SoA)**: `array of structs` (AoS) favorece recorridos que leen varios campos contiguos; `struct of arrays` (SoA) mejora cuando se accede a pocos campos (e.g., solo patrimonio) con mayor densidad de cache. `vector<clase>` es AoS; convertir a SoA puede acelerar análisis específicos.
4) **Datos > RAM**: Usar técnicas out‑of‑core: particionado por ciudad/grupo, archivos mapeados (`mmap`) para lectura secuencial, y/o streaming por lotes. Mantener índices compactos en memoria y procesar agregados por bloques.

## Reproducibilidad y artefactos

* **Binaries**: `programa` en cada variante tras `make -B`.
* **CSVs**: `generador/ConMedidor/medida_clases/estadisticas.csv`, `generador/ConMedidor/medida_estructura/estadisticas.csv`.
* **Comandos recomendados**: ver sección "Cómo reproducir".
