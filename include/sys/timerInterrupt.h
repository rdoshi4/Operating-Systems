#ifndef TIMER_H
#define TIMER_H
#include <sys/initializeIDT.h>
#include <sys/defs.h>

//void timerFunction(reg r);
void timer_initialize(uint32_t frequency);

#endif
