// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"
#include "commands.h"
#include "linkedList.h"

extern struct Job * jobs;
extern int fgPid;
extern char fgCommand[MAX_LINE_SIZE];

void sigintHandler(int signum)
{
	printf("\nsmash: caught ctrl-C\n");
	if (fgPid > 0)
	{
	printf("smash: process %d was killed\n",fgPid);
		if (kill(fgPid,SIGKILL) == -1)
		{
			perror("kill");
		}
	}
	fflush(stdout); 
}

void sigstpHandler(int sig_num)
{
	printf("\nsmash: caught ctrl-Z\n");
	if (fgPid > 0)
	{
		printf("smash: process %d was stopped\n",fgPid);
			if (kill(fgPid,SIGSTOP) == -1)
			{
				perror("kill");
			}
		addJob(&jobs,fgCommand,fgPid,time(NULL),0,1);
		fgPid = 0;
	}
	fflush(stdout);
}
	