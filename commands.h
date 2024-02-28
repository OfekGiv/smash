#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_LINE_SIZE 80
#define MAX_ARG 20

struct Job {
	int jobID;
	char *command[MAX_LINE_SIZE];
	int processID;
	int lastTimeCheckedInSeconds;
	int secondsElapsed;
	int isStopped;
	struct Job* next;
};

typedef enum { FALSE , TRUE } bool;
int BgCmd(char* lineSize, struct Job **jobs);
int ExeCmd(struct Job **jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString);
int getFgPid(void);

#endif

