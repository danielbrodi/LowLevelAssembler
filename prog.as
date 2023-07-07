mcro m1
mov @r1, @r2
sub @r3, @r4
endmcro

mcro m2
add @r5, @r6
endmcro

m1
m2

mov @r1, @r3
cmp @r2, @r3
