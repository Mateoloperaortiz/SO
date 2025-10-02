# Reto de Programación: Sistema de Chat con Colas de Mensajes

## Descripción del Reto

En el mundo actual, las aplicaciones de mensajería instantánea como WhatsApp son fundamentales para la comunicación diaria. En este reto, los estudiantes deberán implementar un sistema de chat simple entre procesos utilizando colas de mensajes (message queues) como mecanismo de comunicación entre procesos (IPC). El sistema debe soportar múltiples salas de chat, donde los usuarios pueden unirse a diferentes salas y enviar mensajes que serán recibidos por todos los miembros de la sala.

### Objetivos del Reto

1. Implementar un sistema de chat que permita la comunicación entre múltiples procesos.
2. Utilizar colas de mensajes como mecanismo de IPC.
3. Soportar múltiples salas de chat, donde cada sala tiene su propia cola de mensajes.
4. Implementar un servidor central que gestione las salas y los mensajes.
5. Permitir que los procesos clientes se unan a salas, envíen mensajes y reciban mensajes de la sala.

### Requisitos Técnicos

- **Lenguaje**: C
- **Mecanismo IPC**: Colas de mensajes (System V o POSIX)
- **Sistema Operativo**: Linux/Mac OS
- **Trabajo en equipo**: Mateo Lopera
- **Fecha de entrega**: Octubre 5, 2025

## Ejemplo Práctico: Funcionamiento del Sistema

### Arquitectura del Sistema

El sistema estará compuesto por:

1. **Servidor Central**: Un proceso que se encarga de:
   - Crear y gestionar las colas de mensajes para cada sala.
   - Recibir mensajes de los clientes y reenviarlos a todos los miembros de la sala.
   - Gestionar la lista de salas y los usuarios en cada sala.

2. **Clientes**: Procesos que representan a los usuarios. Cada cliente:
   - Se une a una sala específica.
   - Puede enviar mensajes a la sala.
   - Recibe todos los mensajes enviados a la sala.

### Flujo de Comunicación

1. **Inicialización**:
   - El servidor crea una cola de mensajes global para solicitudes de clientes.
   - Los clientes se conectan al servidor enviando un mensaje a la cola global.

2. **Unirse a una sala**:
   - Un cliente envía un mensaje de tipo "JOIN" a la cola global, indicando la sala a la que quiere unirse.
   - El servidor crea una cola para la sala (si no existe) y añade al cliente a la lista de esa sala.
   - El servidor responde al cliente con el identificador de la cola de la sala.

3. **Enviar un mensaje**:
   - Un cliente envía un mensaje a la cola de su sala.
   - El servidor recibe el mensaje y lo reenvía a todos los clientes de la sala (excepto al remitente).

4. **Recibir mensajes**:
   - Cada cliente está escuchando en la cola de su sala.
   - Cuando llega un mensaje, el cliente lo muestra en pantalla.

### Ejemplo de Ejecución

Supongamos que tenemos dos salas: "General" y "Deportes". Tres clientes: María, Juan y Camila.

1. **Servidor**:
   ```bash
   $ ./servidor
   Servidor de chat iniciado. Esperando clientes...
   ```

2. **Cliente María** (se une a "General"):
   ```bash
   $ ./cliente María
   Bienvenido, María. Salas disponibles: General, Deportes
   > join General
   Te has unido a la sala: General
   > Hola a todos en General!
   ```

3. **Cliente Juan** (se une a "General"):
   ```bash
   $ ./cliente Juan
   Bienvenido, Juan. Salas disponibles: General, Deportes
   > join General
   Te has unido a la sala: General
   > Hola María!
   ```

4. **Cliente Camila** (se une a "Deportes"):
   ```bash
   $ ./cliente Camila
   Bienvenido, Camila. Salas disponibles: General, Deportes
   > join Deportes
   Te has unido a la sala: Deportes
   > ¿Alguien vio el partido del América?
   ```

5. **Salida en María**:
   ```
   Hola a todos en General!
   Juan: Hola María!
   ```

6. **Salida en Juan**:
   ```
   María: Hola a todos en General!
   ```

7. **Salida en Camila**:
   ```
   ¿Alguien vio el partido del América?
   ```

## Código Base para Cliente/Servidor

