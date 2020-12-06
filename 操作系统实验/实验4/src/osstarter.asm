BITS 16
%include "macro.asm"
[extern startUp]
[extern shell]
[extern Timer]

global _start
_start:
    MOVE_INT_VECTOR 08h, 38h
    WRITE_INT_VECTOR 08h, Timer ; 装填时钟中断向量表
    call dword startUp

Keyboard:
    mov ah, 0
    int 16h
    cmp al, 0dh                 ; 按下回车
    jne Keyboard                ; 无效按键，重新等待用户按键
    call dword shell            ; 进入命令行界面
    jmp Keyboard                ; 无限循环