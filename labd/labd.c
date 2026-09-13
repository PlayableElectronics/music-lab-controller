#define _POSIX_C_SOURCE 200809L

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/utsname.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/labd.sock"
#define CONFIG_PATH "/etc/music-lab/devices.conf"
#define MAX_DEVICES 64
#define MAX_NAME 32
#define MAX_TEXT 256

typedef struct {
    char name[MAX_NAME];
    char transport[16];
    char node[PATH_MAX];
    char vid[16];
    char pid[16];
    char manufacturer[64];
    char product[64];
    char serial[64];
    char path[128];
    char driver[64];
    int configured;
    int online;
} Device;

static Device devices[MAX_DEVICES];
static size_t device_count;
static volatile sig_atomic_t running = 1;

static void stop_daemon(int signal_number)
{
    (void)signal_number;
    running = 0;
}

static void copy_text(char *destination, size_t size, const char *source)
{
    if(size == 0)
        return;
    snprintf(destination, size, "%s", source != NULL ? source : "");
}

static int read_file(const char *path, char *value, size_t size)
{
    FILE *file = fopen(path, "r");
    if(file == NULL)
        return -1;
    if(fgets(value, (int)size, file) == NULL)
    {
        fclose(file);
        return -1;
    }
    value[strcspn(value, "\r\n")] = '\0';
    fclose(file);
    return 0;
}

static void basename_copy(const char *path, char *value, size_t size)
{
    const char *slash = strrchr(path, '/');
    copy_text(value, size, slash == NULL ? path : slash + 1);
}

static int is_serial_name(const char *name)
{
    return strncmp(name, "ttyS", 4) == 0 || strncmp(name, "ttyACM", 6) == 0
           || strncmp(name, "ttyUSB", 6) == 0;
}

static void set_transport(Device *device)
{
    if(strncmp(device->name, "ttyS", 4) == 0)
        copy_text(device->transport, sizeof(device->transport), "uart");
    else if(device->vid[0] != '\0')
        copy_text(device->transport, sizeof(device->transport), "usb-serial");
    else
        copy_text(device->transport, sizeof(device->transport), "serial");
}

static void inspect_usb_parent(Device *device, const char *sysfs_path)
{
    char current[PATH_MAX];
    copy_text(current, sizeof(current), sysfs_path);
    for(int depth = 0; depth < 8; ++depth)
    {
        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/idVendor", current);
        if(device->vid[0] == '\0')
            (void)read_file(path, device->vid, sizeof(device->vid));
        snprintf(path, sizeof(path), "%s/idProduct", current);
        if(device->pid[0] == '\0')
            (void)read_file(path, device->pid, sizeof(device->pid));
        snprintf(path, sizeof(path), "%s/manufacturer", current);
        if(device->manufacturer[0] == '\0')
            (void)read_file(path, device->manufacturer, sizeof(device->manufacturer));
        snprintf(path, sizeof(path), "%s/product", current);
        if(device->product[0] == '\0')
            (void)read_file(path, device->product, sizeof(device->product));
        snprintf(path, sizeof(path), "%s/serial", current);
        if(device->serial[0] == '\0')
            (void)read_file(path, device->serial, sizeof(device->serial));
        if(device->vid[0] != '\0')
        {
            basename_copy(current, device->path, sizeof(device->path));
            char driver_link[PATH_MAX];
            snprintf(driver_link, sizeof(driver_link), "%s/driver", current);
            char driver_path[PATH_MAX];
            if(realpath(driver_link, driver_path) != NULL)
                basename_copy(driver_path, device->driver, sizeof(device->driver));
            return;
        }
        char *slash = strrchr(current, '/');
        if(slash == NULL || slash == current)
            break;
        *slash = '\0';
    }
}

