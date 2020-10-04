.extern ex
.section .data
a: 	.word 22
t: 	.skip 2
r: 	.byte 34
b: 	.byte 3
.skip 1
.section .text
    call *%r1
a:    call *%r2
    add %r1, %r2
label1:
    mov $3, a(%pc)
.end


test1 sa duplom definicijom