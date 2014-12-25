#include "../include/func.h"
#include <string.h>

char* strrev(char *s)
{
    int length = strlen(s);
    int c, i, j;

    for (i = 0, j = length - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }

    return s;
}

int min(int x, int y)
{
    if (x < y) { return x; }
    return y;
}

double fmin(double x, double y)
{
    if (x < y) { return x; }
    return y;
}

int max(int x, int y)
{
    if (x > y) { return x; }
    return y;
}

double fmax(double x, double y)
{
    if (x > y) { return x; }
    return y;
}

