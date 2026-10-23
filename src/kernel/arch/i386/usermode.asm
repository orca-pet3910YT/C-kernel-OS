global jump_usermode
extern test_usermode
jump_usermode:
	mov ax, (4*8) | 3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push (4*8) | 3 ; data selector
	push eax ; esp
	pushf
	push (3*8) | 3 ; code selector
	push test_usermode
	iret
