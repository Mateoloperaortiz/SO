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

# 4 Preguntas de Pensamiento Crítico

1. Memoria: ¿Por qué usar *apuntadores* reduce 75% de memoria con 10M registros?  
2. Datos: Si el calendario depende de dígitos del documento, ¿cómo optimizar búsquedas por grupo?  
3. Localidad: ¿Cómo afecta el acceso a memoria al usar *array de structs* vs. *vector de clases*?  
4. Escalabilidad (Consulta): Si los datos exceden la RAM, ¿cómo usar `mmap` o memoria virtual?
