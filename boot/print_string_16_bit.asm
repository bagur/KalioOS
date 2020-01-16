; KalioOS (C) 2020 Pranav Bagur

; Print function for 16 bit real mode
; BIOS adds some of its own ISRs to the interrupt vector that
; specialise in certain aspects of the computer, for example: interrupt 0x10
; causes the screen-related ISR to be invoked
;
; args - takes address of the null terminated string on stack
print_string:
  push bp                ; save current base ptr on stack
  mov bp, sp             ; copy current sp into bp
  pusha
  mov bx, [bp+4]         ; bp -> sp. bp + 2 -> ret addr. bp + 4 -> first arg
                         ; stack grows down. Next insertion would be at bp-2

start:
  mov cx, [bx]
  mov al,0
  cmp cl, al
  je done
; BIOS scrolling teletype routine will print a single character on the
; screen and advance the cursor, ready for the next character. Here, we
; need interrupt 0x10 and to set ah to 0x0e (to indicate tele-type mode) 
; and al to the ASCII code of the character we wish to print.
  mov al, cl
  mov ah, 0x0e
  int 0x10
  add bx, 0x1
  jmp start

done:
  popa
  pop bp
  ret 2


; Print newline and carriage return
;
; args - no args
print_nl:
  mov ah, 0x0e
  mov al, 0x0a ; newline char
  int 0x10
  mov al, 0x0d ; carriage return
  int 0x10
  ret


; Print string + newline and carriage return
;
; args - takes address of the null terminated string on stack
print_string_with_newline:
  push bp               
  mov bp, sp             
  pusha
  mov bx, [bp+4]         

  push bx
  call print_string
  call print_nl

  popa
  pop bp
  ret 2
