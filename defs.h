#ifndef _DEFS_H_
#define _DEFS_H_

// from kalloc.c
char *kalloc(int n);
void kfree(char *cp, int len);
void kinit(void);

// console.c
void cprintf(char *fmt, ...);
void panic(char *s);

// proc.c
struct proc;
void setupsegs(struct proc *p);
struct proc * newproc(struct proc *op);
void swtch(struct proc *op);

// string.c
void *memcpy(void *dst, void *src, unsigned n);
void *memset(void *dst, int c, unsigned n);

#endif // ! _DEFS_H_
