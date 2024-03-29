#ifndef _SIGS_H
#define _SIGS_H
#define _XOPEN_SOURCE 700  // Include this line for setpgid to be available
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void sigintHandler(int signum);

void sigstpHandler(int sig_num);

#endif

