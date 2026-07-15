#.globl memmove
# deprecated
memmove_old:
	# preserve stack frame and modified string pointers
	push %ebp
	mov %esp, %ebp
	push %esi
	push %edi
	# put arguments into their regs
	mov 8(%ebp), %edi
	mov 12(%ebp), %esi
	mov 16(%ebp), %ecx
	mov %edi, %eax
	cmp %esi, %edi
	# nothing to do
	je .Lmm0
	# backwards copy
	ja .Lmm1
	# fallthrough to forward copy
.Lmm2:
	cld
	# convenient prefix to do the copy
	rep movsb
	jmp .Lmm0
.Lmm1:
	std
	# because backward copy is backwards and
	# rep does a forward copy we have to
	# decrement the regs
	add %ecx, %esi
	dec %esi
	add %ecx, %edi
	dec %edi
	rep movsb
	cld
.Lmm0:
	# restore what we just did
	pop %edi
	pop %esi
	mov %ebp, %esp
	pop %ebp
	ret
