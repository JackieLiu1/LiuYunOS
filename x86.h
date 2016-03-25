#ifndef _X86_H_
#define _X86_H_

#define STATIC_INLINE static __inline
#define __ATTR_INLINE  __attribute__((always_inline))

STATIC_INLINE void
breakpoint(void)
{
    __asm __volatile("int3");
}

STATIC_INLINE uint8_t
inb(int port)
{
    uint8_t data;
    __asm __volatile("inb %w1,%0" : "=a" (data) : "d" (port));
    return data;
}

STATIC_INLINE void
insb(int port, void *addr, int cnt)
{
    __asm __volatile("cld\n\trepne\n\tinsb" :
                    "=D" (addr), "=c" (cnt) :
                    "d" (port), "0" (addr), "1" (cnt) :
                    "memory", "cc");
}

STATIC_INLINE uint16_t
inw(int port)
{
    uint16_t data;
    __asm __volatile("inw %w1,%0" : "=a" (data) : "d" (port));
    return data;
}

STATIC_INLINE void
insw(int port, void *addr, int cnt)
{
    __asm __volatile("cld\n\trepne\n\tinsw" :
                    "=D" (addr), "=c" (cnt) :
                    "d" (port), "0" (addr), "1" (cnt) :
                    "memory", "cc");
}

STATIC_INLINE uint32_t
inl(int port)
{
    uint32_t data;
    __asm __volatile("inl %w1,%0" : "=a" (data) : "d" (port));
    return data;
}

STATIC_INLINE void
insl(int port, void *addr, int cnt)
{
    __asm __volatile("cld\n\trepne\n\tinsl" :
                    "=D" (addr), "=c" (cnt) :
                    "d" (port), "0" (addr), "1" (cnt) :
                    "memory", "cc");
}

STATIC_INLINE void
outb(int port, uint8_t data)
{
    __asm __volatile("outb %0,%w1" :: "a" (data), "d" (port));
}

STATIC_INLINE void
outsb(int port, const void *addr, int cnt)
{
    __asm __volatile("cld\n\trepne\n\toutsb" :
                    "=S" (addr), "=c" (cnt) :
                    "d" (port), "0" (addr), "1" (cnt) :
                    "cc");
}

STATIC_INLINE void
outw(int port, uint16_t data)
{
    __asm __volatile("outw %0,%w1" :: "a" (data), "d" (port));
}

STATIC_INLINE void
outsw(int port, const void *addr, int cnt)
{
    __asm __volatile("cld\n\trepne\n\toutsw" :
                    "=S" (addr), "=c" (cnt) :
                    "d" (port), "0" (addr), "1" (cnt) :
                    "cc");
}

STATIC_INLINE void
outl(int port, uint32_t data)
{
    __asm __volatile("outl %0,%w1" :: "a" (data), "d" (port));
}

STATIC_INLINE void
outsl(int port, const void *addr, int cnt)
{
    __asm __volatile("cld\n\trepne\n\toutsl" :
                    "=S" (addr), "=c" (cnt) :
                    "d" (port), "0" (addr), "1" (cnt) :
                    "cc");
}

STATIC_INLINE void
invlpg(void *addr)
{
    __asm __volatile("invlpg (%0)" :: "r" (addr) : "memory");
}

STATIC_INLINE void
lidt(void *p)
{
    __asm __volatile("lidt (%0)" :: "r" (p));
}

STATIC_INLINE void
lldt(uint16_t sel)
{
    __asm __volatile("lldt %0" :: "r" (sel));
}

STATIC_INLINE void
ltr(uint16_t sel)
{
    __asm __volatile("ltr %0" :: "r" (sel));
}

STATIC_INLINE void
lcr0(uint32_t val)
{
    __asm __volatile("movl %0, %%cr0" :: "r" (val));
}

STATIC_INLINE uint32_t
rcr0(void)
{
    uint32_t val;
    __asm __volatile("movl %%cr0,%0" : "=r" (val));
    return val;
}

STATIC_INLINE uint32_t
rcr2(void)
{
    uint32_t val;
    __asm __volatile("movl %%cr2,%0" : "=r" (val));
    return val;
}

STATIC_INLINE void
lcr3(uint32_t val)
{
    __asm __volatile("movl %0, %%cr3" :: "r" (val));
}

STATIC_INLINE uint32_t
rcr3(void)
{
    uint32_t val;
    __asm __volatile("movl %%cr3,%0" : "=r" (val));
    return val;
}

STATIC_INLINE void
lcr4(uint32_t val)
{
    __asm __volatile("movl %0, %%cr4" :: "r" (val));
}

STATIC_INLINE uint32_t
rcr4(void)
{
    uint32_t val;
    __asm __volatile("movl %%cr4, %0" : "=r" (val));
    return val;
}

STATIC_INLINE void
tlbflush(void)
{
    uint32_t cr3;
    __asm __volatile("movl %%cr3, %0" : "=r" (cr3));
    __asm __volatile("movl %0, %%cr3" :: "r" (cr3));
}

STATIC_INLINE uint32_t
read_eflags(void)
{
    uint32_t eflags;
    __asm __volatile("pushfl; popl %0" : "=r" (eflags));
    return eflags;
}

STATIC_INLINE void
write_eflags(uint32_t eflags)
{
    __asm __volatile("pushl %0, popfl" :: "r" (eflags));
}

STATIC_INLINE uint32_t
read_ebp(void)
{
    uint32_t ebp;
    __asm __volatile("movl %%ebp, %0" : "=r" (ebp));
    return ebp;
}

STATIC_INLINE uint32_t
read_esp(void)
{
    uint32_t esp;
    __asm __volatile("movl %%esp, %0" : "=r" (esp));
    return esp;
}

STATIC_INLINE void
cpuid(uint32_t info, uint32_t *eaxp, uint32_t *ebxp, uint32_t *ecxp, uint32_t *edxp)
{
    uint32_t eax, ebx, ecx, edx;
    asm volatile("cpuid" :
                "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) :
                "a" (info));
    if (eaxp)
        *eaxp = eax;
    if (ebxp)
        *ebxp = ebx;
    if (ecxp)
        *ecxp = ecx;
    if (edxp)
        *edxp = edx;
}

STATIC_INLINE uint64_t
read_tsc(void)
{
    uint64_t tsc;
    __asm __volatile("rdtsc" : "=A" (tsc));
    return tsc;
}

struct PushRegs {
    uint32_t reg_edi;
    uint32_t reg_esi;
    uint32_t reg_ebp;
    uint32_t reg_oesp;  /* Useless */
    uint32_t reg_ebx;
    uint32_t reg_edx;
    uint32_t reg_ecx;
    uint32_t reg_rax;
};

struct Trapframe {
    struct PushRegs tf_regs;
    uint16_t tf_es;
    uint16_t tf_padding1;
    uint16_t tf_ds;
    uint16_t tf_padding2;
    uint32_t tf_trapno;
    /* 以下由 x86 硬件定义 */
    uint32_t tf_err;
    uintptr_t tf_eip;
    uint16_t tf_cs;
    uint16_t tf_padding3;
    uint32_t tf_eflags;
    /* 以下只是在模式切换时使用，比如从用户态切换到内核态 */
    uintptr_t tf_esp;
    uint16_t tf_ss;
    uint16_t tf_padding4;
};

#endif  // ! _X86_H_
