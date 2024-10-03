#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// 8. Write a program that creates two children, and connects the standard output
// of one to the standard input of the other, using the pipe() system call.

int main(int argc, char* argv[]) {
    int pipefd[2];
    pid_t child1, child2;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
    }

    child1 = fork();
    if (child1 < 0) {
        perror("fork");
        exit(1);
    } else if (child1 == 0) {
        // First child process
        close(pipefd[0]);                // Close unused read end
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe
        close(pipefd[1]);

        char* myargs[2];
        myargs[0] = "ls";
        myargs[1] = NULL;
        execvp(myargs[0], myargs);  // Execute ls command
        perror("execvp");
        exit(1);
    }

    child2 = fork();
    if (child2 < 0) {
        perror("fork");
        exit(1);
    } else if (child2 == 0) {
        // Second child process
        close(pipefd[1]);               // Close unused write end
        dup2(pipefd[0], STDIN_FILENO);  // Redirect stdin from pipe
        close(pipefd[0]);

        char* myargs[3];
        myargs[0] = "wc";
        myargs[1] = "-l";
        myargs[2] = NULL;
        execvp(myargs[0], myargs);  // Execute wc -l command
        perror("execvp");
        exit(1);
    }

    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    return 0;
}
