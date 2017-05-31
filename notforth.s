	.file	"notforth.c"
	.section	.text.unlikely,"ax",@progbits
.LCOLDB0:
	.text
.LHOTB0:
	.p2align 4,,15
	.globl	word_bye
	.type	word_bye, @function
word_bye:
.LFB62:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	xorl	%edi, %edi
	call	exit@PLT
	.cfi_endproc
.LFE62:
	.size	word_bye, .-word_bye
	.section	.text.unlikely
.LCOLDE0:
	.text
.LHOTE0:
	.section	.text.unlikely
.LCOLDB1:
	.text
.LHOTB1:
	.p2align 4,,15
	.globl	execute_word
	.type	execute_word, @function
execute_word:
.LFB54:
	.cfi_startproc
	rep ret
	.cfi_endproc
.LFE54:
	.size	execute_word, .-execute_word
	.section	.text.unlikely
.LCOLDE1:
	.text
.LHOTE1:
	.section	.text.unlikely
.LCOLDB2:
	.text
.LHOTB2:
	.p2align 4,,15
	.globl	pop
	.type	pop, @function
pop:
.LFB55:
	.cfi_startproc
	movq	SP@GOTPCREL(%rip), %rdx
	movslq	(%rdx), %rax
	leal	-1(%rax), %ecx
	movl	%ecx, (%rdx)
	movq	STACK@GOTPCREL(%rip), %rdx
	movl	(%rdx,%rax,4), %eax
	ret
	.cfi_endproc
.LFE55:
	.size	pop, .-pop
	.section	.text.unlikely
.LCOLDE2:
	.text
.LHOTE2:
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC3:
	.string	"%d\n"
	.section	.text.unlikely
.LCOLDB4:
	.text
.LHOTB4:
	.p2align 4,,15
	.globl	word_printnum
	.type	word_printnum, @function
word_printnum:
.LFB63:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	xorl	%eax, %eax
	call	pop@PLT
	leaq	.LC3(%rip), %rsi
	movl	%eax, %edx
	movl	$1, %edi
	xorl	%eax, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	jmp	__printf_chk@PLT
	.cfi_endproc
.LFE63:
	.size	word_printnum, .-word_printnum
	.section	.text.unlikely
.LCOLDE4:
	.text
.LHOTE4:
	.section	.text.unlikely
.LCOLDB5:
	.text
.LHOTB5:
	.p2align 4,,15
	.globl	push
	.type	push, @function
push:
.LFB56:
	.cfi_startproc
	movq	SP@GOTPCREL(%rip), %rdx
	movl	(%rdx), %eax
	addl	$1, %eax
	movl	%eax, (%rdx)
	movq	STACK@GOTPCREL(%rip), %rdx
	cltq
	movl	%edi, (%rdx,%rax,4)
	ret
	.cfi_endproc
.LFE56:
	.size	push, .-push
	.section	.text.unlikely
.LCOLDE5:
	.text
.LHOTE5:
	.section	.text.unlikely
.LCOLDB6:
	.text
.LHOTB6:
	.p2align 4,,15
	.globl	word_add
	.type	word_add, @function
word_add:
.LFB64:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	xorl	%eax, %eax
	call	pop@PLT
	movl	%eax, %ebx
	xorl	%eax, %eax
	call	pop@PLT
	leal	(%rbx,%rax), %edi
	popq	%rbx
	.cfi_def_cfa_offset 8
	jmp	push@PLT
	.cfi_endproc
.LFE64:
	.size	word_add, .-word_add
	.section	.text.unlikely
.LCOLDE6:
	.text
.LHOTE6:
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC7:
	.string	"Word not found in dictionary: %s\n"
	.section	.text.unlikely
.LCOLDB8:
	.text
.LHOTB8:
	.p2align 4,,15
	.globl	find_and_execute_word
	.type	find_and_execute_word, @function
find_and_execute_word:
.LFB57:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	.cfi_offset 3, -24
	movq	%rdi, %rbp
	subq	$8, %rsp
	.cfi_def_cfa_offset 32
	movq	DICTIONARY@GOTPCREL(%rip), %rax
	movq	(%rax), %rbx
	testq	%rbx, %rbx
	jne	.L13
	jmp	.L11
	.p2align 4,,10
	.p2align 3
.L12:
	movq	(%rbx), %rbx
	testq	%rbx, %rbx
	je	.L11
.L13:
	leaq	17(%rbx), %rdi
	movq	%rbp, %rsi
	call	strcmp@PLT
	testl	%eax, %eax
	jne	.L12
	movq	8(%rbx), %rax
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	movq	%rbx, %rdi
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	jmp	*%rax
	.p2align 4,,10
	.p2align 3
.L11:
	.cfi_restore_state
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	movq	%rbp, %rdx
	leaq	.LC7(%rip), %rsi
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	movl	$1, %edi
	xorl	%eax, %eax
	jmp	__printf_chk@PLT
	.cfi_endproc
