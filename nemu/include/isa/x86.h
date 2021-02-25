#ifndef __ISA_X86_H__
#define __ISA_X86_H__

#include <common.h>


// memory
#define x86_IMAGE_START 0x100000
#define x86_PMEM_BASE 0x0

// reg

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
  union {
    struct {
      union {
        uint32_t _32;
        uint16_t _16;
        uint8_t _8[2];
      };
    } gpr[8];

    /* Do NOT change the order of the GPRs' definitions. */

    /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
    * in PA2 able to directly access these registers.
    */
    struct {
      rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    };
  };

  vaddr_t pc;

  union {
    rtlreg_t eflags;
    struct {
      uint32_t CF:1;
      uint32_t _reserved1:1; 
      uint32_t PF:1;
      uint32_t _reserved2:1;
      uint32_t AF:1;
      uint32_t _reserved3:1;
      uint32_t ZF:1;
      uint32_t SF:1;
      uint32_t TF:1;
      uint32_t IF:1;
      uint32_t DF:1;
      uint32_t OF:1;
      uint32_t IOPL:2;
      uint32_t NT:1;
      uint32_t _reserved4:1;
      uint32_t RF:1;
      uint32_t VM:1;
      uint32_t padding:14;
    };
  };

  uint32_t cs, ss, ds, es, fs, gs;

  rtlreg_t gdtr, ldtr;
  struct idtr {
    uint16_t limit;
    uint32_t base;
  } idtr;

  rtlreg_t tr;

  rtlreg_t cr0, cr1, cr2, cr3;

  bool INTR;
} x86_CPU_state;

// decode
typedef struct {
  bool is_operand_size_16;
  uint8_t ext_opcode;
  const rtlreg_t *mbase;
  rtlreg_t mbr;
  word_t moff;
} x86_ISADecodeInfo;

#define suffix_char(width) ((width) == 4 ? 'l' : ((width) == 1 ? 'b' : ((width) == 2 ? 'w' : '?')))
#define isa_vaddr_check(vaddr, type, len) (BITS(cpu.cr0, 31, 31))
#define x86_has_mem_exception() (false)

#define GET_DIR(x) BITS((uintptr_t)x, 31, 22)
#define GET_PAGE(x) BITS((uintptr_t)x, 21, 12)
#define GET_OFFSET(x) BITS((uintptr_t)x, 11, 0)

typedef union {struct {
  uint32_t present:1;
  uint32_t read_write:1;
  uint32_t user_supervisor:1;
  uint32_t _reserve1:2;
  uint32_t accessed:1;
  uint32_t dirty:1;
  uint32_t _reserve2:2;
  uint32_t avail:2;
  uint32_t page_frame_address:20;
}; uint32_t val; }PTE;

typedef union {
  struct {
    uint32_t lim_15_0   : 16; // Low bits of segment limit
    uint32_t base_15_0  : 16; // Low bits of segment base address
    uint32_t base_23_16 :  8; // Middle bits of segment base address
    uint32_t type       :  4; // Segment type (see STS_ constants)
    uint32_t s          :  1; // 0 = system, 1 = application
    uint32_t dpl        :  2; // Descriptor Privilege Level
    uint32_t p          :  1; // Present
    uint32_t lim_19_16  :  4; // High bits of segment limit
    uint32_t avl        :  1; // Unused (available for software use)
    uint32_t l          :  1; // 64-bit segment
    uint32_t db         :  1; // 32-bit segment
    uint32_t g          :  1; // Granularity: limit scaled by 4K when set
    uint32_t base_31_24 :  8; // High bits of segment base address
  };
  uint32_t val[2];
} SegDesc;

// CPU rings
#define DPL_KERN       0x0     // Kernel (ring 0)
#define DPL_USER       0x3     // User (ring 3)

#endif
