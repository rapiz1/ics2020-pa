#include <monitor/difftest.h>

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

static inline def_EHelper(ltr) {
  cpu.tr = *id_dest->preg;
  print_asm_template1(ltr);
}

static inline def_EHelper(lidt) {
  rtlreg_t limit = 0, base = 0;
  vaddr_t addr = *s->isa.mbase + s->isa.moff;
  rtl_lm(s, &limit, &addr, 0, 2);
  rtl_lm(s, &base, &addr, 2, 4);
  cpu.idtr.limit = limit;
  cpu.idtr.base = base;
  Log("idtr base:limit=%x:%x at mem %x", base, limit, addr);
  print_asm_template1(lidt);
}

static inline def_EHelper(lgdt) {
  rtlreg_t limit = 0, base = 0;
  vaddr_t addr = *s->isa.mbase + s->isa.moff;
  rtl_lm(s, &limit, &addr, 0, 2);
  rtl_lm(s, &base, &addr, 2, 4);
  cpu.gdtr.limit = limit;
  cpu.gdtr.base = base;
  Log("gdtr base:limit=%x:%x at mem %x", base, limit, addr);
  print_asm_template1(lgdt);
}

static inline def_EHelper(mov_r2cr) {
  //TODO();
  if (id_dest->reg == 0)
    cpu.cr0 = *id_src1->preg;
  else if (id_dest->reg == 3)
    cpu.cr3 = *id_src1->preg;
  else TODO();
  print_asm("movl %%%s,%%cr%d", reg_name(id_src1->reg, 4), id_dest->reg);
}

static inline def_EHelper(mov_cr2r) {
  //TODO();
  if (id_src1->reg == 0)
    operand_write(s, id_dest, &cpu.cr0);
  else if (id_src1->reg == 3)
    operand_write(s, id_dest, &cpu.cr3);
  else TODO();
  print_asm("movl %%cr%d,%%%s", id_src1->reg, reg_name(id_dest->reg, 4));

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

static inline def_EHelper(int) {
  //TODO();
  raise_intr(s, s->dest.imm, s->seq_pc);
  print_asm("int %s", s->dest.str);

#ifndef __DIFF_REF_NEMU__
  difftest_skip_dut(1, 2);
#endif
}

static inline def_EHelper(iret) {
  //TODO();
  if (s->isa.is_operand_size_16) TODO();
  rtl_pop(s, &cpu.pc);
  rtl_pop(s, &cpu.cs);
  rtl_pop(s, &cpu.eflags);
  if ((cpu.cs&0x3) == DPL_USER) {
    rtlreg_t esp, ss;
    rtl_pop(s, &esp);
    rtl_pop(s, &ss);
    // save cpu.esp to tss.esp0
    vaddr_t tss_desc_addr = cpu.gdtr.base + sizeof(SegDesc)*(cpu.tr>>3);
    SegDesc tss_desc = {.val={ vaddr_read(tss_desc_addr, 4), vaddr_read(tss_desc_addr + 4, 4)}};
    /*
    typedef struct {
      uint32_t link;     // Unused
      uint32_t esp0;     // Stack pointers and segment selectors
      uint32_t ss0;      //   after an increase in privilege level
      uint32_t padding[23];
    } __attribute__((packed)) TSS32;
    */
    vaddr_t tss_addr = tss_desc.base_15_0 | (tss_desc.base_23_16 << 16) | (tss_desc.base_31_24 << 24);
    vaddr_write(tss_addr + 4, cpu.esp, 4);
    //Log("write ksp %x to tss", cpu.esp);
    cpu.esp = esp;
    cpu.ss = ss;
  }
  //Log("iret return to %x", cpu.pc);
  s->is_jmp = true;
  s->jmp_pc = cpu.pc;
  print_asm("iret custom:%x %x %x", cpu.pc, cpu.cs, cpu.eflags);

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

static inline def_EHelper(in) {
  //TODO();
  switch (id_dest->width) {
  case 1:
    *id_dest->preg = pio_read_b(*id_src1->preg);
    break;
  case 2:
    *id_dest->preg = pio_read_w(*id_src1->preg);
    break;
  case 4: 
    *id_dest->preg = pio_read_l(*id_src1->preg);
    break;
  default:
    TODO();
    break;
  };
  print_asm_template2(in);
}

static inline def_EHelper(out) {
  //TODO();
  uint32_t val = *id_src1->preg;
  switch (id_src1->width) {
  case 1:
    pio_write_b(*id_dest->preg, val);
    break;
  case 2:
    pio_write_w(*id_dest->preg, val);
    break;
  case 4: 
    pio_write_l(*id_dest->preg, val);
    break;
  default:
    TODO();
    break;
  };
  print_asm_template2(out);
}

static inline def_EHelper(cli) {
  cpu.eflags &= ~0x2;
}

static inline def_EHelper(sti) {
  cpu.eflags |= 0x2;
}
