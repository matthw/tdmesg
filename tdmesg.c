/* print dmesg output with nicer timestamps and level
 * compile with
 *  gcc -O2 -Wall -ansi -posix -pedantic tdmesg.c -o tdmesg
 *
 * Matthieu Walter <matth@polio.be>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <string.h>
#include <time.h>
#include <sys/klog.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <sys/sysinfo.h>

#define SYSLOG_NAMES
#include <syslog.h>

#define CMD_READ_ALL  3
#define CMD_BUFF_SIZE 10

#define DEFAULT_BUFSIZE 16392

#define REGEXP "^<([0-9])>\\[ *([0-9.]+)\\] (.*)$"
#define DATEFMT "%Y-%m-%d %H:%M:%S"
#define DATEFMTLEN 20

static time_t power_on;

long get_uptime() {
    struct sysinfo s_info;
    int err;

    if ((err = sysinfo(&s_info)) != 0)
          return -1;
    return s_info.uptime;
}



int get_line(char **line, char **buf) {
    int len;

    len = strchr(*buf, '\n') - *buf;

    *line = (char *)malloc(len + 1);
    memcpy(*line, *buf, len);
    (*line)[len] = '\0';

    return len;
}

int parse_line(regex_t reg, char *line, int *level, char *ts_str, char **msg) {
    regmatch_t *match;
    int size;
    int rc;
    time_t ts;
    char *tmp;

    match = (regmatch_t *)malloc(sizeof(regmatch_t) * (reg.re_nsub + 1));
    if ((rc = regexec(&reg, line, reg.re_nsub + 1, match, 0)) != 0) {
        free(match);
        return 1;
    }

    for (rc = 1; rc <= reg.re_nsub; rc++) {
        size = match[rc].rm_eo - match[rc].rm_so;
        tmp = (char *)malloc(size + 1);
        memcpy(tmp, line + match[rc].rm_so, size);
        (tmp)[size] = '\0';

        switch (rc) {
            case 1:
                *level = atoi(tmp);
                break;
            case 2:
                ts = atof(tmp) + power_on;
                strftime(ts_str, DATEFMTLEN, DATEFMT, localtime(&ts));
                break;
            case 3:
                *msg = (char *)malloc(size + 1);
                memcpy(*msg, tmp, size + 1);
                break;
        }
        free(tmp);
    }
    free(match);

    return 0;
}


int main(int argc, char *argv[]) {
    char *buf;
    char *line, *msg;
    int level;
    char timestamp[DATEFMTLEN];
    char *pt_buf;
    int b_size;
    int n;
    regex_t reg;

    /* get poweron timestamp*/
    power_on = time(NULL) - get_uptime();

    b_size = klogctl(CMD_BUFF_SIZE, NULL, 0);

    buf = (char *)malloc(b_size);
    b_size = klogctl(CMD_READ_ALL, buf, b_size);


    regcomp(&reg, REGEXP, REG_EXTENDED);

    pt_buf = buf;
    while ((pt_buf - buf) < b_size) {
        n = get_line(&line, &pt_buf);

        if (!parse_line(reg, line, &level, (char *)&timestamp, &msg)) {
            printf("[%-7s] [%s]: %s\n", prioritynames[level].c_name, timestamp, msg);
            free(msg);
        } else
              puts(line);
        free(line);
        pt_buf += n + 1;
    }

    regfree(&reg);
    free(buf);

    return 0;
}