static int configured_match(const Device *device, char *name, size_t name_size)
{
    FILE *file = fopen(CONFIG_PATH, "r");
    if(file == NULL)
        return 0;
    char line[256];
    while(fgets(line, sizeof(line), file) != NULL)
    {
        char configured_name[MAX_NAME] = "";
        char token[64];
        if(line[0] == '#' || sscanf(line, "%31s", configured_name) != 1)
            continue;
        int vid_ok = 0, pid_ok = 0, uart_ok = 0;
        char *cursor = strchr(line, ' ');
        while(cursor != NULL && sscanf(cursor, " %63s", token) == 1)
        {
            if(strncmp(token, "vid=", 4) == 0)
                vid_ok = device->vid[0] != '\0' && strcmp(token + 4, device->vid) == 0;
            else if(strncmp(token, "pid=", 4) == 0)
                pid_ok = device->pid[0] != '\0' && strcmp(token + 4, device->pid) == 0;
            else if(strncmp(token, "uart=", 5) == 0)
                uart_ok = strcmp(token + 5, device->node) == 0;
            char *space = strchr(cursor + 1, ' ');
            cursor = space;
        }
        if((vid_ok && pid_ok) || uart_ok)
        {
            copy_text(name, name_size, configured_name);
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

static void refresh_devices(void)
{
    device_count = 0;
    DIR *directory = opendir("/sys/class/tty");
    if(directory == NULL)
        return;
    struct dirent *entry;
    while((entry = readdir(directory)) != NULL && device_count < MAX_DEVICES)
    {
        if(!is_serial_name(entry->d_name))
            continue;
        Device *device = &devices[device_count];
        memset(device, 0, sizeof(*device));
        copy_text(device->name, sizeof(device->name), entry->d_name);
        snprintf(device->node, sizeof(device->node), "/dev/%s", entry->d_name);
        char sysfs_link[PATH_MAX];
        snprintf(sysfs_link, sizeof(sysfs_link), "/sys/class/tty/%s/device", entry->d_name);
        char sysfs_path[PATH_MAX];
        if(realpath(sysfs_link, sysfs_path) != NULL)
            inspect_usb_parent(device, sysfs_path);
        device->online = access(device->node, F_OK) == 0;
        char configured_name[MAX_NAME] = "";
        device->configured = configured_match(device, configured_name, sizeof(configured_name));
        if(device->configured)
            copy_text(device->name, sizeof(device->name), configured_name);
        set_transport(device);
        ++device_count;
    }
    closedir(directory);
}

static const Device *find_device(const char *name)
{
    for(size_t i = 0; i < device_count; ++i)
        if(strcmp(devices[i].name, name) == 0)
            return &devices[i];
    return NULL;
}

static void write_host_status(FILE *client)
{
    char hostname[128] = "unknown";
    char kernel[256] = "unknown";
    char uptime[64] = "unknown";
    (void)gethostname(hostname, sizeof(hostname));
    FILE *proc = fopen("/proc/uptime", "r");
    if(proc != NULL)
    {
        (void)fgets(uptime, sizeof(uptime), proc);
        fclose(proc);
        uptime[strcspn(uptime, " \r\n")] = '\0';
    }
    FILE *release = fopen("/proc/sys/kernel/osrelease", "r");
    if(release != NULL)
    {
        (void)fgets(kernel, sizeof(kernel), release);
        fclose(release);
        kernel[strcspn(kernel, "\r\n")] = '\0';
    }
    char architecture[32] = "unknown";
    struct utsname system_info;
    if(uname(&system_info) == 0)
        copy_text(architecture, sizeof(architecture), system_info.machine);
    fprintf(client, "HOSTNAME %s\nKERNEL %s\nARCH %s\nUPTIME %s seconds\n", hostname,
            kernel, architecture, uptime);
    FILE *memory = fopen("/proc/meminfo", "r");
    char line[128];
    while(memory != NULL && fgets(line, sizeof(line), memory) != NULL)
    {
        if(strncmp(line, "MemTotal:", 9) == 0 || strncmp(line, "MemFree:", 8) == 0
           || strncmp(line, "MemAvailable:", 13) == 0)
        {
            char key[32], value[32], unit[16];
            if(sscanf(line, "%31s %31s %15s", key, value, unit) >= 2)
                fprintf(client, "%s %s %s\n", key, value, unit);
        }
    }
    if(memory != NULL)
        fclose(memory);
    FILE *net = fopen("/proc/net/fib_trie", "r");
    if(net != NULL)
    {
        while(fgets(line, sizeof(line), net) != NULL)
            if(strstr(line, "32 host LOCAL") != NULL && fgets(line, sizeof(line), net) != NULL)
                fprintf(client, "IP %s", line);
        fclose(net);
    }
}

static void write_device(FILE *client, const Device *device)
{
    fprintf(client, "NAME %s\nTRANSPORT %s\nNODE %s\nSTATE %s\n", device->name,
            device->transport, device->node, device->online ? "online" : "offline");
    if(device->vid[0] != '\0')
        fprintf(client, "VID %s\nPID %s\n", device->vid, device->pid);
    if(device->manufacturer[0] != '\0')
        fprintf(client, "MANUFACTURER %s\n", device->manufacturer);
    if(device->product[0] != '\0')
        fprintf(client, "PRODUCT %s\n", device->product);
    if(device->serial[0] != '\0')
        fprintf(client, "SERIAL %s\n", device->serial);
    if(device->path[0] != '\0')
        fprintf(client, "PATH %s\n", device->path);
    if(device->driver[0] != '\0')
        fprintf(client, "DRIVER %s\n", device->driver);
}

static void write_devices(FILE *client)
{
    fprintf(client, "DEVICES %zu\n", device_count);
    for(size_t i = 0; i < device_count; ++i)
        fprintf(client, "%s %s %s %s\n", devices[i].name, devices[i].transport,
                devices[i].node, devices[i].online ? "online" : "offline");
}

static speed_t baud_rate(void)
{
#ifdef B115200
    return B115200;
#else
    return B9600;
#endif
}

static void configure_raw(struct termios *settings)
{
    settings->c_iflag = 0;
    settings->c_oflag = 0;
    settings->c_lflag = 0;
    settings->c_cflag &= (tcflag_t)~CSIZE;
    settings->c_cflag |= CS8;
    settings->c_cflag &= (tcflag_t)~(PARENB | CSTOPB);
#ifdef CRTSCTS
    settings->c_cflag &= (tcflag_t)~CRTSCTS;
#endif
}

static int open_serial(const char *node)
{
    int fd = open(node, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(fd < 0)
        return -1;
    struct termios settings;
    if(tcgetattr(fd, &settings) < 0)
    {
        close(fd);
        return -1;
    }
    configure_raw(&settings);
    cfsetispeed(&settings, baud_rate());
    cfsetospeed(&settings, baud_rate());
    settings.c_cflag |= CLOCAL | CREAD;
    if(tcsetattr(fd, TCSANOW, &settings) < 0)
    {
        close(fd);
        return -1;
    }
    return fd;
}

static void handle_serial(FILE *client, const Device *device, const char *payload)
{
    int fd = open_serial(device->node);
    if(fd < 0)
    {
        fprintf(client, "ERROR SERIAL_OPEN %s\n", strerror(errno));
        return;
    }
    char message[MAX_TEXT];
    snprintf(message, sizeof(message), "%s\n", payload);
    ssize_t written = write(fd, message, strlen(message));
    if(written < 0)
    {
        fprintf(client, "ERROR SERIAL_WRITE %s\n", strerror(errno));
        close(fd);
        return;
    }
    struct pollfd descriptor = {fd, POLLIN, 0};
    int ready = poll(&descriptor, 1, 500);
    if(ready > 0 && (descriptor.revents & POLLIN))
    {
        char response[MAX_TEXT];
        ssize_t count = read(fd, response, sizeof(response) - 1);
        if(count > 0)
        {
            response[count] = '\0';
            fprintf(client, "OK RESPONSE %s", response);
            if(response[count - 1] != '\n')
                fputc('\n', client);
        }
        else
            fprintf(client, "OK SENT\n");
    }
    else
        fprintf(client, "OK SENT\n");
    close(fd);
}

static void handle_command(FILE *client, char *line)
{
    line[strcspn(line, "\r\n")] = '\0';
    if(strcmp(line, "PING") == 0)
        fprintf(client, "OK PONG\n");
    else if(strcmp(line, "STATUS") == 0)
    {
        fprintf(client, "OK STATUS\n");
        write_host_status(client);
    }
    else if(strcmp(line, "DEVICES") == 0)
        write_devices(client);
    else if(strcmp(line, "REFRESH") == 0)
    {
        refresh_devices();
        fprintf(client, "OK REFRESHED %zu\n", device_count);
    }
    else if(strncmp(line, "DEVICE ", 7) == 0)
    {
        const Device *device = find_device(line + 7);
        if(device != NULL)
        {
            fprintf(client, "OK DEVICE\n");
            write_device(client, device);
        }
        else
            fprintf(client, "ERROR DEVICE_NOT_FOUND\n");
    }
    else if(strncmp(line, "SEND ", 5) == 0)
    {
        char name[MAX_NAME] = "";
        const char *payload = strchr(line + 5, ' ');
        if(payload != NULL)
        {
            size_t name_length = (size_t)(payload - (line + 5));
            if(name_length >= sizeof(name))
                name_length = sizeof(name) - 1;
            memcpy(name, line + 5, name_length);
            name[name_length] = '\0';
            const Device *device = find_device(name);
            if(device != NULL)
                handle_serial(client, device, payload + 1);
            else
                fprintf(client, "ERROR DEVICE_NOT_FOUND\n");
        }
        else
            fprintf(client, "ERROR SEND_SYNTAX\n");
    }
    else if(strncmp(line, "MONITOR ", 8) == 0)
    {
        const Device *device = find_device(line + 8);
        if(device == NULL)
            fprintf(client, "ERROR DEVICE_NOT_FOUND\n");
        else
        {
            int fd = open_serial(device->node);
            if(fd < 0)
                fprintf(client, "ERROR SERIAL_OPEN %s\n", strerror(errno));
            else
            {
                struct pollfd descriptor = {fd, POLLIN, 0};
                int ready = poll(&descriptor, 1, 1000);
                if(ready > 0 && (descriptor.revents & POLLIN))
                {
                    char response[MAX_TEXT];
                    ssize_t count = read(fd, response, sizeof(response) - 1);
                    if(count > 0)
                    {
                        response[count] = '\0';
                        fprintf(client, "OK RESPONSE %s", response);
                    }
                }
                else
                    fprintf(client, "OK NO_DATA\n");
                close(fd);
            }
        }
    }
    else if(strcmp(line, "QUIT") == 0)
        fprintf(client, "OK BYE\n");
    else
        fprintf(client, "ERROR UNKNOWN_COMMAND\n");
    fflush(client);
}

static int make_server(void)
{
    int server = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server < 0)
        return -1;
    unlink(SOCKET_PATH);
    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    copy_text(address.sun_path, sizeof(address.sun_path), SOCKET_PATH);
    if(bind(server, (struct sockaddr *)&address, sizeof(address)) < 0 || listen(server, 4) < 0)
    {
        close(server);
        return -1;
    }
    chmod(SOCKET_PATH, 0666);
    return server;
}

int main(void)
{
    signal(SIGTERM, stop_daemon);
    signal(SIGINT, stop_daemon);
    signal(SIGPIPE, SIG_IGN);
    refresh_devices();
    int server = make_server();
    if(server < 0)
    {
        perror("labd socket");
        return 1;
    }
    time_t next_refresh = time(NULL) + 2;
    while(running)
    {
        struct pollfd descriptor = {server, POLLIN, 0};
        int timeout = 500;
        if(time(NULL) >= next_refresh)
        {
            refresh_devices();
            next_refresh = time(NULL) + 2;
        }
        if(poll(&descriptor, 1, timeout) <= 0 || !(descriptor.revents & POLLIN))
            continue;
        int connection = accept(server, NULL, NULL);
        if(connection < 0)
            continue;
        FILE *client = fdopen(connection, "r+");
        if(client == NULL)
        {
            close(connection);
            continue;
        }
        char line[MAX_TEXT];
        while(fgets(line, sizeof(line), client) != NULL)
        {
            handle_command(client, line);
            if(strncmp(line, "QUIT", 4) == 0)
                break;
        }
        fclose(client);
    }
    close(server);
    unlink(SOCKET_PATH);
    return 0;
}
