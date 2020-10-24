#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "oled.h"
#include "cpu.h"
#include "temp.h"

#include<pthread.h>


pthread_mutex_t mutex1;

/* 毫秒级 延时 */
void sleep_ms(int ms)
{
	struct timeval delay;
	delay.tv_sec = 0;
	delay.tv_usec = ms * 1000;
	select(0, NULL, NULL, NULL, &delay);
}

static void * pthread_cpuusage(void *arg) 
{
    double sysusage = 0.0;
    char *cpu_t = malloc(32);
    printf("enter pthread_cpuusage!\r\n");
    while(1)
    {
        sysusage = get_sysCpuUsage();
        sprintf(cpu_t, "Cpu:%02f%%",sysusage);
        pthread_mutex_lock(&mutex1);
        oled_P8x16Strclear(8*13,2);
        oled_P8x16Str(0, 2, cpu_t);
        pthread_mutex_unlock(&mutex1);
        sleep_ms(100);
    }  
}

int cpuusage(void)
{
    pthread_t tidcpuusage;
    if (pthread_create(&tidcpuusage, NULL, pthread_cpuusage, NULL))
    {
        printf("create tidcpuusage error!\n");
        return 1;
    }
    sleep(1);
    return 0;
}

static void * pthread_cputemp(void *arg) 
{
    double cputemp_f = 0.0;
    char *temp_t = malloc(32);
    printf("enter pthread_cputemp!\r\n");
    pthread_mutex_lock(&mutex1);
    oled_P8x16Strclear(0,0);
    pthread_mutex_unlock(&mutex1);
    while(1)
    {
        cputemp_f = get_cputemp();
        sprintf(temp_t, "Temp:%3.1f",cputemp_f);
        pthread_mutex_lock(&mutex1);
        oled_P8x16Str(0, 0, temp_t);
        pthread_mutex_unlock(&mutex1);
        sleep_ms(100);
    }  
}

int cputemp(void)
{
    pthread_t tidcputemp;
    if (pthread_create(&tidcputemp, NULL, pthread_cputemp, NULL))
    {
        printf("create tidcputemp error!\n");
        return 1;
    }
    sleep(1);
    return 0;
}

static void * pthread_cputime(void *arg) 
{
    struct tm *p;
    time_t *timep = malloc(sizeof(*timep));
    char *day_t = malloc(32);
    char *time_t = malloc(32);
    printf("enter pthread_cputime!\r\n");
    while(1)
    {
        time(timep);
        p = gmtime(timep);
        sprintf(day_t, "Day:%d/%02d/%02d",(1900 + p->tm_year),(1 + p->tm_mon),p->tm_mday);
        sprintf(time_t, "Time:%02d:%02d:%02d",(p->tm_hour+8)%24,p->tm_min,p->tm_sec);
       
        //printf("%s %s\n",day_t, time_t);
        pthread_mutex_lock(&mutex1);
        oled_P8x16Str(0, 4, day_t);
        oled_P8x16Str(0, 6, time_t);
        pthread_mutex_unlock(&mutex1);
        sleep_ms(100);
    }  
}

int cputime(void)
{
    pthread_t tidcputime;
    if (pthread_create(&tidcputime, NULL, pthread_cputime, NULL))
    {
        printf("create tidcputime error!\n");
        return 1;
    }
    sleep(1);
    return 0;
}

int main(void)
{
    pthread_mutex_init(&mutex1,NULL);//初始化互斥锁
    oled_init();
    oled_P8x16Str(0, 0, "STM32MP157");
    // oled_P8x16Str(0, 2, "Funpack1)");
    cpuusage();
    cputemp();
    cputime();
    while (1)
    {
        sleep_ms(1000);
        /* code */
    }
    pthread_mutex_destroy(&mutex1);//释放互斥锁
    close(fd_i2c2);
    return 0;
}

