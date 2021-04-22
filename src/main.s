	.file	"main.c"
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__SREG__ = 0x3f
__RAMPZ__ = 0x3b
__tmp_reg__ = 0
__zero_reg__ = 1
	.stabs	"/build/",100,0,2,.Ltext0
	.stabs	"main.c",100,0,2,.Ltext0
	.text
.Ltext0:
	.stabs	"gcc2_compiled.",60,0,0,0
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	""
.LC1:
	.string	"disabling MT"
.LC2:
	.string	"Starting!"
.LC3:
	.string	"Starting2!"
.LC4:
	.string	"Starting3!"
.LC5:
	.string	"index: %d\n"
.LC6:
	.string	"hi!"
.LC7:
	.string	"Setting SP to %X\n"
.LC8:
	.string	"UNREACHABLE END OF MAIN"
.LC9:
	.string	"FATAL ERROR: %s\n"
	.section	.text.startup,"ax",@progbits
	.stabs	"main:F(0,1)=r(0,1);-32768;32767;",36,0,17,main
	.stabs	"uint8_t:t(0,2)=(0,3)=@s8;r(0,3);0;255;",128,0,126,0
	.stabs	"Task_Status:T(0,4)=@s8;eSLEEPING:0,READY:1,DONE:2,RUNNING:3,;",128,0,0,0
	.stabs	"int:t(0,1)",128,0,0,0
	.stabs	"unsigned char:t(0,3)",128,0,0,0
.global	main
	.type	main, @function
main:
	.stabn	68,0,17,.LM0-.LFBB1
.LM0:
.LFBB1:
	in r28,__SP_L__
	in r29,__SP_H__
	sbiw r28,14
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
/* prologue: function */
/* frame size = 14 */
/* stack size = 14 */
.L__stack_usage = 14
	.stabn	68,0,19,.LM1-.LFBB1
.LM1:
	call uart_init
	.stabn	68,0,20,.LM2-.LFBB1
.LM2:
	movw r30,r28
	adiw r30,1
	ldi r24,lo8(14)
	movw r26,r30
	0:
	st X+,__zero_reg__
	dec r24
	brne 0b
	ldi r24,lo8(3)
	mov r15,r24
	std Y+4,r15
	ldi r24,lo8(gs(uart_putchar))
	ldi r25,hi8(gs(uart_putchar))
	std Y+10,r25
	std Y+9,r24
	ldi r24,lo8(gs(uart_getchar))
	ldi r25,hi8(gs(uart_getchar))
	std Y+12,r25
	std Y+11,r24
	.stabn	68,0,22,.LM3-.LFBB1
.LM3:
	sts __iob+1,r31
	sts __iob,r30
	.stabn	68,0,22,.LM4-.LFBB1
.LM4:
	sts __iob+2+1,r31
	sts __iob+2,r30
	.stabn	68,0,26,.LM5-.LFBB1
.LM5:
	ldi r24,lo8(.LC0)
	ldi r25,hi8(.LC0)
	call puts
	.stabn	68,0,28,.LM6-.LFBB1
.LM6:
	call init_timer_interupts
	.stabn	68,0,31,.LM7-.LFBB1
.LM7:
	lds r24,111
	andi r24,lo8(-3)
	sts 111,r24
	ldi r24,lo8(.LC1)
	ldi r25,hi8(.LC1)
	call puts
	.stabn	68,0,33,.LM8-.LFBB1
.LM8:
	ldi r24,lo8(.LC2)
	ldi r25,hi8(.LC2)
	call puts
	.stabn	68,0,34,.LM9-.LFBB1
.LM9:
	ldi r24,lo8(.LC3)
	ldi r25,hi8(.LC3)
	call puts
	.stabn	68,0,35,.LM10-.LFBB1
.LM10:
	ldi r24,lo8(.LC4)
	ldi r25,hi8(.LC4)
	call puts
	.stabn	68,0,37,.LM11-.LFBB1
.LM11:
	ldi r24,lo8(simplos_schedule+2)
	ldi r25,hi8(simplos_schedule+2)
	call init_empty_queue
	.stabn	68,0,39,.LM12-.LFBB1
.LM12:
	ldi r22,lo8(simplos_schedule+2)
	ldi r23,hi8(simplos_schedule+2)
	ldi r24,0
	call add_task_to_queue
	.stabn	68,0,41,.LM13-.LFBB1
