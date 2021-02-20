#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();

  struct IDTDescr {
    uint16_t offset_1; // offset bits 0..15
    uint16_t selector; // a code segment selector in GDT or LDT
    uint8_t zero;      // unused, set to 0
    uint8_t type_attr; // type and attributes, see below
    uint16_t offset_2; // offset bits 16..31
  };

  vaddr_t addr = (vaddr_t)cpu.idtr.base + NO*2;
  struct IDTDescr gt;
  word_t *pgt = (word_t *)&gt;
  pgt[0] = vaddr_read(addr, 4); 
  pgt[1] = vaddr_read(addr, 4); 

  vaddr_t jmp_pc = gt.offset_1 + (gt.offset_2 << 16);

  rtl_push(s, &cpu.eflags);
  rtl_push(s, &cpu.cs);
  rtl_push(s, &cpu.pc);

  s->is_jmp = true;
  s->jmp_pc = jmp_pc;
  Log("int jmp pc %x", jmp_pc);
}

void query_intr(DecodeExecState *s) {
  TODO();
}
