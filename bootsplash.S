	.code16
	.globl start
start:
	movb %dl,		(drive) # save drive no

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

	movb $0,		%ah # set video mode
	movb $0x13,		%al # 16-bit VGA
	int $0x10

	# movw $0xA000,		%ax # Address 0xA0000 using data segment register
	# movw %ax,		%ds # In i8086 you cannot move imm directly to a *s register. (movw %0xA000, %ds)
	# movb $0xf,		%al # A white pixel
	# movw $0,		%di # Offset from data segment register
	# movb %al,		%ds:(%di) # addr set = %ds*16 + %di

	movb $0x42,		%ah # load image from disk
	movb (drive),		%dl # load saved disk no
	xor %si,		%si # %ds cannot have literals moved in
	mov %si,		%ds # segment
	lea dap,		%si # offset
	int $0x13

stop:
	jmp stop

hello:
	.string "Welcome to xv6@UIC. Press any key to continue"
	hello_len = . - hello

drive:
	.byte 0x0	# save drive

dap:
	.byte 0x10	# size
	.byte 0		# unused
	.word 127	# sectors to read (63kib or 127 512-byte sectors for cover.raw)
	.word 0		# offset to dump sectors to
	.word 0xA000 	# segment to dump sectors to
	.quad 1		# sector to start reading from (1 for after mbr)

	.org 0x1fe
	.word 0xAA55
