BITS 16
%include "macro.asm"
[extern startUp]
[extern shell]
[extern syscaller]
[extern Timer]

extern set_clock

global _start
_start:
    WRITE_INT_VECTOR 21h, syscaller ; 装填系统调用中断向量表

SetTimer:
    mov al,34h                      ; 设控制字值
    out 43h,al                      ; 写控制字到控制字寄存器
    mov ax,29830                    ; 每秒 20 次中断（50ms 一次）
    out 40h,al                      ; 写计数器 0 的低字节
    mov al,ah                       ; AL=AH
    out 40h,al                      ; 写计数器 0 的高字节
    WRITE_INT_VECTOR 08h, Timer
    call dword startUp              ; 进入欢迎界面

Keyboard:
    mov ah, 0
    int 16h
    cmp al, 0Dh                     ; 按下回车
    jne Keyboard                    ; 无效按键，重新等待用户按键
    call dword shell                ; 进入命令行界面
    jmp Keyboard                    ; 无限循环