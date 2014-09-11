
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define	true0 0
#define	true 1
#define	false 0
#define ERROR -1

void PrintTokens (char** tokens);
char** ReadTokens(FILE* stream,int* indexForFile,int* flagEmptyInputFromUser);
void FreeTokens(char** tokens);
int CheckingOutFromLoop(char** st);
void Initialization();
int forkProcess(char** st,int indexIfFileOn);
int writeToFile(char** st,int i);
void freeMem(char** st);
void sigHandle(int sig);

char* loginChar;//* login of computer
char hostname[1024];//*hostname of computer
char** charCopyInput;//* string after ">" - for file option
int pidChild=-1;//* the number of the chile from fork

int main()
{
	int status=0;
	char** word;//* Contain the input from user
	int indexForFile=-1;//Points to the location of the "<" in the input ,If useing writing to file
	int emptyInputFromUser=true;//*Flag:1-input is not empty , 0-input is empty

	Initialization();//*Boot to run the program

	while(true)
	{
		printf("%d %s@%s$ ",status,loginChar,hostname);//*start line

		word=ReadTokens(stdin,&indexForFile,&emptyInputFromUser);//* get input from terminal

		if (emptyInputFromUser==true)//*Input is not empty
		{
			if (CheckingOutFromLoop(word)==true)//*Terms of exit from the program("exit")
			{
				freeMem(word);//*free memory
				return 0;
			}
			status=forkProcess(word,indexForFile);//* frok process

			if (status==1)//*Command is not supported
				status=255;

			indexForFile=-1;//*reset the index of (">")
		}
		emptyInputFromUser=true;//*reset flag
	}
}
void sigHandle(int sig)
//*The method examines a received signal,  if the signal== SIGINT, it's exit from parent
{
	if(pidChild >= 0)//* check if the pid is ok
		if(sig == SIGINT)
				kill(pidChild, sig);//* kill the san  SIGINT

	pidChild = -1;
}
void freeMem(char** st)
//*the method releases the pointer from memory
{
	FreeTokens(st);
	FreeTokens(charCopyInput);
}
int writeToFile(char** st,int i){
	//*The method write to a file the input from  user

	FILE *in;//*file
	int statusExe;

	in=open(st[i+1],O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);//* open the file
	if (in==ERROR)//* the open file Failed
	{
		fprintf(stderr, "ERRO: can't open the file\n");
		return -2;
	}
	dup2(in,fileno(stdout));//*Made the connection between the terminal to the file
	statusExe=execvp(charCopyInput[0],charCopyInput);//*execute the command
	fclose(in);//*close the file

	return statusExe;
}
int forkProcess(char** st,int indexIfFileOn){
	//* fork another process return status (from ג€«ג€×execvpג€¬ג€¬)

	pid_t pid;
	int statusExec=0;

	pid = fork();//*fork process

	if (pid < 0)
	{ /* error occurred */
		fprintf(stderr, "Fork Failed");
		return ERROR;
	}

	else if (pid == 0)
	{   /* child process */

		if (indexIfFileOn!=-1)//*write to file is ON
		{
			statusExec=writeToFile(st,indexIfFileOn);//* write the command from user to file
			if (statusExec==-2)//* can't open the file
				{
					statusExec=ERROR;
					exit(1);
				}
		}
		else//*write to a file is OFF
			statusExec=execvp(st[0], st);//*execute the command

		if (statusExec==ERROR)//*Command is not supported
		{
			printf("%s: command not found \n",st[0]);
			exit(1);
		}
	}
	else/* parent process */
	{
		pidChild=pid;//* set the pid of the child
		wait(&statusExec); //wait for child
		statusExec=WEXITSTATUS(statusExec);//obtain exit status of a child process
	}
	return statusExec;
}
void PrintTokens (char** tokens)
//*The method prints char **
{
	int i=0;

	for (i=0; tokens[i]!=NULL; i++)
		printf("%s \n", tokens[i]);
}
char** ReadTokens(FILE* stream,int* indexForFile,int* flagEmptyInputFromUser)
/* Method that accepts input from the terminal, and puts it in char**
The method checks if input is empty
The method generates two char**:
1. general situation(1)
2. case of writing to a file commend is ON(2) .

 */
{
	char** charInput;//*general situation(1)
	char line1[512],line2[512];
	int i;
	int wordCount;//* count how much words in the input
	char* tmp;


	fgets(line1,512,stream);//*copy from the terminal

	for (i=0;line1[i]!='\n';i++);//* end of the cher
	line1[i]='\0';

	strcpy(line2,line1);//* made a copy

	tmp = strtok(line1," ");//* Returns the string that separated by " "
	wordCount=0;
	while (tmp!=NULL)
	{
		tmp=strtok(NULL," ");//* Returns the string that separated by " "
		wordCount++;//* counter for how mach words was
	}

	if (wordCount==true0)//*if the input is empty
		(*flagEmptyInputFromUser)=0;


	charInput=(char**)malloc((wordCount+1)*sizeof(char*));//*Dynamic allocation(1)
	charCopyInput=(char**)malloc((wordCount+1)*sizeof(char*));//*Dynamic allocation(2)

	if ((charInput==NULL)||(charCopyInput==NULL))//* erro with malloc
	{
		exit(EXIT_FAILURE);
		fprintf(stderr, "malloc failed");
	}
	tmp = strtok(line2," ");//* get the next string
	i=0;
	while (tmp!=NULL)//*Passes the input
	{
		if (strcmp(tmp,">")==true0)//* check if ">" in the input (write to file- ON)
			(*indexForFile)=i;

		if ((*indexForFile)==-1)//* a copy for the char** for the case if command writing a file is ON
		{
			charCopyInput[i]=(char*)malloc((strlen(tmp)+1*sizeof(char)));//*(2)
			if (charCopyInput[i]==NULL)//* erro with malloc
			{
				exit(EXIT_FAILURE);
				fprintf(stderr, "malloc failed");
			}
			strcpy(charCopyInput[i],tmp);//* copy char
		}

		charInput[i]=(char*)malloc((strlen(tmp)+1*sizeof(char)));//(1)
		if (charInput[i]==NULL)//* erro with malloc
		{
			exit(EXIT_FAILURE);
			fprintf(stderr, "malloc failed");
		}
		strcpy(charInput[i],tmp);//* copy char
		tmp=strtok(NULL," ");//* get he next string

		i++;
	}

	if ((*indexForFile)!=-1)//* only if file options is ON
		charCopyInput[(*indexForFile)]=NULL;//put NULL on last index (2)

	charInput[i]=NULL;//put NULL on last index (1)

	return charInput;
}
void FreeTokens(char** tokens)
//*Releases the object char** from the memory
{
	int i;

	for (i=0; tokens[i]!=NULL; i++)
		free(tokens[i]);

	free(tokens[i]); // free the last index
	tokens = NULL; // free the array
	free(tokens);
}
int CheckingOutFromLoop(char** st)
//*The method checks if received word "exit" from the user
{
	if ((strcmp(st[0],"exit")==true0) && (st[1]==NULL))//* exit from the program
		return true;

	return false;
}
void Initialization(){
	//*Initialization for the program

	signal(SIGINT, sigHandle);//*start listener for signal


	hostname[1023] = '\0';
	gethostname(hostname, 1023);//* get the host name
	loginChar=getlogin();//* get the login
}
