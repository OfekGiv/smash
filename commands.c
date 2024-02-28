//		commands.c
//********************************************
#include "commands.h"
#include "linkedList.h"



char lastPwd[MAX_LINE_SIZE] = {0}; 
int fgPid;
char fgCommand[MAX_LINE_SIZE];
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(struct Job **jobList, char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = FALSE; // illegal command
	struct Job* headJob = *jobList;
	int jobID;
	int jobPid;
	char command[MAX_LINE_SIZE];
	int status;
	struct Job* current;
	int signum;
	
	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
	}
	
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{
		if (num_arg > 1) {
			printf("smash error: cd: too many arguments\n");
			return 1;
		}
		
		// Last PWD
		if (strcmp(args[1],"-") == 0)
		{
			if (strcmp(lastPwd,"\0") == 0) // OLDPWD not set yet
			{
				printf("smash error: cd: OLDPWD not set\n");
				return -1;
			}
			else // return to OLDPWD
			{
				getcwd(pwd,MAX_LINE_SIZE); // save current pwd
				chdir(lastPwd);           // change directory to last pwd
				strcpy(lastPwd,pwd);     // save new last pwd
			}	
		}
		else // Change directory 
		{
			getcwd(lastPwd,MAX_LINE_SIZE); // save current pwd
			if (chdir(args[1]) == -1)      // change directory to value in args
			{
				perror("smash error");
				return -1;
			}
		}
	
	} 
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		getcwd(pwd,MAX_LINE_SIZE);
		printf("%s\n",pwd);
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs")) 
	{
		removeTerminatedProcesses(jobList);
 		printJobList(*jobList);
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{
		printf("smash pid is %d\n",getpid());
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		
		
		// Check if job list is empty
		if (*jobList == NULL) 
		{
			printf("smash error: fg: jobs list is empty\n");
			return 0;
		}
		
		// Check invalid arguments
		if (num_arg > 1)
		{
			printf("smash error: fg: invalid arguments\n");
			return 0;
		}
		
		// parse the job ID
		if (num_arg == 0)
		{
			jobID = headJob->jobID;
			
		}
		else if (num_arg == 1)
		{		
			jobID = atoi(args[1]);
			if (jobID == 0) // atoi conversion failed. argument is non-integer
			{
				printf("smash error: fg: invalid arguments\n");
				return 0;
			}
			
		}
		// removeJobByJobId searches for job with jobID in the job list, and removes the job if found
		status = removeJobByJobId(jobList, jobID,&jobPid,command);
		if (status == -1)
		{
			printf("smash error: fg: job-id %d does not exist\n",jobID);
			return 0;
		}
		
		printf("%s : %d\n",command, jobPid);
		fgPid = jobPid;
		if (kill(jobPid,SIGCONT) == -1)
		{
			perror("kill");
		}
		waitpid(jobPid, NULL, 0);
		fgPid = 0;
		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		// Check invalid arguments
		if (num_arg > 1)
		{
			printf("smash error: fg: invalid arguments\n");
			return 0;
		}
		
		// parse the job ID
		if (num_arg == 0)
		{
			// search for stopped processes
			status = searchStoppedJobs(jobList, &jobPid, command);
			switch (status)
			{
				case JOBFOUND:
					// continue process in background
					if (kill(jobPid,SIGCONT) == -1)
					{
						perror("kill");
					}
					printf("%s : %d\n",command, jobPid);
					waitpid(jobPid, NULL, WNOHANG);
					break;
				
				case NOSTOPPEDJOBS:
					printf("smash error: bg: there are no stopped jobs to resume\n");
					break;
				
				default:
					printf("smash error: bg: undefined status\n");
				
			}
		}
		else if (num_arg == 1)
		{		
			jobID = atoi(args[1]);
			if (jobID == 0) // atoi conversion failed. argument is non-integer
			{
				printf("smash error: fg: invalid arguments\n");
				return 0;
			}
			
			// search job with this jobId and run in bg if found and if it was stopped
			status = continueJobInBg(jobList, jobID,&jobPid,command);
			switch (status)
			{
				case JOBFOUND:
					// continue process in background
					if (kill(jobPid,SIGCONT) == -1)
					{
						perror("kill");
					}
					printf("%s : %d\n",command, jobPid);
					waitpid(jobPid, NULL, WNOHANG);
					break;
				
				case NOEXIST:
					printf("smash error: bg: job-id %d does not exist\n",jobID);
					break;
				
				case NOTSTOPPED:
					printf("smash error: bg: job-id %d is already running in the background\n",jobID);
					break;
				
				default:
					printf("smash error: bg: undefined status\n");
				
			}
			
			return 0;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		// quit kill
		if (num_arg == 1 && args[1] != NULL && strcmp(args[1],"kill") == 0)
		{
			
			while (*jobList != NULL)
			{
				current = *jobList;
				*jobList = (*jobList)->next;
				printf("Sending SIGTERM…\n");
				if (kill(current->processID,SIGTERM) == -1)
				{
					perror("kill");
				}
				sleep(5);
				waitpid(current->processID,&status,WNOHANG);
				if (!WIFSIGNALED(status))
				{
					printf("(5 sec passed) Sending SIGKILL…\n");
					if (kill(current->processID,SIGKILL) == -1)
					{
						perror("kill");
					}
				}
				free(current);
			}
			exit(0);
		}
		else // quit
		{
			freeJobList(*jobList);
			exit(0);
		}
	} 
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		// should be only two arguments for this command
   		if (num_arg != 2) 
		{
			printf("smash error: fg: invalid arguments\n");
			return 0;
		} 
		// convert ascii to integer
		signum = atoi(args[1]);
		jobID = atoi(args[2]);
		// atoi conversion failed. argument is non-integer
		if (jobID == 0 || signum == 0) 
		{
			printf("smash error: fg: invalid arguments\n");
			return 0;
		}
		
		current = getJobByJobId(jobList,jobID); // get job with the specified jobid
		if (current == NULL) // did not find job with the specified jobid
		{
			printf("job-id %d does not exist\n",jobID);
		}
		else // found job with the specified jobid
		{
			// printf("jobID: %d signal: %d\n",jobID,signum);
			if (kill(current->processID,signum))
			{
				perror("kill");
				return 0;
			}
			printf("signal number %d was sent to pid %d\n",signum,current->processID);
		}
	} 
	/*************************************************/	
	else if (!strcmp(cmd, "diff"))
	{
   		
	} 
	/*************************************************/	
	else // external command
	{
		ExeExternal(args, cmdString);
	 	return 0;
	}
	if (illegal_cmd == TRUE)
	{
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
	int waitStatus;
	
    switch(pID = fork()) 
	{
		case -1: 
				perror("smash error");
				exit(EXIT_FAILURE);				
			
		case 0 :
				// Child Process
				setpgrp();
				
				if (execv(args[0],args) == -1)
				{
					perror("smash error");
				}
				exit(EXIT_SUCCESS);
		
		default:
				fgPid = pID;
				strcpy(fgCommand,cmdString);
				wait(&waitStatus);
				fgPid = 0;
				
	}
	
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, struct Job **jobList)
{
	char bgLine[MAX_LINE_SIZE];
	char* Command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	int i = 0, num_arg = 0;
	int pID;
	
	size_t lineSizeLength = strlen(lineSize);
	
    if (lineSizeLength > 0 && lineSize[lineSizeLength - 1] == '\n')
	{
		strncpy(bgLine,lineSize,strlen(lineSize)-1);
		bgLine[sizeof(bgLine) - 1] = '\0';  // Null-terminate the string
	}
	
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		Command = strtok(lineSize, delimiters);
		if (Command == NULL)
			return 0; 
		args[0] = Command;
		for (i=1; i<MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters); 
			if (args[i] != NULL) 
				num_arg++; 
		}
		
		
		switch(pID = fork()) 
		{
		case -1: 
				perror("smash error");
				exit(EXIT_FAILURE);
			
		case 0 :
				// Child Process
				setpgrp();
				
				if (execv(Command,args) == -1)
				{
					perror("smash error");
				}
				exit(EXIT_SUCCESS);
								
				
		
		default:
				// waitpid(pID, NULL, WNOHANG);
									
				addJob(jobList,bgLine,pID,time(NULL),0,0);
		}
	
		return 0;
		
	}
	return -1;
}




