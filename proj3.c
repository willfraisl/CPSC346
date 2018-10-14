/*
Class: CPSC 346-02
Team Member 1: Will Fraisl
Team Member 2: N/A
GU Username of project lead: wfraisl
Pgm Name: proj3.c
Pgm Desc: exploraiton of the proc file system
Usage: 1) standard:  ./a.out -s
Usage: 2) history:  ./a.out -h
Usage: 3) load:  ./a.out -l
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void standard();
void history();
void load();

int main(int argc, char* argv[])
{
 if (argc != 2)
  {
   fprintf(stderr, "Error: Options required\n");
   fprintf(stderr, "usage: ./a.out -s|-h|-l\n\n");
   exit(EXIT_FAILURE);
  }

 if (!strcmp(argv[1],"-s"))
  standard();
 else if (!strcmp(argv[1],"-h"))
  history();
 else if (!strcmp(argv[1],"-l"))
  load();
 else{
  fprintf(stderr, "Error: Options required\n");
  fprintf(stderr, "usage: ./a.out -s|-h|-l\n\n");
  exit(EXIT_FAILURE);
 }
}
/*
pre: none
post: displays CPU vendor_id, model name, and OS version
*/
void standard()
{
 char ch;
 FILE* ifp;
 char str[80];

 /*
 I've deliberately used two different mechanisms for writing to the console.
 Use whichever suits you.
 strstr locates a substring
 */

 ifp = fopen("/proc/cpuinfo","r");
 while (fgets(str,80,ifp) != NULL)
  if (strstr(str,"vendor_id") || strstr(str,"model name"))
   puts(str);
 fclose (ifp);

 ifp = fopen("/proc/version","r");
 while ((ch = getc(ifp)) != EOF)
  putchar(ch);
 fclose (ifp);
}

/*
pre: none
post: displays time since the last reboot (DD:HH:MM:SS), time when the system was last booted
      (MM/DD/YY - HH:MM), number of processes that have been created since the last reboot.
Hint: strftime could be useful
*/
void history()
{
	char ch;
	FILE* ifp;
	char str[80];
	int i = 0;
	int n;
	struct tm* buf;
	char formatted_time[80];

	//getting time since last reboot
	ifp = fopen("/proc/uptime","r");
	i=0;
	while ((ch = getc(ifp)) != ' '){
		str[i] = ch;
		i++;
	}
	fclose (ifp);

	//converting time to (DD:HH:MM:SS)
	n = atof(str);
	int days = n / (24 * 3600);
	n = n % (24 * 3600);
	int hours = n / 3600;
	n %= 3600;
	int mins = n / 60;
	n %= 60;
	int secs = n;
	printf("Time since reboot: %02d:%02d:%02d:%02d\n", days, hours, mins, secs);

	//Getting current time
	ifp = fopen("/proc/stat","r");
	while (fgets(str,80,ifp) != NULL){
		if (strstr(str,"btime")){
			char *newstr = str + 6;
			time_t curr_time = (time_t) atof(newstr);
			buf = localtime(&curr_time);
			strftime(formatted_time, sizeof(formatted_time), "%m:%d:%Y - %H:%M\n", buf);
			printf("Time when booted: %s", formatted_time);
		}
	}
	fclose (ifp);

	//getting processes since boot
	ifp = fopen("/proc/stat","r");
	while (fgets(str,80,ifp) != NULL){
		if (strstr(str,"processes")){
			puts(str);
		}
	}
	fclose (ifp);
}

/*
pre: none
post: displays total memory, available memory, load average (avg. number of processes over the last minute)
*/
void load()
{
	char ch;
	FILE* ifp;
	char str[80];

	//Getting total and available memory
	ifp = fopen("/proc/meminfo","r");
	while (fgets(str,80,ifp) != NULL){
  		if (strstr(str,"MemTotal") || strstr(str,"MemAvailable")){
   			puts(str);
		}
	}
 	fclose (ifp);

	//load avg
	ifp = fopen("/proc/loadavg","r");
	puts("Load Avg: ");
 	while ((ch = getc(ifp)) != ' '){
  		putchar(ch);
	}
 	fclose (ifp);
	printf("\n");
}
