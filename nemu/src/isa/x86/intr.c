#include <cpu/exec.h>
#include "local-include/rtl.h"
#define IRQ_TIMER 32
#define FL_IF 0x2

static inline SegDesc get_descriptor(int sel) {
  vaddr_t seg_addr = cpu.gdtr.base + sizeof(SegDesc)*sel;
  return (SegDesc){
    .val = {
      vaddr_read(seg_addr, 4),
      vaddr_read(seg_addr + 4, 4)
    }
  };
}

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();

  //Log("int 0x%x", NO);

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
  //Log("gt offset: %x, %x", gt.off_15_0, gt.off_31_16);

  if ((cpu.cs&0x3) == DPL_USER) {

    // Task State Segment (TSS)
    /*
    typedef struct {
      uint32_t link;     // Unused
      uint32_t esp0;     // Stack pointers and segment selectors
      uint32_t ss0;      //   after an increase in privilege level
      uint32_t padding[23];
    } __attribute__((packed)) TSS32;
    */

    rtlreg_t old_ss = cpu.ss, old_esp = cpu.esp;
    // find tss
    SegDesc tss_desc = get_descriptor(cpu.tr>>3);
    vaddr_t tss_addr = tss_desc.base_15_0 | (tss_desc.base_23_16 << 16) | (tss_desc.base_31_24 << 24);
    Log("get tss addr at %x", tss_addr);
    uint32_t esp0 = vaddr_read(tss_addr + 4, 4);
    uint32_t ss0 = vaddr_read(tss_addr + 8, 4);
    Log("esp0:0x%x ss0:0x%x", esp0, ss0);
    // load ss and esp from tss
    cpu.esp = esp0;
    cpu.ss = ss0;
    // push old ss and sp
    rtl_push(s, &old_ss);
    rtl_push(s, &old_esp);
  }
  rtl_push(s, &cpu.eflags);
  rtl_push(s, &cpu.cs);
  rtl_push(s, &ret_addr);

  cpu.cs &= ~0x3; // set ring0

  cpu.eflags &= ~FL_IF;

  s->is_jmp = true;
  s->jmp_pc = jmp_pc;
  //Log("int jmp pc %x",jmp_pc);
}

void query_intr(DecodeExecState *s) {
  if (cpu.INTR && (cpu.eflags&FL_IF) && cpu.idtr.limit != 0) {
    cpu.INTR = false;
    raise_intr(s, IRQ_TIMER, cpu.pc);
    update_pc(s);
  }
}
