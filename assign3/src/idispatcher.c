#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define debugMode_Parsing false
#define debugMode_Queues false

#define RED "\033[31m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

typedef struct instruction {
    int resourceQueueNeeded;
    int time;
    char event;
    int processID;
    int resourceNumber;
} instruction;

typedef struct pcb {
    int id;
    int readyTime;
    int runningTime;
    int blockedTime;
} pcb;

typedef struct listNode{
    void *data;
    struct listNode *previous;
    struct listNode *next;
} Node;

typedef struct listHead{
    Node *head;
    Node *tail;
    void (*deleteData)(void *toBeDeleted);
    int (*compare)(const void *first,const void *second);
    void (*printData)(void *toBePrinted);
} List;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////#pragma endregion
List *initializeList (void (*printFunction)(void *toBePrinted),void (*deleteFunction)(void *toBeDeleted),int (*compareFunction)(const void *first,const void *second))
{
    List *new = NULL;
    
    new = malloc (sizeof (struct listHead));    

    new->head = NULL;
    new->tail = NULL;
    new->printData = printFunction;
    new->deleteData = deleteFunction;
    new->compare = compareFunction; 
    
    return new;
}

Node *initializeNode (void *data)
{
    Node *new = malloc (sizeof (Node));
    
    new->data = data;
    new->next = NULL;
    new->previous = NULL;
    
    return new;
}

void insertFront (List *list, void *toBeAdded)
{
    Node *new;

    if (list == NULL)
    {
        return;
    }
    
    if (list->head == NULL && list->tail == NULL) 
    { 
        list->head = initializeNode (toBeAdded);
        list->tail = list->head;
        return;
    }
    
    else 
    {
        new = initializeNode (toBeAdded);
        new->next = list->head;
        list->head->previous = new;
        list->head = new;
    }
}

void insertBack (List *list, void *toBeAdded)
{
    Node *new = NULL;
    Node *temp = NULL;
    
    if (list == NULL)
    {
        return;
    }
    
    if (list->tail == NULL && list->head == NULL) 
    { 
        list->head = initializeNode (toBeAdded);
        list->tail = list->head;
        return;
    }
    
    temp = list->head;
    
    while (temp->next != NULL) 
    {
        temp = temp->next;
    }
        
    new = initializeNode (toBeAdded);
    list->tail = new;
    temp->next = new;
    new->previous = temp;
} 

void *getFromFront (List *list)
{
    if ((list == NULL)||(list->head == NULL))
    {
        return NULL;
    }
    
    return (list->head->data);
}

int deleteDataFromList (List *list, void *toBeDeleted)
{
    Node *temp = NULL;

    if ((list == NULL)||(list->head == NULL)||(toBeDeleted == NULL))
    {
        return -1;
    }

    temp = list->head;
     
    while (temp != NULL)
    {
        if (list->compare(temp->data,toBeDeleted) == 0)
        {
            if (temp == list->head)
            {
                list->head = temp->next;
                
                if (list->head != NULL)
                {
                    list->head->previous = NULL;
                }

                if (temp == list->tail)
                    list->tail = list->tail->previous;
            }
            
            else if (temp == list->tail)
            {
                if (temp->previous != NULL)
                {
                    list->tail = list->tail->previous;
                    list->tail->next = NULL;
                }
            }
            
            else
            {
                temp->previous->next = temp->next;
                temp->next->previous = temp->previous;
            }
            
            list->deleteData (temp->data);
            free (temp);
            return EXIT_SUCCESS;
        }
        
        else
        {
            temp = temp->next;
        }   
    } 
    
    return -1;
}

void *getDataFromList(List *list, void *toFind)
{
     Node *temp;
    if ((list == NULL)||(list->head == NULL)||(toFind == NULL))
    {
        return NULL;
    }

    temp = list->head;
     
    while (temp != NULL)
    {
        if (list->compare(temp->data,toFind) == 0)
            return temp->data;
        else
            temp = temp->next;    
    }
    return NULL;
}

void printForward (List *list)
{
    Node *temp;

    if (list == NULL)
    {
        return;
    }

    temp = list->head;

    while (temp != NULL) 
    {
        list->printData (temp->data);
        temp=temp->next;
    }
}

void deleteList (List *list)
{   
    if (list == NULL)
    {
        return;
    }
    
    while (list->head != NULL)
    {
        Node *temp = list->head;
        list->head = list->head->next;
        
        temp->next = NULL;
        temp->previous = NULL;

        list->deleteData (temp->data);
        free (temp);
    }
    
    free (list);
}

void destroyData(void *data)
{
    free((pcb *) data);
}

void printData(void *data)
{
    pcb* a = (pcb *) data;
    printf ("%d ", a->id);
}

