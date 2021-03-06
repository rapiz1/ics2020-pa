#include <am.h>
#include <x86.h>
#include <klib.h>

#define NR_IRQ         256     // IDT size
#define SEG_KCODE      1
#define SEG_KDATA      2
#define NR_SEG 6

static SegDesc gdt[NR_SEG] = {};
static TSS32 tss = {};

static Context* (*user_handler)(Event, Context*) = NULL;

void __am_irq0();
void __am_vecsys();
void __am_vectrap();
void __am_vecnull();


inline static void display_context(Context *c) {
  printf("irq context at 0x%x:\n", c);
  printf("\tcr3: 0x%x at 0x%x\n", c->cr3, &(c->cr3));
  printf("\tedi: 0x%x\t esi:0x%x \t at 0x%x\n", c->edi, c->esi, &(c->edi));
  printf("\tebp: 0x%x\t esp:0x%x\n", c->ebp, c->esp);
  printf("\tebx: 0x%x\t edx:0x%x\n", c->ebx, c->edx);
  printf("\tecx: 0x%x\t eax:0x%x\n", c->ecx, c->eax);
  printf("\tirq: 0x%x\n", c->irq);
  printf("\teip: 0x%x\t cs: 0x%x\n", c->irq, c->cs);
  printf("\teflags: 0x%x\n", c->eflags);
}

Context* __am_irq_handle(Context *c) {
  //display_context(c);
  //printf("irq context at %x\n", c);
  extern void __am_get_cur_as(Context *c);
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->irq) {
      case 32: ev.event = EVENT_IRQ_TIMER; break;
      case 0x80: ev.event = EVENT_SYSCALL; break;
      case 0x81: ev.event = EVENT_YIELD; break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  //printf("handler return context 0x%x\n", c);
  extern void __am_switch(Context *c);
  __am_switch(c);

  return c;
}

bool cte_init(Context*(*handler)(Event, Context*)) {
  static GateDesc32 idt[NR_IRQ];

  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i]  = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecnull, DPL_KERN);
  }

  // ----------------------- interrupts ----------------------------
  idt[32]   = GATE32(STS_IG, KSEL(SEG_KCODE), __am_irq0,    DPL_KERN);
  // ---------------------- system call ----------------------------
  idt[0x80] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecsys,  DPL_USER);
  idt[0x81] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vectrap, DPL_KERN);

  printf("try to set idt at %d\n", idt);
  set_idt(idt, sizeof(idt));

 // initialize GDT
  gdt[1] = SEG32(STA_X | STA_R,   0,      0xffffffff, DPL_KERN);
  gdt[2] = SEG32(STA_W,           0,      0xffffffff, DPL_KERN);
  gdt[3] = SEG32(STA_X | STA_R,   0,      0xffffffff, DPL_USER);
  gdt[4] = SEG32(STA_W,           0,      0xffffffff, DPL_USER);
  gdt[5] = SEG16(STS_T32A,     &tss, sizeof(tss) - 1, DPL_KERN);
  set_gdt(gdt, sizeof(gdt[0]) * NR_SEG);

  // initialize TSS
  printf("tss at 0x%x\n", &tss);
  tss.ss0 = KSEL(2);
  set_tr(KSEL(5));

  // register event handler
  user_handler = handler;

  return true;
}


Context* kcontext(Area kstack, void (*entry)(void *), void *arg) {
  void *st = kstack.end - sizeof(uint32_t);
  *(uint32_t*)st = (uint32_t)arg;
  //printf("arg at %d\n", st);
  Context *cp = st - sizeof(Context) - 4;
  //printf("context at %d\n", cp);
  cp->eip = (uint32_t)entry;
  cp->esp = (uint32_t)&cp->irq;
  cp->cs = KSEL(1);
  cp->cr3 = 0;
  cp->eflags = 0x2;
  return cp;
}

void yield() {
  asm volatile("int $0x81");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
  if (enable) sti();
  else cli();
}
