#ifndef _MMU_H_
#define _MMU_H_

/*
 *  Part 1. Paging data structures and constants.
 */

// A linear address 'la' has a three-part structure as follows:
//
// +--------10--------+------10------+--------12------------+
// |  Page Directory  |  Page Table  |  Offset within Page  |
// |     Index        |      Index   |                      |
// +------------------+--------------+----------------------+
//  \--- PDX(la) ----/ \-- PTX(la)--/ \----- PGOFF(la) ----/
//  \-------------- PPN(la) --------/
//
// The PDX, PTX, PGOFF, and PPN macros decompose linear address as shown.
// To construct a linear address la from PDX(la), PTX(la), and PGOFF(la),
// use PGADDR(PDX(la), PTX(la), PGOFF(la)).

// page number field of address
#define PPN(la)     (((uintptr_t) (la)) >> PTXSHIFT)
#define VPN(la)     PPN(la)    // used to index into vpt[]

// page directory index
#define PDX(la)     ((((uintptr_t) (la)) >> PDXSHIFT) & 0x3FF)
#define VPD(la)     PDX(la)     // used to index into vpd[]

// page table index
#define PTX(la)     ((((uintptr_t) (la)) >> PTXSHIFT) & 0x3FF)

// offset in page
#define PGOFF(la)   (((uintptr_t) (la)) & 0xFFF)

// construct linear address from indexes and offset
#define PGADDR(d, t, o) ((void *) ((d) << PDXSHIFT | (t) << PTXSHIFT | (o)))

// Page directory and page table constants
#define NPDENTRIES  1024    // page directory entries per page directory
#define NPTENTRIES  1024    // page table entries per page table

#define PGSIZE      4096    // bytes mapped by a page
#define PGSHIFT     12      // log2(PGSIZE)

#define PTSIZE      (PGSIZE * NPTENTRIES)   // bytes mapped by a page directory entry
#define PTSHOFT     22      // log2(PTSIZE)

#define PTXSHIFT    12      // offset of PTX in linear address
#define PDXSHIFT    22      // offset of PDX in linear address

// Page table/directory entry flags
#define PTE_P       0x001   // Present
#define PTE_W       0x002   // Writeable
#define PTE_U       0x004   // User
#define PTE_PWT     0x008   // Write-Through
#define PTE_PCD     0x010   // Cache-Disable
#define PTE_A       0x020   // Accessed
#define PTE_D       0x040   // Dirty
#define PTE_PS      0x080   // Page size
#define PTE_MBZ     0x180   // Bits must be zero

// The PTE_AVAIL bits aren't used by the kernel or interpreted by the
// hardware, so user processes are allowed to set them arbitrarily
#define PTE_AVAIL   0xE00   // Available for software use

// Only flags in PTE_USER may be used in system calls
#define PTE_USER    (PTE_AVAIL | PTE_P | PTE_W | PTE_U)

// address in page table entry
#define PTE_ADDRESS ((physaddr_t) (pte) & ~0xFFF)

// Control Register flags
#define CR0_PE  0x00000001  // Protection Enable
#define CR0_MP  0x00000002  // Monitor coProcessor
#define CR0_EM  0x00000004  // Emulation
#define CR0_TS  0x00000008  // Task Switched
#define CR0_ET  0x00000010  // Extension type
#define CR0_NE  0x00000020  // Numberic Error
#define CR0_WP  0x00001000  // Write Protect
#define CR0_AM  0x00004000  // Alignment Mask
#define CR0_NW  0x20000000  // Not Writethrough
#define CR0_CD  0x40000000  // Cache Disable
#define CR0_PG  0x80000000  // Paging

#define CR4_PCE 0x00000100  // Performance counter enable
#define CR4_MCE 0x00000040  // Machine Check enable
#define CR4_PSE 0x00000010  // Page Size Extensions
#define CR4_DE  0x00000008  // Debugging Extensions
#define CR4_TSD 0x00000004  // Time Stamp Disable
#define CR4_PVI 0x00000002  // Protected-Mode Virtual Interrupts
#define CR4_VME 0x00000001  // V86 Mode Extensions

