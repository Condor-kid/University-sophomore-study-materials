DNRT equ 1	; D-下,U-上,R-右,L-左
UPRT equ 2
UPLT equ 3
DNLT equ 4
WIDTH equ 80    ;宽度
HEIGHT equ 25   ;高度
LENGTH equ 17	; 字符串的长度
org 7c00h
section .data
	count dd 1
	x dw 0
	y dw 0
	rdul db DNRT	; 向右下运动
	message db ' 18340066 hwz_aa '
section .text
	mov ax,0B800h
	mov gs,ax	; GS = 0xB800h，文本窗口显存起始地址
myLoop:
	dec dword[count]	; 递减计数变量
	jnz myLoop	; >0：跳转;
	mov dword[count],99999999
	cmp byte[rdul],DNRT
	jz  dnrt
	cmp byte[rdul],UPRT
	jz  uprt
	cmp byte[rdul],UPLT
	jz  uplt
	cmp byte[rdul],DNLT
	jz  dnlt
dnrt:
	inc word[x]
	inc word[y]
	mov ax,HEIGHT-1
	sub ax,word[x]
	jz  dr2ur
	mov ax,WIDTH-LENGTH
	sub ax,word[y]
	jz  dr2dl
	jmp show
dr2ur:
	mov byte[rdul],UPRT
	jmp show
dr2dl:
	mov byte[rdul],DNLT
	jmp show
uprt:
	dec word[x]
	inc word[y]
	mov ax,2
	sub ax,word[x]
	jz  ur2dr
	mov ax,WIDTH-LENGTH
	sub ax,word[y]
	jz  ur2ul
	jmp show
ur2dr:
	mov byte[rdul],DNRT
	jmp show
ur2ul:
	mov byte[rdul],UPLT
	jmp show
uplt:
	dec word[x]
	dec word[y]
	mov ax,2
	sub ax,word[x]
	jz  ul2dl
	mov ax,1
	sub ax,word[y]
	jz  ul2ur
	jmp show
ul2dl:
	mov byte[rdul],DNLT
	jmp show
ul2ur:
	mov byte[rdul],UPRT
	jmp show
dnlt:
	inc word[x]
	dec word[y]
	mov ax,HEIGHT-1
	sub ax,word[x]
	jz  dl2ul
	mov ax,1
	sub ax,word[y]
	jz  dl2dr
	jmp show
dl2ul:
	mov byte[rdul],UPLT
	jmp show
dl2dr:
	mov byte[rdul],DNRT
	jmp show
show:
	xor ax,ax
	mov word ax,[x]
	mov bx,WIDTH
	mul bx
	add word ax,[y]
	mov bx,2
	mul bx
	mov bx,ax
	mov si, message
	mov cx, LENGTH
	printStr:
		mov byte al,[si]
		mov [gs:bx],ax
		inc si
		inc bx
		inc bx
	loop printStr
	jmp myLoop
