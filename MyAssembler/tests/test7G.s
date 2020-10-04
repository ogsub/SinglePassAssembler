.extern ex
.global nes
.section .data
a: 	.word 22
t: 	.skip 2
	.equ x, 2 + t
	.equ x, 3
r: 	.byte 34
b: 	.byte 3
.skip 1
.section .text
    call *%r1
nes:call *x
    add %r1, %r2
label1:
    mov $3, x(%pc)
.end


primer duple definicije pomocu .equ direktive
