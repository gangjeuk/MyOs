start:
/* Hello */
mov     $0x48, %al
outb    %al, $0xf1
mov     $0x65, %al
outb    %al, $0xf1
mov     $0x6c, %al
outb    %al, $0xf1
mov     $0x6c, %al
outb    %al, $0xf1
mov     $0x6f, %al
outb    %al, $0xf1
mov     $0x2c, %al
outb    %al, $0xf1

/* world */
mov     $0x77, %al
outb    %al, $0xf1
mov     $0x6f, %al
outb    %al, $0xf1
mov     $0x72, %al
outb    %al, $0xf1
mov     $0x6c, %al
outb    %al, $0xf1
mov     $0x64, %al
outb    %al, $0xf1

mov     $0x0a, %al
outb    %al, $0xf1

hlt

