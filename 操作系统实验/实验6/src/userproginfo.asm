%include "macro.asm"
%macro UserProgInfoBlock 8    ; 参数：(ProgID,程序名,字节数,柱面,磁头,扇区,内存地址)
    progid%1 db %1           ; 程序编号ProgID；相对偏移0
    name%1 db %2             ; 程序名（至多32字节）；相对偏移1
    times 16-($-name%1) db 0 ; 程序名占6字节
    size%1 dw %3             ; 程序大小；相对偏移17
    cylinder%1 db %4         ; 柱面；相对偏移19
    head%1 db %5             ; 磁头；相对偏移20
    sector%1 db %6           ; 扇区；相对偏移21
    addr_seg%1 dw %7         ; 内存地址段值；相对偏移22
    addr_off%1 dw %8         ; 内存地址偏移量；相对偏移24
%endmacro                    ; 共26个字节

UserProgNumber:
    db 6                     ; 用户程序数量

UserProgInfo:
    UserProgInfoBlock 1, 'b',    1024, 1, 0, 1,  addr_userprog1 >> 4 & 0F000h,    addr_userprog1 & 0FFFFh
    UserProgInfoBlock 2, 'a',   1024, 1, 0, 3,  addr_userprog2 >> 4 & 0F000h,    addr_userprog2 & 0FFFFh
    UserProgInfoBlock 3, 'c',    1024, 1, 0, 5,  addr_userprog3 >> 4 & 0F000h,    addr_userprog3 & 0FFFFh
    UserProgInfoBlock 4, 'd',   1024, 1, 0, 7,  addr_userprog4 >> 4 & 0F000h,    addr_userprog4 & 0FFFFh
    UserProgInfoBlock 5, 'interrupt_caller', 512,  1, 0, 9,  addr_intcaller >> 4 & 0F000h,   addr_intcaller & 0FFFFh
    UserProgInfoBlock 6, 'syscall_test',     1536, 1, 0, 10, addr_syscalltest >> 4 & 0F000h, addr_syscalltest & 0FFFFh

SectorEnding:
    times 512-($-$$) db 0