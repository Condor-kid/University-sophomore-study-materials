BITS 16

[extern startUp]
[extern shell]

global _start
_start:
    call dword startUp

Keyboard:
    mov ah, 0
    int 16h
    cmp al, 0dh      ; 按下回车
    jne Keyboard     ; 无效按键，重新等待用户按键
    call dword shell ; 进入命令行界面
    jmp Keyboard     ; 无限循环