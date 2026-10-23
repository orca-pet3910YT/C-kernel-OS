global flush_tss
flush_tss:
	mov ax, (5*8) | 0;
	ltr ax
	ret
