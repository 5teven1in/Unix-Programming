# Extend the Mini Lib C to Handle Signals

In this homework, you have to extend the mini C library introduced in the class to support signal relevant system calls. You have to implement the following C library functions in Assembly and C using the syntax supported by yasm x86_64 assembler.

1. setjmp: prepare for long jump by saving the current CPU state. In addition, preserve the signal mask of the current process.
2. longjmp: perform the long jump by restoring a saved CPU state. In addition, restore the preserved signal mask.
3. signal and sigaction: setup the handler of a signal.
4. sigprocmask: can be used to block/unblock signals, and get/set the current signal mask.
5. sigpending: check if there is any pending signal.
6. alarm: setup a timer for the current process.
7. functions to handle sigset_t data type: sigemptyset, sigfillset, sigaddset, sigdelset, and sigismember.

The API interface is the same to what we have in the standard C library. However, because we are attempting to replace the standard C library, our test codes will only be linked against the library implemented in this homework. We will use the following commands to assemble, compile, link, and test your implementation.

```
$ yasm -f elf64 -DYASM -D__x86_64__ -DPIC libmini64.asm -o libmini64.o
$ gcc -c -g -Wall -fno-stack-protector -fPIC -nostdlib libmini.c
$ ld -shared -o libmini.so libmini64.o libmini.o
$ yasm -f elf64 -DYASM -D__x86_64__ -DPIC start.asm -o start.o
$ gcc -c -g -Wall -fno-stack-protector -nostdlib -I. -I.. -DUSEMINI test.c
$ ld -m elf_x86_64 --dynamic-linker /lib64/ld-linux-x86-64.so.2 -o test test.o start.o -L. -L.. -lmini
```

Where *libmini64.asm* and *libmini.c* is your extended Assembly and C implmentation for this homework, respectively, and *test.c* is the testing code prepared by the TAs. The start.o file is the program [start routine](https://people.cs.nctu.edu.tw/~chuang/courses/unixprog/resources/hw3_sigasm/start.asm) we have introduced in the class. Please notice that there is a *-nostdlib* parameter passed to the compiler, which means that you could not use any existing functions implemented in the standard C library. Only the functions you have implemented in the *libmini64.asm* and *libmini.c* file can be used. In addition to your library source code, you also have to provide a corresponding *libmini.h* file. The testing codes will include this file and use the function prototypes and data types defined in the header file.

To ensure that your library can handle signals properly, your implemented setjmp function *must save the signal mask* of the current process in your customized jmp_buf data structure. The saved signal mask *must be restored* by the longjmp function. *This requirement is different from the default setjmp/longjmp implementation*.
