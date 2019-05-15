# Monitor File and Directory Activities of Dynamically Linked Programs

In this homework, we are going to practice library injection and API hijacking. Please implement a "library call monitor" (LCM) program that is able to show the activities of an arbitrary binary running on a Linux operating system. You have to implement your LCM as a shared library and inject the shared library into a process by using LD_PRELOAD. You have to dump the library calls as well as the passed parameters and the returned values. Please monitor file and directory relevant functions listed in the section "Minimum Requirements" below. The result should be output to either stderr or a filename, e.g., "**fsmon.log**". By default, the output is written to stderr. But you may write the output to a filename specified by an environment variable "**MONITOR_OUTPUT**".

You have to compile your source codes and generate a shared object. You don't have to implement any monitored program by yourself. Instead, you should work with those binaries already installed in the system, or the test cases provided by the instructor.

## Minimum Requirements

The minimum list of monitored library calls is shown below. It covers almost all the functions we have introduced in the class.

**closedir		opendir		readdir		creat		open		read		write		dup**

**dup2		close		lstat		stat		pwrite		fopen		fclose		fread**

**fwrite		fgetc		fgets		fscanf		fprintf		chdir		chown		chmod**

**remove		rename		link			unlink		readlink	symlink		mkdir		rmdir**

If you would like to monitor more, please read the function lists from the following manual pages: (ordered alphabetically)

1. [dirent.h(P)](http://man7.org/linux/man-pages/man0/dirent.h.0p.html)
2. [fcntl.h(P)](http://man7.org/linux/man-pages/man0/fcntl.h.0p.html)
3. [stdio.h(P)](http://man7.org/linux/man-pages/man0/stdio.h.0p.html)
4. [stdlib.h(P)](http://man7.org/linux/man-pages/man0/stdlib.h.0p.html)
5. [sys_socket.h(7POSIX)](http://man7.org/linux/man-pages/man0/sys_socket.h.0p.html) (network functions)
6. [unistd.h(P)](http://man7.org/linux/man-pages/man0/unistd.h.0p.html)
7. [sys_stat.h(7POSIX)](http://man7.org/linux/man-pages/man0/sys_stat.h.0p.html)

## Display Function Call Parameters and Return Values

You will get a basic score if you only print out the raw value of monitored function calls. For example, the primitive data types **char**, **int**, **short**, **long**, **long long**, **float**, and **double**. For pointers, you can also print out its raw values. If you would like to get higher scores, you should output comprehensible outputs for the user. Here are additional explanations for the comprehensible output format.

1. For **char*** data type, you may optionally print it out as a string.
2. For file descriptors (passed as an **int**), **FILE***, and **DIR*** pointers, you can convert them to corresponding file names.
3. For **struct stat** or its pointer, retrieve meaningful information from the structure. For example, file type, file size, and permissions.
