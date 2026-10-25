global jump_usermode
jump_usermode:
;	mov ax, (4*8) | 3
;	mov ds, ax
;	mov es, ax
;	mov fs, ax
;	mov gs, ax
;	mov eax, esp
;	push (4*8) | 3 ; data selector
;	push eax ; esp
;	pushf
;	push (3*8) | 3 ; code selector
;	push test_usermode
;	iret
mov eax, esp
push (4*8) | 3
push eax
pushf
push (3*8) | 3
push test_usermode
iret

global test_usermode
test_usermode:
	; we're in usermode now.
	; or at least i hope lmao
	mov ax, (4*8) | 3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, 4
	mov ebx, 1 ; output
	mov ecx, test_str
	mov edx, test_str_size
	int 0x80
.loop:
	pause
	jmp .loop
	hlt ; only if jmp $ fails

section .data
test_str db "hey ckos usermode here"
test_str_size equ $ - test_str ; size
