
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
	jne   nAUJ
	jz   nAVS
	addl    %ecx,  %ecx
	notl   %eax
	orl    %edx,  %ecx
	incl   %ecx
nAUJ: 	incl   %eax
	decl   %ecx
	incl   %ecx
	xorl     $0x89c59a5d,   %ecx 
	cmpl    %ecx,  %ecx
	notl   %eax
	decl   %edx
nAUQ: 	jnz   nAVR
	je   nAVI
	subl     $0xcc30c6b4,   %ebx 
	orl    %ecx,  %eax
	xorl     $0x1749db91,   %ecx 
	incl   %ebx
	cmpl    %ebx,  %ecx
	jo   nAUZ
	subl    %edx,  %edx
nAUZ: 	addl    %ebx,  %eax
	movl     $0x076dca6f,   %ebx 
	andl     $0x1a8e9eeb,   %ecx 
nAU4: 	incl   %ebx
	subl    %eax,  %eax
	cmpl     $0x70062d10,   %ecx 
	addl    %edx,  %eax
	decl   %ebx
nAVB: 	subl    %ecx,  %ecx
	xorl    %ebx,  %edx
	addl    %edx,  %ebx
	je   nAVT
	andl     $0x0d1e532b,   %ebx 
	movl     $0x3288cb5b,   %ecx 
	addl     $0xebe6d8d1,   %edx 
nAVI: 	addl    %ebx,  %ebx
	addl     $0x001672c3,   %edx 
nAVK: 	andl    %ebx,  %edx
	jnz   nAUQ
	jno   nAVP
	cmpl     $0xd46fd6e9,   %edx 
	jo   nAU4
nAVP: 	decl   %edx
	xorl     $0x3d74d30b,   %edx 
nAVR: 	jc   nAVB
nAVS: 	addl    %eax,  %ebx
nAVT: 	andl    %eax,  %ecx
	movl     $0xfaab93aa,   %edx 
	jno   nAVK
	incl   %eax
	incl   %edx

	popl %edx
	popl %ecx
	popl %ebx
	ret
	.cfi_endproc
.LFE0:
	.size	evolved_function, .-evolved_function
	.ident	"MicroGP"
	.section	.note.GNU-stack,"",@progbits