Para aquellos estudiantes que no han visto redes de datos, aquí se proporciona un código base que utiliza colas de mensajes de System V. Este código base incluye la estructura básica del servidor y del cliente, pero está incompleto y debe ser extendido para cumplir con todos los requisitos.

### servidor.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MAX_SALAS 10
#define MAX_USUARIOS_POR_SALA 20
#define MAX_TEXTO 256
#define MAX_NOMBRE 50

// Estructura para los mensajes
struct mensaje {
    long mtype;         // Tipo de mensaje
    char remitente[MAX_NOMBRE];
    char texto[MAX_TEXTO];
    char sala[MAX_NOMBRE];
};

// Estructura para una sala de chat
struct sala {
    char nombre[MAX_NOMBRE];
    int cola_id;        // ID de la cola de mensajes de la sala
    int num_usuarios;
    char usuarios[MAX_USUARIOS_POR_SALA][MAX_NOMBRE];
};

struct sala salas[MAX_SALAS];
int num_salas = 0;

// Función para crear una nueva sala
int crear_sala(const char *nombre) {
    if (num_salas >= MAX_SALAS) {
        return -1; // No se pueden crear más salas
    }

    // Crear una cola de mensajes para la sala
    key_t key = ftok("/tmp", num_salas + 1); // Generar una clave única
    int cola_id = msgget(key, IPC_CREAT | 0666);
    if (cola_id == -1) {
        perror("Error al crear la cola de la sala");
        return -1;
    }

    // Inicializar la sala
    strcpy(salas[num_salas].nombre, nombre);
    salas[num_salas].cola_id = cola_id;
    salas[num_salas].num_usuarios = 0;

    num_salas++;
    return num_salas - 1; // Retornar el índice de la sala creada
}

// Función para buscar una sala por nombre
int buscar_sala(const char *nombre) {
    for (int i = 0; i < num_salas; i++) {
        if (strcmp(salas[i].nombre, nombre) == 0) {
            return i;
        }
    }
    return -1; // No encontrada
}

// Función para agregar un usuario a una sala
int agregar_usuario_a_sala(int indice_sala, const char *nombre_usuario) {
    if (indice_sala < 0 || indice_sala >= num_salas) {
        return -1;
    }

    struct sala *s = &salas[indice_sala];
    if (s->num_usuarios >= MAX_USUARIOS_POR_SALA) {
        return -1; // Sala llena
    }

    // Verificar si el usuario ya está en la sala
    for (int i = 0; i < s->num_usuarios; i++) {
        if (strcmp(s->usuarios[i], nombre_usuario) == 0) {
            return -1; // Usuario ya está en la sala
        }
    }

    // Agregar el usuario
    strcpy(s->usuarios[s->num_usuarios], nombre_usuario);
    s->num_usuarios++;
    return 0;
}

// Función para enviar un mensaje a todos los usuarios de una sala
void enviar_a_todos_en_sala(int indice_sala, struct mensaje *msg) {
    if (indice_sala < 0 || indice_sala >= num_salas) {
        return;
    }

    struct sala *s = &salas[indice_sala];
    for (int i = 0; i < s->num_usuarios; i++) {
        // Enviar el mensaje a la cola de la sala
        if (msgsnd(s->cola_id, msg, sizeof(struct mensaje) - sizeof(long), 0) == -1) {
            perror("Error al enviar mensaje a la sala");
        }
    }
}

