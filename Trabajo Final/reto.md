Recurso: https://link.springer.com/book/10.1007/978-1-84800-072-8
https://link.springer.com/book/10.1007/978-1-4842-9450-5
https://link.springer.com/books/a/1

1. Objetivo General
El presente examen busca evaluar la capacidad de los estudiantes para aplicar conceptos fundamentales de sistemas operativos en el diseño y desarrollo de una aplicación de bajo nivel. El estudiante deberá demostrar dominio en la gestión de archivos mediante llamadas al sistema, la implementación de concurrencia para la optimización del rendimiento y la investigación e implementación de algoritmos de compresión y encriptación desde cero, sin depender de librerías externas para dichas funcionalidades.

2. Descripción del Problema
Se debe desarrollar una utilidad de línea de comandos en C o C++ que permita a un usuario comprimir/descomprimir y encriptar/desencriptar archivos o directorios completos de manera eficiente. La eficiencia se logrará procesando múltiples archivos de forma concurrente.

La aplicación deberá ser invocada desde la terminal y aceptar los siguientes parámetros:

Operación: -c (comprimir), -d (descomprimir), -e (encriptar), -u (desencriptar). Se pueden combinar operaciones (ej. -ce para comprimir y luego encriptar).
Algoritmo de Compresión: --comp-alg [nombre_algoritmo]
Algoritmo de Encriptación: --enc-alg [nombre_algoritmo]
Entrada: -i [ruta_archivo_o_directorio]
Salida: -o [ruta_archivo_o_directorio_salida]
Clave (opcional): -k [clave_secreta] para las operaciones de encriptación/desencriptación.
Funcionalidades Clave:
Gestor de Archivos (File Manager): La aplicación debe operar sobre un único archivo o sobre todos los archivos dentro de un directorio. Debe usar llamadas directas al sistema operativo (ej. open, read, write, close, opendir, readdir en POSIX; o CreateFile, ReadFile, WriteFile en Windows API) en lugar de las abstracciones de librerías estándar como <stdio.h> (fopen, fread).
Concurrencia: Al procesar un directorio, la aplicación debe lanzar un hilo o proceso separado para cada archivo. El objetivo es paralelizar el trabajo y mejorar el rendimiento en sistemas multinúcleo. Se debe gestionar adecuadamente el ciclo de vida de estos hilos/procesos.
Algoritmos Propios: Los algoritmos de compresión y encriptación deben ser implementados por el equipo. Está estrictamente prohibido el uso de librerías externas como zlib, OpenSSL, Crypto++, etc., para estas tareas.
Compresión: El equipo es libre de elegir e implementar cualquier algoritmo de compresión sin pérdida. La elección debe estar justificada en el documento técnico.
Sugerencias: Codificación Huffman, Run-Length Encoding (RLE), Lempel-Ziv-Welch (LZW).
Encriptación: El equipo es libre de elegir e implementar cualquier algoritmo de encriptación simétrica (de clave secreta). La elección debe estar justificada.
Sugerencias: Cifrado Vigenère, DES (implementando sus rondas y operaciones), o una versión simplificada de AES (implementando las operaciones de S-box, ShiftRows, MixColumns).
3. Entregables
Código Fuente: Un archivo .zip o .tar.gz con todo el código fuente en C/C++. El código debe estar bien comentado y estructurado, e incluir un Makefile o script de compilación.
Documento Técnico (PDF): Un documento explicando la solución, que debe incluir:
Introducción: Descripción del proyecto.
Diseño de la Solución: Diagramas de arquitectura y flujo de datos.
Justificación de Algoritmos: (Sección Crítica) Explicación técnica de por qué se eligieron los algoritmos específicos, comparando sus ventajas y desventajas (complejidad, rendimiento, nivel de compresión/seguridad).
Implementación de Algoritmos: Explicación detallada de la implementación.
Estrategia de Concurrencia: Descripción del modelo de concurrencia utilizado.
Guía de Uso: Instrucciones claras sobre cómo compilar y ejecutar el programa.
Descripción de un Caso de Uso Válido: Un documento de una página describiendo un escenario real donde la herramienta sería útil.
Video de Sustentación: Un video (máximo 10 minutos) con una demostración en vivo del programa y una explicación de las partes más relevantes del código (llamadas al sistema, concurrencia y algoritmos).
4. Rúbrica de Evaluación (Total: 100 puntos)
Criterio	Descripción de la Evaluación	Puntaje Máximo
Funcionalidad del Programa (30 pts)	Compresión/Descompresión: El programa comprime y descomprime sin corrupción de datos.	10
Encriptación/Desencriptación: El programa encripta y desencripta correctamente según la clave.	10
Manejo de Argumentos y Errores: Interpreta correctamente los parámetros y gestiona fallos de forma robusta.	10
Aplicación de Conceptos de SO (40 pts)	Uso de Llamadas al Sistema: Usa exclusivamente llamadas al sistema para la manipulación de archivos.	15
Implementación de Concurrencia: La concurrencia está bien implementada, gestionando correctamente hilos/procesos y mejorando el rendimiento.	15
Manejo Eficiente de Recursos: No presenta fugas de memoria, descriptores de archivos, o procesos "zombie".	10
Calidad del Código y Algoritmos (20 pts)	Implementación y Justificación de Algoritmos: Los algoritmos elegidos son implementados correctamente desde cero. La justificación de su elección es sólida y demuestra comprensión de sus características.	15
Estructura y Claridad: El código es legible, modular y está bien documentado.	5
Documentación y Presentación (10 pts)	Calidad de Entregables: El documento técnico, el caso de uso y el video son claros, completos y profesionales.	10
5. Ejemplo de un Caso de Uso Válido
Escenario: Una startup de biotecnología genera grandes volúmenes de datos de secuenciación genética. Estos datos son altamente repetitivos (ideales para compresión) y extremadamente confidenciales.

Problema:

Confidencialidad: Proteger la propiedad intelectual y los datos de los pacientes es una prioridad máxima.
Almacenamiento: Los archivos de datos crudos son masivos y costosos de almacenar a largo plazo.
Eficiencia: El procesamiento diario de lotes de datos debe ser lo más rápido posible.
Solución con la Herramienta "GSEA": Un bioinformático podría automatizar el archivado de los resultados de un día con un script que ejecute:

./gsea -ce --comp-alg lzw --enc-alg des -i "./Resultados/2025-10-15/" -o "./Archivados/2025-10-15.bak" -k "G3n0m3S3cur1ty!"

Resultado: La herramienta procesaría concurrentemente cada archivo de resultados en el directorio. Gracias a la elección del algoritmo LZW, obtendría una excelente tasa de compresión debido a la naturaleza repetitiva de los datos genéticos. La encriptación con DES aseguraría que los datos permanezcan confidenciales, cumpliendo con las normativas de protección de datos y asegurando la propiedad intelectual de la empresa.