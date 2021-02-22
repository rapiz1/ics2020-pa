#include <am.h>
#include <x86.h>
#include <klib.h>

#define NR_IRQ         256     // IDT size
#define SEG_KCODE      1
#define SEG_KDATA      2

static Context* (*user_handler)(Event, Context*) = NULL;

void __am_irq0();
void __am_vecsys();
void __am_vectrap();
void __am_vecnull();


inline static void display_context(Context *c) {
  printf("irq context at %d:\n", c);
  printf("\tcr3: %d at %d\n", c->cr3, &(c->cr3));
  printf("\tedi: %d\t esi:%d \t at %d\n", c->edi, c->esi, &(c->edi));
  printf("\tebp: %d\t esp:%d\n", c->ebp, c->esp);
  printf("\tebx: %d\t edx:%d\n", c->ebx, c->edx);
  printf("\tecx: %d\t eax:%d\n", c->ecx, c->eax);
  printf("\tirq: %d\n", c->irq);
  printf("\teip: %d\t cs: %d\n", c->irq, c->cs);
  printf("\teflags: %d\n", c->eflags);
}

Context* __am_irq_handle(Context *c) {
  //display_context(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->irq) {
      case 0x80: ev.event = EVENT_SYSCALL; break;
      case 0x81: ev.event = EVENT_YIELD; break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

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

  // register event handler
  user_handler = handler;

  return true;
}


Context* kcontext(Area kstack, void (*entry)(void *), void *arg) {
  *(uint32_t*)kstack.end = (uint32_t)arg;
  Context *cp = ((Context*)kstack.end) - 1;
  cp->eip = (uint32_t)entry;
  cp->esp = (uint32_t)&cp->irq;
  cp->cs = 8;
  return cp;
}

void yield() {
  asm volatile("int $0x81");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
