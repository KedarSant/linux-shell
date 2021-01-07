
#ifndef SHELL_H
#define SHELL_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char* hd = "";  // Home Directory

struct HistQueue {
    char** arr;
    int front;
    int rear;
    int count;

}*hist; // The history buffer

// Functions used for getting commands from the actual shell and running them

void newLine();
void lsh_command_loop();
char* lsh_read_line();
char** lsh_split_line(char*);
int execute_command(char**);
int lsh_launch(char**);

// Functions used for adding a history feature to the shell

void initHist(FILE* fp);
int isFull();
int isEmpty();
void insertCommand(char* line);
void displayHistory();
void execRec();
void peekHistory(int i);
void cleanHistory(FILE* fp);

//Prints a new line each time

void newLine() {

   char cwd[80];
   printf("");
   if (getcwd(cwd, sizeof(cwd)) != NULL) {

       if(!strcmp(cwd,hd)) printf("~$ ");
       else printf("%s$ ", cwd);
   } 

   else
   perror("getcwd() error");

}

// Runs infinitely until the user exits the shell by typing command exit or pressing control-D

void lsh_command_loop() {

    char** args;
    char* line;
    int status = 1;

    while(status) {

        newLine();
        line = lsh_read_line();
        insertCommand(line);
        args = lsh_split_line(line);
        status = execute_command(args);
        free(line);
        free(args);
    }
}

// Reads a line from STDIN

char* lsh_read_line() {

    char* line = NULL;
    ssize_t bufsize = 0;
    if(getline(&line,&bufsize,stdin)==EOF) {
        line = malloc(5*sizeof(char));
        strcpy(line,"exit");
    }
    return line;

}

//Splits a line into arguments

char** lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

// Classifies a command as a builtin, history or executable command and accordingly passes it for execution

int execute_command(char ** args) {

    int flag = 1;

    if(args[0]==NULL) return flag;

    else if(!strcmp(args[0],"cd")) chdir(args[1]);

    else if(!strcmp(args[0],"history")) 
    displayHistory();
    
    else if(!strcmp(args[0],"!!")) 
    execRec();

    else if(args[0][0] == '!') {
        char* idx_str = args[0];
        peekHistory(atoi(++idx_str));
    }

    else if(!strcmp(args[0],"exit")) {
        printf("exit\n");
        flag = 0;
    }
    
    else 
    lsh_launch(args);

    return flag;
}

// Starts a new child process and executes the command using execvp()

int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  
    if (pid == 0) {
        
        if (execvp(args[0], args) == -1) 
        perror("lsh");
        exit(EXIT_FAILURE);

    } 
    
    else if (pid < 0) 
    perror("lsh");
    
    else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

  return 1;
}


void initHist(FILE* fp) {

    hist = malloc(sizeof(struct HistQueue));
    hist->front = -1;
    hist->rear = -1;
    hist->count = 0;
    hist -> arr = malloc(10*sizeof(char*));
    while(!feof(fp)) {
        char* line = NULL;
        ssize_t bufsize = 0;
        if(getline(&line,&bufsize,fp)==EOF) return;
        insertCommand(line);
    }
}

int isFull() {

    if((hist->front == 0 && hist->rear == 9) || (hist->front == hist->rear+1)) return 1;
    return 0;

}

int isEmpty() {

    return hist->front==-1;
}

void insertCommand(char* line) {

    if(line[0]=='!' || line[0] == '\n') return;

    ++hist->count;
    if(isFull()) {

        free(hist->arr[hist->front]);
        hist->arr[hist->front] = NULL;
        if(hist->front == 9) 
        hist -> front = 0;
        else 
        hist->front = hist->front + 1;
    }

    if(hist->front == -1) {
        hist->front = 0;
        hist->rear = 0;
    } else {
        if(hist->rear == 9)
        hist->rear = 0;
        else
        hist->rear = hist->rear + 1;
    } 
    
    hist->arr[hist->rear] = malloc(sizeof(*line));
    strcpy(hist->arr[hist->rear],line);
    
}

void displayHistory() {

    int i;
    int a = hist->count;

    if(hist->rear >= hist ->front) {
        for(i = hist->rear; i>= hist->front; --i) 
        printf("%d %s",a--,hist->arr[i]);
    }
    else {

        for(i = hist->rear; i>= 0 ; --i)
        printf("%d %s",a--,hist->arr[i]);
        for(i=9; i>= hist->front; --i)
        printf("%d %s",a--,hist->arr[i]);
    }

}

void execRec() {

    if(isEmpty()) {
        printf("No command entry in history\n");
        return;
    }
    char * line = malloc(sizeof(hist->arr[hist->rear]));
    strcpy(line,hist->arr[hist->rear]);
    char ** args = lsh_split_line(line);
    execute_command(args);
    free(line);
    free(args);
}

void peekHistory(int i) {

    int index = hist->count - i;
    if(index<0 || index>9) {
        printf("No such command in history\n");
        return;
    }
    int a;
    if(hist->rear >= hist ->front) {
        for(a = hist->rear; a>= hist->front; --a) 
        if(!index--) break;
    }
    else {

        int flag=0;
        for(a = hist->rear; a>= 0 ; --a)
        if(!index--) {
            flag=1;
            break;
        }

        if(!flag)
        for(a=9; a>= hist->front; --a)
        if(!index--) break;

    }

    char * line = malloc(sizeof(hist->arr[a]));
    strcpy(line,hist->arr[a]);
    insertCommand(line);
    char ** args = lsh_split_line(line);
    execute_command(args);
    free(line);
    free(args);

}

void cleanHistory(FILE* fp) {

    int i;
    fclose(fp);
    remove("history.txt");
    fp = fopen("history.txt","w+");
    if(hist->rear >= hist ->front) {
        for(i = hist->front; i<= hist->rear; ++i) {
            fprintf(fp,"%s",hist->arr[i]);
            free(hist->arr[i]);
        } 
    }
    else {

        for(i = hist->front; i<=9  ; ++i) {
            fprintf(fp,"%s",hist->arr[i]);
            free(hist->arr[i]);
        }
        for(i=0; i<= hist->rear; ++i) {
            fprintf(fp,"%s",hist->arr[i]);
            free(hist->arr[i]);
        }
        
    }
    fclose(fp);
    free(hist->arr);
    free(hist);

}

#endif
