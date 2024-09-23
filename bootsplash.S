	.code16
	.globl start
start:
	movb $0x3,		%ah # get cursor pos
	movb $0,		%bh # video page no
	int $0x10

	movb $0x13,		%ah # write string
	movb $0,		%al # write mode
	movb $7,		%bl # foreground color
	movw $hello_len,	%cx # len
	#movb $0, 		%dh # y
	#movb $0, 		%dl # x

	xor %si,		%si # %es cannot have literals moved in
	mov %si,		%es # segment
	lea hello,		%bp # string addr
	int $0x10

	movb $0,		%ah # wait for keypress
	int $0x16

	hlt


hello:
	.string "Welcome to xv6@UIC. Press any key to continue"
	hello_len = . - hello

	.org 0x1fe
	.word 0xAA55
