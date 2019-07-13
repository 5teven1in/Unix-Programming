#ifndef __LIBMINI_H__
#define __LIBMINI_H__		/* avoid reentrant */

typedef long long size_t;
typedef long long ssize_t;
typedef long long off_t;
typedef int mode_t;
typedef int uid_t;
typedef int gid_t;
typedef int pid_t;

typedef void (*sighandler_t)(int);

extern long errno;

#define	NULL		((void*) 0)

/* from /usr/include/asm-generic/fcntl.h */
#define O_ACCMODE	00000003
#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR		00000002
#ifndef O_CREAT
#define O_CREAT		00000100	/* not fcntl */
#endif
#ifndef O_EXCL
#define O_EXCL		00000200	/* not fcntl */
#endif
#ifndef O_NOCTTY
#define O_NOCTTY	00000400	/* not fcntl */
#endif
#ifndef O_TRUNC
#define O_TRUNC		00001000	/* not fcntl */
#endif
#ifndef O_APPEND
#define O_APPEND	00002000
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK	00004000
#endif
#ifndef O_DSYNC
#define O_DSYNC		00010000	/* used to be O_SYNC, see below */
#endif
#ifndef FASYNC
#define FASYNC		00020000	/* fcntl, for BSD compatibility */
#endif
#ifndef O_DIRECT
#define O_DIRECT	00040000	/* direct disk access hint */
#endif
#ifndef O_LARGEFILE
#define O_LARGEFILE	00100000
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY	00200000	/* must be a directory */
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW	00400000	/* don't follow links */
#endif
#ifndef O_NOATIME
#define O_NOATIME	01000000
#endif
#ifndef O_CLOEXEC
#define O_CLOEXEC	02000000	/* set close_on_exec */
#endif

/* from /usr/include/asm-generic/errno-base.h */
#define	EPERM		 1	/* Operation not permitted */
#define	ENOENT		 2	/* No such file or directory */
#define	ESRCH		 3	/* No such process */
#define	EINTR		 4	/* Interrupted system call */
#define	EIO		 5	/* I/O error */
#define	ENXIO		 6	/* No such device or address */
#define	E2BIG		 7	/* Argument list too long */
#define	ENOEXEC		 8	/* Exec format error */
#define	EBADF		 9	/* Bad file number */
#define	ECHILD		10	/* No child processes */
#define	EAGAIN		11	/* Try again */
#define	ENOMEM		12	/* Out of memory */
#define	EACCES		13	/* Permission denied */
#define	EFAULT		14	/* Bad address */
#define	ENOTBLK		15	/* Block device required */
#define	EBUSY		16	/* Device or resource busy */
#define	EEXIST		17	/* File exists */
#define	EXDEV		18	/* Cross-device link */
#define	ENODEV		19	/* No such device */
#define	ENOTDIR		20	/* Not a directory */
#define	EISDIR		21	/* Is a directory */
#define	EINVAL		22	/* Invalid argument */
#define	ENFILE		23	/* File table overflow */
#define	EMFILE		24	/* Too many open files */
#define	ENOTTY		25	/* Not a typewriter */
#define	ETXTBSY		26	/* Text file busy */
#define	EFBIG		27	/* File too large */
#define	ENOSPC		28	/* No space left on device */
#define	ESPIPE		29	/* Illegal seek */
#define	EROFS		30	/* Read-only file system */
#define	EMLINK		31	/* Too many links */
#define	EPIPE		32	/* Broken pipe */
#define	EDOM		33	/* Math argument out of domain of func */
#define	ERANGE		34	/* Math result not representable */

