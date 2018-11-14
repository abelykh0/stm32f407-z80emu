.syntax unified

@ Arguments:
@  r0  pixels
@  r1  colors
@  r2  destination
.section	.text.Draw4,"ax",%progbits
//.section .ramcode,"ax",%progbits
.balign 4
.global Draw4
.thumb_func
Draw4:
      @ parameters
      pix   .req r0
      col   .req r1
      dst   .req r2

	  push {r4}

	  ldr r0, [pix]     // pixels for characters 0..3
	  ldr r3, [col], #4 // colors for characters 0..1

	// character #0
	  ror r0, r0, #4 // pixels >> 4
	  and r4, r0, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.rept 7
	  ror r0, r0, #31
	  and r4, r0, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.endr

	// character #1
	  ror r3, r3, #16 // colors
	  ror r0, r0, #15
	  and r4, r0, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.rept 7
	  ror r0, r0, #31
	  and r4, r0, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.endr

	  ldr r3, [col], #4 // colors for for characters 2..3

	// character #2
	  ror r0, r0, #15
	  and r4, r0, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.rept 7
	  ror r0, r0, #31
	  and r4, r0, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.endr

	// character #3
	  ror r3, r3, #16 // colors
	  ror r0, r0, #15
	  and r4, r0, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.rept 7
	  ror r0, r0, #31
	  and r4, r0, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.endr

	pop	{r4}
    bx lr
