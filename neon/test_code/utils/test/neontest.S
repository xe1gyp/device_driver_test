.globl Value
.data
.align 2
.type Value, %object
.size Value, 4
Value:
	.word 1000
	.text
	.align 2

.globl Result
.data
.align 2
.type Result, %object
.size Result, 4
Result:
	.word 0
	.text
	.align 2

.globl main
.type main, %function
.text
main:
	ldr	r2, .L3
	ldr	r3, [r2, #0]
	add	r3, #1000
	ldr	r2, .L3+4
	str	r3, [r2, #0]
	mov	r3, #0
	mov	r0, r3
	mov	r1, #1
	adr	r0, printf_string

	mov	r1, #0x400
	vmov.32 d1[1], r1
	mov	r1, #0x100
	vmov.32 d1[0], r1

	mov	r2, #0x600
	vmov.32 d2[1], r2
	mov	r2, #0x200
	vmov.32 d2[0], r2

	vadd.u32 d3, d2, d1
	vadd.u32 d3, d2, d1
	vmov.u32 r1, d3[1]

	vmla.i32 d3, d2, d1
	vmov.i32 r1, d3[1]
	adr	r0, printf_string
	bl	printf

	mov	r0, #100
	bl	malloc

	vld3.8	{d0[0], d1[0], d2[0]}, [r0]!


	b	_exit
.L4:
	.align 2
.L3:
	.word Value
	.word Result

#.data

printf_string:
	.ascii " NEON INSTRUCTIONS TEST PASSED \n"
.end
