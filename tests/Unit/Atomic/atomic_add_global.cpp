// RUN: %amp_device -D__GPU__ %s -m32 -emit-llvm -c -S -O3 -o %t.ll && mkdir -p %t
// RUN: %llc -march=c -o %t/kernel_.cl < %t.ll
// RUN: cat %opencl_math_dir/opencl_math.cl %t/kernel_.cl > %t/kernel.cl
// RUN: pushd %t && objcopy -B i386:x86-64 -I binary -O elf64-x86-64 kernel.cl %t/kernel.o && popd
// RUN: %cxxamp %link %t/kernel.o %s -o %t.out && %t.out
#include <amp.h>
#include <stdlib.h>
#include <iostream>

using namespace concurrency;

int main(void) {
  const int vecSize = 100;

  // Alloc & init input data
  array<int, 1> count(vecSize);
  for(unsigned i = 0; i < vecSize; i++) {
    count[i] = 0;
  }

  parallel_for_each(count.extent, [=, &count](index<1> idx) restrict(amp) {
    for(unsigned i = 0; i < vecSize; i++) {
      atomic_fetch_add(&count[i], 1);
    }
  });

  for(unsigned i = 0; i < vecSize; i++) {
      if(count[i] != vecSize) {
        return 1;
      }
  }

  return 0;
}
