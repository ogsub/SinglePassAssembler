.extern ex
.global label1
.section .data
a: 	.word 22
t: 	.skip 2
	.equ x, 2 + t
	.equ y, x + 2 - r
r: 	.byte 34
b: 	.byte 3
.skip 1
.section .text
    call *%r1
    call *x
    add %r1, %r2
label1:
    mov $3, y(%pc)
    mov y(%pc), %r1
    mov label1(%pc), %r1
.end


izmenjen prethodni primer, ubacujemo jos jednu .equ direktivu, zavisnu od prethodne,
koristimo tu novu pri pc relativnom adresiranju. Koriscenje globalne promenljive pri pc relativnom adresiranju

