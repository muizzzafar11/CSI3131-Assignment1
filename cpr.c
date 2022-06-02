/* ------------------------------------------------ ------------
File: cpr.c

Last name: Sinha and Zafar
Student number: 300186804 and 300203055

Description: This program contains the code for creation
 of a child process and attach a pipe to it.
	 The child will send messages through the pipe
	 which will then be sent to standard output.

Explanation of the zombie process
(point 5 of "To be completed" in the assignment):
A zombie process is a child process in a terminated state. 
This occurs when the child process has finished execution, 
but the child's exit code is not sent to the parent. This is 
called a zombie process because the child process consumes no 
resources but still exists.

------------------------------------------------------------- */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* Prototype */
void createChildAndRead(int);

/* -------------------------------------------------------------
Function: main
Arguments:
	int ac	- number of command arguments
	char **av - array of pointers to command arguments
Description:
	Extract the number of processes to be created from the
	Command line. If an error occurs, the process ends.
	Call createChildAndRead to create a child, and read
	the child's data.
-------------------------------------------------- ----------- */
char **g_av;

int main(int ac, char **av)
{
	int processNumber;
	g_av = av;

	if (ac == 2)
	{
		if (sscanf(av[1], "%d", &processNumber) == 1)
		{
			createChildAndRead(processNumber);
		}
		else
			fprintf(stderr, "Cannot translate argument\n");
	}
	else
		fprintf(stderr, "Invalid arguments\n");
	return (0);
}

/* ------------------------------------------------ -------------
Function: createChildAndRead
Arguments:
	int prcNum - the process number
Description:
	Create the child, passing prcNum-1 to it. Use prcNum
	as the identifier of this process. Also, read the
	messages from the reading end of the pipe and sends it to
	the standard output (df 1). Finish when no data can
	be read from the pipe.
-------------------------------------------------- ----------- */

void createChildAndRead(int prcNum)
{
	// Initializing variables
	int pipeFd[2];
	int forkVal;
	char message[256];
	int byte;

	// Creating pipe and making sure it has no errors
	if (pipe(pipeFd) == -1)
	{
		perror("pipe");
		exit(1);
	}
	// Creating fork and making sure it has no errors
	forkVal = fork();
	if (forkVal == -1)
	{
		perror("fork");
		exit(1);
	}
	// If fork is child process
	else if (forkVal == 0)
	{
		close(pipeFd[0]);
		// Reduce the val of prcNum by 1 to be displayed in the next child process
		char prcNumArg[2];
		sprintf(prcNumArg, "%d", prcNum - 1);
		// Print which process is beginning (current process) to the writing end of pipe
		sprintf(message, "Process %d begins\n", prcNum);
		write(pipeFd[1], message, strlen(message));
		// If prcNum is 1 this means we have reached the end so sleep for 5 seconds
		if (prcNum == 1)
		{
			sleep(5);
		}
		else
		{
			// If prcNum is not 1 this means we can still keep going so use execlp to pass the reduced
			// prcNum and the arguments. If the 'output' file specified was out then the execlp would
			// execute the command './output prcNum-1'
			execlp(g_av[0], g_av[0], prcNumArg, NULL);
		}
	}
	else
	{
		// When returned to the parent process close the writing end of the pipe
		close(pipeFd[1]);
		// write to the file as long as the length of reading end of pipe is greater than 0, meaning
		// while there is still data to be read in the pipe
		while ((byte = read(pipeFd[0], message, strlen(message))) > 0)
		{
			write(STDOUT_FILENO, message, byte);
		}
		// Close the reading end of pipe
		close(pipeFd[0]);
		// Use STDOUT_FILENO (1) to write to the file since pipe has closed
		sprintf(message, "Process %d ends\n", prcNum);
		write(STDOUT_FILENO, message, strlen(message));
	}
}