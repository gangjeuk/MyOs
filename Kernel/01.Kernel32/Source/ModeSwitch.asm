[BITS 32]

global kReadCPUID, kSwitchAndExecute64bitKernel

SECTION .text

kReadCPUID:
    push ebp
    mov ebp, esp
    push eax
    push ebx
    push ecx
    push edx
    push esi
    
    mov eax, dword [ ebp + 8 ]
    cpuid

    mov esi, dword [ebp + 12 ]
    mov dword [ esi ], eax

    mov esi, dword [ ebp + 16 ]
    mov dword [ esi ], ebx

    mov esi, dword [ ebp + 20 ]
    mov dword [ esi ], ecx

    mov esi, dword [ ebp + 24 ]
    mov dword [ esi ], edx

    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    pop ebp
    ret

; IA-32e 모드로 전환하고 64비트 커널을 수행
;   PARAM: 없음
kSwitchAndExecute64bitKernel:
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; CR4 컨트롤 레지스터의 PAE 비트를 1로 설정
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov eax, cr4    ; CR4 컨트롤 레지스터의 값을 EAX 레지스터에 저장
    or eax, 0x20    ; PAE 비트(비트 5)를 1로 설정
    mov cr4, eax    ; PAE 비트가 1로 설정된 값을 CR4 컨트롤 레지스터에 저장
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; CR3 컨트롤 레지스터에 PML4 테이블의 어드레스 및 캐시 활성화
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov eax, 0x100000   ; EAX 레지스터에 PML4 테이블이 존재하는 0x100000(1MB)를 저장
    mov cr3, eax        ; CR3 컨트롤 레지스터에 0x100000(1MB)을 저장
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; IA32_EFER.LME를 1로 설정하여 IA-32e 모드를 활성화
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov ecx, 0xC0000080 ; IA32_EFER MSR 레지스터의 어드레스를 저장
    rdmsr               ; MSR 레지스터를 읽기
    
    or eax, 0x0100      ; EAX 레지스터에 저장된 IA32_EFER MSR의 하위 32비트에서 
                        ; LME 비트(비트 8)을 1로 설정
    wrmsr               ; MSR 레지스터에 쓰기
        
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; CR0 컨트롤 레지스터를 NW 비트(비트 29) = 0, CD 비트(비트 30) = 0, PG 비트(비트 31) = 1로
    ; 설정하여 캐시 기능과 페이징 기능을 활성화
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov eax, cr0            ; EAX 레지스터에 CR0 컨트롤 레지스터를 저장
    or eax, 0xE0000000      ; NW 비트(비트 29), CD 비트(비트 30), PG 비트(비트 31)을 모두 1로 설정
    xor eax, 0x60000000     ; NW 비트(비트 29)와 CD 비트(비트 30)을 XOR하여 0으로 설정
    mov cr0, eax            ; NW 비트 = 0, CD 비트 = 0, PG 비트 = 1로 설정한 값을 다시 
                            ; CR0 컨트롤 레지스터에 저장
    
    jmp 0x08:0x200000   ; CS 세그먼트 셀렉터를 IA-32e 모드용 코드 세그먼트 디스크립터로
                        ; 교체하고 0x200000(2MB) 어드레스로 이동
                            
    ; 여기는 실행되지 않음
    jmp $