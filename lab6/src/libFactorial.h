#ifndef VARIABLE
#define VARIABLE
#include <stddef.h>
#include <stdint.h>
struct FactorialArgs {
  uint64_t begin;
  uint64_t end;touch
  uint64_t mod;
};

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);

#endif