%macro ISR_NO_CODE 1
global isr%1
isr%1:
        cli
        push dword 0 ; normally use this when there is no error code like here. you can omit this instruction
                ; in handlers with error codes
        push dword %1 ; %1 is the value of the int
        jmp common_isr ; the common stub
%endmacro

%macro ISR_CODE 1
global isr%1
isr%1:
        cli
        push dword %1 ; again, %1 is the value of the int
        jmp common_isr
%endmacro

extern isr_handler

common_isr:
        pusha
        mov ax, ds
        push eax
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        cld
        push esp
        call isr_handler
        add esp, 4
        pop eax
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        popa
        add esp, 8
        ; generally, don't reenable interrupts. they might cause more problems if the faulty instruction
        ; triggers something again.
        iret

ISR_NO_CODE 0
ISR_NO_CODE 1
ISR_NO_CODE 2
ISR_NO_CODE 3
ISR_NO_CODE 4
ISR_NO_CODE 5
ISR_NO_CODE 6
ISR_NO_CODE 7
ISR_CODE 8
ISR_NO_CODE 9
ISR_CODE 10
ISR_CODE 11
ISR_CODE 12
ISR_CODE 13
ISR_CODE 14
ISR_NO_CODE 15
ISR_NO_CODE 16
ISR_CODE 17
ISR_NO_CODE 18
ISR_NO_CODE 19
ISR_NO_CODE 20
ISR_NO_CODE 21
ISR_NO_CODE 22
ISR_NO_CODE 23
ISR_NO_CODE 24
ISR_NO_CODE 25
ISR_NO_CODE 26
ISR_NO_CODE 27
ISR_NO_CODE 28
ISR_NO_CODE 29
ISR_CODE 30
ISR_NO_CODE 31

global load_idt
load_idt:
        mov eax, [esp+4]
        lidt [eax]
        ret

%macro IRQ_NOCODE 1
global isr%1
isr%1:
        cli
        push dword 0 ; IRQs have no codes
        push dword %1 ; value of the int.
        jmp common_irq
%endmacro

; 32 - 47
IRQ_NOCODE 32
IRQ_NOCODE 33
IRQ_NOCODE 34
IRQ_NOCODE 35
IRQ_NOCODE 36
IRQ_NOCODE 37
IRQ_NOCODE 38
IRQ_NOCODE 39
IRQ_NOCODE 40
IRQ_NOCODE 41
IRQ_NOCODE 42
IRQ_NOCODE 43
IRQ_NOCODE 44
IRQ_NOCODE 45
IRQ_NOCODE 46
IRQ_NOCODE 47


extern irq_handler

common_irq:
        pusha ; push GPRs
        mov ax, ds
        push eax
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        cld
        push esp
        call irq_handler
        add esp, 4
        pop eax
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        popa
        add esp, 8
        iret ; NOT just ret - remember, we're in an int

