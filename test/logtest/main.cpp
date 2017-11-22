#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "log_inc.h"

static void printLog(ELogLevel level, time_t t, const char *time, const char *msg)
{
    printf("%s\n", msg);
}

int main(int argc, char *argv[])
{
    static ELogLevel LOG_LEVEL[] = {
        DebugLog, InfoLog, WarningLog, ErrorLog, EmphasisLog,
    };
    int count = 1;
    char buf[1024];
    int lv;

    srand(time(NULL));

    log_initialise(AllLog);
    //log_reg_console();
    log_reg_filelog("log", "cur-", "/home/liuy", "store-", "/home/liuy");

    struct CustomLog mylog;
    mylog.print_log = printLog;
    log_reg_custom(&mylog);

    if (argc > 1)
    {
        count = atoi(argv[1]);
    }

    for (int i = 0; i < count; i++)
    {
        lv = rand() % (sizeof(LOG_LEVEL) / sizeof(LOG_LEVEL[0]));
        snprintf(buf, sizeof(buf), "hello world %d", i);
        log_print(LOG_LEVEL[lv], buf);
    }

    log_finalise();
    exit(0);
}
