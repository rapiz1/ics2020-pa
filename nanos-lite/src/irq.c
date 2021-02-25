#include <common.h>
#include "syscall.h"

static Context* do_event(Event e, Context* c) {
  extern void do_syscall(Context *c);
  extern Context *schedule(Context *prev);
  extern bool nanos_inited;
  switch (e.event) {
    case EVENT_SYSCALL: /*Log("Received syscall event %d", c->GPR1);*/ do_syscall(c); break;
    case EVENT_IRQ_TIMER:
      Log("Received timer event");
    case EVENT_YIELD:
      if (!nanos_inited) return c;
      //Log("Received yield event");
      Log("Scheduling");
      c = schedule(c);
      break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
