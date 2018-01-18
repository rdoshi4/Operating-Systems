#include <sys/initializeIDT.h>
#include <sys/timerInterrupt.h>
#include <sys/keyboardHandler.h>
#include <sys/process.h>
#include <sys/virmem.h>
#include <sys/tarfs.h>
#include <sys/kprintf.h>
struct idtContent idtContents[256];
struct idtPointer idtPointers;
uint64_t stack_pointer;
extern void setLidt();
//extern void mmap2();
void idtGateSet(uint8_t number, uint64_t initial, uint16_t segment, uint8_t flag)
{
  idtContents[number].initialLow = initial & 0xFFFF;
  idtContents[number].segment = segment;
  idtContents[number].repeat = 0;
  idtContents[number].flag = flag;
  idtContents[number].initialMid = (initial >> 16) & 0xFFFF;
  idtContents[number].initialHigh = (initial >> 32) & 0xFFFFFFFF;
  idtContents[number].reserve=0;
}
void remapPIC()
{
  outb(0x20,0x20);
  io_wait();
  outb(0x20,0x11);
  io_wait();
  outb(0xA0,0x11);
  io_wait();
  outb(0x21,0x20);
  io_wait();
  outb(0xA1,0x28);
  io_wait();
  outb(0x21,0x04);
  io_wait();
  outb(0xA1,0x02);
  io_wait();
  outb(0x21,0x01);
  io_wait();
  outb(0xA1,0x01);
  io_wait();
}
void handleRegInterrupt(int irqNumber, isr control)
{
  iControl[irqNumber] = control;
}
void controlIRQ(reg r)
{
  outb(0x20, 0x20);
  isr control = iControl[32];
  control(r);
}
void controlIRQ2(reg r)
{
  outb(0x20, 0x20);
  isr control = iControl[33];
  control(r);
}
void sysCall_handle(reg r)
{
  outb(0x20, 0x20);
  isr control = iControl[128];
  control(r);
}
void divide_by_zero_handler(reg r)
{
  outb(0x20, 0x20);
  isr control = iControl[0];
  control(r);
}
void page_fault_handler(reg r)
{
  outb(0x20, 0x20);
  isr control = iControl[14];
  control(r);
}
static uint64_t divide_by_zero(reg r)
{
	kprintf("divide by 0 \n");
	//__asm volatile("hlt;");
	exit(1);
	return 1;
}
void initializeDivideByZero()
{
    handleRegInterrupt(0, divide_by_zero);
}
void idtInitializer()
{
  idtPointers.limit = 256 * sizeof(struct idtContent) - 1;
  idtPointers.initial = (uint64_t)&idtContents;
  setLidt();
}
static uint64_t syscallSelection(reg r)
{
  uint64_t val=-1;
  if(r.rcx == 9)    //syscall 9 - mmap
  {
    val = mmap(r.rbx);
  }
  if(r.rcx == 2)     //syscall 2 - open
  {
    val = open(r.rbx);
  }
  if(r.rcx == 3)     //syscall 3 - read
  {
    __asm volatile("sti");
    val = read(r.rdx,r.rbx,r.r8);
  }
  if(r.rcx == 4)    //syscall 4 - write
  {
     val = write(r.rbx, r.rdx);
  }
  if(r.rcx ==57)
  {
      // __asm volatile("movq %%rsp, %0":: "b"(stack_pointer));
     val = fork();
  }
  if(r.rcx==59)
  {
	val=execve(r.rbx,r.rdx);
  }
  if(r.rcx == 79)
  {
     val = getcwd(r.rbx);
  }
  if(r.rcx == 78)
  {
     val= readdir(r.rbx);	
  }
  if(r.rcx == 80)
  {
    val = chdir(r.rbx);
  }
  if(r.rcx == 247)
  {
	val= wait();
  }
  if(r.rcx==65)
  {
    val = cat(r.rbx);
  }
  if(r.rcx==69)
  {
    clearConsole();
    val = 1;
  }
  if(r.rcx==60)
  {
	val=exit(r.rbx);
  }
  if(r.rcx==35)
  {
	val=sleep(r.rbx);
  }
  if(r.rcx==270)
  {
	val=ps();
  }
  if(r.rcx==62)
  {
        val=kill(r.rbx);
  }
  if(r.rcx==24)
  {
	val=yield();
  }
  return val;
}

void initializeSysCall()
{
  handleRegInterrupt(128, syscallSelection);
}
/*uint64_t selSysCall(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
  if(num == 90)
  {
      return 1;
//    return mmap(arg1, arg2);
  }
  else
  {
    return -1;
  }
}
void handleSysCall(reg r)
{
  uint64_t val;
//  uint64_t sysCallNum = r->rax;
  if(r.rax == 90)
    val = selSysCall(r.rax, r.rdi, r.rsi, r.rdx);
  if(r.rax == 1)
    val = open(r.rax, r.rdi, r.rsi, r.rdx);
  if(r.rax == 2)
    val = write(r.rax, r.rdi, r.rsi, r.rdx);
  r.rax = val;
}
*/
/*
static void selectInterrupt(reg r)
{
  if(r.num == 14)
  {
    initializePageFaultHandler(r);
  }
  if(r.num == 32)
  {
    timerFunction(r);
  }
  if(r.num == 33)
  {
    keyboardInitialize();
  }
  if(r.num == 128)
  {
    handleSysCall(r);
  }
  if(r.num >39)
  {
    outb(0xA0, 0x20);
  }
  outb(0x20, 0x20);
}
*/
void intHandler(reg r)
{
//  selectInterrupt(r);
}
void initializeIDT()
{
  idtInitializer();
  remapPIC();

//  idtGateSet(14, (uint64_t)isr14, 0x08, 0xe);
//  idtGateSet(32, (uint64_t)isr32, 0x08, 0xe);
//  idtGateSet(33, (uint64_t)isr33, 0x08, 0xe);
//  idtGateSet(128, (uint64_t)isr128, 0x08, 0xe);
//  remapPIC();
//  setLidt();

  idtGateSet(32, (uint64_t)&irq0, 0x08, 0x8E);
  idtGateSet(33, (uint64_t)&irq1, 0x08, 0x8E);
  idtGateSet(14, (uint64_t)&page_fault, 0x08, 0x8E);
  idtGateSet(128, (uint64_t)&mmap2, 0x08, 0xEE);
  idtGateSet(0,(uint64_t)&divide_zero,0x08,0xEE);
  __asm__ ("sti");
  timer_initialize(1693);
  io_wait();
  keyboardInitialize();
  io_wait();
  initializeSysCall();
  io_wait();
  initializePageFaultHandler();
  io_wait();
  initializeDivideByZero();
  io_wait();
}
