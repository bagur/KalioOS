; KalioOS (C) 2020 Pranav Bagur

; BIOS provides auto-detection and basic control of our computer’s essential devices,
; such as the screen, keyboard, and hard disks.
; 
; After BIOS completes some low-level tests of the hardware, particularly whether or
; not the installed memory is working correctly, it must boot the operating system stored
; on one of our devices. Here, we are reminded, though, that BIOS cannot simply load a
; file that represents your operating system from a disk, since BIOS has no notion of a filesystem. 
; BIOS must read specific sectors of data (usually 512 bytes in size) from specific
; physical locations of the disk devices, such as Cylinder 2, Head 3, Sector 5 (details of
; disk addressing are described later, in Section XXX).
; 
; So, the easiest place for BIOS to find our OS is in the first sector of one of the disks
; (i.e. Cylinder 0, Head 0, Sector 0), known as the boot sector. Since some of our disks may
; not contain an operating systems (they may simply be connected for additional storage),
; then it is important that BIOS can determine whether the boot sector of a particular
; disk is boot code that is intended for execution or simply data. Note that the CPU does
; not differentiate between code and data: both can be interpreted as CPU instructions,
; where code is simply instructions that have been crafted by a programmer into some
; useful algorithm.
; 
; Again, an unsophisticated means is adopted here by BIOS, whereby the last two
; bytes of an intended boot sector must be set to the magic number 0xaa55. So, BIOS
; loops through each storage device (e.g. floppy drive, hard disk, CD drive, etc.), reads
; the boot sector into memory, and instructs the CPU to begin executing the first boot
; sector it finds that ends with the magic number.

; Once bios loads our boot sector, it tells the CPU to jump to the start of our code.
; But where does BIOS load our boot sector in memory?
;
; We might assume, unless we knew otherwise, that BIOS loaded our code at the start of memory,
; at address 0x0. It’s not so straightforward, though, because we know that BIOS has already 
; being doing initialisation work on the computer long before it loaded our code, and will actually
; continue to service hardware interrupts for the clock, disk drives, and so on. So these
; BIOS routines (e.g. ISRs, services for screen printing, etc.) themselves must be stored
; somewhere in memory and must be preserved (i.e. not overwritten) whilst they are
; still of use. Also, we noted earlier that the interrupt vector is located at the start of
; memory, and were BIOS to load us there, our code would stomp over the table, and upon
; the next interrupt occurring, the computer will likely crash and reboot: the mapping
; between interrupt number and ISR would effectively have been severed.
; 
; As it turns out, BIOS likes always to load the boot sector to the address 0x7c00,
; where it is sure will not be occupied by important routines. 


; bootloader offset 
[org 0x7c00] 
; we will load our kernel into this offset
KERNEL_OFFSET equ 0x9100

; BIOS stores our boot drive in dl. save it at BOOT_DRIVE
    mov [BOOT_DRIVE], dl 

; Setup our stack at 0x9000
    mov bp, 0x9000
    mov sp, bp

    mov bx, MSG_REAL_MODE
    push bx
    call print_string_with_newline

    ; place to load our kernel
    mov bx, KERNEL_OFFSET 
    push bx
    ; sector # to start reading from
    mov ax, 0x2
    push ax
    ; head #
    mov ax, 0x0
    push ax
    ; cylinder #
    push ax
    ; # of sectors to read
    mov ax, KERNEL_SIZE
    push ax
    ; drive #
    mov ah, 0x0
    mov al, [BOOT_DRIVE]
    push ax
    
    call disk_load

    mov bx, MSG_LOAD_KERNEL
    push bx
    call print_string_with_newline

    call switch_to_protected_mode
    jmp $ ; this will actually never be executed

%include "print_string_16_bit.asm"
%include "load_disk.asm"
%include "gdt.asm"
%include "print_string.asm"
%include "kernel_size.asm"
%include "switch_to_32bit.asm"


[bits 32]
BEGIN_PM: ; after the switch we will get here
    mov ebx, MSG_PROT_MODE
    push ebx
    call print_string_pm ; Note that this will be written at the top left corner
    call KERNEL_OFFSET
    jmp $

; globals
MSG_REAL_MODE db "Started in 16-bit real mode", 0
MSG_PROT_MODE db "Loaded 32-bit protected mode", 0
MSG_LOAD_KERNEL db "Loaded kernel into memory", 0
BOOT_DRIVE db 0

times 510-($-$$) db 0
dw 0xaa55
