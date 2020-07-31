/**
 * @file ish.c
 * @date 02/06/2018
 * @author Neel Gondalia
 * @id 0940429
 * @email ngondali@uoguelph.ca
 * @brief my C shell
 */

/**************************************
            LIBRARIES
**************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>

/**************************************
            COLOR MACROS
**************************************/
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define BLACK   "\033[30;8m"
#define RED     "\033[01;31m"
#define GREEN   "\033[32;1m"
#define YELLOW  "\033[33;3m"
#define BLUE    "\033[34;1m"
#define MAGENTA "\033[35;1m"
#define CYAN    "\033[01;36m"
#define DARK_GRAY "\033[01;30m"

/**************************************
        DEBUG CONSOLE CONTROL
**************************************/
#define DEBUG_CONSOLE true
#define DEBUG_CONSOLE_PROMPTINFO_ENABLE false
#define DEBUG_CONSOLE_PIDINFO_ENABLE false
#define DEBUG_CONSOLE_COMMANDINFO_ENABLE false

/**************************************
            CEILING MACROS
**************************************/
#define MAX_PROCESSES 100
#define PATH_MAX 2000
#define ID_MAX 500
#define HOST_MAX 500

/**************************************
        CUSTOMIZATION MACROS
**************************************/
#define FONT_COLOR RESET
#define USERID_FONT_COLOR CYAN
#define USERID_FONT_COLOR2 CYAN
#define CWD_FONT_COLOR DARK_GRAY
#define ERROR_FONT_COLOR RED
#define DEBUG_CONSOLE_FONT_COLOR BLUE
#define DEBUG_CONSOLE_SECONDARY_FONT_COLOR CYAN
#define CUSTOM_COMMAND_DICEROLL_DICE_COLOR BLUE

/**************************************
            GLOBALS
**************************************/
int pids[MAX_PROCESSES];
int pidCounter = 0;

/**************************************
            PROTOTYPES
**************************************/
/*
 * obtains the input from the terminal
 * @return input from terminal 
 */
extern char **getln();

/*
 * Gets if the user is root
 * @return true if user is root, false otherwise
 */
bool isRoot();

/*
 * Obtains the number of arguments given the arguments
 * @param args pointer to an array of arguments
 * @return integer representing the number of arguments
 */
int getArgc(char **args);

/*
 * Processes a single shell command
 * @param args arguments to the shell command
 * @param argc number of arguments to the shell command
 */
void processln(char **args, int argc);

/*
 * Kills all the running child processes
 */
void killAllProcesses();

/*
 * Debug console that prints a custom debug statement to the terminal based on the parameters
 * @param key description of the debug statement
 * @param value definition of the debug statement
 * @param intval integer value that defines the debug statement
 * @param enable true enables the debug statement and prints it, false doesnt print it
 */
void debugConsole(char key[], char value[], int intval, bool enable);

/*
 * Performs a custom command if found
 * @param args arguments to the custom command
 * @param argc number of arguments
 * @return true if custom command valid, false otherwise
 */
bool customCommand(char **args, int argc);

/*
 * Prints an error statement
 * @param str representing a detail message of the error that occured
 */
void printError(char *str);

/*
 * Computes the greatest common denominator of 2 numbers
 * @param num1 decimal or hexadecimal
 * @param num2 decimal or hexadecimal
 * @return int representing gcd of 2 provided numbers
 */
int computeGCD(int num1, int num2);

