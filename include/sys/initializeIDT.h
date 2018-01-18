#ifndef INITIALIZEIDT_H
#define INITIALIZEIDT_H
#include <sys/defs.h>
#include <sys/port.h>

typedef struct
{

  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rbp;
  uint64_t rdi, rsi, rdx, rcx, rbx, rax;
  uint64_t rip, cs, eflags, rsp, ss;
} __attribute((packed)) reg;
/*
typedef struct reg
{
  uint64_t r9;
  uint64_t r8;
  uint64_t rsi;
  uint64_t rbp;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rbx;
  uint64_t rax;
  uint64_t rdi;
  uint64_t num;
  uint64_t ec, rip, cs, rflags, rsp, ss;
}__attribute__((packed)) reg;
*/
typedef struct idtContent
{
  uint16_t initialLow;
  uint16_t segment;
  uint8_t repeat;
  uint8_t flag;
  uint16_t initialMid;
  uint32_t initialHigh;
  uint32_t reserve;
} __attribute__((packed)) idtContent;

typedef struct idtPointer
{
  uint16_t limit;
  uint64_t initial;
} __attribute__((packed)) idtPointer;

extern void isr0();
extern void irq0();
extern void irq1();
extern void page_fault();
extern void mmap2();
extern void divide_zero();

//extern void isr14();
//extern void isr32();
//extern void isr33();
//extern void isr128();
void intHandler(reg r);

typedef uint64_t (*isr)(reg);
isr iControl[256];
void initializeIDT();
void idtGateSet(uint8_t number, uint64_t initial, uint16_t segment, uint8_t flag);
void remapPIC();
void handleRegInterrupt(int irqNumber, isr control);
void controlIRQ(reg r);
void controlIRQ2(reg r);
void sysCall_handle(reg r);
void divide_by_zero_handler(reg r);
void page_fault_handler(reg r);
void idtInitializer();
//uint64_t selSysCall(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3);
//void handleSysCall(reg r);
void initializeSysCall();

#endif
