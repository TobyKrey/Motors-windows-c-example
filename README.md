Motors-windows-c-example
========================

JRK I00060 torxis window motor controls


add try using the following exampole
to implement a time based program for the morotrs movements to aquired current data
#include <sys/time.h>
#include <stdio.h>

int SetTimer(struct timeval &tv, time_t sec) {
    gettimeofday(&tv, NULL);
    tv.tv_sec += sec;

    return 1;
}

int CheckTimer(struct timeval &tv, time_t sec) {
    struct timeval ctv;
    gettimeofday(&ctv, NULL);

    if ((ctv.tv_sec > tv.tv_sec)) {
        gettimeofday(&tv, NULL);
        tv.tv_sec += sec;
        return 1;
    } else {
        return 0;
    }
}

int main() {
    struct timeval tv;
    SetTimer(tv, 5); //set up a delay timer
    printf("start counting.\n");
    while (1)
        if (CheckTimer(tv, 5) == 1)
            printf("Welcome to cc.byexamples.com\n");
    return 0;
}


