#include <sys/port.h>
#include <sys/timerInterrupt.h>
#include <sys/defs.h>
#include <sys/kprintf.h>

uint64_t ticks = 0;
//void timerFunction(reg r)
static uint64_t  timerFunction(reg r)
{
 // printTime(ticks);
  ticks++; 
  return 0;
}

void timer_initialize(uint32_t f)
{
  uint16_t value = (uint16_t)1193180 / f;
  uint8_t lower = (uint8_t)(value & 0xFF);
  uint8_t higher = (uint8_t)((value>>8));
  handleRegInterrupt(32, timerFunction);
  outb(0x43, 0x36);
  outb(0x40, lower);
  outb(0x40, higher);
}
