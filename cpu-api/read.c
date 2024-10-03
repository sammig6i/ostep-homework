#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

void wait_or_die() {
    int rc = wait(NULL);
    assert(rc > 0);
}

int fork_or_die() {
    int rc = fork();
    assert(rc >= 0);
    return rc;
}

int main(int argc, char *argv[]) {
    // process a
    if (fork_or_die() == 0) {
        sleep(8);
        // process b
        if (fork_or_die() == 0) {
            sleep(10);
            // process c
            if (fork_or_die() == 0) {
                sleep(4);
                // process d
                if (fork_or_die() == 0) {
                    sleep(7);
                    // process e
                    if (fork_or_die() == 0) {
                        sleep(3);
                        // process f
                        if (fork_or_die() == 0) {
                            sleep(7);
                            // process g
                            if (fork_or_die() == 0) {
                                sleep(9);
                                // process h
                                if (fork_or_die() == 0) {
                                    sleep(7);
                                    // process i
                                    exit(0);
                                }
                                if (fork_or_die() == 0) {
                                    sleep(3);
                                    // process j
                                    exit(0);
                                }
                                wait_or_die();
                                wait_or_die();
                                exit(0);
                            }
                            wait_or_die();
                            exit(0);
                        }
                        wait_or_die();
                        exit(0);
                    }
                    wait_or_die();
                    exit(0);
                }
                wait_or_die();
                exit(0);
            }
            wait_or_die();
            exit(0);
        }
        wait_or_die();
        exit(0);
    }
    wait_or_die();
    return 0;
}

