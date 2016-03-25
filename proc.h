#ifndef _PROC_H_
#define _PROC_H_

#define SEG_KCODE   1   // 内核代码
#define SEG_KDATA   2   // 内核数据段与堆栈段
#define SEG_UCODE   3
#define SEG_UDATA   4
#define SEG_TSS     5   // 当前进程的任务状态 Task state
#define NSEGS       6

struct proc {
    char        *mem;   // 进程的物理内存起始地址
    unsigned    sz;     // memory 的总大小，包含内核 stack 段
    char        *kstack;// 内核 stack 段，从内存中分离，不可以移动
    enum {
        UNUSED,
        RUNNABLE,
        WAITING
    } state;
    struct Taskstate    ts; // 任务状态结构体
    struct Segdesc      gdt[NSEGS];
    struct Pseudodesc   gdt_pd;
    unsigned esp;       // 内核 stack 段指针
    unsigned ebp;       // 内核 frame 指针
    struct Trapframe *tf;   // 用来查找用户指针
};

extern struct proc proc[];

#endif  // ! _PROC_H_
