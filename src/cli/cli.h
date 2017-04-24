#ifndef __CLI_H
#define __CLI_H

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlog.h>
#include "libcli.h"
#include "../libProSimDataSource.h"

#ifdef __cplusplus
extern “C” {
#endif

#define CLITEST_PORT                8000

struct cli_command *c;
struct cli_def *cli;
int s, x;
struct sockaddr_in addr;

void *cliInit(void *data);

#ifdef __cplusplus
}
#endif

#endif
