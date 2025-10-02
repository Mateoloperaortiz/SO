// Servidor adaptado al esquema "cola por sala".
// - Cola global: control (JOIN, LEAVE, LIST, USERS, QUIT). Clientes envían con mtype=1.
//   El servidor responde con mtype=PID del cliente.
// - Cola de sala: chat. Clientes envían con mtype=1 (CMD_SEND).
//   El servidor difunde a todos los miembros con mtype=PID de cada uno.

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#include "chat.h"

struct member_info {
    pid_t pid;
    char name[MAX_NAME];
};

struct room_info {
    int active;
    char name[MAX_NAME];
    int queue_id;              // msqid de la sala
    pthread_t thread;          // hilo lector de la sala
    int thread_running;
    struct member_info members[MAX_CLIENTS];
    int member_count;
};

static int global_queue_id = -1;
static struct room_info rooms[MAX_ROOMS] = {0};
static pthread_mutex_t rooms_mutex = PTHREAD_MUTEX_INITIALIZER;

static void cleanup_global_queue(void) {
    if (global_queue_id != -1) {
        msgctl(global_queue_id, IPC_RMID, NULL);
        global_queue_id = -1;
    }
}

static void handle_sigint(int sig) {
    (void)sig;
    printf("\nSeñal recibida. Cerrando servidor...\n");

    // Cerrar todas las salas
    pthread_mutex_lock(&rooms_mutex);
    for (int i = 0; i < MAX_ROOMS; ++i) {
        if (rooms[i].active) {
            int qid = rooms[i].queue_id;
            rooms[i].active = 0;
            rooms[i].member_count = 0;
            if (qid != -1) {
                msgctl(qid, IPC_RMID, NULL);
                rooms[i].queue_id = -1;
            }
        }
    }
    pthread_mutex_unlock(&rooms_mutex);

    cleanup_global_queue();
    exit(EXIT_SUCCESS);
}

