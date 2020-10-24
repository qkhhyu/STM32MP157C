#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "temp.h"
 
double get_cputemp (void)
{
    FILE *fd;
    char buff[16] = {0};
    int cputemp = 0;
    double cputemp_f = 0.0;
    fd = fopen ("/sys/class/thermal/thermal_zone0/temp","r");
    if(fd == NULL)
    {
            perror("fopen:");
            exit (0);
    }
    fgets (buff, sizeof(buff), fd);
    cputemp = atoi(buff);
    cputemp_f = cputemp*1.0f/1000;
    fclose(fd);
    // printf("cputemp_f:%f\r\n",cputemp_f);
    return cputemp_f;
}
