#include "cli.h"

//forward decl
void *logHandler;
int socketOptionsForCLI;
int count;

int cmdShowCounters(struct cli_def *cli, const char *command, char *argv[], int argc)
{
        cli_print(cli, "show counters");
        // cli_print(cli, "show counters: %d %d", count++, (datasource_stats_t*)(myDataSourceStats->elementsProcessed));
        // printf("%d\n", myDataSourceStats);
        return CLI_OK;
}

int cmdShowElement(struct cli_def *cli, const char *command, char *argv[], int argc)
{
        if (argc == 0)
        {
                cli_print(cli, "show elememts");
        }
        else
        {
                cli_print(cli, "show elememt");
        }

        return CLI_OK;
}

void registerShowCommands()
{
        struct cli_command *showCommand = cli_register_command(cli, NULL, "show", NULL, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Show various configuration and statistics");
        cli_register_command(cli, showCommand, "counters", cmdShowCounters, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Show the counters that the system uses");
        cli_register_command(cli, showCommand, "element", cmdShowElement, PRIVILEGE_UNPRIVILEGED, MODE_EXEC, "Show an specific element");
}

void *cliInit(void *data)
{
        signal(SIGCHLD, SIG_IGN);
       
        cli = cli_init();
        cli_set_banner(cli, "simPokey Interface");

        cli_set_hostname(cli, "router");
        cli_telnet_protocol(cli, 1);

        registerShowCommands();

        if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
                perror("socket");
                pthread_exit(0);
        }
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &socketOptionsForCLI, sizeof(socketOptionsForCLI));

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(CLITEST_PORT);
        if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
                perror("bind");
                pthread_exit(0);
        }

        if (listen(s, 50) < 0)
        {
                perror("listen");
                pthread_exit(0);
        }

        zlog_info(logHandler, "CLI Listening on port %d", CLITEST_PORT);
        while ((x = accept(s, NULL, 0)))
        {
                int pid = fork();
                if (pid < 0)
                {
                        zlog_info(logHandler, "fork");
                        pthread_exit(0);
                }

                /* parent */
                if (pid > 0)
                {
                        socklen_t len = sizeof(addr);
                        if (getpeername(x, (struct sockaddr *)&addr, &len) >= 0)
                                zlog_info(logHandler, "CLI accepted connection from %s", inet_ntoa(addr.sin_addr));
                        close(x);
                        continue;
                }

                /* child */
                close(s);
                cli_loop(cli, x);
                exit(0);
        }

        cli_done(cli);
        pthread_exit(0);
}
