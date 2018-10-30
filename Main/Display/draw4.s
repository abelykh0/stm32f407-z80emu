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

	  push	{r4, r5}

	  ldr r5, [pix], #4
	  ldr r3, [col], #4
	  ror r5, r5, #4
	  and r4, r5, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.rept 7
	  ror r5, r5, #31
	  and r4, r5, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.endr
	  ror r3, r3, #16
	  ror r5, r5, #15
	  and r4, r5, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.rept 7
	  ror r5, r5, #31
	  and r4, r5, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.endr
	  ldr r3, [col], #4
	  ror r5, r5, #15
	  and r4, r5, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.rept 7
	  ror r5, r5, #31
	  and r4, r5, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.endr
	  ror r3, r3, #16
	  ror r5, r5, #15
	  and r4, r5, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.rept 7
	  ror r5, r5, #31
	  and r4, r5, #8
	  lsr r4, r3, r4
	  strb r4, [dst], #1
	.endr

	pop	{r4, r5}
    bx lr