/* from /usr/include/asm-generic/errno.h */
#define EDEADLK     35  /* Resource deadlock would occur */
#define ENAMETOOLONG    36  /* File name too long */
#define ENOLCK      37  /* No record locks available */
#define ENOSYS      38  /* Invalid system call number */
#define ENOTEMPTY   39  /* Directory not empty */
#define ELOOP       40  /* Too many symbolic links encountered */
#define EWOULDBLOCK EAGAIN  /* Operation would block */
#define ENOMSG      42  /* No message of desired type */
#define EIDRM       43  /* Identifier removed */
#define ECHRNG      44  /* Channel number out of range */
#define EL2NSYNC    45  /* Level 2 not synchronized */
#define EL3HLT      46  /* Level 3 halted */
#define EL3RST      47  /* Level 3 reset */
#define ELNRNG      48  /* Link number out of range */
#define EUNATCH     49  /* Protocol driver not attached */
#define ENOCSI      50  /* No CSI structure available */
#define EL2HLT      51  /* Level 2 halted */
#define EBADE       52  /* Invalid exchange */
#define EBADR       53  /* Invalid request descriptor */
#define EXFULL      54  /* Exchange full */
#define ENOANO      55  /* No anode */
#define EBADRQC     56  /* Invalid request code */
#define EBADSLT     57  /* Invalid slot */
#define EDEADLOCK   EDEADLK
#define EBFONT      59  /* Bad font file format */
#define ENOSTR      60  /* Device not a stream */
#define ENODATA     61  /* No data available */
#define ETIME       62  /* Timer expired */
#define ENOSR       63  /* Out of streams resources */
#define ENONET      64  /* Machine is not on the network */
#define ENOPKG      65  /* Package not installed */
#define EREMOTE     66  /* Object is remote */
#define ENOLINK     67  /* Link has been severed */
#define EADV        68  /* Advertise error */
#define ESRMNT      69  /* Srmount error */
#define ECOMM       70  /* Communication error on send */
#define EPROTO      71  /* Protocol error */
#define EMULTIHOP   72  /* Multihop attempted */
#define EDOTDOT     73  /* RFS specific error */
#define EBADMSG     74  /* Not a data message */
#define EOVERFLOW   75  /* Value too large for defined data type */
#define ENOTUNIQ    76  /* Name not unique on network */
#define EBADFD      77  /* File descriptor in bad state */
#define EREMCHG     78  /* Remote address changed */
#define ELIBACC     79  /* Can not access a needed shared library */
#define ELIBBAD     80  /* Accessing a corrupted shared library */
#define ELIBSCN     81  /* .lib section in a.out corrupted */
#define ELIBMAX     82  /* Attempting to link in too many shared libraries */
#define ELIBEXEC    83  /* Cannot exec a shared library directly */
#define EILSEQ      84  /* Illegal byte sequence */
#define ERESTART    85  /* Interrupted system call should be restarted */
#define ESTRPIPE    86  /* Streams pipe error */
#define EUSERS      87  /* Too many users */
#define ENOTSOCK    88  /* Socket operation on non-socket */
#define EDESTADDRREQ    89  /* Destination address required */
#define EMSGSIZE    90  /* Message too long */
#define EPROTOTYPE  91  /* Protocol wrong type for socket */
#define ENOPROTOOPT 92  /* Protocol not available */
#define EPROTONOSUPPORT 93  /* Protocol not supported */
#define ESOCKTNOSUPPORT 94  /* Socket type not supported */
#define EOPNOTSUPP  95  /* Operation not supported on transport endpoint */
#define EPFNOSUPPORT    96  /* Protocol family not supported */
#define EAFNOSUPPORT    97  /* Address family not supported by protocol */
#define EADDRINUSE  98  /* Address already in use */
#define EADDRNOTAVAIL   99  /* Cannot assign requested address */
#define ENETDOWN    100 /* Network is down */
#define ENETUNREACH 101 /* Network is unreachable */
#define ENETRESET   102 /* Network dropped connection because of reset */
#define ECONNABORTED    103 /* Software caused connection abort */
#define ECONNRESET  104 /* Connection reset by peer */
#define ENOBUFS     105 /* No buffer space available */
#define EISCONN     106 /* Transport endpoint is already connected */
#define ENOTCONN    107 /* Transport endpoint is not connected */
#define ESHUTDOWN   108 /* Cannot send after transport endpoint shutdown */
#define ETOOMANYREFS    109 /* Too many references: cannot splice */
#define ETIMEDOUT   110 /* Connection timed out */
#define ECONNREFUSED    111 /* Connection refused */
#define EHOSTDOWN   112 /* Host is down */
#define EHOSTUNREACH    113 /* No route to host */
#define EALREADY    114 /* Operation already in progress */
#define EINPROGRESS 115 /* Operation now in progress */
#define ESTALE      116 /* Stale file handle */
#define EUCLEAN     117 /* Structure needs cleaning */
#define ENOTNAM     118 /* Not a XENIX named type file */
#define ENAVAIL     119 /* No XENIX semaphores available */
#define EISNAM      120 /* Is a named type file */
#define EREMOTEIO   121 /* Remote I/O error */
#define EDQUOT      122 /* Quota exceeded */
#define ENOMEDIUM   123 /* No medium found */
#define EMEDIUMTYPE 124 /* Wrong medium type */
#define ECANCELED   125 /* Operation Canceled */
#define ENOKEY      126 /* Required key not available */
#define EKEYEXPIRED 127 /* Key has expired */
#define EKEYREVOKED 128 /* Key has been revoked */
#define EKEYREJECTED    129 /* Key was rejected by service */
/* for robust mutexes */
#define EOWNERDEAD  130 /* Owner died */
#define ENOTRECOVERABLE 131 /* State not recoverable */
#define ERFKILL     132 /* Operation not possible due to RF-kill */
#define EHWPOISON   133 /* Memory page has hardware error */

