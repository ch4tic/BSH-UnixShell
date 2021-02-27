/* INCLUDES */
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* DEFINES */
#define SHELL_BUFSIZE 1024 
#define SHELL_TOK_BUFSIZE 64
#define SHELL_TOK_DELIM " \t\r\n\a"

/* BUILTIN SHELL COMMAND DECLARATIONS*/
int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

/* LIST OF BUILTINS */
char *builtin_str[] = {
	"cd",
	"help",
	"exit"
};

int (*builtin_func[]) (char **) = {
	&shell_cd,
	&shell_help,
	&shell_exit
};

int shell_num_builtins()
{
	return sizeof(builtin_str) / sizeof(char *);
}
/* function for cd shell builtin*/
int shell_cd(char **args)
{
	char home_path[] = "/home/ch4tic/";
	if(args[1] == NULL)
	{
		chdir(home_path);
	}else
	{
		if(chdir(args[1]) != 0)
		{
			perror("shell");
		}
	}
	return 1;
}

int shell_help(char **args)
{
	int i;
	printf("Hello, friend.\n");
	printf("The following shell builtins in bsh are:\n");
	for(i = 0; i < shell_num_builtins(); i++)
	{
		printf("%s\n", builtin_str[i]);
	}
	printf("More coming soon.");
	return 1;
}

int shell_exit(char **args)
{
	printf("Goodbye, friend.\n");
	sleep(1);
	return 0;
}

int shell_launch(char **args)
{
	pid_t pid, wpid;
	int current_status;

	pid = fork();
	if(pid == 0)
	{
		/* fork() child process*/
		if(execvp(args[0], args) == -1)
		{
			perror("shell");
		}	
		exit(EXIT_FAILURE);
	/* fork() error handle*/
	}else if(pid < 0)
	{
		perror("shell");
	}else
	{
		/* fork() parent process*/
		do
		{
			wpid = waitpid(pid, &current_status, WUNTRACED); /* waiting for a process to stop or terminate`*/
		}while(!WIFEXITED(current_status) && !WIFSIGNALED(current_status));
	}
	return 1;
}

int shell_execute(char **args)
{
	int i;
	if(args[0] == NULL)
	{
		return 1;
	}
	for (i = 0; i < shell_num_builtins(); i++) {
    		if (strcmp(args[0], builtin_str[i]) == 0) 
		{
      			return (*builtin_func[i])(args);
    		}
  	}
  	return shell_launch(args);
}

char *shell_read_line(void)
{
	int bufsize = SHELL_BUFSIZE; /* setting bufsize equal to our define before*/
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize); /* allocating memory and returning a pointer*/
	int c;
	/* if the program cannot allocate memory to buffer it will quit*/
	if(!buffer)
	{
		fprintf(stderr, "shell: allocation error\n"); 
		exit(EXIT_FAILURE); 
	}

	while(1)
	{
		c = getchar(); /* reading characters*/
		/* if we hit EOF(End Of File) we replace it with a NULL and return it*/
		if(c == EOF || c == '\n')
		{
			buffer[position] = '\0';
			return buffer;
		}else
		{
			buffer[position] = c;
		}
		position++;
	}

	/* if the user exceeded the buffer we reallocate it*/
	if(position >= bufsize)
	{
		bufsize += SHELL_BUFSIZE;
		buffer = realloc(buffer, bufsize);

		/* if the program cannot resize buffer's memory it will quit*/
		if(!buffer)
		{
			fprintf(stderr, "shell: allocation error\n");
			exit(EXIT_FAILURE);
		}
	}
}

char **shell_split_line(char *line)
{
	int bufsize = SHELL_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if(!tokens)
	{
		fprintf(stderr, "shell: allocation error\n");
		exit(EXIT_FAILURE);
	}
	token = strtok(line, SHELL_TOK_DELIM); /* using strtok() we will break the string into tokens*/
	while(token != NULL)
	{
		tokens[position] = token; 
		position++;
	
	
		if(position >= bufsize)
		{
			bufsize += SHELL_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			/* if the program cannot resize token's memory it exits*/
			if(!tokens)
			{
				fprintf(stderr, "shell: allocation error occurred\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, SHELL_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

/* MAIN LOGIC*/
void shell_loop(void)
{
	/* set your system information like user and hostname*/
	char username[] = "ch4tic"; 
	char hostname[] = "hangfire";

	char *line; 
	char **args; 
	int current_status;
	
	do
	{
		printf("%s@%s:> ", username, hostname);
		line = shell_read_line(); /* reading inputted lines*/
		args = shell_split_line(line); /* splitting those lines into different arguments*/
		current_status = shell_execute(args); /* executing those arguments*/

		/* deallocating memory from line and args pointers*/
		free(line);
		free(args);

	}while(current_status);
}

/* INITIALIZATION*/
int main(int argc, char **argv)
{
	system("clear"); /* clearing the screen before calling the main function*/
	printf("- BasicShell -\n\n");
	shell_loop(); /* Calling the shell function*/
	return EXIT_SUCCESS; 
}
