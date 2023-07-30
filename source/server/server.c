#include "server.h"
#include "main.h"
#include "app_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#define LOG_SOCK_INFO(name, addr)                       \
printf(name " address: %s\n" name " port: %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port))

#define LISTEN_BACKLOG                          50

typedef struct
{
    struct sockaddr_in addr;
    int fd;
} socket_t;
static socket_t *gh_server;

void server_init(int port_no)
{
    int ret = 0;
    int opt_val = 1;

    if(gh_server == NULL)
    {
        gh_server = (socket_t *)malloc(sizeof(socket_t));
    }

    /* Init server address */
    gh_server->addr.sin_family = AF_INET;
    gh_server->addr.sin_port = htons(port_no);
    gh_server->addr.sin_addr.s_addr = AI_ //INADDR_ANY;
    LOG_SOCK_INFO("Server", gh_server->addr);

    /* Init socket */
    gh_server->fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(gh_server->fd, "socket()");
    printf("server file descriptor: %d\n", gh_server->fd);
    

    /* Make socket's address can be reusable */
    ret = setsockopt(gh_server->fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    ERROR_CHECK(ret, "setsockopt()");

    /* Bind socket with server address */
    ret = bind(gh_server->fd, (struct sockaddr *)&gh_server->addr, sizeof(gh_server->addr));
    ERROR_CHECK(ret, "bind()");
}

void server_start()
{
    /* Listen to client */
    int ret = listen(gh_server->fd, LISTEN_BACKLOG);
    ERROR_CHECK(ret, "listen()");
}

void server_loop()
{
    /* Accept connection with client */
    struct sockaddr_in h_interface_addr;
    int interface_addr_len = sizeof(h_interface_addr);
    printf("Running accept().\n");
    int interface_fd = accept(gh_server->fd, (struct sockaddr *)&h_interface_addr, (socklen_t *)&interface_addr_len);
    ERROR_CHECK(interface_fd, "accept()");
    printf("client file descriptor: %d\n", interface_fd);
    LOG_SOCK_INFO("Client", h_interface_addr);

    /* Transfer test.txt file */
    int test_fd = open(DATABASE_PATH DATA_FILE_NAME, O_RDONLY);
    ERROR_CHECK(test_fd, "open()");

    struct stat st;
    int ret = stat(DATABASE_PATH DATA_FILE_NAME, &st);
    ERROR_CHECK(ret, "stat()");
    sendfile(interface_fd, test_fd, NULL, st.st_size);
    printf("Transfer complete: %ld bytes\n", st.st_size);

    close(test_fd);

    /* Close client socket */
    close(interface_fd);
}

void server_deinit()
{
    close(gh_server->fd);
    free(gh_server);
}

void chat_func(int new_socket_fd);

int server_handle(int argc, char *argv[])
{
    int ret = 0;
    int port_no = 0;
    int server_fd = 0;
    struct sockaddr_in server_addr = {};
    int client_fd = 0;
    struct sockaddr_in client_addr = {};
    int client_addr_len = 0;
    int opt_val = 1;

    /* Read port number from command line */
    if(argc < 2)
    {
        printf("No port number is provided.\nCommand: ./server <port_number>\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        port_no = atoi(argv[1]);
    }

    /* Init socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(server_fd, "socket()");
    
    /* Init server address */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port =  htons(port_no);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    LOG_SOCK_INFO("Server", server_addr);

    /* Prevent "Address already in use" error */
    ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    ERROR_CHECK(ret, "setsockopt()");

    /* Bind socket with server address */
    ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    ERROR_CHECK(ret, "bind()");

    /* Listen to client */
    ret = listen(server_fd, LISTEN_BACKLOG);
    ERROR_CHECK(ret, "listen()");

    /* Accept connection with client */
    client_addr_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
    ERROR_CHECK(client_fd, "accept()");
    LOG_SOCK_INFO("Client", client_addr);

    /* Transfer test.txt file */
    int test_fd = open("./input/picture_1.png", O_RDONLY);
    ERROR_CHECK(test_fd, "open()");
    char file_buff[BUFF_SIZE];
    int n;
    while((n = read(test_fd, file_buff, BUFF_SIZE)) > 0)
    {
        ret = write(client_fd, file_buff, n);
        ERROR_CHECK(n, "write()");
    }

    // struct stat st;
    // ret = stat("./input/test.txt", &st);
    // ERROR_CHECK(ret, "stat()");
    // sendfile(client_fd, test_fd, NULL, st.st_size);

    close(test_fd);

    /* Close client socket */
    close(client_fd);

    /* Close server socket */
    close(server_fd);

    return 0;
}
