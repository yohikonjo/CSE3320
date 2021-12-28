/*
  Name:Rafel Tsige
  ID:1001417200
*/
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 12     // Mav shell only supports five arguments

#define MAX_NUM_ENTRIES 15      // Shell only holds on to last 15 command entries and process ids

/*Global Variables*/
char *hist[15];
int cmd_count = 0;
pid_t pids[15];
int pid_count = 0;

/*Functions*/
void add_cmd(char *new_cmd);
void add_pid(pid_t pid);

int main()
{

  char *cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
  char history_queue[MAX_NUM_ENTRIES][MAX_COMMAND_SIZE];
  int counter = 0;      //Counts number of commands inputed by the user 
  while(1)
  {
    // Print out the msh prompt
    printf("msh> ");
    
    // Read the command from the commandline. The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while(!fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
   
    // Checks if the user has entered nothing or just pressed 
    // enter without inputting a command and if so it will skip 
    // all the code after this point and reprompt the user to
    // input a command   
    if(strcmp(cmd_str, "\n") == 0)
    {
      continue;
    }

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];
    
    int token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // Function call to add command to global history array
    add_cmd(working_str);
    
    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
      token_count++;
    }

    //separate the command from the parameters to later pass into exec
    //function ex:- msh> ls -l     where ls is the command and -l is
    //the parameter
    char *arguments[MAX_NUM_ARGUMENTS];
    int i;
    int j = 0;
    for (i = 0; i < MAX_NUM_ARGUMENTS; i++)
    {
      arguments[i] = token[j];
      j++; 
    }

    /*Checks if the requested command exists in history and if so will re-run the command*/
    if(token[0][0] == '!')
    {
      int num;
      /*Convert the string after the '!' into an integer*/
      char *sub = malloc(strlen(token[0]));
      sub = token[0] + 1;
      num = atoi(sub);
      
      if(num > 15 || num > cmd_count)
      {
        printf("Command is out of bounds of the current history.\n");
      } 
      else if(num == 0)
      {
        printf("Invalid input.\n");
      }
      else 
      { 
        strcpy(cmd_str, hist[num]);
        int token_count = 0;
        char *argument_ptr;
        char *working_str  = strdup( cmd_str );                
       
        // we are going to move the working_str pointer so
        // keep track of its original value so we can deallocate
        // the correct amount at the end
        char *working_root = working_str;

        // Tokenize the input strings with whitespace used as the delimiter
        while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
        {
          token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
          if( strlen( token[token_count] ) == 0 )
          {
            token[token_count] = NULL;
          }
          token_count++;   
        }
        //separate the command from the parameters to later pass into exec
        //function ex:- msh> ls -l     where ls is the command and -l is
        //the parameter
        char *arguments[MAX_NUM_ARGUMENTS];
        int i;
        int j = 0;
        for (i = 0; i < MAX_NUM_ARGUMENTS; i++)
        {
          arguments[i] = token[j];
          j++; 
        }
        int ret = execvp(token[0], arguments);
        if(ret == -1)
	{
	  printf("%s: Command not found.\n\n", token[0]);
        }
      }
    }

    /*Program exit check*/
    else if (strcmp(token[0], "exit") == 0)
    {
      exit(0);
    }

    /*Program exit check*/
    else if(strcmp(token[0], "quit") == 0)
    {
      exit(0);
    }

    /*Prints upto the last 15 commands entered into the shell*/
    else if(strcmp(token[0], "history") == 0)
    {
      int i;
      for(i = 0; i < 15; i++)
      {
        if(strlen(hist[i])!= 0)
        {
          printf("%d: %s\n", i, hist[i]);
        }
        else
        {
          break;
        }
      }
    }

    /*Prints the PIDs of upto the last 15 commands entered into the shell*/
    else if(strcmp(token[0], "showpids") == 0)
    {
      int i; 
      for(i = 0; i < 15; i++)
      {
        if(pids[i] > 0)
        {
          printf("%d: %d\n", i, pids[i]); 
        }
        else
        {
          break;
        }
      }
    }

    /*Directory change impementation*/
    else if(strcmp(token[0], "cd") == 0)
    {
      chdir(token[1]);
    }

    else
    {
      pid_t pid = fork();
      if( pid == 0)
      {
        int ret = execvp(token[0], arguments);
        if(ret == -1)
	{
	  printf("%s: Command not found.\n\n", token[0]);
	}
      }

      else
      {
        add_pid(pid);
        int status;
        wait(&status);
      }
    }
 
    free(working_root);

  }
  return 0;
}

/*Updates the history with the new command entered*/
void add_cmd(char *new_cmd)
{
  char *temp[15];
  int i;
  int j = 1;
  if(cmd_count < 15)
  {
    hist[cmd_count] = malloc(strlen(new_cmd)+1);
    strcpy(hist[cmd_count], new_cmd);
    cmd_count++;
  }
  else
  {
    for(i = 0; i < 14; i++)
    {
      temp[i] = malloc(strlen(hist[j] + 1));
      strcpy(temp[i], hist[j]);
      free(hist[j]);
      j++;
    }
    j = 1;
    free(hist[0]);
    temp[14] = malloc(strlen(new_cmd+1));
    strcpy(temp[14], new_cmd);
    for(i = 0; i < 15; i++)
    {
      hist[i] = malloc(strlen(temp[i]+1));
      strcpy(hist[i], temp[i]);
    } 
  }
}

/*Updates the PIDS with the PID of the new command entered*/
void add_pid(pid_t pid)
{
  int i;
  int j = 1;
  pid_t temp[15];
  if(pid_count < 15)
  {
    pids[pid_count] = pid;
    pid_count++;
  } 
  else 
  {
    for(i = 0; i < 14; i++)
    {
      temp[i] = pids[j];
      j++;
    }
    j = 1;
    temp[14] = pid;
    for(i = 0; i < 15; i++)
    {
      pids[i] = temp[i];
    }
  }
}

