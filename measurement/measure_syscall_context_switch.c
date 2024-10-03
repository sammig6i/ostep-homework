#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>
#include <sys/wait.h>

#define NUM_ITERATIONS 10000000
#define CONTEXT_SWITCH_ITERATIONS 100000

struct timespec diff(struct timespec start, struct timespec end) {
  struct timespec temp;
  if ((end.tv_nsec - start.tv_nsec) < 0) {
    temp.tv_sec = end.tv_sec - start.tv_sec - 1;
    temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
  }
  else {
    temp.tv_sec = end.tv_sec - start.tv_sec;
    temp.tv_nsec = end.tv_nsec - start.tv_nsec;
  }
  return temp;
}

void PerformSystemCall() {
  read(0, NULL, 0);
}

double MeasureSystemCallCost() {
  struct timespec start, end, delta;
  long long total_ns;

  // Measure loop overhead
  clock_gettime(CLOCK_MONOTONIC, &start);
  for (int i = 0; i < NUM_ITERATIONS; i++) {
    // Empty loop
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  struct timespec overhead = diff(start, end);

  // Measure system call time
  clock_gettime(CLOCK_MONOTONIC, &start);
  for (int i = 0; i < NUM_ITERATIONS; i++) {
    PerformSystemCall();
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  delta = diff(start, end);

  // Subtract overhead
  delta.tv_sec -= overhead.tv_sec;
  delta.tv_nsec -= overhead.tv_nsec;
  if (delta.tv_nsec < 0) {
    delta.tv_sec--;
    delta.tv_nsec += 1000000000;
  }

  total_ns = delta.tv_sec * 1000000000LL + delta.tv_nsec;
  return (double)total_ns / NUM_ITERATIONS;
}

double MeasureContextSwitchCost() {
  int pipe1[2], pipe2[2];
  char buf[1];
  struct timespec start, end, delta;
  pid_t pid;

  if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
    perror("pipe");
    exit(1);
  }

  pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  }

  if (pid == 0) { // Child process
    close(pipe1[1]);
    close(pipe2[0]);

    for (int i = 0; i < CONTEXT_SWITCH_ITERATIONS; i++) {
      if (read(pipe1[0], buf, 1) != 1) {
        perror("read");
        exit(1);
      }
      if (write(pipe2[1], "c", 1) != 1) {
        perror("write");
        exit(1);
      }
    }

    exit(0);
  }
  else { // Parent process

    close(pipe1[0]);
    close(pipe2[1]);

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < CONTEXT_SWITCH_ITERATIONS; i++) {
      if (write(pipe1[1], "p", 1) != 1) {
        perror("write");
        exit(1);
      }
      if (read(pipe2[0], buf, 1) != 1) {
        perror("read");
        exit(1);
      }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    wait(NULL);

    delta = diff(start, end);
    long long total_ns = delta.tv_sec * 1000000000LL + delta.tv_nsec;
    return (double)total_ns / (CONTEXT_SWITCH_ITERATIONS * 2);
  }
}

int main() {
  double avg_syscall_ns = MeasureSystemCallCost();
  printf("Average system call cost: %.2f nanoseconds\n", avg_syscall_ns);

  double avg_context_switch_ns = MeasureContextSwitchCost();
  printf("Average context switch cost: %.2f nanoseconds\n", avg_context_switch_ns);

  return 0;
}