.LM13:
	ldi r27,lo8(10)
	mul r24,r27
	movw r16,r0
	clr __zero_reg__
	subi r16,lo8(-(simplos_schedule))
	sbci r17,hi8(-(simplos_schedule))
	movw r30,r16
	std Z+7,r15
	.stabn	68,0,42,.LM14-.LFBB1
.LM14:
	std Z+2,__zero_reg__
	.stabn	68,0,43,.LM15-.LFBB1
.LM15:
	sts simplos_schedule+52,r24
	.stabn	68,0,45,.LM16-.LFBB1
.LM16:
	push __zero_reg__
	push r24
	ldi r24,lo8(.LC5)
	ldi r25,hi8(.LC5)
	push r25
	push r24
	call printf
	.stabn	68,0,51,.LM17-.LFBB1
.LM17:
	lds r30,task_sp
	lds r31,task_sp+1
	.stabn	68,0,51,.LM18-.LFBB1
.LM18:
	movw r26,r16
	adiw r26,3
	ld r24,X+
	ld r25,X
	.stabn	68,0,51,.LM19-.LFBB1
.LM19:
	std Z+1,r25
	st Z,r24
	.stabn	68,0,53,.LM20-.LFBB1
.LM20:
	ldi r24,lo8(.LC6)
	ldi r25,hi8(.LC6)
	call puts
	.stabn	68,0,55,.LM21-.LFBB1
.LM21:
	lds r30,task_sp
	lds r31,task_sp+1
	ld r24,Z
	ldd r25,Z+1
	push r25
	push r24
	ldi r24,lo8(.LC7)
	ldi r25,hi8(.LC7)
	push r25
	push r24
	call printf
/* #APP */
 ;  55 "main.c" 1
	lds  r26, task_sp               
	lds  r27, task_sp + 1           
	ld   r28, x+                
	out  __SP_L__, r28          
	ld   r29, x+                
	out  __SP_H__, r29          
	
 ;  0 "" 2
	.stabn	68,0,57,.LM22-.LFBB1
.LM22:
 ;  57 "main.c" 1
	rcall 0
 ;  0 "" 2
	.stabn	68,0,58,.LM23-.LFBB1
.LM23:
/* #NOAPP */
	ldi r24,lo8(simplos_schedule)
	ldi r25,hi8(simplos_schedule)
	call idle_fn
	.stabn	68,0,59,.LM24-.LFBB1
.LM24:
/* #APP */
 ;  59 "main.c" 1
	cli
 ;  0 "" 2
/* #NOAPP */
	ldi r24,lo8(.LC8)
	ldi r25,hi8(.LC8)
	push r25
	push r24
	ldi r24,lo8(.LC9)
	ldi r25,hi8(.LC9)
	push r25
	push r24
	call printf
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r29
	out __SREG__,__tmp_reg__
	out __SP_L__,r28
.L2:
	rjmp .L2
	.size	main, .-main
	.stabs	"uart_file:(0,5)=(0,6)=xs__file:",128,0,20,1
	.stabs	"__file:T(0,6)=s14buf:(0,7)=*(0,8)=r(0,8);0;127;,0,16;unget:(0,3),16,8;flags:(0,2),24,8;size:(0,1),32,16;len:(0,1),48,16;put:(0,9)=*(0,10)=f(0,1),64,16;get:(0,11)=*(0,12)=f(0,1),80,16;udata:(0,13)=*(0,14)=(0,14),96,16;;",128,0,0,0
	.stabs	"FILE:t(0,5)",128,0,277,0
	.stabs	"char:t(0,8)",128,0,0,0
	.stabs	"void:t(0,14)",128,0,0,0
	.stabs	"index:r(0,15)=k(0,2)",64,0,39,24
	.stabn	192,0,0,.LFBB1-.LFBB1
	.stabn	224,0,0,.Lscope1-.LFBB1
.Lscope1:
	.stabs	"",36,0,0,.Lscope1-.LFBB1
	.section	.rodata.str1.1
.LC10:
	.string	"Context switch!"
.LC11:
	.string	"Restoring context to task: %d\n"
.LC12:
	.string	"End of context switch"
	.text
	.stabs	"__vector_17:F(0,14)",36,0,69,__vector_17
