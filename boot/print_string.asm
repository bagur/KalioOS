; KalioOS (C) 2020 Pranav Bagur

[ bits 32]

; The Display device can be configured into one of several resolutions in one of two modes,
; text mode and graphics mode; and that what is displayed on the screen is a visual 
; representation of a specific range of memory. So, in order to manipulate the screen,
; we must manipulate the specific memory range that it is using in its current mode.
; The display device is an example of memory-mapped hardware because it works in this way.
; 
; When most computers boot, despite that they may infact have more advanced graphics
; hardware, they begin in a simple Video Graphics Array (VGA) colour text mode with
; dimmensions 80x25 characters. In text mode, the programmer does not need to render
; individual pixels to describe specific characters, since a simple font is already defined
; in the internal memory of the VGA display device. Instead, each character cell of the
; screen is represented by two bytes in memory: the first byte is the ASCII code of the
; character to be displayed, and the second byte encodes the characters attributes, such
; as the foreground and background colour and if the character should be blinking.
; 
; So, if we’d like to display a character on the screen, then we need to set its ASCII
; code and attributes at the correct memory address for the current VGA mode, which
; is usually at address 0xb8000.

; Define some constants
VIDEO_MEMORY equ 0xb8000
COLOR_ATTR equ 0x0f

; Prints a null terminated string
;
; args - takes address of the null terminated string on stack
print_string_pm :
  push ebp               
  mov ebp, esp             
  pusha
  mov ebx, [ebp+8]         

  mov edx , VIDEO_MEMORY          ; Set edx to the start of vid mem.
print_string_pm_loop :
  mov al , [ ebx ]                ; Store the char at EBX in AL
  mov ah , COLOR_ATTR             ; Store the attributes in AH
                                  ; 0x0f = white on blak background

  cmp al , 0                      ; if (al == 0), at end of string , so
  je print_string_pm_done         ; jump to done
  mov [edx], ax                   ; Store char and attributes at current
                                  ; character cell.
  add ebx , 1                     ; Increment EBX to the next char in string.
  add edx , 2                     ; Move to next character cell in vid mem.
  jmp print_string_pm_loop        ; loop around to print the next char.

print_string_pm_done :
  popa
  pop ebp
  ret 4
