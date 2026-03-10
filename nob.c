#include <stdbool.h>
#include <string.h>
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "thirdparty/nob.h"

#define BUILD_FOLDER "build"
#define SHARED_FOLDER BUILD_FOLDER "/shared"
#define STATIC_FOLDER BUILD_FOLDER "/static"
#define SHARED_ALLOCATOR_FOLDER SHARED_FOLDER "/allocators"
#define STATIC_ALLOCATOR_FOLDER STATIC_FOLDER "/allocators"
#define TEST_FOLDER BUILD_FOLDER "/tests"

#define CC "cc"

#define cmd_append_cflags(cmd) cmd_append((cmd), "-ggdb")
#define cmd_append_lflags(cmd)                                                 \
  cmd_append((cmd), "-Lthirdparty/RGFW-1.70_linux_amd64/lib/", "-lX11",        \
             "-lXrandr", "-lGLX", "-lGL", "-lfreetype", "-lm")

#define cmd_append_shared_cflags(cmd)                                          \
  cmd_append((cmd), "-fPIC", "-rdynamic", "-shared")

void cleanDirectory(Cmd *cmd, const char *dir) {
  cmd_append(cmd, "rm", "-rf", dir);
  cmd_run(cmd);

  cmd_append(cmd, "mkdir", "-p", dir);
  cmd_run(cmd);
}

bool buildSharedLib(Cmd *cmd) {
  cleanDirectory(cmd, SHARED_FOLDER);

  // Build core.o used to build shared library
  cmd_append(cmd, CC, "-fPIC");
  cmd_append_cflags(cmd);
  cmd_append(cmd, "-c", "core.c", "-llibRGFW.a");
  cmd_append_lflags(cmd);
  cmd_append(cmd, "-o", SHARED_FOLDER "/core.o");

  if (!cmd_run(cmd)) {
    nob_log(NOB_ERROR, "[SHARED]: Failed to compile core.o\n");
    return false;
  }

  // Build shared library
  cmd_append(cmd, CC);
  cmd_append_cflags(cmd);
  cmd_append_shared_cflags(cmd);
  cmd_append(cmd, "-o", SHARED_FOLDER "/libcore.so", SHARED_FOLDER "/core.o",
             "-lRGFW");
  cmd_append_lflags(cmd);

  return cmd_run(cmd);
}

bool buildStaticLib(Cmd *cmd) {
  cleanDirectory(cmd, STATIC_FOLDER);

  // Build static .o file to be used in archive
  cmd_append(cmd, CC);
  cmd_append_cflags(cmd);
  cmd_append(cmd, "-c", "core.c");
  cmd_append(cmd, "-o", STATIC_FOLDER "/core.o");
  cmd_append_lflags(cmd);

  if (!cmd_run(cmd)) {
    nob_log(NOB_ERROR, "[STATIC]: Failed to compile core.o\n");
    return false;
  }

  cmd_append(cmd, CC);
  cmd_append_cflags(cmd);
  cmd_append(cmd, "-c", "path/path.c");
  cmd_append(cmd, "-o", STATIC_FOLDER "/path.o");
  cmd_append_lflags(cmd);

  if (!cmd_run(cmd)) {
    nob_log(NOB_ERROR, "[STATIC]: Failed to compile path.o\n");
    return false;
  }

  // Build static archive
  cmd_append(cmd, "ar", "rcs", STATIC_FOLDER "/libcore.a",
             STATIC_FOLDER "/core.o", STATIC_FOLDER "/path.o",
             "./thirdparty/RGFW-1.70_linux_amd64/lib/libRGFW.a");
  return cmd_run(cmd);
}