.LFE57:
	.size	find_and_execute_word, .-find_and_execute_word
	.section	.text.unlikely
.LCOLDE8:
	.text
.LHOTE8:
	.section	.text.unlikely
.LCOLDB9:
	.text
.LHOTB9:
	.p2align 4,,15
	.globl	eat_line
	.type	eat_line, @function
eat_line:
.LFB58:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	stdin@GOTPCREL(%rip), %rbx
	.p2align 4,,10
	.p2align 3
.L21:
	movq	(%rbx), %rdi
	call	_IO_getc@PLT
	cmpl	$10, %eax
	jne	.L21
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE58:
	.size	eat_line, .-eat_line
	.section	.text.unlikely
.LCOLDE9:
	.text
.LHOTE9:
	.section	.rodata.str1.1
.LC10:
	.string	"error! name too long"
	.section	.text.unlikely
.LCOLDB11:
	.text
.LHOTB11:
	.p2align 4,,15
	.globl	read_word
	.type	read_word, @function
read_word:
.LFB59:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movsbl	%dil, %ebx
	subq	$72, %rsp
	.cfi_def_cfa_offset 128
	movq	%fs:40, %rax
	movq	%rax, 56(%rsp)
	xorl	%eax, %eax
	cmpb	$92, %dil
	je	.L43
	cmpb	$35, %dil
	movl	%edi, %ebp
	je	.L42
	call	__ctype_b_loc@PLT
	movq	%rax, %r15
	movsbq	%bl, %rax
	movq	(%r15), %rdx
	movzwl	(%rdx,%rax,2), %eax
	testb	$8, %ah
	jne	.L30
	andw	$24576, %ax
	cmpw	$16384, %ax
	jne	.L37
	leaq	16(%rsp), %r12
	movb	%bpl, 16(%rsp)
	movq	stdin@GOTPCREL(%rip), %r13
	movl	$1, %ebp
	leaq	1(%r12), %r14
	jmp	.L32
	.p2align 4,,10
	.p2align 3
.L35:
	addl	$1, %ebp
	movb	%al, (%r14)
	addq	$1, %r14
	cmpl	$32, %ebp
	je	.L44
.L32:
	movq	0(%r13), %rdi
	call	_IO_getc@PLT
	movq	(%r15), %rdx
	movsbq	%al, %rcx
	movq	%rcx, %rbx
	movzwl	(%rdx,%rcx,2), %edx
	andw	$24576, %dx
	cmpw	$16384, %dx
	je	.L35
.L31:
	movslq	%ebp, %rbp
	movq	%r12, %rdi
	movb	$0, 16(%rsp,%rbp)
	call	find_and_execute_word@PLT
	movl	%ebx, %eax
	jmp	.L28
	.p2align 4,,10
	.p2align 3
.L30:
	movq	stdin@GOTPCREL(%rip), %r13
	xorl	%ebp, %ebp
	.p2align 4,,10
	.p2align 3
.L33:
	movq	0(%r13), %rdi
	leal	0(%rbp,%rbp,4), %eax
	leal	-48(%rbx,%rax,2), %ebp
	call	_IO_getc@PLT
	movq	(%r15), %rdx
	movsbl	%al, %ebx
	movsbq	%bl, %rcx
	testb	$8, 1(%rdx,%rcx,2)
	jne	.L33
	movl	%ebp, %edi
	movl	%eax, 12(%rsp)
	call	push@PLT
	movl	12(%rsp), %eax
.L28:
	movq	56(%rsp), %rsi
	xorq	%fs:40, %rsi
	jne	.L45
	addq	$72, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L44:
	.cfi_restore_state
	leaq	.LC10(%rip), %rdi
	call	puts@PLT
.L42:
	xorl	%eax, %eax
	call	eat_line@PLT
	movl	$10, %eax
	jmp	.L28
	.p2align 4,,10
	.p2align 3
.L43:
	call	__ctype_b_loc@PLT
	movq	%rax, %rbp
	jmp	.L26
	.p2align 4,,10
	.p2align 3
.L27:
	movq	stdin@GOTPCREL(%rip), %rax
	movq	(%rax), %rdi
	call	_IO_getc@PLT
	movl	%eax, %ebx
.L26:
	movq	0(%rbp), %rdx
	movsbq	%bl, %rax
	movzwl	(%rdx,%rax,2), %eax
	andw	$24576, %ax
	cmpw	$16384, %ax
	je	.L27
	movl	%ebx, %eax
	jmp	.L28
.L37:
	xorl	%ebp, %ebp
	leaq	16(%rsp), %r12
	jmp	.L31
.L45:
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE59:
	.size	read_word, .-read_word
	.section	.text.unlikely
.LCOLDE11:
	.text
.LHOTE11:
	.section	.text.unlikely
.LCOLDB12:
	.text
.LHOTB12:
	.p2align 4,,15
	.globl	read_line
	.type	read_line, @function
