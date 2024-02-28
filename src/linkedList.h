#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "commands.h"

// back ground process statuses
#define JOBFOUND      1
#define	NOEXIST       2
#define	NOTSTOPPED    3
#define	NOSTOPPEDJOBS 4


struct Job* createJob(int jobID, const char *command, int processID,int lastTimeCheckedInSeconds, int secondsElapsed,int isStopped);
void addJob(struct Job** head, const char *command, int processID,int lastTimeCheckedInSeconds, int secondsElapsed,int isStopped);

/* Search if a job with JobId exists and removes it from job list.
 * This function also returns the process pid in jobPid and command to jobCommand.
 * returns 0 if jobs found an removed
 * returns -1 if job is not found
 */
int removeJobByJobId(struct Job** head, int jobID,int* jobPid,char jobCommand[MAX_LINE_SIZE]);
void removeJobByPid(struct Job** head, int pid);
void removeTerminatedProcesses(struct Job** head);
int continueJobInBg(struct Job** head, int jobID,int* jobPid,char jobCommand[MAX_LINE_SIZE]);
int searchStoppedJobs(struct Job** head, int* jobPid,char jobCommand[MAX_LINE_SIZE]);
struct Job* getJobByJobId(struct Job** head, int jobID);
void printJobList(struct Job* head);
void freeJobList(struct Job* head);

#endif
