; KalioOS (C) 2020 Pranav Bagur
;
; To jump into 32-bit mode:
;
; 1. Disable interrupts
;      The first thing we have to do is disable interrupts using the cli (clear interrupt)
;      instruction, which means the CPU will simply ignore any future interrupts that may
;      happen, at least until interrupts are later enabled. This is very important, because, like
;      segment based addressing, interrupt handling is implemtented completely differently in
;      protected mode than in real mode, making the current IVT that BIOS set up at the start
;      of memory completely meaningless; and even if the CPU could map interrupt signals to
;      their correct BIOS routines (e.g. when the user pressed a key, store its value in a buffer),
;      the BIOS routines would be executing 16-bit code, which will have no concept of the
;      32-bit segments we defined in our GDT and so will ulimately crash the CPU by having
;      segment register values that assume the 16-bit real mode segmenting scheme
;
; 2. Load our GDT
;      using : lgdt [ gdt_descriptor ]
;
; 3. Set a bit on the CPU control register cr0
;      we make the actual switch over, by setting the first bit of a
;      special CPU control register, cr0 (using a general purpose register)
;     
; -------- After cr0 has been updated, the cpu is in 32 bit protected mode --------------------
;
; 4. Flush the CPU pipeline by issuing a carefully crafted far jump
;      Switching CPU modes is a special case, since there is a risk that the CPU may 
;      process some stages of an instruction’s execution in the wrong mode. So what we
;      need to do, immediately after instructing the CPU to switch mode, is to
;      force the CPU to finish any jobs in its pipeline, so that we can be confident that all
;      future instructions will be executed in the correct mode.
;
; 5. Update all the segment registers
;      By the very definition of a far jump, it will automatically cause the CPU to update our
;      cs register to the target segment. A good thing to do once we have entered
;      32-bit mode proper is to update all of the other segment registers so they now point to
;      our 32-bit data segment 
;
; 6. Update the stack
;
; 7. Call to a well-known label which contains the first useful code in 32 bits


[bits 16]

; Switch to protected mode
;
; args - no args
switch_to_protected_mode:
; step 1
    cli
; step 2
    lgdt [gdt_descriptor]
; step 3
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
; step 4
    jmp CODE_SEG:init_pm


; We need to use the [bits 32] directive to tell our assembler that,
; from that point onwards, it should encode in 32-bit mode instructions
[bits 32]


; Initialize registers and stack once we are in protected mode
;
; args - no args
init_pm: ; we are now using 32-bit instructions
; step 5. Our code segment register is now already pointing to the correct
; code selector. Update the other segment registers to the ata selector we
; defined in our GDT
    mov ax, DATA_SEG 
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

; step 6. Update the stack right at the top of the free space
    mov ebp, 0x90000 
    mov esp, ebp
; step 7
    call BEGIN_PM

