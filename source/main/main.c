#include "main.h"
#include "server.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
<<<<<<< Updated upstream
    if(argc == 3)
    {
        if(strcmp("server", argv[1]) == 0)
        {
            server_handle(2, argv + 1);
        }
    }
    else if(argc == 4)
    {
        if(strcmp("client", argv[1]) == 0)
        {
            client_handle(3, argv + 1);
        }
    }
    else
    {
        printf("Not enough arguments.\n");
        exit(EXIT_FAILURE);
    }
    
=======
    app_main_init(argc, argv);

    while(1)
    {
        app_main_loop();
    }

    app_main_deinit();

>>>>>>> Stashed changes
    return 0;
}