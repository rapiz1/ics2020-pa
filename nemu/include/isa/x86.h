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

  union {
    rtlreg_t eflags;
    struct {
      unsigned CF:1;
      unsigned _reserved1:1; 
      unsigned PF:1;
      unsigned _reserved2:1;
      unsigned AF:1;
      unsigned _reserved3:1;
      unsigned ZF:1;
      unsigned SF:1;
      unsigned TF:1;
      unsigned IF:1;
      unsigned DF:1;
      unsigned OF:1;
      unsigned IOPL:1;
      unsigned NT:1;
      unsigned _reserved4:1;
      unsigned RF:1;
      unsigned VM:1;
      unsigned padding:15;
    };
  };

  vaddr_t pc;
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
#define isa_vaddr_check(vaddr, type, len) (MEM_RET_OK)
#define x86_has_mem_exception() (false)

#endif
