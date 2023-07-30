#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* Device is server(1) or client(0) */
/* Be defined in makefile */
#ifndef SERVER_OR_CLIENT
#define SERVER_OR_CLIENT            1
#endif

/* Path to database */
#define DATABASE_PATH               "./input/"

/* Data file name */
#define DATA_FILE_NAME              "test.txt"

/* Path to folder holding output file */
#define OUTPUT_PATH                 "./output/"

/* Output file's name */
#define OUTPUT_FILE_NAME            "recv_test.txt"

/* Enable logging socket info */
#define ENABLE_LOG_SOCK_INFO        1

/* Enable while(1) loop. If disable, loop only runs 1 time */
#define ENABLE_LOOP                 1

#endif
