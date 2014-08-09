Motors-windows-c-example
========================

JRK I00060 torxis window motor controls


add try using the following exampole
to implement a time based program for the morotrs movements to aquired current data
#include <sys/time.h>
#include <stdio.h>


int SetTimer(struct timeval &tv, int usec)
{
    gettimeofday(&tv,NULL);
    tv.tv_usec+=usec;
 
    return 1;
}
 
int CheckTimer(struct timeval &tv, int usec)
{
    struct timeval ctv;
    gettimeofday(&ctv,NULL);
 
    if( (ctv.tv_usec >= tv.tv_usec) || (ctv.tv_sec > tv.tv_sec) )
    {
        gettimeofday(&tv,NULL);
        tv.tv_usec+=usec;
        return 1;
    }
    else
        return 0;
}

int main()
{
    struct timeval tv;
    int time =0;
    SetTimer(tv,5); //set up a delay timer
    printf("start counting.\n");
    while(time < 1000)
      {
            if (CheckTimer(tv,1000)==1)
            {time = time + 1;
            printf("%d ms \n",time);
        }}
    return 0;
}
