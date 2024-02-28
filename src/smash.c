/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "linkedList.h"
#include "signals.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20

char* L_Fg_Cmd;
struct Job * jobs = NULL; //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
char lineSize[MAX_LINE_SIZE]; 

//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
	
    char cmdString[MAX_LINE_SIZE]; 	

	// testFunc(&jobs,1);
	// testFunc(&jobs,2);
	// testFunc(&jobs,3);
	// char cmd1[] = "abc";
	// char cmd2[] = "zxc";
	// char cmd3[] = "qwe";
	
	// addJob(&jobs,1,cmd1,123,time(NULL),0);
	// printJobList(jobs);
	// printf("**********\n");
	// addJob(&jobs,2,cmd2,425,time(NULL),2);
	// printJobList(jobs);
	// printf("**********\n");
	// addJob(&jobs,3,cmd3,543,time(NULL),4);
	// printJobList(jobs);
	// printf("**********\n");
	
	struct sigaction sigstpAct;
	sigstpAct.sa_handler = &sigstpHandler;
	sigstpAct.sa_flags = 0;
	// sigaction(SIGINT , &sigstpAct, NULL);
	
	if (sigaction(SIGTSTP , &sigstpAct, NULL) == -1)
	{
		perror("signal");
	}
	
	// remove comments after debugging ****************
	struct sigaction sigintAct;
	sigintAct.sa_handler = &sigintHandler;
	sigintAct.sa_flags = 0;
	sigaction(SIGINT , &sigintAct, NULL);
	
	if (sigaction(SIGINT , &sigintAct, NULL) == -1)
	{
		perror("signal");
	}
		
	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL) 
			exit (-1); 
	L_Fg_Cmd[0] = '\0';
	
	while (1)
	{
		printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';
					// background command	
		if(!BgCmd(lineSize, &jobs)) continue; 
					// built in commands
		ExeCmd(&jobs, lineSize, cmdString);
		
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
	
	
    return 0;
}

