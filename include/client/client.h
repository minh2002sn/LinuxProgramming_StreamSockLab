#ifndef CLIENT_H
#define CLIENT_H

void client_init(const char *ip_str, int port_no);
void client_start();
void client_deinit();
int client_handle(int argc, char *argv[]);

#endif
