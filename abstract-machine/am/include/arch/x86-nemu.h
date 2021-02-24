#ifndef ARCH_H__
#define ARCH_H__

struct Context {
  void *cr3;
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  int irq;
  uintptr_t eip, cs, eflags;
};

#define GPR1 eax
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPRx eax

#define BITMASK(bits) ((1ull << (bits)) - 1)
#define BIT(x, idx) (((x)>>(idx))&1)
#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1)) // similar to x[hi:lo] in verilog
#define GET_DIR(x) BITS((uintptr_t)x, 31, 22)
#define GET_PAGE(x) BITS((uintptr_t)x, 21, 12)
#define GET_OFFSET(x) BITS((uintptr_t)x, 11, 0)

typedef struct PTE {
  uintptr_t present:1;
  uintptr_t read_write:1;
  uintptr_t user_supervisor:1;
  uintptr_t _reserve1:2;
  uintptr_t accessed:1;
  uintptr_t dirty:1;
  uintptr_t _reserve2:2;
  uintptr_t avail:2;
  uintptr_t page_frame_address:20;
} PTE;


#endif
