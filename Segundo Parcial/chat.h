#ifndef CHAT_H
#define CHAT_H

#include <sys/types.h>

enum { MAX_ROOMS = 16, MAX_CLIENTS = 64 };

enum { MAX_NAME = 50, MAX_TEXT = 512 };

// Clave para la cola global del servidor
#define PROJECT_PATH "/tmp"
#define PROJECT_ID 'C'

// Tipos de mensaje (mtype) que usaremos con msgrcv/msgsnd
// - En la cola global, los clientes envían con mtype = 1 y
//   el servidor responde con mtype = PID del cliente.
// - En la cola de cada sala, los clientes envían con mtype = 1 y
//   el servidor difunde con mtype = PID de cada destinatario.
#define MTYPE_GLOBAL_REQUEST 1L
#define MTYPE_ROOM_CLIENT    1L

// Comandos que el cliente puede enviar al servidor por la cola global
enum client_command {
    CMD_JOIN = 1,
    CMD_LEAVE,
    CMD_SEND,        // Para mensajes hacia la sala (se usa en la cola de sala)
    CMD_LIST_ROOMS,
    CMD_LIST_USERS,
    CMD_QUIT
};

// Códigos de respuesta/semántica del servidor
enum server_response {
    SRV_TEXT = 100,  // Mensaje de chat
    SRV_INFO,        // Información (confirmaciones, avisos)
    SRV_ERROR,       // Errores
    SRV_ROOMS,       // Listado de salas
    SRV_USERS        // Listado de usuarios
};

// Mensaje genérico que circula por la cola global o por la cola de una sala
struct chat_message {
    long mtype;               // Ver notas de MTYPE_* arriba
    pid_t pid;                // PID del cliente
    int command;              // CMD_* o SRV_*
    char room[MAX_NAME];      // Nombre de la sala (si aplica)
    char sender[MAX_NAME];    // Nombre de usuario
    char text[MAX_TEXT];      // Texto del mensaje
    int room_queue_id;        // Para respuestas de JOIN: msqid de la sala
};

#endif
