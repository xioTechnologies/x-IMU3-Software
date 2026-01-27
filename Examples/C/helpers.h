#ifndef HELPERS_H
#define HELPERS_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static char get_key() {
    fflush(stdout);

    char string[256];

    if (scanf("%255s", string) != 1) {
        return 0;
    }

    if (strlen(string) != 1) {
        return 0;
    }

    return toupper(string[0]);
}

static bool yes_or_no(const char *question) {
    while (true) {
        printf("%s [Y/N]\n", question);

        switch (get_key()) {
            case 'Y':
                return true;
            case 'N':
                return false;
            default:
                break;
        }
    }
}

static void sleep(const time_t seconds) {
    const time_t timeout = time(NULL) + seconds;

    while (time(NULL) < timeout) {
        fflush(stdout);
    }
}

#endif
