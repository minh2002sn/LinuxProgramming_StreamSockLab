#include "client.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define LISTEN_BACKLOG                          50
#define BUFF_SIZE                               255

int client_handle(int argc, char *argv[])
{
    int port_no = 0;
    struct sockaddr_in server_addr = {};
    int server_fd = 0;
    int ret = 0;

    /* Read port number from command line */
    if(argc < 3)
    {
        printf("Not enough information are provided.\nCommand: ./server <server_address> <server_port_number>\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        port_no = atoi(argv[2]);
    }

    /* Init socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(server_fd, "socket()");
    
    /* Init server address */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_no);
    inet_pton(AF_INET, argv[1], &(server_addr.sin_addr));

    /* Connect to server */
    ret = connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    ERROR_CHECK(ret, "connect()");

    /* Receiving data */
    int recv_file_fd = open("./output/recv_test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    ERROR_CHECK(recv_file_fd, "open()");
    char recv_test_buff[BUFF_SIZE];
    int n;
    while((n = read(server_fd, recv_test_buff, BUFF_SIZE)) > 0)
    {
        ret = write(recv_file_fd, recv_test_buff, BUFF_SIZE);
        ERROR_CHECK(ret, "write()");
    }
    close(recv_file_fd);
    printf("Done receiving.\n");

    /* Error checking */
    // int test_fd = open("./input/test.txt", O_RDONLY);
    // int m;
    // char test_buff[BUFF_SIZE];
    // int count = 1;
    // int is_error = 0;
    // while(((n = read(recv_file_fd, recv_test_buff, BUFF_SIZE)) > 0) && 
    //         ((m = read(test_fd, test_buff, BUFF_SIZE)) > 0))
    // {
    //     for(int i = 0; i < n; i++)
    //     {
    //         if(recv_test_buff[i] != test_buff[i])
    //         {
    //             is_error = 1;
    //             printf("[%d] %d -x-> %d\n", count*10 + i, test_buff[i], recv_test_buff[i]);
    //         }
    //     }
    //     count++;
    // }
    // printf((is_error) ? "Error.\n" : "No error.\n");

    /* Close server socket */
    close(server_fd);

    return 0;
}