// Eflags register
#define FL_CF   0x00000001  // Carry Flag
#define FL_PF   0x00000004  // Parity Flag
#define FL_AF   0x00000010  // Auxiliary carry Flag
#define FL_ZF   0x00000040  // Zero Flag
#define FL_SF   0x00000080  // Sign Flag
#define FL_TF   0x00000100  // Trap Flag
#define FL_IF   0x00000200  // Interrupt Flag
#define FL_DF   0x00000400  // Direction Flag
#define FL_OF   0x00000800  // Overflow Flag
#define FL_IOPL_MASK    0x00003000  // I/O Privilege Level bitmask
#define FL_IOPL_0       0x00000000  // IOPL == 0
#define FL_IOPL_1       0x00001000  // IOPL == 1
#define FL_IOPL_2       0x00002000  // IOPL == 2
#define FL_IOPL_3       0x00003000  // IOPL == 3
#define FL_NT   0x00004000  // Nested Task
#define FL_RF   0x00010000  // Resume Flag
#define FL_VM   0x00020000  // Virtual 8086 Mode
#define FL_AC   0x00040000  // Alignment Check
#define FL_VIF  0x00080000  // Virtual Interrupt Flag
#define FL_VIP  0x00100000  // Virtual Interrupt Pending
#define FL_ID   0x00200000  // ID flag

// Page fault error codes
#define FEC_PR  0x1     // Page Fault caused by protection violation
#define FEC_WR  0x2     // Page Fault caused by a write
#define FEC_U   0x4     // Page fault occured while in user Mode

/*
 *  Part 2. Segmentation data structures and constants
 */
#ifdef __ASSEMBLER__

/*
 *  Macros to build GDT entries in assembly
 */
#define SEG_NULL    \
    .word 0, 0;     \
    .byte 0, 0, 0, 0
