#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/labd.sock"

static int send_command(const char *command)
{
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        perror("lab socket");
        return 1;
    }
    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, sizeof(address.sun_path), "%s", SOCKET_PATH);
    if(connect(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        fprintf(stderr, "lab: labd is unavailable at %s: %s\n", SOCKET_PATH, strerror(errno));
        close(socket_fd);
        return 1;
    }
    dprintf(socket_fd, "%s\n", command);
    shutdown(socket_fd, SHUT_WR);
    char buffer[512];
    FILE *input = fdopen(socket_fd, "r");
    if(input == NULL)
    {
        close(socket_fd);
        return 1;
    }
    while(fgets(buffer, sizeof(buffer), input) != NULL)
        fputs(buffer, stdout);
    fclose(input);
    return 0;
}

int main(int argc, char **argv)
{
    if(argc == 2 && strcmp(argv[1], "ping") == 0)
        return send_command("PING");
    if(argc == 2 && strcmp(argv[1], "status") == 0)
        return send_command("STATUS");
    if(argc == 2 && strcmp(argv[1], "devices") == 0)
        return send_command("DEVICES");
    if(argc == 2 && strcmp(argv[1], "refresh") == 0)
        return send_command("REFRESH");
    if(argc >= 4 && strcmp(argv[1], "send") == 0)
    {
        char command[512];
        int used = snprintf(command, sizeof(command), "SEND %s", argv[2]);
        for(int i = 3; i < argc && used < (int)sizeof(command) - 1; ++i)
            used += snprintf(command + used, sizeof(command) - (size_t)used, "%s%s",
                             i == 3 ? " " : " ", argv[i]);
        return send_command(command);
    }
    if(argc == 3 && strcmp(argv[1], "monitor") == 0)
    {
        char command[256];
        snprintf(command, sizeof(command), "MONITOR %s", argv[2]);
        return send_command(command);
    }
    if(argc == 3 && strcmp(argv[1], "device") == 0)
    {
        char command[256];
        snprintf(command, sizeof(command), "DEVICE %s", argv[2]);
        return send_command(command);
    }
    fprintf(stderr, "Usage: %s {ping|status|devices|refresh|device NAME|send NAME TEXT|monitor NAME}\n", argv[0]);
    return 2;
}
