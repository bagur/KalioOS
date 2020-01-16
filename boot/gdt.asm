; KalioOS (C) 2020 Pranav Bagur
;
; Once the CPU has been switched into 32-bit protected mode, the process by which it translates
; logical addresses (i.e. the combination of a segment register and an offset) to physical 
; address is completely different: rather than multiply the value of a segment register by
; 16 and then add to it the offset, a segment register becomes an index to a particular 
; segment descriptor (SD) in the GDT.
; 
; A segment descriptor is an 8-byte structure that defines the following properties 
; of a protected-mode segment:
; 
; • Base address (32 bits), which defines where the segment begins in physical memory
; • Segment Limit (20 bits), which defines the size of the segment
; • Various flags, which affect how the CPU interprets the segment, such as the 
;   privilige level of code that runs within it or whether it is read- or write-only
;
; +-----------------+------------------+------------------+-----------------+
; |                 |                  |                  |                 |
; |                 |                  |                  |                 |
; |     Base (1b)   | G|D/B|L|AVL|SEG  |  P|DPL|S|TYPE    |    Base (1b)    |
; |                 | 1| 1 |1| 1 | 4   |  1| 2 |1| 4      |                 |
; |                 |                  |                  |                 |
; +-----------------+------------------+------------------+-----------------+
; 
; 
; 
; +------------------------------------+------------------------------------+
; |                                    |                                    |
; |                                    |                                    |
; |        Base (2 bytes)              |           Limit (2 bytes)          |
; |                                    |                                    |
; |                                    |                                    |
; +------------------------------------+------------------------------------+

; The simplest workable configuration of segment registers is described by Intel as the
; basic flat model, whereby two overlapping segments are defined that cover the full 4 GB
; of addressable memory, one for code and the other for data. The fact that in this model
; these two segments overlap means that there is no attempt to protect one segment from
; the other, nor is there any attempt to use the paging features for virtual memory. It
; pays to keep things simple early on, especially since later we may alter the segment
; descriptors more easily once we have booted into a higher-level language.
; In addition to the code and data segments, the CPU requires that the first entry
; in the GDT purposely be an invalid null descriptor

; Base: Segment base
; Limit: Segment offset
; Present (P): 1 if segment is present in memory 
; Privilige (DPL): ring # (0 highest, 3 lowest)
; Descriptor type (S): 1 for code or data segment, 0 is used for traps
; Type (TYPE):
;   Code:       1 for code segment
;   Conforming: 0, by not corming it means code in a segment with a lower
;               privilege may not call code in this segment - this a key 
;               to memory protection
;   Readable:   1 if readible, 0 if execute-only. Readible allows us to read
;               constants defined in the code.
;   Accessed:   This is often used for debugging and virtual memory techniques,
; 
; Granularity(G): If set, this multiplies our limit by 4 K (i.e. 16*16*16), so our
;                 0xfffff would become 0xfffff000 (i.e. shift 3 hex digits to the left), 
;                 allowing our segment to span 4 Gb of memory
; 32-bit default(D/B): 1, since our segment will hold 32-bit code
; 64-bit code segment(L): We won't use it as we are in 32-bit mode
; AVL: We can set this for our own uses (e.g. debugging) but we will not use it

gdt_start: 
; Mandatory null descriptor
; in 32 bit mode dd - 4 bytes, dw - 2 bytes, db - 1 byte
    dd 0x0 ; 4 byte
    dd 0x0 ; 4 byte

; Code segment descriptor
; Base: 0x0
; Limit: 0xfffff
; Present (P): 1 
; Privilige (DPL): 00
; Descriptor type (S): 1
; Type (TYPE):
;   Code:       1
;   Conforming: 0
;   Readable:   1 
;   Accessed:   0 
; Granularity(G): 1 
; 32-bit default(D/B): 1
; 64-bit code segment(L): 0 
; AVL: 0 
gdt_code: 
    dw 0xffff    ; segment length, bits 0-15
    dw 0x0       ; segment base, bits 0-15
    db 0x0       ; segment base, bits 16-23
    db 10011010b ; flags (8 bits)
    db 11001111b ; flags (4 bits) + segment length, bits 16-19
    db 0x0       ; segment base, bits 24-31

; Data segment descriptor
; Base: 0x0
; Limit: 0xfffff
; Present (P): 1 
; Privilige (DPL): 00
; Descriptor type (S): 1
; Type (TYPE):
;   Code:       0
;   Conforming: 0
;   Readable:   1 
;   Accessed:   0 
; Granularity(G): 1 
; 32-bit default(D/B): 1
; 64-bit code segment(L): 0 
; AVL: 0 
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:


; GDT descriptor
; The CPU needs to know how long our GDT is. Therefore we don’t actually 
; directly give the CPU the start address of our GDT but instead give it
; the address of a much simpler structure called the GDT descriptor (i.e. 
; something that describes the GDT). The GDT is a 6-byte structure containing:
; 
; • GDT size (16 bits)
; • GDT address (32 bits)
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size (16 bit), always one less of its true size
    dd gdt_start ; address (32 bit)

; Define some constants for later use
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
