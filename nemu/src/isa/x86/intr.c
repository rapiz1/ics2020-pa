#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();

  Log("int 0x%x", NO);

  // Gate descriptors for interrupts and traps
  typedef struct {
    uint32_t off_15_0  : 16; // Low 16 bits of offset in segment
    uint32_t cs        : 16; // Code segment selector
    uint32_t args      :  5; // # args, 0 for interrupt/trap gates
    uint32_t rsv1      :  3; // Reserved(should be zero I guess)
    uint32_t type      :  4; // Type(STS_{TG,IG32,TG32})
    uint32_t s         :  1; // Must be 0 (system)
    uint32_t dpl       :  2; // Descriptor(meaning new) privilege level
    uint32_t p         :  1; // Present
    uint32_t off_31_16 : 16; // High bits of offset in segment
  } GateDesc32;

  vaddr_t addr = (vaddr_t)(cpu.idtr.base + NO*sizeof(GateDesc32));
  GateDesc32 gt;
  word_t *pgt = (word_t *)&gt;
  pgt[0] = vaddr_read(addr, 4); 
  pgt[1] = vaddr_read(addr+4, 4); 

  vaddr_t jmp_pc = gt.off_15_0 | (((uint32_t)gt.off_31_16) << 16);
  Log("gt offset: %x, %x", gt.off_15_0, gt.off_31_16);

  rtl_push(s, &cpu.eflags);
  rtl_push(s, &cpu.cs);
  rtl_push(s, &cpu.pc);

  s->is_jmp = true;
  s->jmp_pc = jmp_pc;
  Log("int jmp pc %x",jmp_pc);
}

void query_intr(DecodeExecState *s) {
  TODO();
}
