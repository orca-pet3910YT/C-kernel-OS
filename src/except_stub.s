.global isr0
isr0:
	cli
	call isr0_handle
	hlt
