; KalioOS (C) 2020 Pranav Bagur
;
; The specific BIOS routine we are interested in here is accessed by raising
; interrupt 0x13 after setting the register al to 0x02. This BIOS routine expects
; us to set up a few other registers with details of which disk device to use, 
; which blocks we wish to read from the disk, and where to store the blocks in 
; memory. The most difficult part of using this routine is that we must specify
; the first block to be read using a CHS addressing scheme; otherwise, it is just
; a case of filling in the expected registers
;
; Must be executed in 16-bit real mode
; 
; load 'dh' sectors from drive 'dl' into ES:BX
;
; args -
; drive # 
; # of sectors to read
; cylinder #
; head #
; sector #
; Where to load data
; 
disk_load:
  push bp               
  mov bp, sp             
  pusha
  
  mov bx, [bp+14]  ; Where to load data (ES:BX) 
  mov cl, [bp+12]  ; sector to start reading from       
  mov dh, [bp+10]  ; head number
  mov ch, [bp+8]   ; cylinder 
  mov al, [bp+6]   ; # of sectors to read
  mov dl, [bp+4]   ; drive to read from

  mov ah, 0x02     ; BIOS read sector function

  int 0x13         ; BIOS interrupt

  jc disk_error    ; Jump if error ( i.e. carry flag set )

; The BIOS routine will set al to the number of sectors
; actuall read
  mov dh, [bp+6]   ; The number of sectors we wanted to read
  cmp dh, al
  jne disk_error

  popa
  pop bp
  ret 14

disk_error:
  mov bx, DISK_ERROR_MSG
  push bx
  call print_string_with_newline
  jmp $

DISK_ERROR_MSG: db "Disk read error", 0