.global	__vector_17
	.type	__vector_17, @function
__vector_17:
	.stabn	68,0,69,.LM25-.LFBB2
.LM25:
.LFBB2:
/* prologue: naked */
/* frame size = 0 */
/* stack size = 0 */
.L__stack_usage = 0
.LBB12:
.LBB13:
	.stabs	"simplos.h",132,0,0,.Ltext1
.Ltext1:
	.stabn	68,0,215,.LM26-.LFBB2
.LM26:
	ldi r24,lo8(.LC10)
	ldi r25,hi8(.LC10)
	call puts
.LBB14:
.LBB15:
	.stabn	68,0,192,.LM27-.LFBB2
.LM27:
	sbi 0x5,5
	.stabn	68,0,193,.LM28-.LFBB2
.LM28:
/* #APP */
 ;  193 "simplos.h" 1
	push  r0                    
	in    r0, __SREG__          
	cli                         
	push  r0                    
	push  r1                    
	clr   r1                    
	push  r2                    
	push  r3                    
	push  r4                    
	push  r5                    
	push  r6                    
	push  r7                    
	push  r8                    
	push  r9                    
	push  r10                   
	push  r11                   
	push  r12                    
	push  r13                    
	push  r14                    
	push  r15                    
	push  r16                    
	push  r17                    
	push  r18                    
	push  r19                    
	push  r20                    
	push  r21                    
	push  r22                    
	push  r23                    
	push  r24                    
	push  r25                    
	push  r26                    
	push  r27                    
	push  r28                    
	push  r29                    
	push  r30                    
	push  r31                    
	
 ;  0 "" 2
	.stabn	68,0,194,.LM29-.LFBB2
.LM29:
 ;  194 "simplos.h" 1
	lds   r26, task_sp               
	lds   r27, task_sp + 1           
	in    r0, __SP_L__           
	st    x+, r0                 
	in    r0, __SP_H__           
	st    x+, r0                 
	
 ;  0 "" 2
	.stabn	68,0,197,.LM30-.LFBB2
.LM30:
/* #NOAPP */
	lds r30,simplos_schedule+52
	.stabn	68,0,198,.LM31-.LFBB2
.LM31:
	lds r26,task_sp
	lds r27,task_sp+1
	ld r24,X+
	ld r25,X
	.stabn	68,0,198,.LM32-.LFBB2
.LM32:
	ldi r18,lo8(10)
	mul r30,r18
	movw r30,r0
	clr __zero_reg__
	subi r30,lo8(-(simplos_schedule))
	sbci r31,hi8(-(simplos_schedule))
	std Z+4,r25
	std Z+3,r24
	.stabn	68,0,199,.LM33-.LFBB2
.LM33:
	ldi r24,lo8(1)
	std Z+7,r24
.LBE15:
.LBE14:
	.stabn	68,0,218,.LM34-.LFBB2
.LM34:
	ldi r24,lo8(simplos_schedule)
	ldi r25,hi8(simplos_schedule)
	call select_next_task
	movw r28,r24
.LBB16:
.LBB17:
	.stabn	68,0,204,.LM35-.LFBB2
.LM35:
	ldd r24,Y+3
	push __zero_reg__
	push r24
	ldi r24,lo8(.LC11)
	ldi r25,hi8(.LC11)
	push r25
	push r24
	call printf
	.stabn	68,0,205,.LM36-.LFBB2
.LM36:
	cbi 0x5,5
	.stabn	68,0,207,.LM37-.LFBB2
.LM37:
	ldi r24,lo8(3)
	std Y+5,r24
	.stabn	68,0,208,.LM38-.LFBB2
.LM38:
	lds r30,task_sp
	lds r31,task_sp+1
	.stabn	68,0,208,.LM39-.LFBB2
.LM39:
	ldd r24,Y+1
	ldd r25,Y+2
	.stabn	68,0,208,.LM40-.LFBB2
.LM40:
	std Z+1,r25
	st Z,r24
	.stabn	68,0,209,.LM41-.LFBB2
.LM41:
	ld r24,Z
	ldd r25,Z+1
	push r25
	push r24
	ldi r24,lo8(.LC7)
	ldi r25,hi8(.LC7)
	push r25
	push r24
	call printf
