# Metodología de implementación y medición

## Qué medí y cómo

* **Variantes:**

  * `generador/ConMedidor/medida_estructura/` → Struct (C)
  * `generador/ConMedidor/medida_clases/` → Class (C++)

* **Métricas:**

  * `Monitor` registra **tiempo** en ms y **memoria** en KB.
  * Soporta **resumen en consola** y **exporte a CSV** (`estadisticas.csv`).

* **Datos de entrada:**

  * Generador sintético realista: nombres, ciudades de Colombia, fechas, ingresos/patrimonio, deudas, declarantes.
  * **Calendario tributario A/B/C** según los **últimos 2 dígitos del ID**:

    * A: 00–39, B: 40–79, C: 80–99.

* **Flujo de prueba recomendado:**

  * Menú: `0 → 4 → 5 → 7 → 8 → 9`.
  * Evito el **\[6] listados** con N grande por el costo de I/O.

---

## Notas puntuales para macOS y Linux

* **Memoria (RSS):**

  * macOS: `Monitor::obtener_memoria()` usa `mach_task_basic_info` vía `task_info(mach_task_self(), ...)` y toma `resident_size` en bytes. Luego lo paso a KB dividiendo por 1024.
  * Linux: leo `/proc/self/statm` y multiplico páginas residentes por `sysconf(_SC_PAGESIZE)/1024`.

* **Tiempo:**

  * En ambos SO uso `std::chrono::high_resolution_clock`.
  * Clases reporta ms con decimales; Estructuras usa `duration_cast<milliseconds>` (enteros).
  * La resolución distinta no cambia la comparación relativa.

* **Unidades:**

  * Memoria siempre en **KB** y tiempos en **ms** para comparar parejo entre SO.

---

## Cómo reproducir (paso a paso)

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

* Salidas CSV:

  * `medida_estructura/estadisticas.csv`
  * `medida_clases/estadisticas.csv`

---

## Resultados de benchmark (1,000,000 registros)

### Clases (C++)

* Crear datos: **145.566 ms**, **148,496 KB**
* Análisis longevidad: **410.487 ms**, **256 KB**
* Análisis patrimonio: **31.6232 ms**, **32 KB**
* **Suma aprox.**: **587.68 ms** | **Pico de memoria** en “Crear datos”: **148,496 KB**

### Estructuras (C)

* Crear datos: **838.000 ms**, **148,496 KB**
* Análisis longevidad: **595.000 ms**, **240 KB**
* Análisis patrimonio: **268.000 ms**, **64 KB**
* **Suma aprox.**: **1701.00 ms** | **Pico de memoria** en “Crear datos”: **148,496 KB**

**Observaciones rápidas**

* Generación: **Clases ≈ 5.75× más rápido** (−82.6%). Misma memoria pico porque el vector guarda los mismos campos.
* Longevidad: **Clases ≈ 1.45×** más rápido (−31%).
* Patrimonio: **Clases ≈ 8.48×** más rápido (−88%), con menos memoria transitoria.

---

## Cobertura de las preguntas obligatorias (vía menú)

* **\[4] Persona más longeva**: global y por ciudad.
  En N grande muestro solo el resumen.
* **\[5] Mayor patrimonio**: global, por ciudad y por grupo (A/B/C) con **formato monetario fijo a 2 decimales**.
* **\[6] Declarantes por calendario**: listar/contar por A/B/C y validar IDs.
  En N grande prefiero conteos para no saturar la salida.

---

## Comparaciones requeridas

* **Struct (C) vs. Class (C++)**: en mis pruebas, **Clases** ganó en tiempo manteniendo la **misma memoria pico** del dataset.
* **Valores vs. Apuntadores**:

  * **Estado actual**: almaceno por **valor** en `std::vector<Persona>`. Retorno `const Persona*` en utilidades para evitar copias, pero no uso un `vector<Persona*>` dedicado.
  * **Cómo compararía**: repetir pruebas con `std::vector<std::unique_ptr<Persona>>` o `std::vector<Persona*>` (con ownership claro) y medir:

    1. tiempo de generación/recorrido,
    2. huella de memoria,
    3. locality y branch prediction.
       Esperado: menos copias intermedias. No siempre baja la memoria total si cada objeto es único; el beneficio es más de **evitar copias** que de reducir la base.

---

## Tres preguntas extra que propongo y cómo las resolví

1. **Ciudades con patrimonio promedio más alto**

   * `unordered_map<string, pair<double,int>>` para sumar y contar.
   * O(N). Muestro Top-5 con promedio y tamaño de muestra.

2. **% de personas > 80 años por calendario (A/B/C)**

   * Cuento por grupo y mayores de 80 con `calcularEdad()`.
   * `pct = mayores80[g] / total[g] * 100`. O(N).

3. **Top-5 ciudades por ratio declarantes/total**

   * `ratio[c] = declarantes[c] / total[c]`.
   * O(N) + sort O(C log C). Reporto ciudad, ratio, declarantes y total.

Estas salen como subopciones nuevas del menú, usando `Monitor` y exporte a CSV.

---

## Conclusiones

* **Rendimiento**: Clases gana en generación y análisis, con **igual pico de memoria** del dataset.
* **Localidad y acceso**: Recorridos lineales en `std::vector` favorecen la cache; usar referencias o `const Persona*` ayuda a **evitar copias** innecesarias.
* **Salida**: Formato monetario fijo a 2 decimales para que los números sean legibles y comparables.
* **Escalabilidad**: Con ≥1M registros, evitar listados masivos. Enfocarse en agregados O(N) y CSV para análisis offline.

---

## Respuestas a las 4 preguntas de pensamiento crítico

1. **Memoria y punteros**: Los punteros evitan copiar objetos grandes al pasarlos o retornarlos, lo que reduce memoria transitoria y tráfico de cache. Un “ahorro 75%” depende del patrón: si varias estructuras comparten una instancia, sí se ahorra bastante; si cada objeto es único, el ahorro real está en **evitar copias temporales**, no en la base.
2. **Búsquedas por grupo (A/B/C)**: Mapeo A/B/C con O(1) usando `grupoCalendarioPorID()` y `unordered_map<char, ...>` o buckets fijos. También puedo precomputar listas por grupo al generar.
3. **Localidad (AoS vs. SoA)**: `vector<clase>` es AoS. Va bien si leo varios campos juntos. Si solo uso uno o dos campos (ej. patrimonio), **SoA** puede pegar mejor en cache.
4. **Datos que no caben en RAM**: Particionar por ciudad/grupo, usar archivos mapeados (`mmap`) y streaming por lotes. Mantener índices compactos en memoria y procesar agregados por bloques.

---

## Artefactos y reproducibilidad

* **Binaries**: `programa` en cada variante luego de `make -B`.
* **CSVs**:

  * `generador/ConMedidor/medida_clases/estadisticas.csv`
  * `generador/ConMedidor/medida_estructura/estadisticas.csv`
* **Comandos**: ver la sección “Cómo reproducir”.
