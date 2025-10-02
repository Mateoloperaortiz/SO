// Cliente adaptado a "cola por sala":
// - Usa la cola global para control (JOIN, LEAVE, LIST, USERS, QUIT).
// - Tras JOIN exitoso, recibe el msqid de la sala y:
//   - Envía mensajes a la sala con mtype=1.
//   - Recibe difusiones del servidor leyendo en la cola de la sala con mtype=PID.

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

static int global_queue_id = -1;
static int room_queue_id = -1; // cola de la sala actual
static char user_name[MAX_NAME];
static char current_room[MAX_NAME] = "";
static volatile sig_atomic_t running = 1;
static pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;

static void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

static void print_help(void) {
    printf("Comandos disponibles:\n");
    printf("  /join <sala>    -> Unirse o crear una sala\n");
    printf("  /leave          -> Salir de la sala actual\n");
    printf("  /list           -> Listar salas disponibles\n");
    printf("  /users          -> Listar usuarios de la sala actual\n");
    printf("  /quit           -> Salir del chat\n");
    printf("  /help           -> Mostrar esta ayuda\n");
    printf("  Mensaje libre   -> Enviar al chat\n\n");
}

static int send_global_request_wait(pid_t pid, int command, const char *room,
                                    const char *text, struct chat_message *resp_out) {
    struct chat_message msg = {0};
    msg.mtype = MTYPE_GLOBAL_REQUEST;
    msg.pid = pid;
    msg.command = command;
    if (room) {
        strncpy(msg.room, room, MAX_NAME - 1);
        msg.room[MAX_NAME - 1] = '\0';
    }
    if (text) {
        strncpy(msg.text, text, MAX_TEXT - 1);
        msg.text[MAX_TEXT - 1] = '\0';
    }
    strncpy(msg.sender, user_name, MAX_NAME - 1);

    if (msgsnd(global_queue_id, &msg, sizeof(struct chat_message) - sizeof(long), 0) == -1) {
        perror("msgsnd (global)");
        return -1;
    }

    if (resp_out) {
        struct chat_message resp = {0};
        ssize_t r = msgrcv(global_queue_id, &resp, sizeof(struct chat_message) - sizeof(long),
                           (long)pid, 0);
        if (r == -1) {
            perror("msgrcv (respuesta global)");
            return -1;
        }
        *resp_out = resp;
    }
    return 0;
}

static void update_room_state(const char *room, int qid) {
    pthread_mutex_lock(&state_mutex);
    if (room) {
        strncpy(current_room, room, MAX_NAME - 1);
        current_room[MAX_NAME - 1] = '\0';
    } else {
        current_room[0] = '\0';
    }
    room_queue_id = qid; // puede ser -1 si no hay sala
    pthread_mutex_unlock(&state_mutex);
}

