.global label1
.section .data
label1: .skip 2
.section .text
    jeqw 13
    jeqb a
    jeq *%r1h
a:  jeq *(%r1h)
    mov $3, %r1h
    mov $300, (%r1l)
.end


razlicite velicine operanada