
#include "linkedList.h"

struct Job* createJob(int jobID, const char *command, int processID,int lastTimeCheckedInSeconds, int secondsElapsed,int isStopped)
{
	struct Job* newJob = (struct Job*)malloc(sizeof(struct Job));
	if (newJob == NULL)
	{
		fprintf(stderr, "Memory allication failed.\n");
		exit(EXIT_FAILURE);
	}
	newJob->jobID = jobID;
	strcpy((char*)(newJob->command),command);
	newJob->processID = processID;
	newJob->lastTimeCheckedInSeconds = lastTimeCheckedInSeconds;
	newJob->secondsElapsed = secondsElapsed;
	newJob->isStopped = isStopped;
	newJob->next = NULL;
	
	return newJob;
}

void addJob(struct Job** head, const char *command, int processID,int lastTimeCheckedInSeconds, int secondsElapsed,int isStopped)
{
	struct Job* lastJob = *head;
	struct Job* newJob;
	int jobID;
	
	// decide which jobID to give to the new job
	if (lastJob == NULL) 
		jobID = 1;                  // if list is empty new jobID = 1
	else
		jobID = lastJob->jobID + 1; // if list is not empty increment jobID
	
	newJob = createJob(jobID,command, processID,lastTimeCheckedInSeconds,secondsElapsed,isStopped);
    newJob->next = *head;
    *head = newJob;
}

int removeJobByJobId(struct Job** head, int jobID,int* jobPid,char jobCommand[MAX_LINE_SIZE]) 
{
    struct Job* current = *head;
    struct Job* prev = NULL;
	int retValue = -1;

    // Search for the job with the specified jobID
    while (current != NULL && current->jobID != jobID) {
        prev = current;
        current = current->next;
    }

    // If the job is found, remove it from the list
    if (current != NULL) {
        if (prev != NULL) {
            // Update the 'next' pointer of the previous node
            prev->next = current->next;
        } else {
            // If the job to be removed is the first node, update the head pointer
            *head = current->next;
        }
		*jobPid = current->processID;
		if (jobCommand != NULL)
			strcpy(jobCommand,(char*)current->command);
		retValue = 0;
		
        // Free the memory of the removed job
        free(current);
    }
	return retValue;
}

void removeJobByPid(struct Job** head, int pid) 
{
    struct Job* current = *head;
    struct Job* prev = NULL;

    // Search for the job with the specified jobID
    while (current != NULL && current->processID != pid) {
        prev = current;
        current = current->next;
    }

    // If the job is found, remove it from the list
    if (current != NULL) {
        if (prev != NULL) {
            // Update the 'next' pointer of the previous node
            prev->next = current->next;
        } else {
            // If the job to be removed is the first node, update the head pointer
            *head = current->next;
        }

        // Free the memory of the removed job
        free(current);
    }
}

void removeTerminatedProcesses(struct Job** head)
{
	// struct Job* current = *head;
	int status;
	pid_t terminatedPid;
	
	// waitpid (<-1> what for any child process) (<status> returns the status of the process) (<WNOHANG> return immediately if not child exists)
	while(1)
	{
		terminatedPid = waitpid(-1,&status,WNOHANG);
		if ((terminatedPid == -1 && errno == ECHILD) || terminatedPid == 0)
		{
			break;
		}
		else if (terminatedPid == -1 && errno != ECHILD)
		{
			perror("waitpid");
		}
		else
		{
			if (terminatedPid > 0)
			{
				// check if child process exited
				if (WIFEXITED(status) || WIFSIGNALED(status)) 
				{
					// remove child process from job list
					removeJobByPid(head,terminatedPid);
				}
				
			}
		}	
	}

}

int continueJobInBg(struct Job** head, int jobID,int* jobPid,char jobCommand[MAX_LINE_SIZE]) 
{
	struct Job* current = *head;

    // Search for the job with the specified jobID
    while (current != NULL && current->jobID != jobID)
        current = current->next;
	
	// if job was found 
	if (current != NULL) {
		if (current->isStopped == 0)
			return NOTSTOPPED;                              // if job is not stopped return not stopped indication
										                    
		else if (current->isStopped == 1)                   // if job is found and stopped
		{		                                            
			current->isStopped = 0;                         // change job state from stopped to running
			*jobPid = current->processID;                   // return jobPid
			if (jobCommand != NULL)
				strcpy(jobCommand,(char*)current->command); // return job command string
			return JOBFOUND;                                // return success status
		}
	}
	// if job was not found
	return NOEXIST;										    // return no job found status
}

int searchStoppedJobs(struct Job** head, int* jobPid,char jobCommand[MAX_LINE_SIZE]) 
{
	struct Job* current = *head;

    // Search for a stopped job
    while (current != NULL && current->isStopped != 1)
        current = current->next;
	
	// if job was found 
	if (current != NULL) {
		current->isStopped = 0;                          // change job state from stopped to running
		*jobPid = current->processID;					 // return jobPid
		if (jobCommand != NULL)
			strcpy(jobCommand,(char*)current->command);  // return job command string
		return JOBFOUND;                                 // return success status
	}
	// if job was not found
	return NOSTOPPEDJOBS;                                // return no stopped job found status
}


void printJobList(struct Job* head)
{
	struct Job* current = head;
	char stopString[MAX_LINE_SIZE] = "";
	if (head != NULL)
	{
		printJobList(head->next);
		current->secondsElapsed = (int)difftime(time(NULL),current->lastTimeCheckedInSeconds);
		if (current->isStopped == 1) 
		{
			strcpy(stopString,"(stopped)");
		}
		printf("[%d] %s : %d %d secs %s\n",current->jobID, (char*)current->command, current->processID,current->secondsElapsed,stopString);
	}
	
}

struct Job* getJobByJobId(struct Job** head, int jobID) 
{
	struct Job* current = *head;

    // Search for the job with the specified jobID
    while (current != NULL && current->jobID != jobID)
        current = current->next;
	
	if (current != NULL)
	{
		return current;
	}
	
	return NULL;
}

void freeJobList(struct Job* head) 
{
    struct Job* current;
    while (head != NULL) {
		current = head;
        head = head->next;
        free(current);
    }
}
