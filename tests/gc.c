#include "../gc.h"

void allocAndLeak() {
  uint64_t ptrs_count = 100;
  void *ptrs[ptrs_count];
  (void)ptrs;
  for (uint64_t i = 0; i < ptrs_count; i++) {
    if (i % 3 == 0) {
      ptrs[i] = CORE_GCAlloc((i + 1) * 8);
    } else {
      CORE_GCAlloc((i + 1) * 8);
    }
  }

  CORE_GCCollect();

  CORE_GCDumpInfo();
}

int main() {
  allocAndLeak();

  CORE_GCCollect();
  CORE_GCDumpInfo();

  return 0;
}
