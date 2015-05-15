
	.text
	.globl	evolved_function
	.type	evolved_function, @function
evolved_function:
.LFB0:
	.cfi_startproc
	pushl %ebx
	pushl %ecx
	pushl %edx
	movl $0x00000505, %eax
	movl $0x00050500, %ebx
	movl $0x00050500, %ecx
	movl $0x05050000, %edx
	incl   %eax
nAHN: 	cmpl    %eax,  %edx
	jz   nAHT
nAHP: 	cmpl    %edx,  %eax
	orl     $0xde3cfda2,   %eax 
	subl     $0x204ccdab,   %ebx 
	andl     $0x5e0fde52,   %ebx 
nAHT: 	jno   nAIM
	je   nAH4
	jne   nAIV
	jz   nAHP
	andl     $0xf04c89f4,   %ecx 
	cmpl     $0xa23b2224,   %ebx 
	incl   %ecx
	movl    %edx,  %eax
nAH3: 	cmpl     $0xa0145583,   %ecx 
nAH4: 	ja   nAHN
nAH5: 	incl   %ecx
	jz   nAH3
	decl   %ebx
	orl    %ebx,  %eax
	orl     $0x4773f8d5,   %ecx 
	notl   %ecx
	orl     $0xd53cf7c7,   %eax 
	decl   %ecx
	incl   %eax
	notl   %ecx
	movl    %ecx,  %edx
nAII: 	cmpl    %edx,  %ecx
	xorl     $0x5f316f96,   %eax 
	jc   nAH4
	orl     $0xcf22ff02,   %ebx 
nAIM: 	jnc   nAH3
	decl   %ebx
	cmpl    %edx,  %eax
	incl   %edx
	incl   %eax
	subl     $0x248dbec3,   %ebx 
	andl     $0xdee9a111,   %edx 
	notl   %eax
	jne   nAIX
nAIV: 	addl     $0x28f9d419,   %ebx 
	decl   %eax
nAIX: 	ja   nAH5
	cmpl     $0xd7a060f5,   %ebx 
	jnz   nAII
	decl   %ecx

	popl %edx
	popl %ecx
	popl %ebx
	ret
	.cfi_endproc
.LFE0:
	.size	evolved_function, .-evolved_function
	.ident	"MicroGP"
	.section	.note.GNU-stack,"",@progbits
