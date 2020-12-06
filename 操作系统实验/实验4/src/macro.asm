offset_usrprog1 equ 0A300h
offset_usrprog2 equ 0A700h
offset_usrprog3 equ 0AB00h
offset_usrprog4 equ 0AF00h
offset_intcaller equ 0xB300

offset_upinfo equ 7E00h         ; 用户程序信息表被装入的位置


%macro WRITE_INT_VECTOR 2       ; 写中断向量表；参数：（中断号，中断处理程序地址）
    push ax
    push es
    mov ax, 0
    mov es, ax                  ; ES = 0
    mov word[es:%1*4], %2       ; 设置中断向量的偏移地址
    mov ax,cs
    mov word[es:%1*4+2], ax     ; 设置中断向量的段地址=CS
    pop es
    pop ax
%endmacro

%macro MOVE_INT_VECTOR 2        ; 将参数1的中断向量转移至参数2处
    push ax
    push es
    push si
    mov ax, 0
    mov es, ax
    mov si, [es:%1*4]
    mov [es:%2*4], si
    mov si, [es:%1*4+2]
    mov [es:%2*4+2], si
    pop si
    pop es
    pop ax
%endmacro

%macro PRINT_IN_POS 4
    pusha                    ; 保护现场
    push ds
    push es
    mov	ax, cs               ; 置其他段寄存器值与CS相同
    mov	ds, ax               ; 数据段
    mov	bp, %1               ; BP=当前串的偏移地址
    mov	ax, ds               ; ES:BP = 串地址
    mov	es, ax               ; 置ES=DS
    mov	cx, %2               ; CX = 串长（=9）
    mov	ax, 1301h            ; AH = 13h（功能号）、AL = 01h（光标置于串尾）
    mov	bx, 0007h            ; 页号为0(BH = 0) 黑底白字(BL = 07h)
    mov dh, %3               ; 行号=0
    mov	dl, %4               ; 列号=0
    int	10h                  ; BIOS的10h功能：显示一行字符
    pop es
    pop ds
    popa                     ; 恢复现场
%endmacro