/* from /usr/include/x86_64-linux-gnu/asm/signal.h */
#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGBUS		 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGURG		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGIO		29
#define SIGPOLL		SIGIO

#define SIGCANCEL   32
#define SIGSETXID   33

#define NSIG        64 + 1

/* Fake signal functions.  */
#define        SIG_ERR         ((sighandler_t) -1)        /* Error return.  */
#define        SIG_DFL         ((sighandler_t)  0)        /* Default action.  */
#define        SIG_IGN         ((sighandler_t)  1)        /* Ignore signal.  */

/* from /usr/include/x86_64-linux-gnu/bits/sigaction.h */
#define	SA_NOCLDSTOP  1		 /* Don't send SIGCHLD when children stop.  */
#define SA_NOCLDWAIT  2		 /* Don't create zombie on child death.  */
#define SA_SIGINFO    4		 /* Invoke signal-catching function with
                                three arguments instead of one.  */
# define SA_ONSTACK   0x08000000 /* Use signal stack by using `sa_restorer'. */
# define SA_RESTART   0x10000000 /* Restart syscall on signal return.  */
# define SA_INTERRUPT 0x20000000 /* Historical no-op.  */
# define SA_NODEFER   0x40000000 /* Don't automatically block the signal when
                                    its handler is being executed.  */
# define SA_RESETHAND 0x80000000 /* Reset to SIG_DFL on entry to handler.  */

#define	SIG_BLOCK     0		 /* Block signals.  */
#define	SIG_UNBLOCK   1		 /* Unblock signals.  */
#define	SIG_SETMASK   2		 /* Set the set of blocked signals.  */

struct timespec {
    long	tv_sec;		/* seconds */
    long	tv_nsec;	/* nanoseconds */
};

struct timeval {
    long	tv_sec;		/* seconds */
    long	tv_usec;	/* microseconds */
};

struct timezone {
    int	tz_minuteswest; /* minutes west of Greenwich */
    int	tz_dsttime;     /* type of DST correction */
};

typedef struct {
    unsigned long sig[16];
} sigset_t;

typedef struct {
    unsigned char s[128];
} siginfo_t;

