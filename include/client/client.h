#ifndef CLIENT_H
#define CLIENT_H

void client_init(const char *ip_str, int port_no);
void client_handle();
void client_deinit();

#endif
