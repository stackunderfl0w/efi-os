; intel Syntax, nasm
extern _ap_start

bits 16
section .smp

align 4096

core_wakeup:
    ; cli                     ; Disable interrupts, we want to be left alone

    ; xor ax, ax
    ; mov ds, ax              ; Set DS-register to 0 - used by lgdt

    ; lgdt [gdt_desc]         ; Load the GDT descriptor

    ; mov eax, cr0            ; Copy the contents of CR0 into EAX
    ; or eax, 1               ; Set bit 0
    ; mov cr0, eax            ; Copy the contents of EAX into CR0

    ; jmp 08h:smp_protected_entry
GLOBAL core_wakeup
gdt:                    ; Address for the GDT
gdt_null:               ; Null Segment
        dd 0
        dd 0

gdt_code:               ; Code segment, read/execute, nonconforming
        dw 0xFFFF
        dw 0
        db 0
        db 10011010b
        db 11001111b
        db 0

gdt_data:               ; Data segment, read/write, expand down
        dw 0xFFFF
        dw 0
        db 0
        db 10010010b
        db 11001111b
        db 0

gdt_end:                ; Used to calculate the size of the GDT

gdt_desc:                       ; The GDT descriptor
        dw gdt_end - gdt - 1    ; Limit (size)
        dd gdt                  ; Address of the GDT

bits 32
section .smp.protected
extern KERNEL_PL4
extern kprintf
smp_protected_entry:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax

    jmp _ap_start

_ap_start:
    ; load P4 to cr3 register (cpu uses this to access the P4 table)
    ;mov eax, p4_table
    mov eax, KERNEL_PL4
    mov cr3, eax

    ; enable PAE-flag in cr4 (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; set the long mode bit in the EFER MSR (model specific register)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging in the cr0 register
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; JMP to long
    lgdt [gdt64.pointer]

    ;jmp gdt64.code:_ap_long_mode_start

.loop:
    hlt
    jmp _ap_start.loop
BITS 64
fmt:    db "from asm %x", 10, 0          ; The kprintf format, "\n",'0'
extern kprintf

loup:
    mov rdi,fmt
    mov rsi,[rsp]
    xor rax,rax     ; or can be  xor  rax,rax
    call    kprintf      ; Call C function

    jmp loup


section .rodata.init
gdt64:
    dq 0 ; zero entry
.code: equ $ - gdt64 ; new
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; code segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64