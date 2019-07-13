#include "libmini.h"

long errno;

#define	WRAPPER_RETval(type)	errno = 0; if(ret < 0) { errno = -ret; return -1; } return ((type) ret);
#define	WRAPPER_RETptr(type)	errno = 0; if(ret < 0) { errno = -ret; return NULL; } return ((type) ret);

#define __set_errno(val) (errno = (val))

ssize_t	read(int fd, char *buf, size_t count) {
	long ret = sys_read(fd, buf, count);
	WRAPPER_RETval(ssize_t);
}

ssize_t	write(int fd, const void *buf, size_t count) {
	long ret = sys_write(fd, buf, count);
	WRAPPER_RETval(ssize_t);
}

/* open is implemented in assembly, because of variable length arguments */

int	close(unsigned int fd) {
	long ret = sys_close(fd);
	WRAPPER_RETval(int);
}

void *	mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off) {
	long ret = sys_mmap(addr, len, prot, flags, fd, off);
	WRAPPER_RETptr(void *);
}

int	mprotect(void *addr, size_t len, int prot) {
	long ret = sys_mprotect(addr, len, prot);
	WRAPPER_RETval(int);
}

int	munmap(void *addr, size_t len) {
	long ret = sys_munmap(addr, len);
	WRAPPER_RETval(int);
}

int	pipe(int *filedes) {
	long ret = sys_pipe(filedes);
	WRAPPER_RETval(int);
}

int	dup(int filedes) {
	long ret = sys_dup(filedes);
	WRAPPER_RETval(int);
}

int	dup2(int oldfd, int newfd) {
	long ret = sys_dup2(oldfd, newfd);
	WRAPPER_RETval(int);
}

int	pause() {
	long ret = sys_pause();
	WRAPPER_RETval(int);
}

int	nanosleep(struct timespec *rqtp, struct timespec *rmtp) {
	long ret = nanosleep(rqtp, rmtp);
	WRAPPER_RETval(int);
}

pid_t	fork(void) {
	long ret = sys_fork();
	WRAPPER_RETval(pid_t);
}

void	exit(int error_code) {
	sys_exit(error_code);
	/* never returns? */
}

char *	getcwd(char *buf, size_t size) {
	long ret = sys_getcwd(buf, size);
	WRAPPER_RETptr(char *);
}

int	chdir(const char *pathname) {
	long ret = sys_chdir(pathname);
	WRAPPER_RETval(int);
}

int	rename(const char *oldname, const char *newname) {
	long ret = sys_rename(oldname, newname);
	WRAPPER_RETval(int);
}

int	mkdir(const char *pathname, int mode) {
	long ret = sys_mkdir(pathname, mode);
	WRAPPER_RETval(int);
}

int	rmdir(const char *pathname) {
	long ret = sys_rmdir(pathname);
	WRAPPER_RETval(int);
}

int	creat(const char *pathname, int mode) {
	long ret = sys_creat(pathname, mode);
	WRAPPER_RETval(int);
}

int	link(const char *oldname, const char *newname) {
	long ret = sys_link(oldname, newname);
	WRAPPER_RETval(int);
}

int	unlink(const char *pathname) {
	long ret = sys_unlink(pathname);
	WRAPPER_RETval(int);
}

ssize_t	readlink(const char *path, char *buf, size_t bufsz) {
	long ret = sys_readlink(path, buf, bufsz);
	WRAPPER_RETval(ssize_t);
}

int	chmod(const char *filename, mode_t mode) {
	long ret = sys_chmod(filename, mode);
	WRAPPER_RETval(int);
}

int	chown(const char *filename, uid_t user, gid_t group) {
	long ret = sys_chown(filename, user, group);
	WRAPPER_RETval(int);
}

int	umask(int mask) {
	long ret = sys_umask(mask);
	WRAPPER_RETval(int);
}

int	gettimeofday(struct timeval *tv, struct timezone *tz) {
	long ret = sys_gettimeofday(tv, tz);
	WRAPPER_RETval(int);
}

uid_t	getuid() {
	long ret = sys_getuid();
	WRAPPER_RETval(uid_t);
}

gid_t	getgid() {
	long ret = sys_getgid();
	WRAPPER_RETval(uid_t);
}

int	setuid(uid_t uid) {
	long ret = sys_setuid(uid);
	WRAPPER_RETval(int);
}

int	setgid(gid_t gid) {
	long ret = sys_setgid(gid);
	WRAPPER_RETval(int);
}

uid_t	geteuid() {
	long ret = sys_geteuid();
	WRAPPER_RETval(uid_t);
}

gid_t	getegid() {
	long ret = sys_getegid();
	WRAPPER_RETval(uid_t);
}

void bzero(void *s, size_t size) {
	char *ptr = (char *) s;
	while(size-- > 0) *ptr++ = '\0';
}

size_t strlen(const char *s) {
	size_t count = 0;
	while(*s++) count++;
	return count;
}

unsigned int alarm(unsigned int seconds) {
    long ret = sys_alarm(seconds);
    WRAPPER_RETval(unsigned int);
}

/* Return is sig is used internally.  */
static inline int __is_internal_signal (int sig) {
  return (sig == SIGCANCEL) || (sig == SIGSETXID);
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oset) {
    switch (how) {
        case SIG_BLOCK:
        case SIG_UNBLOCK:
        case SIG_SETMASK:
            break;
        default:
            __set_errno (EINVAL);
            return -1;
    }
    __set_errno (ENOSYS);
    return -1;
}

