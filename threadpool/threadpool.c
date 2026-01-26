#include "../threadpool.h"
#include <asm-generic/errno.h>
#include <assert.h>
#include <bits/time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  pthread_t thread;
  pthread_mutex_t start_mutex;
  pthread_mutex_t result_mutex;
  pthread_cond_t start_cond;
  pthread_cond_t result_cond;
  Thread_Executor *executor;
  void *arg;
  void *result;
} Thread__ExecutorContext;

#define N_THREADS 12
#define NSEC_PER_MSEC (1000 * 1000)
#define NSEC_PER_SEC (1000 * 1000 * 1000)

Thread__ExecutorContext threads[N_THREADS];

void *Thread_StartRoutine(void *arg) {
  printf("Thread %lu started\n", pthread_self());

  Thread__ExecutorContext *ctx = (Thread__ExecutorContext *)arg;

  for (;;) {
    pthread_mutex_lock(&ctx->start_mutex);
    printf("Thread %lu waiting for start condition (%p) with mutex (%p)\n",
           pthread_self(), &ctx->start_cond, &ctx->start_mutex);
    pthread_cond_wait(&ctx->start_cond, &ctx->start_mutex);
    pthread_mutex_unlock(&ctx->start_mutex);

    printf("Thread %lu received start condition\n", pthread_self());

    if (ctx->executor != 0) {
      ctx->result = (*ctx->executor)(ctx->arg);
    } else {
      break;
    }

    pthread_mutex_lock(&ctx->result_mutex);
    printf("Thread %lu sending result condition\n", pthread_self());
    pthread_cond_signal(&ctx->result_cond);
    pthread_mutex_unlock(&ctx->result_mutex);
  }

  printf("Thread %lu finished\n", pthread_self());
  return NULL;
}

void Thread_InitializePool() {
  for (int32_t i = 0; i < N_THREADS; i++) {
    threads[i] = (Thread__ExecutorContext){
        .start_mutex = PTHREAD_MUTEX_INITIALIZER,
        .result_mutex = PTHREAD_MUTEX_INITIALIZER,
        .start_cond = PTHREAD_COND_INITIALIZER,
        .result_cond = PTHREAD_COND_INITIALIZER,
        .executor = 0,
        .result = 0,
    };

    pthread_create(&threads[i].thread, 0, &Thread_StartRoutine,
                   (void *)&threads[i]);
  }
}

Thread_ExecutionHandle Thread_StartExecution(Thread_Executor *executor,
                                             void *arg) {
  Thread_ExecutionHandle h = 0;
  for (; h < N_THREADS; h++) {
    if (threads[h].executor == 0) {
      threads[h].executor = executor;
      threads[h].arg = arg;
      threads[h].result = 0;
      break;
    }
  }
  if (h >= N_THREADS) {
    printf("Threadpool is fully occupied\n");
    abort();
  }

  Thread__ExecutorContext *ctx = &threads[h];
  pthread_mutex_lock(&ctx->start_mutex);
  printf("Main thread sending start condition (%p) with mutex (%p) to thread: "
         "%lu\n",
         &ctx->start_cond, &ctx->start_mutex, ctx->thread);
  pthread_cond_signal(&ctx->start_cond);
  pthread_mutex_unlock(&ctx->start_mutex);

  return h;
}

Thread_ExecutionResult Thread_PollResult(Thread_ExecutionHandle handle) {
  assert(0 <= handle && handle < N_THREADS);

  Thread__ExecutorContext *ctx = &threads[handle];

  struct timespec t = {0};
  clock_gettime(CLOCK_REALTIME, &t);
  long tempNsec = (t.tv_nsec + 5 * NSEC_PER_MSEC);
  t.tv_sec += tempNsec / NSEC_PER_SEC;
  t.tv_nsec = tempNsec % NSEC_PER_SEC;

  Thread_ExecutionResult res = {0};

  pthread_mutex_lock(&ctx->result_mutex);

  int r = pthread_cond_timedwait(&ctx->result_cond, &ctx->result_mutex, &t);
  if (r == 0) {
    // Result is ready
    res.kind = Thread_ExecutionResultReady;
    res.as.result = ctx->result;

    ctx->executor = 0;
    ctx->arg = 0;
    ctx->result = 0;
  } else if (r == ETIMEDOUT) {
    res.kind = Thread_ExecutionResultNotReady;
  } else {
    printf("An error occured during thread execution, errno: %d\n", r);
    abort();
  }
  pthread_mutex_unlock(&ctx->result_mutex);

  return res;
}

void Thread_ShutdownPool() {
  for (int32_t i = 0; i < N_THREADS; i++) {
    pthread_cancel(threads[i].thread);
  }

  for (int32_t i = 0; i < N_THREADS; i++) {
    pthread_join(threads[i].thread, NULL);
  }
}
