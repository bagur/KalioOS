; KalioOS (C) 2020 Pranav Bagur

; Defined in isr.c
[extern isr_handler]
[extern irq_handler]

; Common ISR code
isr_common:
  ; See isr.h (1 & 2 are already done by this point)
  ; 3. Save CPU state
	pusha
	mov ax, ds    ; Lower 16-bits of eax = ds.

  ; 4. Save the current data segment descriptor
	push eax

  ; Load the kernel data segment descriptor
	mov ax, 0x10  
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
  ; Call common C handler
	call isr_handler
	
  ; Restore state
	pop eax 
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	popa
	add esp, 8 ; Cleans up the pushed error code and pushed ISR number
	sti
  ; The IRET instruction is specifically designed to return from an
  ; interrupt. It pops code segment, instruction pointer, flags register,
  ; stack segment and stack pointer values off the stack and returns 
  ; the processor to the state it was in originally.
	iret


; Common IRQ code. Identical to ISR code except for the 'call' 
; and the 'pop ebx'
irq_common:
    pusha 
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call irq_handler    ; Different than the ISR code
    pop ebx             ; Different than the ISR code
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    sti
    iret 

%macro ISR_NOERRCODE 1  ; define a macro, taking one parameter
  [GLOBAL isr%1]        ; %1 accesses the first parameter.
  isr%1:
    cli                 ; Disable interrupts 
    push byte 0         ; Push a dummy error code (if ISR doesn't push it's own error code)
    push byte %1        ; Push the interrupt number
    jmp isr_common      ; Call into the common stub
%endmacro

%macro ISR_ERRCODE 1
  [GLOBAL isr%1]
  isr%1:
    cli                 ; Disable interrupts 
    push byte %1        ; Push the interrupt number 
    jmp isr_common      ; Call into the common stub
%endmacro 

%macro IRQ 2            ; Define a macro that takes two args (IRQ#, ISR#)
  global irq%1          ; Will expand to irq#
  irq%1:
    cli                 ; Disable interrupt
    push byte 0         
    push byte %2        ; push the ISR number 
    jmp irq_common      ; Call into common stub
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

IRQ 0,  32
IRQ 1,  33
IRQ 2,  34
IRQ 3,  35
IRQ 4,  36
IRQ 5,  37
IRQ 6,  38
IRQ 7,  39
IRQ 8,  40
IRQ 9,  41
IRQ 10, 42
IRQ 11, 43 
IRQ 12, 44 
IRQ 13, 45 
IRQ 14, 46 
IRQ 15, 47 
