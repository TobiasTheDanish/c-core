#include <stdio.h>
#include <unistd.h>

#include "../threadpool.h"

void *executor(void *arg) {
  printf("Executor started!\n");

  printf("Executor sleeping for 1 seconds\n");
  usleep(1 * 1000 * 1000);

  printf("Executor finished!\n");
  return NULL;
}

int main() {
  Thread_InitializePool();

  int32_t polls = 0;
  Thread_ExecutionHandle handle = Thread_StartExecution(&executor, 0);
  while (true) {
    polls++;
    Thread_ExecutionResult r = Thread_PollResult(handle);
    if (r.kind == Thread_ExecutionResultReady) {
      printf("Main thread received result after %d polls\n", polls);
      break;
    }
  }

  Thread_ShutdownPool();

  return 0;
}
