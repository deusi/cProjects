	.file	"thermo_update.c"
	.text
	.globl	set_temp_from_ports
	.type	set_temp_from_ports, @function
set_temp_from_ports:
.LFB0:
	.cfi_startproc
	movzwl	THERMO_SENSOR_PORT(%rip), %edx
	movzwl	%dx, %eax
	cmpl	$64000, %eax
	jg	.L5
	movl	%edx, %ecx
	andl	$63, %ecx
	shrl	$6, %eax
	movl	%eax, %edx
	cmpw	$31, %cx
	jle	.L3
	leal	1(%rax), %edx
.L3:
	subl	$500, %edx
	cmpb	$0, THERMO_STATUS_PORT(%rip)
	je	.L4
	leal	(%rdx,%rdx,8), %ecx
	movl	$1717986919, %edx
	movl	%ecx, %eax
	imull	%edx
	sarl	%edx
	sarl	$31, %ecx
	subl	%ecx, %edx
	addl	$320, %edx
.L4:
	movw	%dx, (%rdi)
	movzbl	THERMO_STATUS_PORT(%rip), %eax
	movb	%al, 2(%rdi)
	movl	$0, %eax
	ret
.L5:
	movl	$1, %eax
	ret
	.cfi_endproc
.LFE0:
	.size	set_temp_from_ports, .-set_temp_from_ports
	.globl	set_display_from_temp
	.type	set_display_from_temp, @function
set_display_from_temp:
.LFB1:
	.cfi_startproc
	movl	%edi, %eax
	sall	$8, %eax
	sarl	$24, %eax
	cmpb	$1, %al
	jbe	.L21
	movl	$1, %eax
	ret
.L21:
	testb	%al, %al
	jne	.L8
	leal	500(%rdi), %edx
	cmpw	$1000, %dx
	ja	.L17
.L8:
	cmpb	$1, %al
	je	.L22
.L9:
	movw	$63, -32(%rsp)
	movw	$6, -30(%rsp)
	movw	$91, -28(%rsp)
	movw	$79, -26(%rsp)
	movw	$102, -24(%rsp)
	movw	$109, -22(%rsp)
	movw	$125, -20(%rsp)
	movw	$7, -18(%rsp)
	movw	$127, -16(%rsp)
	movw	$111, -14(%rsp)
	movw	$0, -12(%rsp)
	movw	$64, -10(%rsp)
	movl	%edi, %ecx
	testw	%di, %di
	js	.L23
	movl	$0, %edi
.L10:
	movswl	%cx, %edx
	imull	$-31981, %edx, %edx
	shrl	$16, %edx
	addl	%ecx, %edx
	sarw	$9, %dx
	movl	%ecx, %r8d
	sarw	$15, %r8w
	subl	%r8d, %edx
	imulw	$1000, %dx, %r8w
	subl	%r8d, %ecx
	movl	%ecx, %r9d
	movswl	%cx, %r8d
	imull	$5243, %r8d, %r8d
	sarl	$19, %r8d
	sarw	$15, %cx
	subl	%ecx, %r8d
	leal	0(,%r8,4), %ecx
	addl	%r8d, %ecx
	leal	0(,%rcx,4), %r10d
	addl	%r10d, %ecx
	leal	0(,%rcx,4), %r10d
	subl	%r10d, %r9d
	movl	%r9d, %r10d
	movswl	%r9w, %ecx
	imull	$26215, %ecx, %ecx
	sarl	$18, %ecx
	sarw	$15, %r9w
	subl	%r9d, %ecx
	leal	0(,%rcx,4), %r9d
	addl	%ecx, %r9d
	leal	(%r9,%r9), %r11d
	subl	%r11d, %r10d
	movl	%r10d, %r9d
	testb	%al, %al
	je	.L20
	movl	$536870912, %eax
.L11:
	testw	%dx, %dx
	je	.L12
	movswq	%dx, %rdx
	movswl	-32(%rsp,%rdx,2), %edx
	sall	$21, %edx
	orl	%edx, %eax
.L13:
	testw	%r8w, %r8w
	sete	%dl
	testb	%dl, %dil
	je	.L14
	orl	$1048576, %eax
.L15:
	movswq	%cx, %rcx
	movswl	-32(%rsp,%rcx,2), %edx
	sall	$7, %edx
	orl	%edx, %eax
	movswq	%r9w, %r9
	movswl	-32(%rsp,%r9,2), %edx
	orl	%edx, %eax
	movl	%eax, (%rsi)
	movl	$0, %eax
	ret
.L22:
	leal	580(%rdi), %edx
	cmpw	$1800, %dx
	jbe	.L9
	movl	$1, %eax
	ret
.L23:
	negl	%ecx
	movl	$1, %edi
	jmp	.L10
.L20:
	movl	$268435456, %eax
	jmp	.L11
.L12:
	testw	%r8w, %r8w
	setne	%dl
	testb	%dl, %dil
	je	.L13
	orl	$134217728, %eax
	jmp	.L13
.L14:
	testw	%r8w, %r8w
	je	.L15
	movswq	%r8w, %rdx
	movswl	-32(%rsp,%rdx,2), %edx
	sall	$14, %edx
	orl	%edx, %eax
	jmp	.L15
.L17:
	movl	$1, %eax
	ret
	.cfi_endproc
.LFE1:
	.size	set_display_from_temp, .-set_display_from_temp
	.globl	thermo_update
	.type	thermo_update, @function
thermo_update:
.LFB2:
	.cfi_startproc
	subq	$16, %rsp
	.cfi_def_cfa_offset 24
	leaq	12(%rsp), %rdi
	call	set_temp_from_ports
	testl	%eax, %eax
	je	.L29
	movl	$1, %eax
.L24:
	addq	$16, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L29:
	.cfi_restore_state
	leaq	8(%rsp), %rsi
	movl	12(%rsp), %edi
	call	set_display_from_temp
	testl	%eax, %eax
	jne	.L27
	movl	8(%rsp), %edx
	movl	%edx, THERMO_DISPLAY_PORT(%rip)
	jmp	.L24
.L27:
	movl	$1, %eax
	jmp	.L24
	.cfi_endproc
.LFE2:
	.size	thermo_update, .-thermo_update
	.ident	"GCC: (Debian 6.3.0-18+deb9u1) 6.3.0 20170516"
	.section	.note.GNU-stack,"",@progbits