struct sigaction {
    void        (*sa_handler)(int);
    void        (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t    sa_mask;
    int         sa_flags;
    void        (*sa_restorer)(void);
};

typedef struct jmp_buf_s {
    long long reg[8];
    sigset_t mask;
} jmp_buf[1];

/* system calls */
long sys_read(int fd, char *buf, size_t count);
long sys_write(int fd, const void *buf, size_t count);
long sys_open(const char *filename, int flags, ... /*mode*/);
long sys_close(unsigned int fd);
long sys_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off);
long sys_mprotect(void *addr, size_t len, int prot);
long sys_munmap(void *addr, size_t len);
long sys_pipe(int *filedes);
long sys_dup(int filedes);
long sys_dup2(int oldfd, int newfd);
long sys_pause();
long sys_nanosleep(struct timespec *rqtp, struct timespec *rmtp);
long sys_fork(void);
long sys_exit(int error_code) __attribute__ ((noreturn));
long sys_getcwd(char *buf, size_t size);
long sys_chdir(const char *pathname);
long sys_rename(const char *oldname, const char *newname);
long sys_mkdir(const char *pathname, int mode);
long sys_rmdir(const char *pathname);
long sys_creat(const char *pathname, int mode);
long sys_link(const char *oldname, const char *newname);
long sys_unlink(const char *pathname);
long sys_readlink(const char *path, char *buf, size_t bufsz);
long sys_chmod(const char *filename, mode_t mode);
long sys_chown(const char *filename, uid_t user, gid_t group);
long sys_umask(int mask);
long sys_gettimeofday(struct timeval *tv, struct timezone *tz);
long sys_getuid();
long sys_getgid();
long sys_setuid(uid_t uid);
long sys_setgid(gid_t gid);
long sys_geteuid();
long sys_getegid();

/* extend system calls */
long sys_alarm(unsigned int seconds);

/* wrappers */
ssize_t	read(int fd, char *buf, size_t count);
ssize_t	write(int fd, const void *buf, size_t count);
int	open(const char *filename, int flags, ... /*mode*/);
int	close(unsigned int fd);
void *	mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off);
int	mprotect(void *addr, size_t len, int prot);
int	munmap(void *addr, size_t len);
int	pipe(int *filedes);
int	dup(int filedes);
int	dup2(int oldfd, int newfd);
int	pause();
int	nanosleep(struct timespec *rqtp, struct timespec *rmtp);
pid_t	fork(void);
void	exit(int error_code);
char *	getcwd(char *buf, size_t size);
int	chdir(const char *pathname);
int	rename(const char *oldname, const char *newname);
int	mkdir(const char *pathname, int mode);
int	rmdir(const char *pathname);
int	creat(const char *pathname, int mode);
int	link(const char *oldname, const char *newname);
int	unlink(const char *pathname);
ssize_t	readlink(const char *path, char *buf, size_t bufsz);
int	chmod(const char *filename, mode_t mode);
int	chown(const char *filename, uid_t user, gid_t group);
int	umask(int mask);
int	gettimeofday(struct timeval *tv, struct timezone *tz);
uid_t	getuid();
gid_t	getgid();
int	setuid(uid_t uid);
int	setgid(gid_t gid);
uid_t	geteuid();
gid_t	getegid();

void bzero(void *s, size_t size);
size_t strlen(const char *s);
void perror(const char *prefix);
unsigned int sleep(unsigned int s);

/* extend wrappers */
unsigned int alarm(unsigned int seconds);
int sigprocmask(int how, const sigset_t *set, sigset_t *oset);
int sigpending(sigset_t *set);
sighandler_t signal(int sig, sighandler_t handler);
int sigaction(int sig, const struct sigaction *act, struct sigaction *oact);

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signum);
int sigdelset(sigset_t *set, int signum);
int sigismember(const sigset_t *set, int signum);

#endif	/* __LIBMINI_H__ */