static void *receiver_thread(void *arg) {
    (void)arg;
    pid_t mypid = getpid();

    while (running) {
        int qid;
        pthread_mutex_lock(&state_mutex);
        qid = room_queue_id;
        pthread_mutex_unlock(&state_mutex);

        if (qid == -1) {
            usleep(100000); // 100ms
            continue;
        }

        struct chat_message msg = {0};
        ssize_t r = msgrcv(qid, &msg, sizeof(struct chat_message) - sizeof(long), (long)mypid,
                           IPC_NOWAIT);
        if (r == -1) {
            if (errno == ENOMSG) {
                usleep(50000); // 50ms
                continue;
            }
            if (errno == EIDRM || errno == EINVAL) {
                // Cola eliminada o inválida
                update_room_state(NULL, -1);
                continue;
            }
            if (errno == EINTR) {
                continue;
            }
            perror("msgrcv (sala)");
            usleep(100000);
            continue;
        }

        if (msg.command == SRV_TEXT) {
            if (msg.room[0] != '\0') {
                printf("\n[%s] %s: %s\n", msg.room, msg.sender, msg.text);
            } else {
                printf("\n%s: %s\n", msg.sender, msg.text);
            }
            fflush(stdout);
            printf("> ");
            fflush(stdout);
        } else if (msg.command == SRV_INFO) {
            printf("\n[INFO] %s\n", msg.text);
            fflush(stdout);
        } else if (msg.command == SRV_ERROR) {
            fprintf(stderr, "\n[ERROR] %s\n", msg.text);
            fflush(stderr);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nombre_usuario>\n", argv[0]);
        return EXIT_FAILURE;
    }

    strncpy(user_name, argv[1], MAX_NAME - 1);
    user_name[MAX_NAME - 1] = '\0';

    key_t key = ftok(PROJECT_PATH, PROJECT_ID);
    if (key == -1) {
        perror("ftok");
        return EXIT_FAILURE;
    }

    global_queue_id = msgget(key, 0666);
    if (global_queue_id == -1) {
        perror("msgget (cola global)");
        return EXIT_FAILURE;
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, receiver_thread, NULL) != 0) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }

    printf("Bienvenido, %s. Usa /help para ver comandos.\n", user_name);
    print_help();

    char buffer[MAX_TEXT];
    pid_t mypid = getpid();
    while (running) {
        printf("> ");
        fflush(stdout);
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            if (feof(stdin)) break;
            if (errno == EINTR && !running) break;
            clearerr(stdin);
            continue;
        }

        buffer[strcspn(buffer, "\n")] = '\0';
        if (buffer[0] == '\0') continue;

        if (strncmp(buffer, "/join ", 6) == 0) {
            const char *room_name = buffer + 6;
            if (room_name[0] == '\0') {
                printf("Debes indicar el nombre de la sala.\n");
                continue;
            }
            struct chat_message resp = {0};
            if (send_global_request_wait(mypid, CMD_JOIN, room_name, NULL, &resp) == 0) {
                if (resp.command == SRV_INFO) {
                    printf("%s\n", resp.text);
                    update_room_state(resp.room, resp.room_queue_id);
                } else if (resp.command == SRV_ERROR) {
                    fprintf(stderr, "%s\n", resp.text);
                }
            }
        } else if (strcmp(buffer, "/leave") == 0) {
            struct chat_message resp = {0};
            if (send_global_request_wait(mypid, CMD_LEAVE, NULL, NULL, &resp) == 0) {
                if (resp.command == SRV_INFO) {
                    printf("%s\n", resp.text);
                    update_room_state(NULL, -1);
                } else if (resp.command == SRV_ERROR) {
                    fprintf(stderr, "%s\n", resp.text);
                }
            }
        } else if (strcmp(buffer, "/list") == 0) {
            struct chat_message resp = {0};
            if (send_global_request_wait(mypid, CMD_LIST_ROOMS, NULL, NULL, &resp) == 0) {
                if (resp.command == SRV_ROOMS || resp.command == SRV_INFO) {
                    printf("%s", resp.text);
                } else if (resp.command == SRV_ERROR) {
                    fprintf(stderr, "%s\n", resp.text);
                }
            }
        } else if (strcmp(buffer, "/users") == 0) {
            struct chat_message resp = {0};
            if (send_global_request_wait(mypid, CMD_LIST_USERS, NULL, NULL, &resp) == 0) {
                if (resp.command == SRV_USERS || resp.command == SRV_INFO) {
                    printf("%s", resp.text);
                } else if (resp.command == SRV_ERROR) {
                    fprintf(stderr, "%s\n", resp.text);
                }
            }
        } else if (strcmp(buffer, "/quit") == 0) {
            // Intentar salir limpiamente
            struct chat_message resp = {0};
            send_global_request_wait(mypid, CMD_QUIT, NULL, NULL, &resp);
            running = 0;
            break;
        } else if (strcmp(buffer, "/help") == 0) {
            print_help();
        } else if (buffer[0] == '/') {
            printf("Comando no reconocido. Usa /help.\n");
        } else {
            // Enviar mensaje a la cola de la sala
            pthread_mutex_lock(&state_mutex);
            int qid = room_queue_id;
            char room_name[MAX_NAME];
            strncpy(room_name, current_room, MAX_NAME - 1);
            room_name[MAX_NAME - 1] = '\0';
            pthread_mutex_unlock(&state_mutex);

            if (qid == -1 || room_name[0] == '\0') {
                printf("No estás en ninguna sala. Usa '/join <sala>'.\n");
                continue;
            }

            struct chat_message msg = {0};
            msg.mtype = MTYPE_ROOM_CLIENT;
            msg.pid = mypid;
            msg.command = CMD_SEND;
            strncpy(msg.sender, user_name, MAX_NAME - 1);
            strncpy(msg.room, room_name, MAX_NAME - 1);
            strncpy(msg.text, buffer, MAX_TEXT - 1);
            if (msgsnd(qid, &msg, sizeof(struct chat_message) - sizeof(long), 0) == -1) {
                perror("msgsnd (sala)");
            }
        }
    }

    running = 0;
    pthread_join(thread_id, NULL);
    return EXIT_SUCCESS;
}
