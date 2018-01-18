#ifndef __KPRINTF_H
#define __KPRINTF_H

void kprintf(const char *fmt,...);
int shift_pressed;
int ctrl_pressed;
void printTime(int c);
void printKey(char c);
void printEnter();
void printEscape();
void keyRemove();
void clearConsole();
#endif
