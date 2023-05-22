#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <fcntl.h>

#define SEM_NAME "/sem"

int main(int argc, char *argv[])
{
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    int listen_sock;
    struct sockaddr_in addr;
    int port;
    char buffer[1024];
    socklen_t addr_len = sizeof(addr);
    /*char *shared_memory = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_memory == MAP_FAILED) {
        perror("memory failed");
        exit(-1);
    }
    shared_memory[0] = '0';*/

    if (argc != 3) {
        printf("Usage: %s <port> or not count in input\n", argv[0]);
        exit(1);
    }
    int cnt_proc = strtoll(argv[1], NULL, 10);
    if (cnt_proc <= 0) {
        return 0;
    }
    port = atoi(argv[2]);
    printf("port: %d, count: %d\n", port, cnt_proc);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    sem_t **sem = malloc(cnt_proc * sizeof(*sem));
    pid_t *sons = malloc(cnt_proc * sizeof(sons));
    for (int i = 0; i < cnt_proc; ++i) {
        char *path[100];
        sprintf(path, "%s%d", SEM_NAME, i);
        sem[i] = sem_open(path, O_CREAT, 0644, 0); // Создание именованного семафора
        if (sem[i] == SEM_FAILED) {
            printf("failed sem_open");

            // close inited sems
            for (int j = i - 1; j > -1; --j) {
                sem_close(sem[j]);
            }
            // close inited sems

            exit(1);
        }
    }

    int conn_socks[cnt_proc];
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(listen_sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        printf("bind() error", argv[0]);
        exit(1);
    }

    if (listen(listen_sock, cnt_proc) == -1) {
        printf("listen() error", argv[0]);
        exit(1);
    }

    int log_sock = accept(listen_sock, (struct sockaddr *) &addr, &addr_len);

    for (int i = 0; i < cnt_proc; ++i) {
        conn_socks[i] = accept(listen_sock, (struct sockaddr *) &addr, &addr_len);
        int name[2] = {i, 0};
        send(conn_socks[i], name, sizeof(name), 0);
    }

    for (int i = 0; i < cnt_proc; ++i) {
        sons[i] = fork();
        if (sons[i] < 0) {
            printf("Can\'t fork()\n");
            exit(-1);
        } else if (sons[i]) {
            srand(i);
            int state = i;
            if (!state) { // if it waits call
                sem_post(sem[i]);
            }
            int conn_sock = conn_socks[i];
            if (conn_sock == -1) {
                sprintf(buffer, "accept() error", argv[0]);
                send(conn_sock, buffer, strlen(buffer), 0);
                exit(1);
            }
            while (1) {
                if (state) { // it wants to call
                    sprintf(buffer, "%d %s", i, "назначен звонящим\n");
                    send(conn_sock, buffer, strlen(buffer), 0);
                    int want_to_call = rand() % cnt_proc;
                    int check = -1;
                    while (want_to_call == i || check == -1) {
                        want_to_call = rand() % cnt_proc;
                        if (want_to_call != i) {
                            check = sem_trywait(sem[want_to_call]);
                        }
                    }
                    sprintf(buffer, "%d %s", i, "звонит\n");
                    send(conn_sock, buffer, strlen(buffer), 0);
                    int nums[2] = {2, want_to_call};
                    send(conn_sock, nums, sizeof(nums), 0);
                } else { // it waits for call
                    sprintf(buffer, "%d %s", i, "назначен ждущим\n");
                    send(conn_sock, buffer, strlen(buffer), 0);
                    int own_value;
                    while (1) { // process of waiting call
                        sem_getvalue(sem[i], &own_value);
                        if (own_value == 0) {
                            int nums[2] = {1, i};
                            sprintf(buffer, "%d %s", i, "принял\n");
                            send(conn_sock, buffer, strlen(buffer), 0);
                            send(conn_sock, nums, sizeof(nums), 0);
                            break; // stop waiting call
                        }
                    }
                }

                // change state
                state = rand() % 2;
                if (!state) { // if it waits for call
                    sem_post(sem[i]); // increase semaphore
                }
            }
        }
    }

    // time for program execution
    int exec_time = 20;
    sleep(exec_time);
    // time for program execution

    for (int i = 0; i < 1024)

    send(log_sock, text_now, strlen(text_now), 0);

    for (int i = 0; i < cnt_proc; i++) {
        int nums[2] = {-1, -1};
        send(conn_socks[i], nums, sizeof(nums), 0);
        close(conn_socks[i]);
        kill(sons[i], SIGKILL);
        wait(NULL);
    }
    for (int i = 0; i < cnt_proc; ++i) {
        sem_close(sem[i]);
    }

    free(sem);
    free(sons);

    for (int i = 0; i < cnt_proc; ++i) {
        char *path[100];
        sprintf(path, "%s%d", SEM_NAME, i);
        sem_unlink(path);
    }
    sprintf(buffer, "The End");
    send(conn_sock, buffer, strlen(buffer), 0);
    close(listen_sock);
    close(log_sock);
    return 0;
}