static int find_room_index_by_name(const char *name) {
    for (int i = 0; i < MAX_ROOMS; ++i) {
        if (rooms[i].active && strcmp(rooms[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

// (Funciones auxiliares eliminadas por no usarse)

static void send_global_response(pid_t pid, int code, const char *room, const char *text, int room_qid) {
    if (pid <= 0 || global_queue_id == -1) return;
    struct chat_message out = {0};
    out.mtype = (long)pid;  // respuesta dirigida al cliente
    out.command = code;
    out.pid = 0;
    if (room) {
        strncpy(out.room, room, MAX_NAME - 1);
        out.room[MAX_NAME - 1] = '\0';
    }
    if (text) {
        strncpy(out.text, text, MAX_TEXT - 1);
        out.text[MAX_TEXT - 1] = '\0';
    }
    out.room_queue_id = room_qid;

    if (msgsnd(global_queue_id, &out, sizeof(struct chat_message) - sizeof(long), 0) == -1) {
        perror("msgsnd (respuesta global)");
    }
}

static void broadcast_text_to_room_locked(int room_idx, const char *sender, const char *text, pid_t exclude) {
    // rooms_mutex debe estar bloqueado antes de llamar
    if (room_idx < 0 || room_idx >= MAX_ROOMS) return;
    if (!rooms[room_idx].active) return;
    int qid = rooms[room_idx].queue_id;
    struct chat_message out = {0};
    out.command = SRV_TEXT;
    strncpy(out.room, rooms[room_idx].name, MAX_NAME - 1);
    if (sender) {
        strncpy(out.sender, sender, MAX_NAME - 1);
    }
    if (text) {
        strncpy(out.text, text, MAX_TEXT - 1);
    }

    for (int i = 0; i < rooms[room_idx].member_count; ++i) {
        pid_t target = rooms[room_idx].members[i].pid;
        if (target == exclude) continue;
        out.mtype = (long)target;
        if (msgsnd(qid, &out, sizeof(struct chat_message) - sizeof(long), 0) == -1) {
            perror("msgsnd (broadcast sala)");
        }
    }
}

static void *room_thread_func(void *arg) {
    int room_idx = *(int *)arg;
    free(arg);
    int qid;

    pthread_mutex_lock(&rooms_mutex);
    qid = rooms[room_idx].queue_id;
    pthread_mutex_unlock(&rooms_mutex);

    while (1) {
        struct chat_message msg = {0};
        ssize_t r = msgrcv(qid, &msg, sizeof(struct chat_message) - sizeof(long), MTYPE_ROOM_CLIENT, 0);
        if (r == -1) {
            if (errno == EINTR) continue;
            if (errno == EIDRM || errno == EINVAL) {
                break; // cola eliminada
            }
            perror("msgrcv (sala)");
            break;
        }

        if (msg.command != CMD_SEND) {
            // Ignorar otros comandos en cola de sala
            continue;
        }

        pthread_mutex_lock(&rooms_mutex);
        if (!(room_idx >= 0 && room_idx < MAX_ROOMS && rooms[room_idx].active)) {
            pthread_mutex_unlock(&rooms_mutex);
            continue;
        }
        // Difundir a todos menos al remitente
        broadcast_text_to_room_locked(room_idx, msg.sender, msg.text, msg.pid);
        pthread_mutex_unlock(&rooms_mutex);
    }

    pthread_mutex_lock(&rooms_mutex);
    if (room_idx >= 0 && room_idx < MAX_ROOMS) {
        rooms[room_idx].thread_running = 0;
    }
    pthread_mutex_unlock(&rooms_mutex);
    return NULL;
}

static int create_room_and_thread(const char *name) {
    for (int i = 0; i < MAX_ROOMS; ++i) {
        if (!rooms[i].active) {
            int qid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
            if (qid == -1) {
                perror("msgget (sala)");
                return -1;
            }
            rooms[i].active = 1;
            rooms[i].queue_id = qid;
            rooms[i].member_count = 0;
            rooms[i].thread_running = 1;
            strncpy(rooms[i].name, name, MAX_NAME - 1);
            rooms[i].name[MAX_NAME - 1] = '\0';

            int *arg = (int *)malloc(sizeof(int));
            if (!arg) {
                rooms[i].active = 0;
                msgctl(qid, IPC_RMID, NULL);
                return -1;
            }
            *arg = i;
            if (pthread_create(&rooms[i].thread, NULL, room_thread_func, arg) != 0) {
                perror("pthread_create (sala)");
                rooms[i].active = 0;
                rooms[i].thread_running = 0;
                msgctl(qid, IPC_RMID, NULL);
                free(arg);
                return -1;
            }
            printf("Sala creada: %s (qid=%d)\n", rooms[i].name, rooms[i].queue_id);
            return i;
        }
    }
    return -1;
}

static int ensure_room(const char *name) {
    int idx = find_room_index_by_name(name);
    if (idx != -1) return idx;
    return create_room_and_thread(name);
}

static int add_member_to_room_locked(int room_idx, pid_t pid, const char *name) {
    // rooms_mutex debe estar bloqueado
    if (room_idx < 0 || room_idx >= MAX_ROOMS) return -1;
    if (!rooms[room_idx].active) return -1;
    if (rooms[room_idx].member_count >= MAX_CLIENTS) return -1;

    // Ya existe?
    for (int i = 0; i < rooms[room_idx].member_count; ++i) {
        if (rooms[room_idx].members[i].pid == pid) {
            // Actualiza nombre por si cambió
            if (name) {
                strncpy(rooms[room_idx].members[i].name, name, MAX_NAME - 1);
                rooms[room_idx].members[i].name[MAX_NAME - 1] = '\0';
            }
            return 0;
        }
    }

    int m = rooms[room_idx].member_count++;
    rooms[room_idx].members[m].pid = pid;
    if (name) {
        strncpy(rooms[room_idx].members[m].name, name, MAX_NAME - 1);
        rooms[room_idx].members[m].name[MAX_NAME - 1] = '\0';
    } else {
        rooms[room_idx].members[m].name[0] = '\0';
    }
    return 0;
}

static int remove_member_from_room_locked(int room_idx, pid_t pid) {
    // rooms_mutex debe estar bloqueado
    if (room_idx < 0 || room_idx >= MAX_ROOMS) return -1;
    if (!rooms[room_idx].active) return -1;
    int found = -1;
    for (int i = 0; i < rooms[room_idx].member_count; ++i) {
        if (rooms[room_idx].members[i].pid == pid) {
            found = i;
            break;
        }
    }
    if (found == -1) return -1;
    for (int j = found; j < rooms[room_idx].member_count - 1; ++j) {
        rooms[room_idx].members[j] = rooms[room_idx].members[j + 1];
    }
    rooms[room_idx].member_count--;
    return 0;
}

static int find_room_of_member_locked(pid_t pid) {
    // rooms_mutex debe estar bloqueado
    for (int i = 0; i < MAX_ROOMS; ++i) {
        if (!rooms[i].active) continue;
        for (int j = 0; j < rooms[i].member_count; ++j) {
            if (rooms[i].members[j].pid == pid) return i;
        }
    }
    return -1;
}

static void handle_join(const struct chat_message *msg) {
    if (msg->room[0] == '\0') {
        send_global_response(msg->pid, SRV_ERROR, NULL, "Debes especificar una sala.", -1);
        return;
    }

    pthread_mutex_lock(&rooms_mutex);
    int room_idx = ensure_room(msg->room);
    if (room_idx == -1) {
        pthread_mutex_unlock(&rooms_mutex);
        send_global_response(msg->pid, SRV_ERROR, NULL, "No se pueden crear más salas.", -1);
        return;
    }

    // Si estaba en otra sala, sácalo
    int prev_idx = find_room_of_member_locked(msg->pid);
    if (prev_idx != -1 && prev_idx != room_idx) {
        remove_member_from_room_locked(prev_idx, msg->pid);
    }

    if (add_member_to_room_locked(room_idx, msg->pid, msg->sender) == -1) {
        pthread_mutex_unlock(&rooms_mutex);
        send_global_response(msg->pid, SRV_ERROR, rooms[room_idx].name, "La sala está llena.", -1);
        return;
    }

    int room_qid = rooms[room_idx].queue_id;
    char joined_text[MAX_TEXT];
    snprintf(joined_text, sizeof(joined_text), "Te has unido a la sala: %s", rooms[room_idx].name);
    send_global_response(msg->pid, SRV_INFO, rooms[room_idx].name, joined_text, room_qid);

    // Aviso a los demás
    char notice[MAX_TEXT];
    snprintf(notice, sizeof(notice), "%s se ha unido a la sala.", msg->sender);
    broadcast_text_to_room_locked(room_idx, "Servidor", notice, msg->pid);

    pthread_mutex_unlock(&rooms_mutex);

    printf("Usuario %s (%d) se unió a %s\n", msg->sender, (int)msg->pid, msg->room);
}

static void handle_leave(const struct chat_message *msg) {
    pthread_mutex_lock(&rooms_mutex);
    int room_idx = find_room_of_member_locked(msg->pid);
    if (room_idx == -1) {
        pthread_mutex_unlock(&rooms_mutex);
        send_global_response(msg->pid, SRV_INFO, NULL, "No estás en ninguna sala.", -1);
        return;
    }

    char room_name[MAX_NAME];
    strncpy(room_name, rooms[room_idx].name, MAX_NAME - 1);
    room_name[MAX_NAME - 1] = '\0';

    remove_member_from_room_locked(room_idx, msg->pid);

    send_global_response(msg->pid, SRV_INFO, room_name, "Has salido de la sala.", -1);

    char notice[MAX_TEXT];
    snprintf(notice, sizeof(notice), "%s ha salido de la sala.", msg->sender);
    broadcast_text_to_room_locked(room_idx, "Servidor", notice, msg->pid);

    // Si la sala queda vacía, eliminar la cola y cerrar hilo
    if (rooms[room_idx].member_count == 0) {
        int qid = rooms[room_idx].queue_id;
        rooms[room_idx].active = 0;
        rooms[room_idx].queue_id = -1;
        msgctl(qid, IPC_RMID, NULL);
        pthread_mutex_unlock(&rooms_mutex);
        // Esperar al hilo lector para que termine
        pthread_join(rooms[room_idx].thread, NULL);
        return;
    }

    pthread_mutex_unlock(&rooms_mutex);
}

static void handle_list_rooms(const struct chat_message *msg) {
    (void)msg;
    char buffer[MAX_TEXT] = "Salas:\n";
    size_t used = strlen(buffer);
    int count = 0;

    pthread_mutex_lock(&rooms_mutex);
    for (int i = 0; i < MAX_ROOMS; ++i) {
        if (rooms[i].active) {
            ++count;
            int written = snprintf(buffer + used, sizeof(buffer) - used, "- %s (%d usuarios)\n",
                                   rooms[i].name, rooms[i].member_count);
            if (written < 0 || (size_t)written >= sizeof(buffer) - used) {
                strncpy(buffer + sizeof(buffer) - 5, "...\n", 4);
                break;
            }
            used += (size_t)written;
        }
    }
    pthread_mutex_unlock(&rooms_mutex);

    if (count == 0) {
        strncpy(buffer, "No hay salas disponibles.\n", sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';
    }

    send_global_response(msg->pid, SRV_ROOMS, NULL, buffer, -1);
}

static void handle_list_users(const struct chat_message *msg) {
    char buffer[MAX_TEXT];
    pthread_mutex_lock(&rooms_mutex);
    int room_idx = find_room_of_member_locked(msg->pid);
    if (room_idx == -1) {
        pthread_mutex_unlock(&rooms_mutex);
        send_global_response(msg->pid, SRV_ERROR, NULL,
                             "Únete a una sala para ver sus usuarios.", -1);
        return;
    }
    int written = snprintf(buffer, sizeof(buffer), "Usuarios en %s:\n", rooms[room_idx].name);
    size_t used = (size_t)((written < 0) ? 0 : written);
    for (int i = 0; i < rooms[room_idx].member_count; ++i) {
        written = snprintf(buffer + used, sizeof(buffer) - used, "- %s\n",
                           rooms[room_idx].members[i].name);
        if (written < 0 || (size_t)written >= sizeof(buffer) - used) {
            strncpy(buffer + sizeof(buffer) - 5, "...\n", 4);
            break;
        }
        used += (size_t)written;
    }
    char room_name[MAX_NAME];
    strncpy(room_name, rooms[room_idx].name, MAX_NAME - 1);
    room_name[MAX_NAME - 1] = '\0';
    pthread_mutex_unlock(&rooms_mutex);

    send_global_response(msg->pid, SRV_USERS, room_name, buffer, -1);
}

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);

    key_t key = ftok(PROJECT_PATH, PROJECT_ID);
    if (key == -1) {
        perror("ftok");
        return EXIT_FAILURE;
    }

    global_queue_id = msgget(key, IPC_CREAT | 0666);
    if (global_queue_id == -1) {
        perror("msgget (cola global)");
        return EXIT_FAILURE;
    }

    atexit(cleanup_global_queue);
    signal(SIGINT, handle_sigint);

    printf("Servidor de chat iniciado. Esperando clientes...\n");

    while (1) {
        struct chat_message msg = {0};
        ssize_t received = msgrcv(global_queue_id, &msg, sizeof(struct chat_message) - sizeof(long),
                                  MTYPE_GLOBAL_REQUEST, 0);
        if (received == -1) {
            if (errno == EINTR) continue;
            perror("msgrcv (global)");
            break;
        }

        switch (msg.command) {
            case CMD_JOIN:
                handle_join(&msg);
                break;
            case CMD_LEAVE:
                handle_leave(&msg);
                break;
            case CMD_LIST_ROOMS:
                handle_list_rooms(&msg);
                break;
            case CMD_LIST_USERS:
                handle_list_users(&msg);
                break;
            case CMD_QUIT:
                handle_leave(&msg);
                break;
            default:
                send_global_response(msg.pid, SRV_ERROR, NULL, "Comando no reconocido.", -1);
                break;
        }
    }

    cleanup_global_queue();
    return EXIT_SUCCESS;
}
