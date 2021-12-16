#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static char GetKey()
{
    fflush(stdout);
    char string[256];
    if (scanf("%255s", string) != 1)
    {
        return 0;
    }
    if (strlen(string) != 1)
    {
        return 0;
    }
    return toupper(string[0]);
}

static bool YesOrNo(const char* question)
{
    while (true)
    {
        printf("%s [Y/N]\n", question);
        switch (GetKey())
        {
            case 'Y':
                return true;
            case 'N':
                return false;
            default:
                break;
        }
    }
}

static void Wait(const int seconds)
{
    const time_t timeout = time(NULL) + seconds;
    while ((time(NULL) < timeout) || (seconds < 0))
    {
        fflush(stdout);
    }
}
