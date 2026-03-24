extern cpu_vendor
global get_cpu_vendor
get_cpu_vendor:
	push ebx
	xor eax, eax
	cpuid
	; you should now read in order: EBX, EDX, ECX. interesting design choice,
	; but i'm not a kernel developer to question intel or AMD standards
	mov [cpu_vendor], ebx
	mov [cpu_vendor+4], edx
	mov [cpu_vendor+8], ecx
	mov byte [cpu_vendor+12], 0
	mov eax, cpu_vendor
	pop ebx
	ret
