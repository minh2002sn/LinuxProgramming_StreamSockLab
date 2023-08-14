#include "server.h"
#include "app_config.h"
#include "error_checker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/wait.h>
#include <signal.h>


#define LOG_SOCK_INFO(name, addr)                                                                      \
    printf("\n====================\n" name ":\n    address: %s\n    port: %d\n====================\n", \
           inet_ntoa(addr.sin_addr), ntohs(addr.sin_port))

#define LISTEN_BACKLOG 50

typedef struct
{
    struct sockaddr_in addr;
    int fd;
} socket_data_t;
static socket_data_t *gh_server;

static void grimReaper(int sig)
{
    int savedErrno = errno; /* waitpid() might change 'errno' */
    int child_pid = 0;

    while((child_pid = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        printf("Process %d terminate.\n", child_pid);
        continue;
    }

    errno = savedErrno;
}

static void request_handle(int interface_fd)
{
    int test_fd = 0;
    int ret = 0;
    struct stat st;

    /* Transfer test.txt file */
    test_fd = open(DATA_FOLDER DATA_FILE_NAME, O_RDONLY);
    ERROR_CHECK(test_fd, "open()");

    ret = stat(DATA_FOLDER DATA_FILE_NAME, &st);
    ERROR_CHECK(ret, "stat()");
    sendfile(interface_fd, test_fd, NULL, st.st_size);
    printf("Done transfer %ld bytes.\n", st.st_size);

    close(test_fd);
}

void server_init(int port_no)
{
    int ret = 0;
    int opt_val = 1;
    struct sigaction sa;

    if (gh_server == NULL)
    {
        gh_server = (socket_data_t *)malloc(sizeof(socket_data_t));
    }

    /* Init server address */
    gh_server->addr.sin_family = AF_INET;
    gh_server->addr.sin_port = htons(port_no);
    gh_server->addr.sin_addr.s_addr = INADDR_ANY;
    LOG_SOCK_INFO("Server", gh_server->addr);

    /* Init socket */
    gh_server->fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(gh_server->fd, "socket()");

    /* Make socket's address can be reusable */
    ret = setsockopt(gh_server->fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    ERROR_CHECK(ret, "setsockopt()");

    /* Bind socket with server address */
    ret = bind(gh_server->fd, (struct sockaddr *)&gh_server->addr, sizeof(gh_server->addr));
    ERROR_CHECK(ret, "bind()");

    /* Establish SIGCHILD handler */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    ret = sigaction(SIGCHLD, &sa, NULL);
    ERROR_CHECK(ret, "sigaction()");
}


void server_handle()
{
    /* Accept connection with client */
    struct sockaddr_in h_client_addr;
    int client_addr_len = sizeof(h_client_addr);
    int interface_fd = 0;
    int ret = 0;

    /* Listen to client */
    ret = listen(gh_server->fd, LISTEN_BACKLOG);
    ERROR_CHECK(ret, "listen()");

    while(1)
    {
        /* Accept connection */
        interface_fd = accept(gh_server->fd, (struct sockaddr *)&h_client_addr, (socklen_t *)&client_addr_len);
        ERROR_CHECK(interface_fd, "accept()");
        LOG_SOCK_INFO("Client", h_client_addr);

        /* Create child process to handle request */
        int child_pid = fork();
        ERROR_CHECK(child_pid, "fork()");
        if(child_pid == 0)
        {
            /* Child process */
            /* Handle request */
            request_handle(interface_fd);

            /* Close client socket */
            close(interface_fd);

            exit(EXIT_SUCCESS);
        }
        else
        {
            /* Parent process */
            /* Close client socket */
            close(interface_fd);
        }

    }
}

void server_deinit()
{
    close(gh_server->fd);
    free(gh_server);
}
