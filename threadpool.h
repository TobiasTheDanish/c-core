#ifndef CORE_THREADPOOL_H
#define CORE_THREADPOOL_H
#include "common.h"

typedef void *(Thread_Executor)(void *arg);
typedef uint64_t Thread_ExecutionHandle;
typedef enum {
  Thread_ExecutionResultNotReady = 0,
  Thread_ExecutionResultReady,
  Thread_ExecutionError,
} Thread_ExecutionResultKind;
typedef struct {
  Thread_ExecutionResultKind kind;
  union {
    void *result;
    void *error;
  } as;
} Thread_ExecutionResult;

void Thread_InitializePool();
Thread_ExecutionHandle Thread_StartExecution(Thread_Executor *executor,
                                             void *arg);
Thread_ExecutionResult Thread_PollResult(Thread_ExecutionHandle handle);
void Thread_ShutdownPool();

#endif // !CORE_THREADPOOL_H
