#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ITERATE 50
#define MAX_BUFFER_SIZE 256
#define MAX_NAME_SIZE 50

struct process 
{
    float maxMem;
    char process[30];
    float maxCpu;
    int pid;
    int index;
};

struct process intenseProcess;
int flowOfExecution = 0;

void max(char arr[MAX_BUFFER_SIZE][MAX_NAME_SIZE], float mem[MAX_BUFFER_SIZE]) 
{
    char str[30] = "";
    float memMax = 0;
    int index = -1;
    char *notToKill[8] = {"Xorg","cinnamon","gnome-shell", "gnome-terminal-", "isolated", "gjs", "systemd", "top"};
    int len = sizeof(notToKill) / sizeof(notToKill[0]);
    for (int i = 0; i < ITERATE; i++) 
    {
        int skipProcess = 0;
        for (int j = 0; j<len; j++) 
        {
            if (strcmp(arr[i], notToKill[j]) == 0) 
            {
                skipProcess = 1;
                break;
            }
        }
        if (skipProcess)
        {
            continue;
            }
        if (memMax < mem[i] && strcmp(str, arr[i]) != 0 ) 
        {
            memMax = mem[i];
            strcpy(str, arr[i]);
            index = i;
        }
    }
    intenseProcess.maxMem = memMax;
    strcpy(intenseProcess.process, str);
    intenseProcess.index = index;
}

void* closeMaxProcess(void* arg) 
{

    int choice;
    pid_t pid = fork();
    if(pid == 0)
    {
        printf("Do you want to close %s using %fMB memory? (1-Yes, 0-No): \n", intenseProcess.process, intenseProcess.maxMem * (6144 / 100));
        scanf("%d", &choice);

        if (choice == 1) 
        {
            printf("Closing %s.\n\n\n", intenseProcess.process);
            int result = execlp("killall", "killall", intenseProcess.process, NULL);
            if (result == -1) 
            {
                perror("Failed to execute killall command");
                printf("Error: %s\n", strerror(errno));
            }
        } 
        else 
            printf("Process not closed.\n\n\n");
    
        
    }
    else
    {       
        int status;    
        pid_t result = waitpid(pid, &status, 0);
    }
    flowOfExecution = 0;
    return NULL;
}



void *getIntenseProcess(void *p) 
{

    char buffer[MAX_BUFFER_SIZE];
    
    float mem[MAX_BUFFER_SIZE];
    float cpu[MAX_BUFFER_SIZE];
    int pid[MAX_BUFFER_SIZE], i = 0;
    char pnames[MAX_BUFFER_SIZE][MAX_NAME_SIZE];

    time_t startTime = time(NULL);
    time_t endTime = startTime + 6;

    while (i < ITERATE) 
    {
        FILE *topOutput = popen("top -b -n 1", "r");
        if (topOutput == NULL) 
        {
            printf("Failed to execute the top command.\n");
            return NULL;
        }

        fgets(buffer, sizeof(buffer), topOutput);
        fgets(buffer, sizeof(buffer), topOutput);
        fgets(buffer, sizeof(buffer), topOutput);
        fgets(buffer, sizeof(buffer), topOutput);
        fgets(buffer, sizeof(buffer), topOutput);
        fgets(buffer, sizeof(buffer), topOutput);
        fgets(buffer, sizeof(buffer), topOutput);
        fgets(buffer, sizeof(buffer), topOutput);
        
        sscanf(buffer, "%d %*s %*s %*s %*s %*s %*s %*s %f %f %*s %s", &pid[i], &cpu[i], &mem[i], pnames[i]);

        i++;
    }

    max(pnames, mem);

    int index = intenseProcess.index;
    intenseProcess.maxCpu = cpu[index];
    intenseProcess.pid = pid[index];
    printf("PID: %d\nCPU: %f%%\nMem: %f%%\nProcess Name: %s\n", pid[index], cpu[index], mem[index], pnames[index]);

    flowOfExecution = 1;

    return NULL;
}

int main() 
{

    pthread_t tid1[10], tid2[10];

    for (int i = 0; i < 10; i++) 
    {
        if(flowOfExecution == 0)
        {
            pthread_create(&tid1[i], NULL, getIntenseProcess, NULL);
            pthread_join(tid1[i], NULL);
        }
        else if(flowOfExecution == 1)
        {
            pthread_create(&tid2[i], NULL, closeMaxProcess, NULL);
            pthread_join(tid2[i], NULL);
        }
    }


    return 0;
}

