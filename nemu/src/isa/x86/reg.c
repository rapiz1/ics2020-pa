#include <isa.h>
#include <stdlib.h>
#include <time.h>
#include "local-include/reg.h"

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  word_t sample[8];
  word_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

#define PRINT_REG(name) do {printf("%s: 0x%08x\t", #name, cpu.name);} while(0)
void isa_reg_display() {
  PRINT_REG(eax); PRINT_REG(ebx); putchar('\n');
  PRINT_REG(ecx); PRINT_REG(edx); putchar('\n');
  PRINT_REG(esp); PRINT_REG(ebp); putchar('\n');
  PRINT_REG(esi); PRINT_REG(edi); putchar('\n');
  PRINT_REG(pc); putchar('\n');
}

#define VERDICT_AND_FETCH_REG(s, name) if(!strcmp(s, #name)) {return cpu.name;}
word_t isa_reg_str2val(const char *s, bool *success) {
  *success = true;
  VERDICT_AND_FETCH_REG(s, eax)
  else VERDICT_AND_FETCH_REG(s, ebx)
  else VERDICT_AND_FETCH_REG(s, ecx)
  else VERDICT_AND_FETCH_REG(s, edx)
  else VERDICT_AND_FETCH_REG(s, esp)
  else VERDICT_AND_FETCH_REG(s, ebp)
  else VERDICT_AND_FETCH_REG(s, esi)
  else VERDICT_AND_FETCH_REG(s, edi)
  else VERDICT_AND_FETCH_REG(s, pc)
  else {
    Log("unidentify register name %s", s);
    *success = false;
  }
  return 0;
}