/* #APP */
 ;  209 "simplos.h" 1
	lds  r26, task_sp               
	lds  r27, task_sp + 1           
	ld   r28, x+                
	out  __SP_L__, r28          
	ld   r29, x+                
	out  __SP_H__, r29          
	
 ;  0 "" 2
	.stabn	68,0,210,.LM42-.LFBB2
.LM42:
 ;  210 "simplos.h" 1
	pop  r31                    
	pop  r30                    
	pop  r29                    
	pop  r28                    
	pop  r27                    
	pop  r26                    
	pop  r25                    
	pop  r24                    
	pop  r23                    
	pop  r22                    
	pop  r21                    
	pop  r20                    
	pop  r19                    
	pop  r18                    
	pop  r17                    
	pop  r16                    
	pop  r15                    
	pop  r14                    
	pop  r13                    
	pop  r12                    
	pop  r11                    
	pop  r10                    
	pop  r9                     
	pop  r8                     
	pop  r7                     
	pop  r6                     
	pop  r5                     
	pop  r4                     
	pop  r3                     
	pop  r2                     
	pop  r1                     
	pop  r0                     
	out  __SREG__, r0           
	pop  r0                     
	
 ;  0 "" 2
/* #NOAPP */
.LBE17:
.LBE16:
	.stabn	68,0,221,.LM43-.LFBB2
.LM43:
	ldi r24,lo8(.LC12)
	ldi r25,hi8(.LC12)
	call puts
.LBE13:
.LBE12:
	.stabs	"main.c",132,0,0,.Ltext2
.Ltext2:
	.stabn	68,0,69,.LM44-.LFBB2
.LM44:
	in r24,__SP_L__
	in r25,__SP_H__
	adiw r24,8
	in __tmp_reg__,__SREG__
	cli
	out __SP_H__,r25
	out __SREG__,__tmp_reg__
	out __SP_L__,r24
/* epilogue start */
	.size	__vector_17, .-__vector_17
	.stabs	"new_task:r(0,16)=*(0,17)=(0,18)=s10empty:(0,19)=@s8;-16;,0,8;task_sp_adr:(0,20)=(0,21)=r(0,21);0;0177777;,8,16;task_memory_block:(0,2),24,8;priority:(0,2),32,8;status:(0,4),40,8;sleep_until:(0,22)=(0,23)=@s32;r(0,23);0;037777777777;,48,32;;",64,0,218,28
	.stabs	"Simplos_Task:t(0,17)",128,0,26,0
	.stabs	"_Bool:t(0,19)",128,0,0,0
	.stabs	"uint16_t:t(0,20)",128,0,128,0
	.stabs	"unsigned int:t(0,21)",128,0,0,0
	.stabs	"uint32_t:t(0,22)",128,0,130,0
	.stabs	"long unsigned int:t(0,23)",128,0,0,0
	.stabn	192,0,0,.LBB13-.LFBB2
	.stabn	224,0,0,.LBE13-.LFBB2
.Lscope2:
	.stabs	"",36,0,0,.Lscope2-.LFBB2
.global	task_sp
	.data
	.type	task_sp, @object
	.size	task_sp, 2
task_sp:
	.word	_task_sp_adr
	.stabs	"task_sp:G(0,24)=*(0,25)=B(0,20)",32,0,15,0
.global	_task_sp_adr
	.section .bss
	.type	_task_sp_adr, @object
	.size	_task_sp_adr, 2
_task_sp_adr:
	.zero	2
	.stabs	"_task_sp_adr:G(0,25)",32,0,14,0
.global	simplos_schedule
	.type	simplos_schedule, @object
	.size	simplos_schedule, 54
simplos_schedule:
	.zero	54
	.stabs	"simplos_schedule:G(0,26)=B(0,27)=(0,28)=s54force_prev:(0,19),0,8;prev_task:(0,2),8,8;queue:(0,29)=(0,30)=s51task_queue:(0,31)=ar(0,32)=r(0,32);0;0177777;;0;4;(0,17),0,400;curr_task_index:(0,2),400,8;;,16,408;enabled:(0,19),424,8;;",32,0,9,0
	.stabs	"Scheduler:t(0,27)",128,0,39,0
	.text
	.stabs	"",100,0,0,.Letext0
.Letext0:
	.ident	"GCC: (Fedora 10.2.0-1.fc33) 10.2.0"
.global __do_copy_data
.global __do_clear_bss
