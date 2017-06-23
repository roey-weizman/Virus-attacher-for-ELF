all: skeleton
 
skeleton: skeleton.o
	ld -melf_i386 -g  skeleton.o -o skeleton
skeleton.o: skeleton.s
	nasm -f elf -g skeleton.s -o skeleton.o
clean:
	rm -f *.o  skeleton