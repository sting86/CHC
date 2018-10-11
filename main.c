#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#include <signal.h>
#include <iostream>
#include "sqlData.h"

using namespace std;


#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

static void
handler(int sig, siginfo_t *si, void *uc)
{
   /* Note: calling printf() from a signal handler is not safe
	  (and should not be done in production programs), since
	  printf() is not async-signal-safe; see signal-safety(7).
	  Nevertheless, we use printf() here as a simple way of
	  showing that the handler was called. */

   printf("Caught signal %d\n", sig);
   signal(sig, SIG_IGN);
}

void timer_thread(union sigval arg)
{
	cout<<"timer handler\n";
}

int main(int argc, char *argv[])
{
    char path[50] = "/sys/bus/w1/devices/";
    char rom[20];
    char buf[100];
    DIR *dirp;
    struct dirent *direntp;
    int fd =-1;
    char *temp;
    float value;
    // These tow lines mount the device:
    //system("sudo modprobe w1-gpio");
    //system("sudo modprobe w1-therm");
    // Check if /sys/bus/w1/devices/ exists.
    if((dirp = opendir(path)) == NULL)
    {
        printf("opendir error\n");
        return 1;
    }
    // Reads the directories or files in the current directory.
    while((direntp = readdir(dirp)) != NULL)
    {
        // If 28-00000 is the substring of d_name,
        // then copy d_name to rom and print rom.  
        if(strstr(direntp->d_name,"28-"))
        {
            strcpy(rom,direntp->d_name);
            printf(" rom: %s\n",rom);
        } else {
            printf("no romes\n");
        }
    }
    closedir(dirp);
    // Append the String rom and "/w1_slave" to path
    // path becomes to "/sys/bus/w1/devices/28-00000xxxx/w1_slave"
    strcat(path,rom);
    strcat(path,"/w1_slave");
	
	
	{
		timer_t timerid;
		
           struct sigevent sev;
		struct itimerspec its;
		long long freq_nanosecs;
		sigset_t mask;
		struct sigaction sa;
		
		sa.sa_flags = SA_SIGINFO;
		sa.sa_sigaction = handler;
		sigemptyset(&sa.sa_mask);
		if (sigaction(SIG, &sa, NULL) == -1)
			cout<<("sigaction\n");

		
           /* Block timer signal temporarily */

           // printf("Blocking signal %d\n", SIG);
           // sigemptyset(&mask);
           // sigaddset(&mask, SIG);
           // if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
               // cout <<("sigprocmask\n");

		sev.sigev_notify = SIGEV_THREAD;
		//sev.sigev_signo = SIG;
		sev.sigev_notify_function = timer_thread;
		sev.sigev_notify_attributes = NULL;
		sev.sigev_value.sival_ptr = &timerid;
		if (timer_create(CLOCKID, &sev, &timerid) == -1)
			cout<<("timer_create\n");
		   
		freq_nanosecs = 1*1000*1000*1000;
		its.it_value.tv_sec = freq_nanosecs / 1000000000;
		its.it_value.tv_nsec = freq_nanosecs % 1000000000;
		its.it_interval.tv_sec = its.it_value.tv_sec;
		its.it_interval.tv_nsec = its.it_value.tv_nsec;
		
           if (timer_settime(timerid, 0, &its, NULL) == -1)
                cout<<("timer_settime\n");
	}
	
    while(1)
    {
        // Open the file in the path.
        if((fd = open(path,O_RDONLY)) < 0)
        {
            printf("open error %d\n", fd);
			sleep(10);
			continue;
            //return 1;
        }
        // Read the file
        if(read(fd,buf,sizeof(buf)) < 0)
        {
            printf("read error\n");
			sleep(10);
			continue;
            //return 1;
        }
        // Returns the first index of 't'.
        temp = strchr(buf,'t');
        // Read the string following "t=".
        sscanf(temp,"t=%s",temp);
        // atof: changes string to float.
        value = atof(temp)/1000;
		
		try {
			DB db;
			db.logTemp(rom, value);
		
		}
		catch (unsigned int param) {cout<<"Database error: " << param << endl;}
		catch (...) {cout<<"Unknow database error\n";}
		
        //addDataSet(rom, value);
        //printf(" temp : %3.3f °C\n",value);
 
        sleep(60);
    }
    return 0;
}
