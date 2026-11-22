
# Note: a32.out/a64.out depend only on call_shellcode.c (the command the
# shellcode runs lives in codefile_32, which is loaded at run time). The sandbox
# pre-builds a32.out and installs it setuid-root for the Task 1 flag keeper;
# keeping real prerequisites here means a plain `make` is a no-op when you have
# only edited shellcode_32.py, so it will NOT strip that setuid bit.

all: a32.out a64.out

a32.out: call_shellcode.c
	gcc -m32 -z execstack -o a32.out call_shellcode.c

a64.out: call_shellcode.c
	gcc -z execstack -o a64.out call_shellcode.c

clean:
	rm -f a32.out a64.out codefile_32 codefile_64