/**************************************
                MAIN
**************************************/
int main()
{
    int colorswap = 0;
    
    while(1)
    {
        colorswap++;
        char cwd[PATH_MAX] = {'\0'};
        char userID[ID_MAX] = {'\0'};
        char host[HOST_MAX] = {'\0'};

        //getting userID, hostname and working directory
        struct passwd *pw = getpwuid(geteuid());
        strncpy(userID,pw->pw_name,strlen(pw->pw_name));
        gethostname(host,HOST_MAX);
        getcwd(cwd, sizeof(cwd));
        debugConsole("getting hostname..",host, -1, DEBUG_CONSOLE_PROMPTINFO_ENABLE);
        debugConsole("getting user ID..",userID, -1, DEBUG_CONSOLE_PROMPTINFO_ENABLE);
        debugConsole("getting is user root..","",isRoot(), DEBUG_CONSOLE_PROMPTINFO_ENABLE);
        debugConsole("getting current working directory..",cwd, -1, DEBUG_CONSOLE_PROMPTINFO_ENABLE);

        //shell prompt
        if (colorswap%2 == 0)
        {    
            printf(RESET USERID_FONT_COLOR "%s@%s" RESET ":", userID, host);
            printf(CWD_FONT_COLOR "%s" RESET, cwd);
            printf("%s", isRoot() ? USERID_FONT_COLOR2 "# " FONT_COLOR : USERID_FONT_COLOR2 "$ " FONT_COLOR);
        }
        else
        {
            printf(RESET USERID_FONT_COLOR2 "%s@%s" RESET ":", userID, host);
            printf(CWD_FONT_COLOR "%s" RESET, cwd);
            printf("%s", isRoot() ? USERID_FONT_COLOR "# " FONT_COLOR : USERID_FONT_COLOR "$ " FONT_COLOR);
        }

        char **args = getln();
        int argc = getArgc(args);   
        
        if (argc == 1 && strcmp(args[0],"exit") == 0)
        {
            killAllProcesses();
            exit(EXIT_SUCCESS);
        }
        else if (argc > 0 && strcmp(args[0], "cd") == 0)
        {
            if (chdir(args[1]) == -1)
                printError(strerror(errno));
        }
        else if (customCommand(args,argc))
        {
            //DO NOTHING   
        }
        else
            processln(args,argc);
    }
    return 0;
}

/**************************************
        FUNCTION DEFINITIONS
**************************************/
void processln(char **args, int argc)
{
    bool runInBackground = false;

    if (argc > 1 && strcmp(args[argc - 1], "&") == 0)
    {
        runInBackground = true;
        args[argc-1] = NULL;
    }

    pid_t pid = fork();
    pids[pidCounter++] = pid;
    
    //fork successful: child process created
    if (pid == 0)
    {
        debugConsole("child pid..","",pid, DEBUG_CONSOLE_PIDINFO_ENABLE);

        // for (int j = 0; j < pidCounter; j++)
        // {
        //     debugConsole(GREEN "*CHECKING PIDS ARRAY" RESET, "", pids[j]);
        // }

        int i = 1;
        
        for (i = 1; i < argc && args[i] != NULL; i++)
        {
            if (strcmp(args[i], "<") == 0)
            {
                if (freopen(args[i+1], "r", stdin) == NULL)
                    printError(strerror(errno));
                args[i] = NULL;
                args[i+1] = NULL;
            }
            else if (strcmp(args[i], ">") == 0)
            {
                if (freopen(args[i+1], "w+", stdout) == NULL)
                    printError(strerror(errno));
                args[i] = NULL;
                args[i+1] = NULL;
            }
        }

        if (execvp(args[0],args) == -1)
        {
            printError("Command not found");
            exit(EXIT_FAILURE);
        }        
    }
    //fork failed: child process not created
    else if (pid == -1)
    {
        printError(strerror(errno));
    }
    //parent process after successful creation of child process
    else if (!runInBackground)
    {
        int status;
        debugConsole("parent pid..","",pid, DEBUG_CONSOLE_PIDINFO_ENABLE);

        // for (int j = 0; j < pidCounter; j++)
        // {
        //     debugConsole(GREEN "*CHECKING PIDS ARRAY" RESET, "", pids[j]);
        // }

        //WUNTRACED option forces the return of status information for a specified process that has either stopped or terminated
        //as waitpid relies upon a return of a status report
        waitpid(pid,&status,WUNTRACED);
    }
}

void killAllProcesses() 
{
    int i = 0;
    for (i = 0; i < pidCounter; i++)
        kill(pids[i], SIGKILL);
}

bool isRoot()
{
    if (getuid() == 0)
        return true;
    return false;
}

int getArgc(char **args)
{
    int i = 0, argc = 0;
    for (i = 0; args[i] != NULL; i++)
    {
        argc++;
        debugConsole("command argument..", args[i], argc, DEBUG_CONSOLE_COMMANDINFO_ENABLE);
    }
    return argc;
}

