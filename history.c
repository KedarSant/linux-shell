
#include "shell.h"

int main(int argc,char* argv[] ) {
 
    FILE* fp;
    if((fp=fopen("history.txt","r+"))==NULL) {
        perror("Could not run history buffer.\n");
        exit(1);
    }
    initHist(fp);

    lsh_command_loop();

    cleanHistory(fp);
    
}