int main() {
    // Crear la cola global para solicitudes de clientes
    key_t key_global = ftok("/tmp", 'A');
    int cola_global = msgget(key_global, IPC_CREAT | 0666);
    if (cola_global == -1) {
        perror("Error al crear la cola global");
        exit(1);
    }

    printf("Servidor de chat iniciado. Esperando clientes...\n");

    struct mensaje msg;

    while (1) {
        // Recibir mensajes de la cola global
        if (msgrcv(cola_global, &msg, sizeof(struct mensaje) - sizeof(long), 0, 0) == -1) {
            perror("Error al recibir mensaje");
            continue;
        }

        // Procesar el mensaje según su tipo
        if (msg.mtype == 1) { // JOIN
            printf("Solicitud de unirse a la sala: %s por %s\n", msg.sala, msg.remitente);

            // Buscar o crear la sala
            int indice_sala = buscar_sala(msg.sala);
            if (indice_sala == -1) {
                indice_sala = crear_sala(msg.sala);
                if (indice_sala == -1) {
                    printf("No se pudo crear la sala %s\n", msg.sala);
                    continue;
                }
                printf("Nueva sala creada: %s\n", msg.sala);
            }

            // Agregar el usuario a la sala
            if (agregar_usuario_a_sala(indice_sala, msg.remitente) == 0) {
                printf("Usuario %s agregado a la sala %s\n", msg.remitente, msg.sala);

                // Enviar confirmación al cliente (usando el mismo tipo de mensaje)
                msg.mtype = 2; // Tipo de respuesta
                sprintf(msg.texto, "Te has unido a la sala: %s", msg.sala);
                if (msgsnd(cola_global, &msg, sizeof(struct mensaje) - sizeof(long), 0) == -1) {
                    perror("Error al enviar confirmación");
                }
            } else {
                printf("No se pudo agregar al usuario %s a la sala %s\n", msg.remitente, msg.sala);
            }
        } else if (msg.mtype == 3) { // MSG
            printf("Mensaje en la sala %s de %s: %s\n", msg.sala, msg.remitente, msg.texto);

            // Buscar la sala
            int indice_sala = buscar_sala(msg.sala);
            if (indice_sala != -1) {
                // Reenviar el mensaje a todos en la sala
                enviar_a_todos_en_sala(indice_sala, &msg);
            }
        }
    }

    return 0;
}
```

### cliente.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_TEXTO 256
#define MAX_NOMBRE 50

// Estructura para los mensajes
struct mensaje {
    long mtype;         // Tipo de mensaje
    char remitente[MAX_NOMBRE];
    char texto[MAX_TEXTO];
    char sala[MAX_NOMBRE];
};

int cola_global;
int cola_sala = -1;
char nombre_usuario[MAX_NOMBRE];
char sala_actual[MAX_NOMBRE] = "";

// Función para el hilo que recibe mensajes
void *recibir_mensajes(void *arg) {
    struct mensaje msg;

    while (1) {
        if (cola_sala != -1) {
            // Recibir mensajes de la cola de la sala
            if (msgrcv(cola_sala, &msg, sizeof(struct mensaje) - sizeof(long), 0, 0) == -1) {
                perror("Error al recibir mensaje de la sala");
                continue;
            }

            // Mostrar el mensaje si no es del propio usuario
            if (strcmp(msg.remitente, nombre_usuario) != 0) {
                printf("%s: %s\n", msg.remitente, msg.texto);
            }
        }
        usleep(100000); // Pequeña pausa para no consumir demasiado CPU
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <nombre_usuario>\n", argv[0]);
        exit(1);
    }

    strcpy(nombre_usuario, argv[1]);

    // Conectarse a la cola global
    key_t key_global = ftok("/tmp", 'A');
    cola_global = msgget(key_global, 0666);
    if (cola_global == -1) {
        perror("Error al conectar a la cola global");
        exit(1);
    }

    printf("Bienvenido, %s. Salas disponibles: General, Deportes\n", nombre_usuario);

    // Crear un hilo para recibir mensajes
    pthread_t hilo_receptor;
    pthread_create(&hilo_receptor, NULL, recibir_mensajes, NULL);

    struct mensaje msg;
    char comando[MAX_TEXTO];

    while (1) {
        printf("> ");
        fgets(comando, MAX_TEXTO, stdin);
        comando[strcspn(comando, "\n")] = '\0'; // Eliminar el salto de línea

        if (strncmp(comando, "join ", 5) == 0) {
            // Comando para unirse a una sala
            char sala[MAX_NOMBRE];
            sscanf(comando, "join %s", sala);

            // Enviar solicitud de JOIN al servidor
            msg.mtype = 1; // JOIN
            strcpy(msg.remitente, nombre_usuario);
            strcpy(msg.sala, sala);
            strcpy(msg.texto, "");

            if (msgsnd(cola_global, &msg, sizeof(struct mensaje) - sizeof(long), 0) == -1) {
                perror("Error al enviar solicitud de JOIN");
                continue;
            }

            // Esperar confirmación del servidor
            if (msgrcv(cola_global, &msg, sizeof(struct mensaje) - sizeof(long), 2, 0) == -1) {
                perror("Error al recibir confirmación");
                continue;
            }

            printf("%s\n", msg.texto);

            // Obtener la cola de la sala
            key_t key_sala = ftok("/tmp", atoi(sala)); // Esto es un ejemplo, debe ser mejorado
            cola_sala = msgget(key_sala, 0666);
            if (cola_sala == -1) {
                perror("Error al conectar a la cola de la sala");
                continue;
            }

            strcpy(sala_actual, sala);
        } else if (strlen(comando) > 0) {
            // Enviar un mensaje a la sala actual
            if (strlen(sala_actual) == 0) {
                printf("No estás en ninguna sala. Usa 'join <sala>' para unirte a una.\n");
                continue;
            }

            msg.mtype = 3; // MSG
            strcpy(msg.remitente, nombre_usuario);
            strcpy(msg.sala, sala_actual);
            strcpy(msg.texto, comando);

            if (msgsnd(cola_sala, &msg, sizeof(struct mensaje) - sizeof(long), 0) == -1) {
                perror("Error al enviar mensaje");
                continue;
            }
        }
    }

    return 0;
}
```