#define SEG(type, base, lim)    \
    .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);  \
    .byte (((base) >> 16) & 0xff), (0x90 | (type)),     \
        (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

#else // not __ASSEMBLER__

// Segment Descriptiors
struct Segdesc {
    unsigned sd_lim_15_0 : 16;  // Low bits of segment limit
    unsigned sd_base_15_0 : 16; // Low bits of segment base address
    unsigned sd_base_23_0 : 8;  // Middle bits of segment base address
    unsigned sd_type : 4;       // Segment type (see STS_ Constants)
    unsigned sd_s : 1;          // 0 = system, 1 = application
    unsigned sd_dpl : 2;        // Descriptor Privilege Level
    unsigned sd_p : 1;          // Present
    unsigned sd_lim_19_16 : 4;  // High bits of segment limit
    unsigned sd_avl : 1;        // Unused (available for software use)
    unsigned sd_rsv1 : 1;       // Reserved
    unsigned sd_db : 1;         // 0 = 16-bit segment, 1 = 32-bit segment
    unsigned sd_g : 1;          // Granularity: limit scaled by 4K when set
    unsigned sd_base_31_24 : 8; // High bits of segment base address
};

// Null segment
#define SEG_NULL    (struct Segdesc) {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
// Segment that is loadable but faults when used
#define SEG_FAULT   (struct Segdesc) {0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0 };
// Normal segment
#define SEG(type, base, lim, dpl) (struct Segdesc)  \
    {  ((lim) >> 12) & 0xffff, (base) & 0xffff, ((base) >> 16) & 0xff,  \
        type, 1, dpl, 1, (unsigned) (lim) >> 28, 0, 0, 1, 1, \
        (unsigned) (base) >> 24 \
    }
#define SEG16(type, base, lim, dpl) (struct Segdesc)    \
    {   \
        (lim) & 0xffff, (base) & 0xffff, ((base) >> 16) & 0xff, \
        type, 1, dpl, 1, (unsigned) (lim) >> 16, 0, 0, 1, 0, \
        (unsigned) (base) >> 24 \
    }
#endif // ! __ASSEMBLER__

// application segment type bits
#define STA_X       0x8     // Executable segment
#define STA_E       0x4     // Expand down (non-exectuable segment)
#define STA_C       0x4     // Conforming code segment (executable only)
#define STA_W       0x2     // Writeable (non-exectuable segments)
#define STA_R       0x2     // Readable (executable segments)
#define STA_A       0x1     // Accessed

// System segment type bits
#define STS_T16A    0x1     // Available 16-bit TSS
#define STS_LDT     0x2     // Local Descriptor Table
#define STS_T16B    0x3     // Busy 16-bits TSS
#define STS_CG16    0x4     // 16-bit Call Gate
#define STS_TG      0x5     // Task Gate / Coum Transmitions
#define STS_IG16    0x6     // 16-bits Interrupt Gate
#define STS_TG16    0x7     // 16-bits Trap Gate
#define STS_T32A    0x9     // Available 32-bits TSS
#define STS_T32B    0xB     // Busy 32-bit TSS
#define STS_CG32    0xC     // 32-bit Call Gate
#define STS_IG32    0xE     // 32-bit Interrupt Gate
#define STS_TG32    0xF     // 32-bit Trap Gate

/*
 *  Part 3. Traps.
 */
#ifndef __ASSEMBLER__

// Task state segment format (as described by the Pentium architecture book)
struct Taskstate {
    uint32_t    ts_link;    // Old ts sector
    uintptr_t   ts_esp0;    // Stack pointers and segment selectors
    uint16_t    ts_ss0;     // after an increase in Privilege level
    uint16_t    ts_padding1;
    uintptr_t   ts_esp1;
    uint16_t    ts_ss1;
    uint16_t    ts_padding2;
    uintptr_t   ts_esp2;
    uint16_t    ts_ss2;
    uint16_t    ts_padding3;
    physaddr_t  ts_cr3;     // Page directory base
    uintptr_t   ts_eip;     // Saved state from last task Switch
    uint32_t    ts_eflags;
    uint32_t    ts_eax;     // More saved state (registers)
    uint32_t    ts_ecx;
    uint32_t    ts_edx;
    uint32_t    ts_ebx;
    uintptr_t   ts_esp;
    uintptr_t   ts_ebp;
    uint32_t    ts_esi;
    uint32_t    ts_edi;
    uint16_t    ts_es;  // Even more saved state (segment selectors)
    uint16_t    ts_padding4;
    uint16_t    ts_cs;
    uint16_t    ts_padding5;
    uint16_t    ts_ss;
    uint16_t    ts_padding6;
    uint16_t    ts_ds;
    uint16_t    ts_padding7;
    uint16_t    ts_fs;
    uint16_t    ts_padding8;
    uint16_t    ts_gs;
    uint16_t    ts_padding9;
    uint16_t    ts_lgt;
    uint16_t    ts_padding10;
    uint16_t    ts_t;       // Trap on task switch
    uint16_t    ts_iomb;    // I/O map base address
};

// Gate descriptors for interrupts and traps
struct Gatedesc {
    unsigned gd_off_15_0 : 16;  // low 16-bits of offset in segment
    unsigned gd_ss : 16;        // segment selector
    unsigned gd_args : 5;       // # args, 0 for interrupt/trap gates
    unsigned gd_rsv1 : 3;       // reserved (should be zero I guess)
    unsigned gd_type : 4;       // type (STS_{TG,IG32,TG32})
    unsigned gd_s : 1;          // must be 0 (system)
    unsigned gd_dpl : 2;        // descriptor(meaning new) privilege level
    unsigned gd_p : 1;          // Present
    unsigned gd_off_31_16 : 16; // high bits of offset in segment
};

// Set up normal interrupt/trap gate descriptor.
// - istrap: 1 for a trap (= exception) gate, 0 for an interrupt gate
// - sel: Code segment selector for interrupt/trap handler
// - off: Offset in code segment for interrupt/trap handler
// - dpl: Descriptor privilege level -
//     the privilege level required for software to invoke
//     this interrupt/trap gate explicitly using an int instruction.
#define SETGATE(gate, istrap, sel, off, dpl)    \
{   \
    (gate).gd_off_15_0 = (uint32_t) (off) & 0xffff; \
    (gate).gd_ss = (sel);   \
    (gate).gd_args = 0; \
    (gate).gd_rsv1 = 0; \
    (gate).gd_type = (istrap) ? STS_TG32 : STS_IG32; \
    (gate).gd_s = 0;    \
    (gate).gd_dpl = (dpl);  \
    (gate).gd_p = 1;    \
    (gate).gd_off_31_16 = (uint32_t) (off) >> 16;   \
}

// Set up a call gate descriptor.
#define SETCALLGATE(gate, ss, off, dpl) \
{   \
    (gate).gd_off_15_0 = (uint32_t) (off) & 0xffff; \
    (gate).gd_ss = (ss);    \
    (gate).gd_args = 0; \
    (gate).gd_rsv1 = 0; \
    (gate).gd_type = STS_CG32;  \
    (gate).gd_s = 0;    \
    (gate).gd_dpl = (dpl);  \
    (gate).gd_p = 1;    \
    (gate).gd_off_31_16 = (uint32_t) (off) >> 16;   \
}

// Pseudo-descriptors used for LGDT, LLDT and LIDT instrucations.
struct Pseudodesc {
    uint16_t pd__garbage;   // LGDT supposed to be from address 4N+2
    uint16_t pd_lim;        // limit
    uint32_t pd_base __attribute__ ((packed));  // Base Address
};
#define PD_ADDR(desc)   (&(desc).pd_lim)

#endif // ! __ASSEMBLER__
#endif // ! _MMU_H_
