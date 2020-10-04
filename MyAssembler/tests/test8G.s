.extern nej
.global label1
.section .data
label1: .skip 2
.section .text
    jeq 13
    jeq a
    jeq *%r1
    jeq *(%r1)
	jeq *3(%r1)
    jeq *a(%r1)
a:  jeq *a(%pc)
    jeq *13
    jeq *a

    mov $3, %r1
    mov $a, $3
    mov $3, (%r1)
    mov $3, 13(%r1)
    mov $3, a(%r1)
    mov $3, a(%pc)
    mov $3, 3
    mov $3, a
.end


Pogresno iskoriscen operand neposredno na mesto dst
