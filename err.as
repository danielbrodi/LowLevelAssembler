.entry
.extern W
MAIN: mov @r3 LENGTH
LOOP: jmp L1,
prn , -5
bne W
.data: sub @r1 ,, @r4
mov: bne L3
L1: inc K, 1
.entry LOOP
.extern L, L2
STR:jmp W
END: stop @r1
STR: .string "abcdef"
LENGTH: .data 6, -9, 15
K: .data 22
.extern L3

