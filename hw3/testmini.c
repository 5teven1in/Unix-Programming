#include "libmini.h"

int main() {
	char s[] = "sleeping for 5s ...\n";
	char m[] = "hello, world!\n";
	write(1, s, sizeof(s));
	sleep(5);
	write(1, m, sizeof(m));
	return 0;	
}

