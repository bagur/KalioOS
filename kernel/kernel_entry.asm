; KalioOS (C) 2020 Pranav Bagur
;
; We are jumping to whatever is at KERNEL_OFFSET and therefore we
; are too dependant on ordering of elements (functions, data etc)
;
; Place this at KERNEL_OFFSET and then we'll always jump to our main code
; The linker will place our call to main, with the correct address (depending
; on where main ends up in the linked file)
[bits 32]
[extern main] ; Define calling point. Must have same name as kernel.c 'main' function
call main ; Calls the C function. The linker will know where it is placed in memory
jmp $
