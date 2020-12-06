%include "head.asm"
org 0A100h

start:
    call ClearScreen ; 清屏
    PRINT_IN_POS str_subtitle, subtitlelen, 6, 29
    PRINT_IN_POS str_hint1, hint1len, 15, 10

Keyboard:
    mov ah, 0; Bochs: 0000:a173
    int 16h
    cmp al, '1'; 按下1
    je offset_usrprog1   ; 执行用户程序b
    cmp al, '2'; 按下2
    je offset_usrprog2   ; 执行用户程序a
    cmp al, '3'; 按下3
    je offset_usrprog3   ; 执行用户程序c
    cmp al, '4'; 按下4
    je offset_usrprog4   ; 执行用户程序d
    jmp Keyboard; 无效按键，重新等待用户按键

ClearScreen:         ; 函数：清屏
    pusha
    mov ax, 0003h
    int 10h          ; 中断调用，清屏
    popa
    ret

DataArea:

    str_subtitle db '18340066_hwz'
    subtitlelen equ ($-str_subtitle)

    str_hint1 db 'Press a key (1/2/3/4) to run the user program.'
    hint1len equ ($-str_hint1)

SectorEnding:
    times 510-($-$$) db 0
    db 0x55,0xaa