read_line:
.LFB60:
	.cfi_startproc
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	movq	stdin@GOTPCREL(%rip), %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	movq	0(%rbp), %rdi
	call	_IO_getc@PLT
	movl	%eax, %ebx
	call	__ctype_b_loc@PLT
	movq	%rax, %r12
	.p2align 4,,10
	.p2align 3
.L47:
	movq	(%r12), %rdx
	movsbq	%bl, %rax
	testb	$32, 1(%rdx,%rax,2)
	je	.L52
.L49:
	cmpb	$10, %bl
	je	.L46
	movq	0(%rbp), %rdi
	call	_IO_getc@PLT
	movq	(%r12), %rdx
	movl	%eax, %ebx
	movsbq	%bl, %rax
	testb	$32, 1(%rdx,%rax,2)
	jne	.L49
.L52:
	movsbl	%bl, %edi
	call	read_word@PLT
	movl	%eax, %ebx
	jmp	.L47
	.p2align 4,,10
	.p2align 3
.L46:
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE60:
	.size	read_line, .-read_line
	.section	.text.unlikely
.LCOLDE12:
	.text
.LHOTE12:
	.section	.rodata.str1.1
.LC13:
	.string	"Register: %p %s\n"
	.section	.text.unlikely
.LCOLDB14:
	.text
.LHOTB14:
	.p2align 4,,15
	.globl	register_word
	.type	register_word, @function
register_word:
.LFB61:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rsi, %rdx
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	movq	%rsi, %r14
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	leaq	.LC13(%rip), %rsi
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rdi, %rcx
	movq	%rdi, %r12
	xorl	%eax, %eax
	movl	$1, %edi
	call	__printf_chk@PLT
	movq	%r12, %rdi
	call	strlen@PLT
	leaq	9(%rax), %rdi
	movq	%rax, %rbp
	call	malloc@PLT
	movq	DICTIONARY@GOTPCREL(%rip), %r13
	movq	%rax, %rbx
	leaq	1(%rbp), %rdx
	leaq	17(%rbx), %rdi
	movq	%r12, %rsi
	movq	%r14, 8(%rbx)
	movb	$0, 16(%rbx)
	movq	0(%r13), %rax
	movq	%rax, (%rbx)
	call	memcpy@PLT
	movq	%rbx, 0(%r13)
	popq	%rbx
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE61:
	.size	register_word, .-register_word
	.section	.text.unlikely
.LCOLDE14:
	.text
.LHOTE14:
	.section	.rodata.str1.1
.LC15:
	.string	"Pointer size: %lu\n"
.LC16:
	.string	"Word size: %lu\n"
	.section	.rodata.str1.8
	.align 8
.LC17:
	.string	"Offsets: 0x%lx 0x%lx 0x%lx 0x%lx\n"
	.section	.rodata.str1.1
.LC18:
	.string	"bye"
.LC19:
	.string	"."
.LC20:
	.string	"+"
.LC21:
	.string	"] "
	.section	.text.unlikely
.LCOLDB22:
	.section	.text.startup,"ax",@progbits
.LHOTB22:
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB65:
	.cfi_startproc
	leaq	.LC15(%rip), %rsi
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$8, %edx
	movl	$1, %edi
	xorl	%eax, %eax
	call	__printf_chk@PLT
	leaq	.LC16(%rip), %rsi
	movl	$24, %edx
	movl	$1, %edi
	xorl	%eax, %eax
	call	__printf_chk@PLT
	leaq	.LC17(%rip), %rsi
	movl	$17, %r9d
	movl	$16, %r8d
	movl	$8, %ecx
	xorl	%edx, %edx
	movl	$1, %edi
	xorl	%eax, %eax
	call	__printf_chk@PLT
	movq	word_bye@GOTPCREL(%rip), %rsi
	leaq	.LC18(%rip), %rdi
	call	register_word@PLT
	movq	word_printnum@GOTPCREL(%rip), %rsi
	leaq	.LC19(%rip), %rdi
	call	register_word@PLT
	movq	word_add@GOTPCREL(%rip), %rsi
	leaq	.LC20(%rip), %rdi
	call	register_word@PLT
	.p2align 4,,10
	.p2align 3
.L56:
	leaq	.LC21(%rip), %rsi
	movl	$1, %edi
	xorl	%eax, %eax
	call	__printf_chk@PLT
	xorl	%eax, %eax
	call	read_line@PLT
	jmp	.L56
	.cfi_endproc
.LFE65:
	.size	main, .-main
	.section	.text.unlikely
.LCOLDE22:
	.section	.text.startup
.LHOTE22:
	.globl	SP
	.bss
	.align 4
	.type	SP, @object
	.size	SP, 4
SP:
	.zero	4
	.comm	STACK,512,32
	.globl	DICTIONARY
	.align 8
	.type	DICTIONARY, @object
	.size	DICTIONARY, 8
DICTIONARY:
	.zero	8
	.ident	"GCC: (GNU) 5.4.0"
	.section	.note.GNU-stack,"",@progbits