int sigpending(sigset_t *set) {
    if (set == NULL) {
        __set_errno (EINVAL);
        return -1;
    }
    __set_errno (ENOSYS);
    return -1;
}

sighandler_t signal(int sig, sighandler_t handler) {
    __set_errno (ENOSYS);
    return SIG_ERR;
}

int sigaction(int sig, const struct sigaction *act, struct sigaction *oact) {
    if (sig <= 0 || sig >= NSIG || __is_internal_signal(sig)) {
        __set_errno (EINVAL);
        return -1;
    }
    __set_errno (ENOSYS);
    return -1;
}

int sigemptyset(sigset_t *set) {
    if (set == NULL) {
        __set_errno (EINVAL);
        return -1;
    }
    bzero(set, sizeof (sigset_t));
    return 0;
}

#define	PERRMSG_MIN	0
#define	PERRMSG_MAX	133

static const char *errmsg[] = {
	"Success",
	"Operation not permitted",
	"No such file or directory",
	"No such process",
	"Interrupted system call",
	"I/O error",
	"No such device or address",
	"Argument list too long",
	"Exec format error",
	"Bad file number",
	"No child processes",
	"Try again",
	"Out of memory",
	"Permission denied",
	"Bad address",
	"Block device required",
	"Device or resource busy",
	"File exists",
	"Cross-device link",
	"No such device",
	"Not a directory",
	"Is a directory",
	"Invalid argument",
	"File table overflow",
	"Too many open files",
	"Not a typewriter",
	"Text file busy",
	"File too large",
	"No space left on device",
	"Illegal seek",
	"Read-only file system",
	"Too many links",
	"Broken pipe",
	"Math argument out of domain of func",
	"Math result not representable",
    "Resource deadlock would occur",
    "File name too long",
    "No record locks available",
    "Invalid system call number",
    "Directory not empty",
    "Too many symbolic links encountered",
    "Operation would block",
    "No message of desired type",
    "Identifier removed",
    "Channel number out of range",
    "Level 2 not synchronized",
    "Level 3 halted",
    "Level 3 reset",
    "Link number out of range",
    "Protocol driver not attached",
    "No CSI structure available",
    "Level 2 halted",
    "Invalid exchange",
    "Invalid request descriptor",
    "Exchange full",
    "No anode",
    "Invalid request code",
    "Invalid slot",
    "Resource deadlock would occur",
    "Bad font file format",
    "Device not a stream",
    "No data available",
    "Timer expired",
    "Out of streams resources",
    "Machine is not on the network",
    "Package not installed",
    "Object is remote",
    "Link has been severed",
    "Advertise error",
    "Srmount error",
    "Communication error on send",
    "Protocol error",
    "Multihop attempted",
    "RFS specific error",
    "Not a data message",
    "Value too large for defined data type",
    "Name not unique on network",
    "File descriptor in bad state",
    "Remote address changed",
    "Can not access a needed shared library",
    "Accessing a corrupted shared library",
    ".lib section in a.out corrupted",
    "Attempting to link in too many shared libraries",
    "Cannot exec a shared library directly",
    "Illegal byte sequence",
    "Interrupted system call should be restarted",
    "Streams pipe error",
    "Too many users",
    "Socket operation on non-socket",
    "Destination address required",
    "Message too long",
    "Protocol wrong type for socket",
    "Protocol not available",
    "Protocol not supported",
    "Socket type not supported",
    "Operation not supported on transport endpoint",
    "Protocol family not supported",
    "Address family not supported by protocol",
    "Address already in use",
    "Cannot assign requested address",
    "Network is down",
    "Network is unreachable",
    "Network dropped connection because of reset",
    "Software caused connection abort",
    "Connection reset by peer",
    "No buffer space available",
    "Transport endpoint is already connected",
    "Transport endpoint is not connected",
    "Cannot send after transport endpoint shutdown",
    "Too many references: cannot splice",
    "Connection timed out",
    "Connection refused",
    "Host is down",
    "No route to host",
    "Operation already in progress",
    "Operation now in progress",
    "Stale file handle",
    "Structure needs cleaning",
    "Not a XENIX named type file",
    "No XENIX semaphores available",
    "Is a named type file",
    "Remote I/O error",
    "Quota exceeded",
    "No medium found",
    "Wrong medium type",
    "Operation Canceled",
    "Required key not available",
    "Key has expired",
    "Key has been revoked",
    "Key was rejected by service",
    "Owner died",
    "State not recoverable",
    "Operation not possible due to RF-kill",
    "Memory page has hardware error",
};

void perror(const char *prefix) {
	const char *unknown = "Unknown";
	long backup = errno;
	if(prefix) {
		write(2, prefix, strlen(prefix));
		write(2, ": ", 2);
	}
	if(errno < PERRMSG_MIN || errno > PERRMSG_MAX) write(2, unknown, strlen(unknown));
	else write(2, errmsg[backup], strlen(errmsg[backup]));
	write(2, "\n", 1);
	return;
}

#if 0	/* we have an equivalent implementation in assembly */
unsigned int sleep(unsigned int seconds) {
	long ret;
	struct timespec req, rem;
	req.tv_sec = seconds;
	req.tv_nsec = 0;
	ret = sys_nanosleep(&req, &rem);
	if(ret >= 0) return ret;
	if(ret == -EINTR) {
		return rem.tv_sec;
	}
	return 0;
}
#endif
