static inline def_EHelper(mov) {
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(mov);
}

static inline def_EHelper(push) {
  //TODO();
  rtl_push(s, s->dest.preg);
  print_asm_template1(push);
}

static inline def_EHelper(pop) {
  //TODO();
  rtl_pop(s, s->dest.preg);
  print_asm_template1(pop);
}

static inline def_EHelper(pusha) {
  TODO();
  print_asm("pusha");
}

static inline def_EHelper(popa) {
  TODO();
  print_asm("popa");
}

static inline def_EHelper(leave) {
  //TODO();
  rtl_mv(s, &reg_l(R_ESP), &reg_l(R_EBP));
  rtl_pop(s, &reg_l(R_EBP));
  print_asm("leave");
}

static inline def_EHelper(cltd) {
  if (s->isa.is_operand_size_16) {
    //TODO();
    reg_w(R_DX) = SEXT(reg_w(R_AX), 16)>>16;
  }
  else {
    //TODO();
    reg_l(R_EDX) = SEXT(reg_l(R_EAX), 32)>>32;
  }
  print_asm(s->isa.is_operand_size_16 ? "cwtl" : "cltd");
}

static inline def_EHelper(cwtl) {
  if (s->isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }
  print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

static inline def_EHelper(movs) {
  if (id_dest->width == 1) {
    //TODO();
    rtlreg_t data;
    rtl_lm(s, &data, &reg_l(R_ESI), 0, 1);
    rtl_sm(s, &reg_l(R_EDI), 0, &data, 1);
    print_asm("movsb");
  } else if (id_dest->width == 2) {
    //TODO();
    rtlreg_t data;
    rtl_lm(s, &data, &reg_l(R_ESI), 0, 2);
    rtl_sm(s, &reg_l(R_EDI), 0, &data, 2);
    print_asm("movsw");
  } else {
    //TODO();
    rtlreg_t data;
    rtl_lm(s, &data, &reg_l(R_ESI), 0, 4);
    rtl_sm(s, &reg_l(R_EDI), 0, &data, 4);
    print_asm("movsd");
  }
  rtl_addi(s, &reg_l(R_ESI), &reg_l(R_ESI), s->width);
  rtl_addi(s, &reg_l(R_EDI), &reg_l(R_EDI), s->width);
}

static inline def_EHelper(movsx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, ddest);
  if (id_dest->width == 2) {
    print_asm_template2(movsb);
  } else {
    print_asm_template2(movsb);
  }
  assert(id_src1->width == 1);
}
static inline def_EHelper(movzx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_zext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, dsrc1);
  if (id_dest->width == 2) {
    print_asm_template2(movzb);
  } else {
    print_asm_template2(movzb);
  }
  assert(id_src1->width == 1);
}

static inline def_EHelper(lea) {
  rtl_addi(s, ddest, s->isa.mbase, s->isa.moff);
  operand_write(s, id_dest, ddest);
  print_asm_template2(lea);
}
