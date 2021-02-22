#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct EnvVar
{
	char *name;
	char *value;
	struct EnvVar *next;
} EnvVar;

typedef struct Node
{
	char *string;
	char *timeString;
	int code;

	struct Node *next;
} Node;

struct Node *headCommand = NULL;
struct Node *tailCommand = NULL;

struct EnvVar *headVar = NULL;
struct EnvVar *tailVar = NULL;

char *COLOR = "\033[0m";
const char *ResetCOLOR = "\033[0m";
//Command functions

void InsertCommand(char *instruction)
{
	time_t timer;
	struct tm *info;

	time(&timer);
	info = localtime(&timer);

	char *infoString = asctime(info);

	//allocate mem for node
	Node *newNode = (Node *)malloc(sizeof(Node));
	//save instruction string
	newNode->string = malloc(strlen(instruction) + 1);
	strcpy(newNode->string, instruction);
	//save time string
	newNode->timeString = malloc(strlen(infoString) + 1);
	strcpy(newNode->timeString, infoString);

	if (headCommand == NULL)
	{
		headCommand = newNode;
		tailCommand = newNode;
	}
	else
	{
		tailCommand->next = newNode;
		tailCommand = newNode;
	}
}

void FreeCommandList()
{
	printf("Deleting Commands...\n");
	Node *ptr = headCommand;
	Node *tmp;
	while (ptr)
	{
		tmp = ptr->next;
		free(ptr->string);
		free(ptr->timeString);
		free(ptr);
		ptr = tmp;
	}
}

void printList()
{
	Node *ptr = headCommand;
	printf("\033[32m");
	while (ptr)
	{
		
		printf("%s", ptr->timeString);
		printf(" %s %d \n", ptr->string, ptr->code);
		
		ptr = ptr->next;
	}
	printf("%s", ResetCOLOR);
}

// variable Functions

void InsertVar(const char *varName, const char *varValue)
{

	//checking if variable is in the list already
	EnvVar *ptr = headVar;
	while (ptr)
	{
		if (strcmp(varName, ptr->name) == 0)
		{
			strcpy(ptr->value, varValue);
			break;
		}
		ptr = ptr->next;
	}

	EnvVar *newNode = (EnvVar *)malloc(sizeof(EnvVar));
	newNode->name = malloc(strlen(varName) + 1);
	newNode->value = malloc(strlen(varValue) + 1);
	strcpy(newNode->name, varName);
	strcpy(newNode->value, varValue);

	if (headVar == NULL)
	{
		headVar = newNode;
		tailVar = newNode;
	}

	else
	{
		tailVar->next = newNode;
		tailVar = newNode;
	}
}

void FreeVarList()
{
	printf("Deleting Variables...\n");
	EnvVar *ptr = headVar;
	EnvVar *tmp;
	while (ptr)
	{
		tmp = ptr->next;
		free(ptr->name);
		free(ptr->value);
		free(ptr);
		ptr = tmp;
	}
}

char *FindVar(const char *varName)
{

	EnvVar *ptr = headVar;
	while (ptr)
	{
		if (strcmp(ptr->name, varName) == 0)
		{
			char *value = strdup(ptr->value);
			return value;
		}

		ptr = ptr->next;
	}
	return "No variable found";
}

void printVariables()
{
	EnvVar *ptr = headVar;
	while (ptr)
	{
		printf("%s = %s\n", ptr->name, ptr->value);
		ptr = ptr->next;
	}
}

////////////////
void FreeLists()
{
	FreeCommandList();
	FreeVarList();
}

