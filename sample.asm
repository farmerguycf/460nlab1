	.orig x3000
	add r0, r0, r0
	add r0, r0, #4
	and r1, r1, r0
	and r1, r1, #1
label 	add r1, r1, #5
	br  label
second  add r4, r4, #0
	ldw r2, num	
	halt

num	.fill 0x1500
.end
