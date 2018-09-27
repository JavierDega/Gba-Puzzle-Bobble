
.THUMB				@ turn on thumb
.ALIGN  2			@ align code correctly for GBA
.GLOBAL  addAndLoopFunc	@ name of first function goes here
.GLOBAL  updateFunc	@ name of second function goes here
.GLOBAL updateExtentFunc @ name of third function goes here
.GLOBAL edgeBounceFunc @ name of fourth function goes here
.GLOBAL clampFunc	@ name of fifth function goes here

.THUMB_FUNC			@ we are about to declare a thumb function
					@ c equivalent: void addAndLoopFunc(unsigned short * number, unsigned short begin, unsigned short end);
					
addAndLoopFunc:	
@ the magic happens here!

	ldr r3, [r0]		@Load dereferenced r0 into r1
	add r3, #1			@Add one to value in r1
	cmp r3, r2			@Z Flag in CPSR will turn to one if we went over the number allowed
	bne end				@If we didnt overflow , then skip the relooping
	mov r3, r1			@If we did overflow (beq = true), then execute this line of code
	end:
	str r3, [r0] 		@Store value in r1 into address stored at r0

bx lr				@ "branch and exchange" (return) back to C, using the value for lr
@ ==================================


.THUMB_FUNC			@ we are about to declare a thumb function
					@ c equivalent: void updateFunc(int * px, int * py, int vx, int vy);
					
updateFunc:

push { r4-r7, lr }	@ push r4-r7 and link register onto stack. Your function might use these
					@ registers, so we need to preserve the values just in case!
					@ we don't need to worry about r0-r3 as it is assumed they will be regularly messed up anyway

	
@ the magic happens here!
	
	ldr r4, [r0]
	ldr r5, [r1]
	add r4, r4, r2
	add r5, r5, r3
	str r4, [r0]
	str r5, [r1]
	
pop { r4-r7 }		@ pop first 4 values from stack back into r4-r7, and also
pop { r3 }			@ pop the next value from stack (stored value for lr) into some unused register, e.g. r3 -
					@ we cannot overwrite lr so we have to do it via a normal register
bx r3				@ "branch and exchange" (return) back to C, using the previous value for lr stored in r3
@ ==================================


.THUMB_FUNC			@ we are about to declare a thumb function
					@ c equivalent: void updateExtentFunc(int * maxExt , int pos, int ballSize);
					
updateExtentFunc:	
@ the magic happens here!
	
	ldr r3, [r0]
	add r3, r1, r2
	str r3, [r0]
	
bx lr				@ "branch and exchange" (return) back to C, using the previous value for lr 
@ ==================================


.THUMB_FUNC			@ we are about to declare a thumb function
					@ c equivalent: void edgeBounceFunc(int px, int * vx, int xmin, int xmax);
					
edgeBounceFunc:

push { r4-r7, lr }	@ push r4-r7 and link register onto stack. Your function might use these
					@ registers, so we need to preserve the values just in case!
					@ we don't need to worry about r0-r3 as it is assumed they will be regularly messed up anyway

	
@ the magic happens here!
	
	ldr r4, [r1]
	cmp r2, r0 			@CPSR Flags. Check px and xmin
	bpl flipvel			@if px<xmin flip velocity. if px>= xmin then no need to flip
	cmp r0, r3			@Check px and xmax
	bpl flipvel			@if px>xmax flip velocity. if xmax>=px then no need to flip
	end2:
	str r4, [r1]
	
	
pop { r4-r7 }		@ pop first 4 values from stack back into r4-r7, and also
pop { r3 }			@ pop the next value from stack (stored value for lr) into some unused register, e.g. r3 -
					@ we cannot overwrite lr so we have to do it via a normal register
bx r3				@ "branch and exchange" (return) back to C, using the previous value for lr stored in r3
	
	flipvel:		@store value in a temporary register to perform double substraction
	ldr r5, [r1]
	sub r4, r4, r5
	sub r4, r4, r5
	bal end2
@ ==================================


.THUMB_FUNC			@ we are about to declare a thumb function
					@ c equivalent: void clampFunc(int * val, int min, int max);
					
clampFunc:
@ the magic happens here!
	
	ldr r3, [r0]
	cmp r3, r1 			@CPSR Flags. Check val and min
	bmi clampmin		@if val<min then set to min
	cmp r2, r3			@Check val and max
	bmi clampmax		@if val>max then set to max
	end3:
	
bx lr				@ "branch and exchange" (return) back to C, using the previous value for lr
	
	clampmin:		@Set to minimum value
	str r1, [r0]
	bal end3
	clampmax:		@Set to max value
	str r2, [r0]
	bal end3
@ ==================================