int compare(const void *first, const void *second)
{
    pcb* a = (pcb *) first;
    pcb* b = (pcb *) second;
    
    if (a->id == b->id)
        return 0;

    return -1;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void analyzeCommands(instruction *commands, int counter);
pcb initializePCB(int id);
pcb *copy(pcb *a);

int main(int argc, char *argv[])
{
    instruction fileCommands[1000];

    char buffer[50] = {'\0'};
    int commandCounter = 0;

    while (fgets(buffer,50,stdin) != NULL)
    {
        if (strlen(buffer) < 3 || buffer[0] == ' ')
        {
            strncpy(buffer,"\0",50);
            continue;
        }

        int i = 0;
        int time = -1;
        char event = 'W';
        int rp1 = 0;
        int rp2 = 0;

        for (i=0; i<strlen(buffer);i++)
        {
            char t[10] = {'\0'};
            char r1[10] = {'\0'};
            char r2[10] = {'\0'};
            int i = 0, j = 0;
            
            while (buffer[i] != ' ')
            {
                t[i] = buffer[i];
                i++;
            }
            time = atoi(t);
            i++;
            event = buffer[i];
            i = i+2;
            
            while (i < strlen(buffer) && buffer[i] != ' ')
            {
                r1[j] = buffer[i];
                i++;
                j++;
            }
            rp1 = atoi(r1);
            i++;
            j = 0;
            while (i < strlen(buffer) && buffer[i] != ' ')
            {
                r2[j] = buffer[i];
                i++;
                j++;
            } 
            rp2 = atoi(r2);
        }

        //Obtained time, event, resource number and process id of one command
        fileCommands[commandCounter].resourceQueueNeeded = (rp2 == 0) ? 0 : 1;
        fileCommands[commandCounter].time = time;
        fileCommands[commandCounter].event = event;
        fileCommands[commandCounter].processID = (rp2 == 0) ? rp1 : rp2;
        fileCommands[commandCounter].resourceNumber = (rp2 == 0) ? -1 : rp1;
        if (debugMode_Parsing) printf(RED "%d: %d %c %d %d" RESET, commandCounter, fileCommands[commandCounter].time, fileCommands[commandCounter].event, fileCommands[commandCounter].processID, fileCommands[commandCounter].resourceNumber);
        if (debugMode_Parsing) printf("\n");
        commandCounter++;

        strncpy(buffer,"\0",50);
    }

    analyzeCommands(fileCommands,commandCounter);
}


void analyzeCommands(instruction *commands, int counter)
{
    //initialize queues
    int queueInitCounter = 0;
    List *readyQueue = initializeList(&printData,&destroyData,&compare);
    List *resourceQueue[5] = {NULL};
    for (queueInitCounter = 0; queueInitCounter < 5; queueInitCounter++)
        resourceQueue[queueInitCounter] = initializeList(&printData,&destroyData,&compare);

    pcb processes[100];
    processes[0] = initializePCB(0);
    int pcbCounter = 1;
    int i = 0, pcbOutputCounter = 0;

    for (i = 0; i < counter; i++)
    {
        if (debugMode_Queues) printf(BLUE "\nCOMMAND> TIME:%d EVENT:%c ID:%d RESOURCE:%d" RESET, commands[i].time, commands[i].event, commands[i].processID, commands[i].resourceNumber);
        if (debugMode_Queues) printf("\n");

        //idle state at start
        if (i == 0)
        {
            processes[0].runningTime += commands[i].time - 0;
        }
        else
        {
            //process queues for timing
            /*process ready queue*/
            int timePassedBetweenCommands = commands[i].time - commands[i-1].time;
            if (readyQueue->head != NULL)
            {
                //running process
                pcb *runningProcess = (pcb *) readyQueue->head->data;
                runningProcess->runningTime += timePassedBetweenCommands;
                
                //ready processes
                Node *next = readyQueue->head->next;
                while (next != NULL)
                {
                    pcb *readyProcess = (pcb *) next->data;
                    readyProcess->readyTime += timePassedBetweenCommands;
                    next = next->next;
                }
            }
            else
            {
                processes[0].runningTime += timePassedBetweenCommands;
            }
            
            /*process resource queues*/
            for (queueInitCounter = 0; queueInitCounter < 5; queueInitCounter++)
            {
                Node *next = resourceQueue[queueInitCounter]->head;
                while (next != NULL)
                {
                    pcb *readyProcess = (pcb *) next->data;
                    readyProcess->blockedTime += timePassedBetweenCommands;
                    next = next->next;
                }
            }
        }
        
        //process current command
        if (commands[i].event == 'C')
        {
            pcb *newprocess = calloc(1,sizeof(pcb));
            newprocess->id = commands[i].processID;
            newprocess->readyTime = 0;
            newprocess->runningTime = 0;
            newprocess->blockedTime = 0;

            insertBack(readyQueue,(void *) newprocess);
        }
        else if (commands[i].event == 'T')
        {
            pcb *toMove = (pcb *) getFromFront(readyQueue);
            if (toMove == NULL) continue;
            pcb *copiedProcess = copy(toMove);
            deleteDataFromList(readyQueue, (void *) toMove);
            insertBack(readyQueue,(void *) copiedProcess);
        }
        else if (commands[i].event == 'R')
        {
            pcb *temp = calloc(1,sizeof(pcb));
            temp->id = commands[i].processID;
            pcb *toPlaceInRunningState = getDataFromList(readyQueue, temp);
            if (toPlaceInRunningState == NULL) 
            {
                free(temp);
                continue;
            }
            pcb *copiedProcess = copy(toPlaceInRunningState);
            deleteDataFromList(readyQueue, (void *) toPlaceInRunningState);
            insertBack(resourceQueue[commands[i].resourceNumber - 1], (void *) copiedProcess);
            free(temp);
        }
        else if (commands[i].event == 'I')
        {
            pcb *temp = calloc(1,sizeof(pcb));
            temp->id = commands[i].processID;
            pcb *toPlaceInRunningState = getDataFromList(resourceQueue[commands[i].resourceNumber - 1], temp);
            if (toPlaceInRunningState == NULL) 
            {
                free(temp);
                continue;
            }
            pcb *copiedProcess = copy(toPlaceInRunningState);
            deleteDataFromList(resourceQueue[commands[i].resourceNumber - 1], (void *) toPlaceInRunningState);
            insertBack(readyQueue, (void *) copiedProcess);
            free(temp);
        }
        else if (commands[i].event == 'E')
        {
            pcb *temp = calloc(1,sizeof(pcb));
            temp->id = commands[i].processID;
            //printForward(readyQueue);
            pcb *exitingProcess = getDataFromList(readyQueue, temp);
            
            if (exitingProcess == NULL) 
            {
                free(temp);
                continue;
            }

            processes[pcbCounter] = initializePCB(commands[i].processID);
            processes[pcbCounter].readyTime = exitingProcess->readyTime;
            processes[pcbCounter].blockedTime = exitingProcess->blockedTime;
            processes[pcbCounter].runningTime = exitingProcess->runningTime;
            pcbCounter++;
            deleteDataFromList(readyQueue, (void *) exitingProcess);
            free(temp);
        }

        if (debugMode_Queues)
        {
            printf(YELLOW"READY QUEUE: ");
            printForward(readyQueue);
            printf(GREEN"\nRESOURCE QUEUES:\n" RESET);
            for (queueInitCounter = 0; queueInitCounter < 5; queueInitCounter++)
            {
                printf(RESET"%d-> ", queueInitCounter+1);
                printForward(resourceQueue[queueInitCounter]);
                printf(RESET"\n");
            }
        }
    } 

    int x = 0;
    for (pcbOutputCounter = 1; pcbOutputCounter < pcbCounter; pcbOutputCounter++)
    {
        for (x = 1; x < pcbCounter; x++)
        {
            if (processes[x].id > processes[pcbOutputCounter].id)
            {
                pcb temp = processes[pcbOutputCounter];
                processes[pcbOutputCounter] = processes[x];
                processes[x] = temp;
            }
        }   
    }
    for (pcbOutputCounter = 0; pcbOutputCounter < pcbCounter; pcbOutputCounter++)
    {
        if (pcbOutputCounter == 0)
            printf ("%d %d\n", processes[0].id, processes[0].runningTime);
        else
            printf ("%d %d %d %d\n", processes[pcbOutputCounter].id, processes[pcbOutputCounter].runningTime, processes[pcbOutputCounter].readyTime, processes[pcbOutputCounter].blockedTime);
    }

    deleteList(readyQueue);
    for (queueInitCounter = 0; queueInitCounter < 5; queueInitCounter++)
        deleteList(resourceQueue[queueInitCounter]);
}

pcb initializePCB(int id) 
{
    pcb new;
    new.id = id;
    new.blockedTime = 0;
    new.readyTime = 0;
    new.runningTime = 0;
    return new;
}

pcb *copy(pcb *a)
{
    pcb *newprocess = calloc(1,sizeof(pcb));
    newprocess->id = a->id;
    newprocess->readyTime = a->readyTime;
    newprocess->runningTime = a->runningTime;
    newprocess->blockedTime = a->blockedTime;
    return newprocess;
}