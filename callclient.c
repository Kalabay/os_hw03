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

    int name;
    recv(sock, nums, sizeof(nums), 0);
    name = nums[0];
    printf("меня зовут %d\n", name);
    while (1) {
        memset(nums, 0, sizeof(nums));
        recv(sock, nums, sizeof(nums), 0);
        if (nums[0] == 2) {
            int want_to_call = nums[1];
            printf("%d %s %d %s", name, "позвонил", want_to_call, "\n");
            // TALKING
            sleep(10);
            printf("%d %s %d %s", name, "завершился", want_to_call, "\n");
            // call finished
        } else if (nums[0] == 1) {
            printf("%d %s", name, "болтает\n");
            // TALKING
            sleep(10);
            printf("%d %s %s", name, "завершился", "\n");
            // call finished
        } else {
            break;
        }
        nums[0] = -1;
    }
    close(sock);

    return 0;
}
