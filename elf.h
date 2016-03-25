/* coding = utf8 */

/***************************************************************
 * Author          : JackieLiu
 * Email           : <liuyun01@kylinos.com.cn>
 *
 * Created Time    : 2016年03月24日 星期四 09时15分27秒
 * Last Modified   : 2016年03月24日 星期四 09时17分40秒
 *
 * File Name       : elf.h
 * License         : Copyright © Kylin
 *
 * Description     :
 *
 ***************************************************************/

#ifndef _ELF_H_
#define _ELF_H_

/* "\x7FELF" 魔数，小端模式 */
#define ELF_MAGIC   0x464c457FU

struct Elf {
    uint32_t    e_magic;    // 必须等于 ELF_MAGIC
    uint8_t     e_elf[12];
    uint16_t    e_type;     // 文件类型，1 代表可重定位，2 代表可执行
    uint16_t    e_machine;  // 目标体系架构
    uint32_t    e_version;  // 目标文件版本：设定为 1
    uint32_t    e_entry;    // 程序入口地址
    uint32_t    e_phoff;    // Program Header Table 的文件偏移
    uint32_t    e_shoff;    // Section Header Table 的文件偏移
    uint32_t    e_flags;    // 未使用
    uint16_t    e_ehsize;   // ELF 头大小
    uint16_t    e_phentsize;// Program Header Table 中每一个条目的大小
    uint16_t    e_phnum;    // Progrma Header Table 中的条目数量
    uint16_t    e_shentsize;// Section Hedaer Table 中每一个条目的大小
    uint16_t    e_shnum;    // Section Header Table 中的条目个数
    uint16_t    e_shstrndx; // 包含 Section 名列表的 Section Header 索引
};

struct Proghdr {
    uint32_t    p_type;     // 段类型
    uint32_t    p_offset;   // 段 raw 数据的文件偏移
    uint32_t    p_va;       // 载入时段在内存中的驻留地址
    uint32_t    p_pa;       // 未使用
    uint32_t    p_filesz;   // 段在文件中的大小，可以为0
    uint32_t    p_memsz;    // 段在内存中的大小，可以为0
    uint32_t    p_flags;    // 段的可读可写可执行权限位
    uint32_t    p_align;    // 段在内存&文件中的对齐方式
};

// Proghdr::p_type 的变量选择范围
#define ELF_PROG_LOAD   1

// Proghdr::p_flags 的取值
#define ELF_PROG_FLAG_EXEC  1
#define ELF_PROG_FLAG_WRITE 2
#define ELF_PROG_FLAG_READ  4

#endif  // ! _ELF_H_