void debugConsole(char key[], char value[], int intval, bool enable)
{
    if (DEBUG_CONSOLE && enable)
    {
        printf("*******************************************************\n");
        printf(DEBUG_CONSOLE_FONT_COLOR "debugconsole@customsChell**" RESET);
        if (intval != -1) 
            printf(DEBUG_CONSOLE_SECONDARY_FONT_COLOR " %s> " RESET "%d %s\n", key, intval, value);
        else
            printf(DEBUG_CONSOLE_SECONDARY_FONT_COLOR " %s> " RESET "%s\n", key, value);
        printf("*******************************************************\n");
    }
}

void printError(char *str)
{
    printf(ERROR_FONT_COLOR "OOPS! Something went wrong :(\n" RESET);
    printf(ERROR_FONT_COLOR "%s" RESET "\n", str);
}

bool customCommand(char **args, int argc)
{
    if (strcmp(args[0], "gcd") == 0)
    {
        if (argc != 3)
        {
            printError("Usage: gcd number1 number2\nnumber1 and number2 are either a hexadecimal (0xabc) or decimal (123)");
            return true;
        }
        
        int num1 = (int) strtol(args[1],NULL,0);
        int num2 = (int) strtol(args[2],NULL,0);
        if ((num1 == 0 || num2 == 0) && errno == EINVAL)
        {
            printError("Usage: gcd number1 number2\nnumber1 and number2 are either a hexadecimal (0xabc) or decimal (123)");
            return true;
        }
        int gcd = computeGCD(num1,num2);
        printf("GCD(%s,%s) = %d\n", args[1], args[2], gcd);
        return true;
    }
    else if (strcmp(args[0], "args") == 0)
    {
        int i = 0;
        printf ("argc = %d, args = ", argc-1);
        for (i = 1; i < argc; i++)
        {
            if (i+1 == argc)
                printf("%s", args[i]);
            else
                printf("%s,", args[i]);
        }
        printf("\n");
        return true;
    }
    else if (strcmp(args[0],"diceroll") == 0)
    {
        if (argc != 1)
        {
            printError("Usage: diceroll");
            return true;
        }
        int diceroll = rand() % 7;

        switch(diceroll)
        {
            case 1:
                printf(CUSTOM_COMMAND_DICEROLL_DICE_COLOR " ----------\n");
                printf("|         |\n");
                printf("|    *    |\n");
                printf("|         |\n");
                printf(" ----------" RESET "\n");
                break;

            case 2:
                printf(CUSTOM_COMMAND_DICEROLL_DICE_COLOR " ----------\n");
                printf("| *       |\n");
                printf("|         |\n");
                printf("|       * |\n");
                printf(" ----------" RESET "\n");
                break;

            case 3:
                printf(CUSTOM_COMMAND_DICEROLL_DICE_COLOR " ----------\n");
                printf("| *       |\n");
                printf("|    *    |\n");
                printf("|       * |\n");
                printf(" ----------" RESET "\n");
                break;
            
            case 4:
                printf(CUSTOM_COMMAND_DICEROLL_DICE_COLOR " ----------\n");
                printf("| *     * |\n");
                printf("|         |\n");
                printf("| *     * |\n");
                printf(" ----------" RESET "\n");
                break;
            
            case 5:
                printf(CUSTOM_COMMAND_DICEROLL_DICE_COLOR " ----------\n");
                printf("| *     * |\n");
                printf("|    *    |\n");
                printf("| *     * |\n");
                printf(" ----------" RESET "\n");
                break;

            case 6:
                printf(CUSTOM_COMMAND_DICEROLL_DICE_COLOR " ----------\n");
                printf("| *     * |\n");
                printf("| *     * |\n");
                printf("| *     * |\n");
                printf(" ----------" RESET "\n");
                break;

            default:
                break;
        }
        return true;
    }
    return false;
}

int computeGCD(int num1, int num2)
{
    if (num2 == 0)
        return num1;
    return computeGCD(num2, num1 % num2);
}