bool buildAllocatorLibs(Cmd *cmd) {
  cleanDirectory(cmd, STATIC_ALLOCATOR_FOLDER);

  // Build static .o file to be used in archive
  cmd_append(cmd, CC);
  cmd_append_cflags(cmd);
  cmd_append(cmd, "-c", "allocators/gc/gc.c");
  cmd_append(cmd, "-o", STATIC_ALLOCATOR_FOLDER "/gc.o");
  // cmd_append_lflags(cmd);

  if (!cmd_run(cmd)) {
    nob_log(NOB_ERROR, "[STATIC]: Failed to compile gc.o\n");
    return false;
  }

  // Build static archive
  cmd_append(cmd, "ar", "rcs", STATIC_ALLOCATOR_FOLDER "/libgc.a",
             STATIC_ALLOCATOR_FOLDER "/gc.o");
  if (!cmd_run(cmd)) {
    nob_log(NOB_ERROR, "[STATIC]: Failed to create archive from gc.o\n");
    return false;
  }

  // Build static .o file to be used in archive
  cmd_append(cmd, CC);
  cmd_append_cflags(cmd);
  cmd_append(cmd, "-c", "allocators/heap/heap.c");
  cmd_append(cmd, "-o", STATIC_ALLOCATOR_FOLDER "/heap.o");
  // cmd_append_lflags(cmd);

  if (!cmd_run(cmd)) {
    nob_log(NOB_ERROR, "[STATIC]: Failed to compile heap.o\n");
    return false;
  }

  // Build static archive
  cmd_append(cmd, "ar", "rcs", STATIC_ALLOCATOR_FOLDER "/libheap.a",
             STATIC_ALLOCATOR_FOLDER "/heap.o");
  if (!cmd_run(cmd)) {
    nob_log(NOB_ERROR, "[STATIC]: Failed to create archive from heap.o\n");
    return false;
  }

  // Build static .o file to be used in archive
  cmd_append(cmd, CC);
  cmd_append_cflags(cmd);
  cmd_append(cmd, "-c", "allocators/arena/arena.c");
  cmd_append(cmd, "-o", STATIC_ALLOCATOR_FOLDER "/arena.o");
  // cmd_append_lflags(cmd);

  if (!cmd_run(cmd)) {
    nob_log(NOB_ERROR, "[STATIC]: Failed to compile arena.o\n");
    return false;
  }

  // Build static archive
  cmd_append(cmd, "ar", "rcs", STATIC_ALLOCATOR_FOLDER "/libarena.a",
             STATIC_ALLOCATOR_FOLDER "/arena.o");
  if (!cmd_run(cmd)) {
    nob_log(NOB_ERROR, "[STATIC]: Failed to create archive from arena.o\n");
    return false;
  }

  return true;
}

bool buildTestCases(Cmd *cmd) {
  cleanDirectory(cmd, TEST_FOLDER);

  bool result = true;

  cmd_append(cmd, CC, "-o", TEST_FOLDER "/path");
  cmd_append(cmd, "./tests/path.c", "./path/path.c");
  if (!cmd_run(cmd)) {
    result = false;
  }

  cmd_append(cmd, CC, "-o", TEST_FOLDER "/threadpool");
  cmd_append(cmd, "./tests/threadpool.c", "./threadpool/threadpool.c");
  if (!cmd_run(cmd)) {
    result = false;
  }

  cmd_append(cmd, CC, "-o", TEST_FOLDER "/gc");
  cmd_append(cmd, "./tests/gc.c", "./gc/gc.c");
  if (!cmd_run(cmd)) {
    result = false;
  }

  return result;
}

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  bool buildTests = false;
  while (argc) {
    char *arg = argv[0];

    if (strcmp(arg, "test") == 0) {
      buildTests = true;
    }

    nob_shift_args(&argc, &argv);
  }

  Cmd cmd = {0};

  if (buildTests) {
    nob_log(NOB_INFO, "Building test cases\n");

    if (!buildTestCases(&cmd)) {
      nob_log(NOB_ERROR, "[TESTS]: Build FAILED!");
      return 1;
    }
    nob_log(NOB_INFO, "[TESTS]: Build successful\n");

  } else {
    nob_log(NOB_INFO, "Building shared and static libraries\n");

    cmd_append(&cmd, "mkdir", "-p", BUILD_FOLDER);
    cmd_run(&cmd);

    nob_log(NOB_INFO, "[SHARED]: Build started");
    if (!buildSharedLib(&cmd)) {
      nob_log(NOB_ERROR, "[SHARED]: Build FAILED!");
      return 1;
    }
    nob_log(NOB_INFO, "[SHARED]: Build successful\n");

    nob_log(NOB_INFO, "[STATIC]: Build started");
    if (!buildStaticLib(&cmd)) {
      nob_log(NOB_ERROR, "[STATIC]: Build FAILED!");
      return 1;
    }
    nob_log(NOB_INFO, "[STATIC]: Build successful\n");

    nob_log(NOB_INFO, "[STATIC]: Allocator builds started");
    if (!buildAllocatorLibs(&cmd)) {
      nob_log(NOB_ERROR, "[STATIC]: Allocator builds FAILED!");
      return 1;
    }
    nob_log(NOB_INFO, "[STATIC]: Allocator builds successful");
  }

  return 0;
}
