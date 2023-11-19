#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wait.h>


char **split_string(char *chemin, char* separateur){
    char *chem = strdup(chemin);
    int size = 1;
    for(int i=0; i<strlen(chem); ++i)
        if(chem[i]==separateur[0]) ++size;
    char** composantes = calloc(size+2, sizeof(char*));
    int i=0;
    for(char* mot = strtok(chem, separateur); mot!= NULL; mot = strtok(NULL, separateur)){
        composantes[i]=strdup(mot);
        i++;
    }
    free(chem);
    composantes[size + 1] = NULL;
    return composantes;
}


typedef struct command_t {
    char **argv;
    int argc;
} command_t;


void free_command(command_t *command){
    for (int i=0; i<command->argc; ++i) free(command->argv[i]);
    free(command->argv);
}

command_t *read_command(){
    char *read = readline(">");
    if (read == NULL) return NULL;
    add_history(read);
    char **argv = split_string(read," ");
    int argc;
    for (argc=0; argv[argc] != NULL; ++argc);

    command_t *out = malloc(sizeof(command_t));
    *out = (command_t){ .argc = argc, .argv = argv};
    return out;
}

int main(){
    int status;
    int return_code = 0;

    char buf[1024];

    rl_initialize();
    rl_outstream = stderr;

    while(1) {
        command_t *command = read_command();


        if (strcmp (command->argv[0],"?") == 0){
            printf("%d\n", return_code);
        }
        else if (strcmp (command->argv[0],"pwd") == 0){
            if(getcwd(buf, sizeof(buf)) != NULL) {
                printf("%s\n", buf);
            }
            printf("pwd Failure\n");
        }
        else if (fork() == 0) {
            execvp(command->argv[0], command->argv);
        } else {
            waitpid(0, &status, 0);
            return_code = WEXITSTATUS(status);
        }

        free_command(command);
    }

    return 0;
}

