BITS 16

[global clearScreen]
[global printInPos]
[global putchar]
[global getch]
[global powerOff]
[global getUsrProgNum]
[global getUsrProgName]
[global getUsrProgSize]
[global getUsrProgCylinder]
[global getUsrProgHead]
[global getUsrProgSector]
[global getUsrProgAddr]
[global loadAndRun]

offset_upinfo equ 7E00h    ; 用户程序信息表被装入的位置

clearScreen:               ; 函数：清屏
    push ax
    mov ax, 0003h
    int 10h                ; 中断调用，清屏
    pop ax
    retf


printInPos:                ; 函数：在指定位置显示字符串
    pusha                  ; 保护现场（压栈16字节）
    mov si, sp             ; 由于代码中要用到bp，因此使用si来为参数寻址
    add si, 16+4           ; 首个参数的地址
    mov	ax, cs             ; 置其他段寄存器值与CS相同
    mov	ds, ax             ; 数据段
    mov	bp, [si]           ; BP=当前串的偏移地址
    mov	ax, ds             ; ES:BP = 串地址
    mov	es, ax             ; 置ES=DS
    mov	cx, [si+4]         ; CX = 串长（=9）
    mov	ax, 1301h          ; AH = 13h（功能号）、AL = 01h（光标置于串尾）
    mov	bx, 0007h          ; 页号为0(BH = 0) 黑底白字(BL = 07h)
    mov dh, [si+8]         ; 行号=0
    mov	dl, [si+12]        ; 列号=0
    int	10h                ; BIOS的10h功能：显示一行字符
    popa                   ; 恢复现场（出栈16字节）
    retf


putchar:                   ; 函数：在光标处打印一个字符
    pusha
    mov bp, sp
    add bp, 16+4           ; 参数地址
    mov al, [bp]           ; al=要打印的字符
    mov bh, 0              ; bh=页码
    mov ah, 0Eh            ; 功能号：打印一个字符
    int 10h                ; 打印字符
    popa
    retf


getch:                     ; 函数：读取一个字符到tempc（无回显）
    mov ah, 0              ; 功能号
    int 16h                ; 读取字符，al=读到的字符
    mov ah, 0              ; 为返回值做准备
    retf


powerOff:                  ; 函数：强制关机
    mov ax, 2001H
    mov dx, 1004H
    out dx, ax


getUsrProgNum:
    mov al, [offset_upinfo]
    mov ah, 0
    retf


getUsrProgName:
    push bp
    push bx
    mov bp, sp
    add bp, 4+4
    mov al, [bp]           ; al=pid
    add al, -1             ; al=pid-1
    mov bl, 24             ; 每个用户程序的信息块大小为24字节
    mul bl                 ; ax = (pid-1) * 24
    add ax, 1              ; ax = 1 + (pid-1) * 24
    add ax, 1              ; 加上name在用户程序信息中的偏移
    add ax, offset_upinfo  ; 不用方括号，因为就是要访问字符串所在的地址
    pop bx
    pop bp
    retf


getUsrProgSize:
    push bp
    push bx
    mov bp, sp
    add bp, 4+4
    mov al, [bp]           ; al=pid
    add al, -1             ; al=pid-1
    mov bl, 24             ; 每个用户程序的信息块大小为24字节
    mul bl                 ; ax = (pid-1) * 24
    add ax, 1              ; ax = 1 + (pid-1) * 24
    add ax, 17             ; 加上size在用户程序信息中的偏移
    mov bx, ax
    add bx, offset_upinfo
    mov ax, [bx]
    pop bx
    pop bp
    retf


getUsrProgCylinder:
    push bp
    push bx
    mov bp, sp
    add bp, 4+4
    mov al, [bp]           ; al=pid
    add al, -1             ; al=pid-1
    mov bl, 24             ; 每个用户程序的信息块大小为24字节
    mul bl                 ; ax = (pid-1) * 24
    add ax, 1              ; ax = 1 + (pid-1) * 24
    add ax, 19             ; 加上cylinder在用户程序信息中的偏移
    mov bx, ax
    add bx, offset_upinfo
    mov al, [bx]
    mov ah, 0
    pop bx
    pop bp
    retf


getUsrProgHead:
    push bp
    push bx
    mov bp, sp
    add bp, 4+4
    mov al, [bp]           ; al=pid
    add al, -1             ; al=pid-1
    mov bl, 24             ; 每个用户程序的信息块大小为24字节
    mul bl                 ; ax = (pid-1) * 24
    add ax, 1              ; ax = 1 + (pid-1) * 24
    add ax, 20             ; 加上head在用户程序信息中的偏移
    mov bx, ax
    add bx, offset_upinfo
    mov al, [bx]
    mov ah, 0
    pop bx
    pop bp
    retf


getUsrProgSector:
    push bp
    push bx
    mov bp, sp
    add bp, 4+4
    mov al, [bp]           ; al=pid
    add al, -1             ; al=pid-1
    mov bl, 24             ; 每个用户程序的信息块大小为24字节
    mul bl                 ; ax = (pid-1) * 24
    add ax, 1              ; ax = 1 + (pid-1) * 24
    add ax, 21             ; 加上sector在用户程序信息中的偏移
    mov bx, ax
    add bx, offset_upinfo
    mov al, [bx]
    mov ah, 0
    pop bx
    pop bp
    retf


getUsrProgAddr:
    push bp
    push bx
    mov bp, sp
    add bp, 4+4
    mov al, [bp]           ; al=pid
    add al, -1             ; al=pid-1
    mov bl, 24             ; 每个用户程序的信息块大小为24字节
    mul bl                 ; ax = (pid-1) * 24
    add ax, 1              ; ax = 1 + (pid-1) * 24
    add ax, 22             ; 加上addr在用户程序信息中的偏移
    mov bx, ax
    add bx, offset_upinfo
    mov ax, [bx]
    pop bx
    pop bp
    retf


loadAndRun:                ; 函数：从软盘中读取扇区到内存并运行用户程序
    pusha
    mov bp, sp
    add bp, 16+4           ; 参数地址
    mov ax,cs              ; 段地址; 存放数据的内存基地址
    mov es,ax              ; 设置段地址（不能直接mov es,段地址）
    mov bx, [bp+16]        ; 偏移地址; 存放数据的内存偏移地址
    mov ah,2               ; 功能号
    mov al,[bp+12]         ; 扇区数
    mov dl,0               ; 驱动器号; 软盘为0，硬盘和U盘为80H
    mov dh,[bp+4]          ; 磁头号; 起始编号为0
    mov ch,[bp]            ; 柱面号; 起始编号为0
    mov cl,[bp+8]          ; 起始扇区号 ; 起始编号为1
    int 13H                ; 调用读磁盘BIOS的13h功能
    call dword pushCsIp    ; 手动压栈CS、IP
    pushCsIp:
    mov si, sp             ; si指向栈顶
    mov word[si], afterrun ; 修改栈中IP的值，这样用户程序返回后就可以继续执行了
    jmp [bp+16]
    afterrun:
    popa
    retf