/*
Parsing method

*/
void ParseCommand(const char *command)
{

	char *copyOfCommand = strdup(command);

	// read first character to see if its a variable
	if (copyOfCommand[0] == '$')
	{

		char *token;

		token = strtok(copyOfCommand, " =\n");

		char *varName = strdup(token);

		token = strtok(NULL, " ");
		if (token == NULL)
		{
			printf("Error, no variable value\n");
			tailCommand->code =-1;
			return;
		}
		char *varValue = strdup(token);

		InsertVar(varName, varValue);
		char *value = FindVar(varName);
		tailCommand->code = 0;
		return;
	}

	//if not variable
	char *token;

	token = strtok(copyOfCommand, " \n");

	if (strcmp("cd", token) == 0)
	{

		token = strtok(NULL, " ");

		int status = chdir(token);

		if (status == -1)
		{
			printf("Path not found\n");
			tailCommand->code = -1;
		}
		
		
		return;
	}
	

	//exit command
	if (strcmp("exit", token) == 0)
	{
		printf("Exiting...\n");
		FreeLists();
		exit(0);
	}

	//log command
	if (strcmp("log", token) == 0)
	{
		printf("Log:\n");
		printList();
		tailCommand->code = 0;
		return;
	}

	//Print value command
	if (strcmp("print", token) == 0)
	{

		token = strtok(NULL, " ");

		if (token[0] == '$')
		{
			while (token != NULL)
			{

				char *value = FindVar(token);

				printf("%s = %s \n", token, value);
				token = strtok(NULL, " ");
			}
			tailCommand->code = 0;
			return;
		}
		else
		{
			while (token != NULL)
			{

				printf("%s ", token);
				token = strtok(NULL, " ");
			}
			printf("\n");
			tailCommand->code = 0;
			return;
		}
	}

	//Change theme command
	if (strcmp("theme", token) == 0)
	{
		token = strtok(NULL, " ");
		//printf("Print variable: %s\n", token);
		if (token == NULL)
		{
			printf("Error, No color selected");
			tailCommand->code = -1;
		}

		else if (strcmp(token, "reset") == 0)
		{
			COLOR = "\033[0m";
			printf("\033[0m");
		}

		else if (strcmp(token, "red") == 0)
		{
			COLOR = "\033[31m";
			//printf("\033[31m");
		}
		else if (strcmp(token, "green") == 0)
		{
			COLOR = "\033[32m";
			//printf("\033[32m");
		}
		else if (strcmp(token, "blue") == 0)
		{
			COLOR = "\033[36m";
			//printf("\033[36m");
		}
		tailCommand->code = 0;
		return;
	}

	else
	{

		char *params[10];

		int i = 0;

		while (token != NULL)
		{
			params[i] = strdup(token);
			token = strtok(NULL, " \n");

			i++;
		}
		params[i] = (char *)0;

		char *cmd = params[0];

		if (cmd[0] == '.' && cmd[1] == '/')
		{

			//need to add a pipe to connect the color

			char line[100];
			FILE *file;
			int c;
			int i = 0;

			char PATH[100] = "";
			strcat(PATH, &cmd[2]);

			strcat(PATH, ".txt");

			file = fopen(PATH, "r");

			if (file)
			{
				while ((fgets(line, 100, file)) != NULL)
				{
					if (strcmp(line, "\n") == 0)
					{
						continue;
					}
					++i;

					printf("line %d: %s", i, line);
					InsertCommand(line);
					ParseCommand(tailCommand->string);
				}

				printf("\n");

				fclose(file);
			}
			
			return;
		}

		if (fork() != 0)
		{

			wait(NULL);
			//printf("Parent running again\n");
		}

		else
		{

			char path[100] = "/bin/";
			//printf("Path: %s \n", path);

			strcat(path, cmd);
			//printf("Path: %s \n", path);

			execvp(path, params);
			exit(0);
		}
	}
}

///

//Main

///
int main(int argc, char const *argv[])
{
	char command[50];

	printf("%scshell$ %s", COLOR, ResetCOLOR);

	while (fgets(command, 100, stdin))
	{
		if (strcmp(command, "\n") == 0)
		{
			printf("%scshell$ %s", COLOR, ResetCOLOR);
			continue;
		}

		command[strcspn(command, "\n")] = 0;
		InsertCommand(command);

		//execute latest command
		ParseCommand(tailCommand->string);

		printf("%scshell$ %s", COLOR, ResetCOLOR);
	}

	printf("Cleaing..");
	FreeLists();
	return 0;
}