## Rúbricas de Evaluación

El reto será evaluado en base a los siguientes criterios:

### 1. Funcionalidad (40 puntos)

| Criterio | Puntos | Descripción |
|----------|--------|-------------|
| Creación y gestión de colas | 10 | El servidor crea correctamente las colas para las salas y los clientes se conectan adecuadamente. |
| Unirse a salas | 10 | Los clientes pueden unirse a salas existentes y el servidor gestiona correctamente la lista de miembros. |
| Envío de mensajes | 10 | Los mensajes enviados por un cliente son recibidos por todos los demás miembros de la sala. |
| Múltiples salas | 10 | El sistema soporta múltiples salas simultáneamente sin interferencia entre ellas. |

### 2. Diseño y Arquitectura (20 puntos)

| Criterio | Puntos | Descripción |
|----------|--------|-------------|
| Estructura de mensajes | 5 | Los mensajes tienen una estructura clara y bien definida. |
| Modularidad | 5 | El código está organizado en funciones y módulos lógicos. |
| Manejo de errores | 5 | El sistema maneja adecuadamente errores (ej: sala no existe, cola llena, etc.). |
| Documentación | 5 | El código está documentado con comentarios claros y explicativos. |

### 3. Calidad del Código (20 puntos)

| Criterio | Puntos | Descripción |
|----------|--------|-------------|
| Legibilidad | 5 | El código es fácil de leer y entender. |
| Estilo y convenciones | 5 | Sigue convenciones de estilo consistentes (nombres de variables, indentación, etc.). |
| Eficiencia | 5 | El código es eficiente en el uso de recursos (CPU, memoria). |
| Robustez | 5 | El sistema funciona correctamente en diferentes escenarios (múltiples clientes, mensajes rápidos, etc.). |

### 4. Trabajo en Equipo (10 puntos)

| Criterio | Puntos | Descripción |
|----------|--------|-------------|
| Colaboración | 5 | Todos los miembros del equipo participan activamente en el desarrollo. |
| Integración | 5 | El trabajo de los diferentes miembros se integra correctamente. |

### 5. Entrega y Presentación (10 puntos)

| Criterio | Puntos | Descripción |
|----------|--------|-------------|
| Demostración | 10 | La demostración del proyecto es clara y muestra todas las funcionalidades. Se entrega por video corto. |

## Condiciones del Reto

- **Equipos**: 2-5 personas por equipo.
- **Fecha de entrega**: 10 días a partir de la publicación del reto.
- **Plataforma**: Linux.
- **Lenguaje**: C.
- **Mecanismo IPC**: Colas de mensajes (System V o POSIX).
- **Entrega**: 
  - Código fuente completo.
  - Documentación breve (README.md) explicando cómo compilar y ejecutar.
  - Video de demostración.
  - La entrega se hace por interactiva virtual en el buzón

## Bonus (Hasta 10 puntos extra)
- **Comandos adicionales**: Implementar comandos como `/list` (listar salas), `/users` (listar usuarios en la sala), `/leave` (abandonar sala).
- **Persistencia**: Guardar el historial de mensajes en archivos.
