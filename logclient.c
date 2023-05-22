#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in echoServAddr;
    int nums[2];
    char servIP[20];
    char buffer[1024];
    int echoServPort;

    if (argc != 3) {
        printf("Usage: %s <servIP> <echoServPort>\n", argv[0]);
        exit(-1);
    }

    strncpy(servIP, argv[1], sizeof(servIP));
    if (sscanf(argv[2], "%d", &echoServPort) != 1) {
        printf("Invalid port.\n");
        exit(-1);
    }

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family      = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port        = htons(echoServPort);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(sock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0) {
        perror("Connection failed");
        exit(-1);
    }
    printf("Connected to %s:%d\n", servIP, echoServPort);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        recv(sock, buffer, sizeof(buffer), 0);

        if (strcmp(buffer, "The End") == 0) {
            break;
        }

        printf("Recieved: %s\n", buffer);
    }
    close(sock);

    return 0;
}
