template<>
class ABIDefs<ExecModeMips32>{
 public:
  typedef uint32_t Tpointer_t;
  typedef int8_t   Tchar;
  typedef uint8_t  Tuchar;
  typedef int16_t  Tshort;
  typedef uint16_t Tushort;
  typedef int32_t  Tint;
  typedef uint32_t Tuint;
  typedef int32_t  Tlong;
  typedef uint32_t Tulong;
  typedef int64_t  Tllong;
  typedef int32_t  Tssize_t;
  typedef int32_t  Tptrdiff_t;
  typedef uint32_t Tsize_t;
  static const Tint VSTDIN_FILENO = 0x00000000;
  static const Tint VSTDOUT_FILENO = 0x00000001;
  static const Tint VSTDERR_FILENO = 0x00000002;
  typedef int32_t  Tpid_t;
  static const Tint VEPERM = 0x00000001;
  static const Tint VENOENT = 0x00000002;
  static const Tint VESRCH = 0x00000003;
  static const Tint VEINTR = 0x00000004;
  static const Tint VEIO = 0x00000005;
  static const Tint VENXIO = 0x00000006;
  static const Tint VE2BIG = 0x00000007;
  static const Tint VENOEXEC = 0x00000008;
  static const Tint VEBADF = 0x00000009;
  static const Tint VECHILD = 0x0000000a;
  static const Tint VEAGAIN = 0x0000000b;
  static const Tint VENOMEM = 0x0000000c;
  static const Tint VEACCES = 0x0000000d;
  static const Tint VEFAULT = 0x0000000e;
  static const Tint VENOTBLK = 0x0000000f;
  static const Tint VEBUSY = 0x00000010;
  static const Tint VEEXIST = 0x00000011;
  static const Tint VEXDEV = 0x00000012;
  static const Tint VENODEV = 0x00000013;
  static const Tint VENOTDIR = 0x00000014;
  static const Tint VEISDIR = 0x00000015;
  static const Tint VEINVAL = 0x00000016;
  static const Tint VENFILE = 0x00000017;
  static const Tint VEMFILE = 0x00000018;
  static const Tint VENOTTY = 0x00000019;
  static const Tint VETXTBSY = 0x0000001a;
  static const Tint VEFBIG = 0x0000001b;
  static const Tint VENOSPC = 0x0000001c;
  static const Tint VESPIPE = 0x0000001d;
  static const Tint VEROFS = 0x0000001e;
  static const Tint VEMLINK = 0x0000001f;
  static const Tint VEPIPE = 0x00000020;
  static const Tint VEDOM = 0x00000021;
  static const Tint VERANGE = 0x00000022;
  static const Tint VENOSYS = 0x00000059;
  static const Tint VEAFNOSUPPORT = 0x0000007c;
  static const Tint VSIGHUP = 0x00000001;
  static const Tint VSIGINT = 0x00000002;
  static const Tint VSIGQUIT = 0x00000003;
  static const Tint VSIGILL = 0x00000004;
  static const Tint VSIGTRAP = 0x00000005;
  static const Tint VSIGABRT = 0x00000006;
  static const Tint VSIGFPE = 0x00000008;
  static const Tint VSIGKILL = 0x00000009;
  static const Tint VSIGBUS = 0x0000000a;
  static const Tint VSIGSEGV = 0x0000000b;
  static const Tint VSIGPIPE = 0x0000000d;
  static const Tint VSIGALRM = 0x0000000e;
  static const Tint VSIGTERM = 0x0000000f;
  static const Tint VSIGUSR1 = 0x00000010;
  static const Tint VSIGUSR2 = 0x00000011;
  static const Tint VSIGCHLD = 0x00000012;
  static const Tint VSIGSTOP = 0x00000017;
  static const Tint VSIGTSTP = 0x00000018;
  static const Tint VSIGCONT = 0x00000019;
  static const Tint VCSIGNAL = 0x000000ff;
  static const Tint VCLONE_VM = 0x00000100;
  static const Tint VCLONE_FS = 0x00000200;
  static const Tint VCLONE_FILES = 0x00000400;
  static const Tint VCLONE_SIGHAND = 0x00000800;
  static const Tint VCLONE_VFORK = 0x00004000;
  static const Tint VCLONE_PARENT = 0x00008000;
  static const Tint VCLONE_THREAD = 0x00010000;
  static const Tint VCLONE_NEWNS = 0x00020000;
  static const Tint VCLONE_SYSVSEM = 0x00040000;
  static const Tint VCLONE_SETTLS = 0x00080000;
  static const Tint VCLONE_PARENT_SETTID = 0x00100000;
  static const Tint VCLONE_CHILD_CLEARTID = 0x00200000;
  static const Tint VCLONE_DETACHED = 0x00400000;
  static const Tint VCLONE_UNTRACED = 0x00800000;
  static const Tint VCLONE_CHILD_SETTID = 0x01000000;
  static const Tint VCLONE_STOPPED = 0x02000000;
  static const Tint V__NR_clone = 0x00001018;
  static const Tint V__NR_fork = 0x00000fa2;
  static const Tint V__NR_execve = 0x00000fab;
  static const Tint V__NR_exit = 0x00000fa1;
  static const Tint V__NR_exit_group = 0x00001096;
  static const Tint VWNOHANG = 0x00000001;
  static const Tint VWUNTRACED = 0x00000002;
  static const Tint V__NR_waitpid = 0x00000fa7;
  static const Tint V__NR_wait4 = 0x00001012;
  static const Tint V__NR_getpid = 0x00000fb4;
  static const Tint V__NR_gettid = 0x0000107e;
  static const Tint V__NR_getppid = 0x00000fe0;
  static const Tint V__NR_setpgid = 0x00000fd9;
  static const Tint V__NR_getpgid = 0x00001024;
  static const Tint V__NR_getpgrp = 0x00000fe1;
  static const Tint V__NR_setsid = 0x00000fe2;
  static const Tint V__NR_getsid = 0x00001037;
  static const Tint VFUTEX_PRIVATE_FLAG = 0x00000080;
  static const Tint VFUTEX_CMD_MASK = 0xffffff7f;
  static const Tint VFUTEX_WAIT = 0x00000000;
  static const Tint VFUTEX_WAKE = 0x00000001;
  static const Tint VFUTEX_FD = 0x00000002;
  static const Tint VFUTEX_REQUEUE = 0x00000003;
  static const Tint VFUTEX_CMP_REQUEUE = 0x00000004;
  static const Tint VFUTEX_WAKE_OP = 0x00000005;
  static const Tint VFUTEX_OP_OPARG_SHIFT = 0x00000008;
  static const Tint VFUTEX_OP_SET = 0x00000000;
  static const Tint VFUTEX_OP_ADD = 0x00000001;
  static const Tint VFUTEX_OP_OR = 0x00000002;
  static const Tint VFUTEX_OP_ANDN = 0x00000003;
  static const Tint VFUTEX_OP_XOR = 0x00000004;
  static const Tint VFUTEX_OP_CMP_EQ = 0x00000000;
  static const Tint VFUTEX_OP_CMP_NE = 0x00000001;
  static const Tint VFUTEX_OP_CMP_LT = 0x00000002;
  static const Tint VFUTEX_OP_CMP_LE = 0x00000003;
  static const Tint VFUTEX_OP_CMP_GT = 0x00000004;
  static const Tint VFUTEX_OP_CMP_GE = 0x00000005;
  static const Tint V__NR_futex = 0x0000108e;
  class Trobust_list{
   public:
    static const size_t Size_All=4;
    typedef Tpointer_t Type_next;
    static const size_t Offs_next=0;
  };
  class Trobust_list_head{
   public:
    static const size_t Size_All=12;
    typedef Trobust_list Type_list;
    static const size_t Offs_list=0;
    typedef Tlong Type_futex_offset;
    static const size_t Offs_futex_offset=4;
    typedef Tpointer_t Type_list_op_pending;
    static const size_t Offs_list_op_pending=8;
  };
  static const Tint V__NR_set_robust_list = 0x000010d5;
  static const Tint V__NR_get_robust_list = 0x000010d6;
  static const Tint V__NR_set_tid_address = 0x0000109c;
  static const Tint V__NR_kill = 0x00000fc5;
  static const Tint V__NR_tkill = 0x0000108c;
  static const Tint V__NR_tgkill = 0x000010aa;
  static const Tint V__NR_rt_sigqueueinfo = 0x00001066;
  static const Tint VSA_NOCLDSTOP = 0x00000001;
  static const Tint VSA_NOCLDWAIT = 0x00010000;
  static const Tuint VSA_RESETHAND = 0x80000000;
  static const Tint VSA_ONSTACK = 0x08000000;
  static const Tint VSA_SIGINFO = 0x00000008;
  static const Tint VSA_RESTART = 0x10000000;
  static const Tint VSA_NODEFER = 0x40000000;
  static const Tint VSA_INTERRUPT = 0x20000000;
  static const Tpointer_t VSIG_DFL = 0x00000000;
  static const Tpointer_t VSIG_IGN = 0x00000001;
  static const Tint V__NR_sigaction = 0x00000fe3;
  static const Tint V__NR_rt_sigaction = 0x00001062;
  static const Tint VSIG_BLOCK = 0x00000001;
  static const Tint VSIG_UNBLOCK = 0x00000002;
  static const Tint VSIG_SETMASK = 0x00000003;
  static const Tint V__NR_sigprocmask = 0x0000101e;
  static const Tint V__NR_rt_sigprocmask = 0x00001063;
  static const Tint V__NR_sigpending = 0x00000fe9;
  static const Tint V__NR_rt_sigpending = 0x00001064;
  static const Tint V__NR_sigsuspend = 0x00000fe8;
  static const Tint V__NR_rt_sigsuspend = 0x00001067;
  static const Tint V__NR_rt_sigtimedwait = 0x00001065;
  static const Tint V__NR_signal = 0x00000fd0;
  static const Tint V__NR_signalfd = 0x000010dd;
  static const Tint V__NR_sigaltstack = 0x0000106e;
  static const Tint V__NR_sigreturn = 0x00001017;
  static const Tint V__NR_rt_sigreturn = 0x00001061;
  typedef int32_t  Tclock_t;
  class Ttms{
   public:
    static const size_t Size_All=16;
    typedef Tclock_t Type_tms_utime;
    static const size_t Offs_tms_utime=0;
    typedef Tclock_t Type_tms_stime;
    static const size_t Offs_tms_stime=4;
    typedef Tclock_t Type_tms_cutime;
    static const size_t Offs_tms_cutime=8;
    typedef Tclock_t Type_tms_cstime;
    static const size_t Offs_tms_cstime=12;
  };
  static const Tint V__NR_times = 0x00000fcb;
  typedef int32_t  Ttime_t;
  static const Tint V__NR_time = 0x00000fad;
  typedef int32_t  Tsuseconds_t;
  class Ttimeval{
   public:
    static const size_t Size_All=8;
    typedef Ttime_t Type_tv_sec;
    static const size_t Offs_tv_sec=0;
    typedef Tsuseconds_t Type_tv_usec;
    static const size_t Offs_tv_usec=4;
  };
  class Ttimezone{
   public:
    static const size_t Size_All=8;
    typedef Tint Type_tz_minuteswest;
    static const size_t Offs_tz_minuteswest=0;
    typedef Tint Type_tz_dsttime;
    static const size_t Offs_tz_dsttime=4;
  };
  static const Tint V__NR_settimeofday = 0x00000fef;
  static const Tint V__NR_gettimeofday = 0x00000fee;
  static const Tint V__NR_setitimer = 0x00001008;
  static const Tint V__NR_getitimer = 0x00001009;
  typedef int32_t  Tclockid_t;
  static const Tint V__NR_clock_getres = 0x000010a8;
  static const Tint V__NR_clock_settime = 0x000010a6;
  static const Tint V__NR_clock_gettime = 0x000010a7;
  static const Tint V__NR_alarm = 0x00000fbb;
  class Ttimespec{
   public:
    static const size_t Size_All=8;
    typedef Ttime_t Type_tv_sec;
    static const size_t Offs_tv_sec=0;
    typedef Tlong Type_tv_nsec;
    static const size_t Offs_tv_nsec=4;
  };
  static const Tint V__NR_nanosleep = 0x00001046;
  static const Tint V__NR_clock_nanosleep = 0x000010a9;
  static const Tint V__NR_sched_yield = 0x00001042;
  static const Tint V__NR_getpriority = 0x00001000;
  static const Tint V__NR_setpriority = 0x00001001;
  static const Tint V__NR_sched_getparam = 0x0000103f;
  static const Tint V__NR_sched_setparam = 0x0000103e;
  static const Tint V__NR_sched_setscheduler = 0x00001040;
  static const Tint V__NR_sched_getscheduler = 0x00001041;
  static const Tint V__NR_sched_get_priority_max = 0x00001043;
  static const Tint V__NR_sched_get_priority_min = 0x00001044;
  static const Tint V__NR_sched_setaffinity = 0x0000108f;
  static const Tint V__NR_sched_getaffinity = 0x00001090;
  static const Tint V__NR_sched_rr_get_interval = 0x00001045;
  typedef uint32_t Tuid_t;
  typedef uint32_t Tgid_t;
  static const Tint V__NR_setuid = 0x00000fb7;
  static const Tint V__NR_getuid = 0x00000fb8;
  static const Tint V__NR_setreuid = 0x00000fe6;
  static const Tint V__NR_setresuid = 0x00001059;
  static const Tint V__NR_setfsuid = 0x0000102a;
  static const Tint V__NR_geteuid = 0x00000fd1;
  static const Tint V__NR_getresuid = 0x0000105a;
  static const Tint V__NR_getgid = 0x00000fcf;
  static const Tint V__NR_setgid = 0x00000fce;
  static const Tint V__NR_setregid = 0x00000fe7;
  static const Tint V__NR_setresgid = 0x0000105e;
  static const Tint V__NR_setfsgid = 0x0000102b;
  static const Tint V__NR_getegid = 0x00000fd2;
  static const Tint V__NR_getresgid = 0x0000105f;
  static const Tint V__NR_setgroups = 0x00000ff1;
  static const Tint V__NR_getgroups = 0x00000ff0;
  static const Tint V__NR_brk = 0x00000fcd;
  static const Tint V__NR_set_thread_area = 0x000010bb;
  static const Tuint VMAP_SHARED = 0x00000001;
  static const Tuint VMAP_PRIVATE = 0x00000002;
  static const Tuint VMAP_FIXED = 0x00000010;
  static const Tuint VMAP_ANONYMOUS = 0x00000800;
  static const Tint VPROT_NONE = 0x00000000;
  static const Tint VPROT_READ = 0x00000001;
  static const Tint VPROT_WRITE = 0x00000002;
  static const Tint VPROT_EXEC = 0x00000004;
  static const Tint V__NR_mmap = 0x00000ffa;
  static const Tint V__NR_mmap2 = 0x00001072;
  static const Tuint VMREMAP_MAYMOVE = 0x00000001;
  static const Tuint VMREMAP_FIXED = 0x00000002;
  static const Tint V__NR_mremap = 0x00001047;
  static const Tint V__NR_munmap = 0x00000ffb;
  static const Tint V__NR_mprotect = 0x0000101d;
  static const Tint VO_ACCMODE = 0x00000003;
  static const Tint VO_RDONLY = 0x00000000;
  static const Tint VO_WRONLY = 0x00000001;
  static const Tint VO_RDWR = 0x00000002;
  static const Tint VO_APPEND = 0x00000008;
  static const Tint VO_SYNC = 0x00000010;
  static const Tint VO_NONBLOCK = 0x00000080;
  static const Tint VO_CREAT = 0x00000100;
  static const Tint VO_TRUNC = 0x00000200;
  static const Tint VO_EXCL = 0x00000400;
  static const Tint VO_NOCTTY = 0x00000800;
  static const Tint VO_ASYNC = 0x00001000;
  static const Tint VO_LARGEFILE = 0x00002000;
  static const Tint VO_DIRECT = 0x00008000;
  static const Tint VO_DIRECTORY = 0x00010000;
  static const Tint VO_NOFOLLOW = 0x00020000;
  typedef uint     Tmode_t;
  static const Tuint VS_IRUSR = 0x00000100;
  static const Tuint VS_IWUSR = 0x00000080;
  static const Tuint VS_IXUSR = 0x00000040;
  static const Tuint VS_IRGRP = 0x00000020;
  static const Tuint VS_IWGRP = 0x00000010;
  static const Tuint VS_IXGRP = 0x00000008;
  static const Tuint VS_IROTH = 0x00000004;
  static const Tuint VS_IWOTH = 0x00000002;
  static const Tuint VS_IXOTH = 0x00000001;
  static const Tint V__NR_open = 0x00000fa5;
  static const Tint V__NR_pipe = 0x00000fca;
  static const Tint V__NR_dup = 0x00000fc9;
  static const Tint V__NR_dup2 = 0x00000fdf;
  static const Tint VF_DUPFD = 0x00000000;
  static const Tint VF_GETFD = 0x00000001;
  static const Tint VF_SETFD = 0x00000002;
  static const Tint VF_GETFL = 0x00000003;
  static const Tint VF_SETFL = 0x00000004;
  static const Tint VFD_CLOEXEC = 0x00000001;
  static const Tint V__NR_fcntl = 0x00000fd7;
  static const Tint V__NR_fcntl64 = 0x0000107c;
  static const Tint V__NR_read = 0x00000fa3;
  static const Tint V__NR_write = 0x00000fa4;
  class Tiovec{
   public:
    static const size_t Size_All=8;
    typedef Tpointer_t Type_iov_base;
    static const size_t Offs_iov_base=0;
    typedef Tsize_t Type_iov_len;
    static const size_t Offs_iov_len=4;
  };
  static const Tint V__NR_writev = 0x00001032;
  typedef int32_t  Toff_t;
  typedef int64_t  Tloff_t;
  static const Tint VSEEK_SET = 0x00000000;
  static const Tint VSEEK_CUR = 0x00000001;
  static const Tint VSEEK_END = 0x00000002;
  static const Tint V__NR_lseek = 0x00000fb3;
  static const Tint V__NR__llseek = 0x0000102c;
  typedef uint32_t Tino_t;
  typedef uint64_t Tino64_t;
  typedef int64_t  Toff64_t;
  class Tdirent{
   public:
    static const size_t Size_All=268;
    typedef Tulong Type_d_ino;
    static const size_t Offs_d_ino=0;
    typedef Tulong Type_d_off;
    static const size_t Offs_d_off=4;
    typedef Tushort Type_d_reclen;
    static const size_t Offs_d_reclen=8;
    typedef Tchar Type_d_name;
    static const size_t Offs_d_name=10;
    static const size_t Step_d_name=1;
    static const size_t Size_d_name=256;
  };
  class Tdirent64{
   public:
    static const size_t Size_All=280;
    typedef Tino64_t Type_d_ino;
    static const size_t Offs_d_ino=0;
    typedef Toff64_t Type_d_off;
    static const size_t Offs_d_off=8;
    typedef Tushort Type_d_reclen;
    static const size_t Offs_d_reclen=16;
    typedef Tuchar Type_d_type;
    static const size_t Offs_d_type=18;
    typedef Tchar Type_d_name;
    static const size_t Offs_d_name=19;
    static const size_t Step_d_name=1;
    static const size_t Size_d_name=256;
  };
  static const Tint V__NR_getdents = 0x0000102d;
  static const Tint V__NR_getdents64 = 0x0000107b;
  static const Tint VTCGETS = 0x0000540d;
  static const Tint VTCSETS = 0x0000540e;
  static const Tint VTCSETSW = 0x0000540f;
  static const Tint VTCSETSF = 0x00005410;
  static const Tint VTCGETA = 0x00005401;
  static const Tint VTCSETA = 0x00005402;
  static const Tint VTCSETAW = 0x00005403;
  static const Tint VTCSETAF = 0x00005404;
  static const Tlong VTIOCGWINSZ = 0x40087468;
  class Twinsize{
   public:
    static const size_t Size_All=8;
    typedef Tushort Type_ws_row;
    static const size_t Offs_ws_row=0;
    typedef Tushort Type_ws_col;
    static const size_t Offs_ws_col=2;
    typedef Tushort Type_ws_xpixel;
    static const size_t Offs_ws_xpixel=4;
    typedef Tushort Type_ws_ypixel;
    static const size_t Offs_ws_ypixel=6;
  };
  static const Tint V__NR_ioctl = 0x00000fd6;
  class Tpollfd{
   public:
    static const size_t Size_All=8;
    typedef Tint Type_fd;
    static const size_t Offs_fd=0;
    typedef Tshort Type_events;
    static const size_t Offs_events=4;
    typedef Tshort Type_revents;
    static const size_t Offs_revents=6;
  };
  static const Tint VPOLLIN = 0x00000001;
  static const Tint VPOLLPRI = 0x00000002;
  static const Tint VPOLLOUT = 0x00000004;
  static const Tint VPOLLERR = 0x00000008;
  static const Tint VPOLLHUP = 0x00000010;
  static const Tint VPOLLNVAL = 0x00000020;
  static const Tint V__NR_poll = 0x0000105c;
  static const Tint V__NR_close = 0x00000fa6;
  static const Tint V__NR_truncate = 0x00000ffc;
  static const Tint V__NR_truncate64 = 0x00001073;
  static const Tint V__NR_ftruncate = 0x00000ffd;
  static const Tint V__NR_ftruncate64 = 0x00001074;
  static const Tint V__NR_chmod = 0x00000faf;
  typedef uint32_t Tst_dev_t;
  typedef uint64_t Tst_ino_t;
  typedef uint32_t Tst_nlink_t;
  typedef int64_t  Tst_size_t;
  typedef uint32_t Tst_blksize_t;
  typedef int64_t  Tst_blocks_t;
  class Tstat{
   public:
    static const size_t Size_All=104;
    typedef Tst_dev_t Type_st_dev;
    static const size_t Offs_st_dev=0;
    typedef Tst_ino_t Type_st_ino;
    static const size_t Offs_st_ino=16;
    typedef Tmode_t Type_st_mode;
    static const size_t Offs_st_mode=24;
    typedef Tst_nlink_t Type_st_nlink;
    static const size_t Offs_st_nlink=28;
    typedef Tuid_t Type_st_uid;
    static const size_t Offs_st_uid=32;
    typedef Tgid_t Type_st_gid;
    static const size_t Offs_st_gid=36;
    typedef Tst_dev_t Type_st_rdev;
    static const size_t Offs_st_rdev=40;
    typedef Tst_size_t Type_st_size;
    static const size_t Offs_st_size=56;
    typedef Tuint Type_st_atime_sec;
    static const size_t Offs_st_atime_sec=64;
    typedef Tuint Type_st_mtime_sec;
    static const size_t Offs_st_mtime_sec=72;
    typedef Tuint Type_st_ctime_sec;
    static const size_t Offs_st_ctime_sec=80;
    typedef Tst_blksize_t Type_st_blksize;
    static const size_t Offs_st_blksize=88;
    typedef Tst_blocks_t Type_st_blocks;
    static const size_t Offs_st_blocks=96;
  };
  typedef uint32_t Tnlink_t;
  typedef int32_t  Tblksize_t;
  typedef int64_t  Tblkcnt64_t;
  typedef uint32_t Tst_dev64_t;
  class Tstat64{
   public:
    static const size_t Size_All=160;
    typedef Tst_dev64_t Type_st_dev;
    static const size_t Offs_st_dev=0;
    typedef Tino64_t Type_st_ino;
    static const size_t Offs_st_ino=16;
    typedef Tmode_t Type_st_mode;
    static const size_t Offs_st_mode=24;
    typedef Tnlink_t Type_st_nlink;
    static const size_t Offs_st_nlink=28;
    typedef Tuid_t Type_st_uid;
    static const size_t Offs_st_uid=32;
    typedef Tgid_t Type_st_gid;
    static const size_t Offs_st_gid=36;
    typedef Tst_dev64_t Type_st_rdev;
    static const size_t Offs_st_rdev=40;
    typedef Toff64_t Type_st_size;
    static const size_t Offs_st_size=56;
    typedef Ttime_t Type_st_atime;
    static const size_t Offs_st_atime=64;
    typedef Ttime_t Type_st_mtime;
    static const size_t Offs_st_mtime=72;
    typedef Ttime_t Type_st_ctime;
    static const size_t Offs_st_ctime=80;
    typedef Tblksize_t Type_st_blksize;
    static const size_t Offs_st_blksize=88;
    typedef Tblkcnt64_t Type_st_blocks;
    static const size_t Offs_st_blocks=96;
  };
  static const Tint V__NR_stat = 0x0000100a;
  static const Tint V__NR_stat64 = 0x00001075;
  static const Tint V__NR_lstat = 0x0000100b;
  static const Tint V__NR_lstat64 = 0x00001076;
  static const Tint V__NR_fstat = 0x0000100c;
  static const Tint V__NR_fstat64 = 0x00001077;
  static const Tint V__NR_fstatfs = 0x00001004;
  static const Tint V__NR_fstatfs64 = 0x000010a0;
  static const Tint V__NR_unlink = 0x00000faa;
  static const Tint V__NR_symlink = 0x00000ff3;
  static const Tint V__NR_rename = 0x00000fc6;
  static const Tint V__NR_chdir = 0x00000fac;
  static const Tint VF_OK = 0x00000000;
  static const Tint VR_OK = 0x00000004;
  static const Tint VW_OK = 0x00000002;
  static const Tint VX_OK = 0x00000001;
  static const Tint V__NR_access = 0x00000fc1;
  static const Tint V__NR_getcwd = 0x0000106b;
  static const Tint V__NR_mkdir = 0x00000fc7;
  static const Tint V__NR_rmdir = 0x00000fc8;
  static const Tint V__NR_umask = 0x00000fdc;
  static const Tint V__NR_readlink = 0x00000ff5;
  static const Tint V__NR_socket = 0x00001057;
  static const Tint V__NR_socketpair = 0x00001058;
  static const Tint V__NR_bind = 0x00001049;
  static const Tint V__NR_accept = 0x00001048;
  static const Tint V__NR_connect = 0x0000104a;
  static const Tint V__NR_send = 0x00001052;
  typedef uint32_t Trlim_t;
  class Trlimit{
   public:
    static const size_t Size_All=8;
    typedef Trlim_t Type_rlim_cur;
    static const size_t Offs_rlim_cur=0;
    typedef Trlim_t Type_rlim_max;
    static const size_t Offs_rlim_max=4;
  };
  static const Tint VRLIMIT_AS = 0x00000006;
  static const Tint VRLIMIT_CORE = 0x00000004;
  static const Tint VRLIMIT_CPU = 0x00000000;
  static const Tint VRLIMIT_DATA = 0x00000002;
  static const Tint VRLIMIT_FSIZE = 0x00000001;
  static const Tint VRLIMIT_LOCKS = 0x0000000a;
  static const Tint VRLIMIT_MEMLOCK = 0x00000009;
  static const Tint VRLIMIT_NOFILE = 0x00000005;
  static const Tint VRLIMIT_NPROC = 0x00000008;
  static const Tint VRLIMIT_RSS = 0x00000007;
  static const Tint VRLIMIT_STACK = 0x00000003;
  static const Tlong VRLIM_INFINITY = 0x7fffffff;
  static const Tint V__NR_setrlimit = 0x00000feb;
  static const Tint V__NR_getrlimit = 0x00000fec;
  class Trusage{
   public:
    static const size_t Size_All=72;
    typedef Ttimeval Type_ru_utime;
    static const size_t Offs_ru_utime=0;
    typedef Ttimeval Type_ru_stime;
    static const size_t Offs_ru_stime=8;
    typedef Tlong Type_ru_maxrss;
    static const size_t Offs_ru_maxrss=16;
    typedef Tlong Type_ru_ixrss;
    static const size_t Offs_ru_ixrss=20;
    typedef Tlong Type_ru_idrss;
    static const size_t Offs_ru_idrss=24;
    typedef Tlong Type_ru_isrss;
    static const size_t Offs_ru_isrss=28;
    typedef Tlong Type_ru_minflt;
    static const size_t Offs_ru_minflt=32;
    typedef Tlong Type_ru_majflt;
    static const size_t Offs_ru_majflt=36;
    typedef Tlong Type_ru_nswap;
    static const size_t Offs_ru_nswap=40;
    typedef Tlong Type_ru_inblock;
    static const size_t Offs_ru_inblock=44;
    typedef Tlong Type_ru_oublock;
    static const size_t Offs_ru_oublock=48;
    typedef Tlong Type_ru_msgsnd;
    static const size_t Offs_ru_msgsnd=52;
    typedef Tlong Type_ru_msgrcv;
    static const size_t Offs_ru_msgrcv=56;
    typedef Tlong Type_ru_nsignals;
    static const size_t Offs_ru_nsignals=60;
    typedef Tlong Type_ru_nvcsw;
    static const size_t Offs_ru_nvcsw=64;
    typedef Tlong Type_ru_nivcsw;
    static const size_t Offs_ru_nivcsw=68;
  };
  static const Tint VRUSAGE_SELF = 0x00000000;
  static const Tint VRUSAGE_CHILDREN = 0xffffffff;
  static const Tint V__NR_getrusage = 0x00000fed;
  class Tutsname{
   public:
    static const size_t Size_All=390;
    typedef Tchar Type_sysname;
    static const size_t Offs_sysname=0;
    static const size_t Step_sysname=1;
    static const size_t Size_sysname=65;
    typedef Tchar Type_nodename;
    static const size_t Offs_nodename=65;
    static const size_t Step_nodename=1;
    static const size_t Size_nodename=65;
    typedef Tchar Type_release;
    static const size_t Offs_release=130;
    static const size_t Step_release=1;
    static const size_t Size_release=65;
    typedef Tchar Type_version;
    static const size_t Offs_version=195;
    static const size_t Step_version=1;
    static const size_t Size_version=65;
    typedef Tchar Type_machine;
    static const size_t Offs_machine=260;
    static const size_t Step_machine=1;
    static const size_t Size_machine=65;
  };
  static const Tint V__NR_uname = 0x0000101a;
  class T__sysctl_args{
   public:
    static const size_t Size_All=40;
    typedef Tpointer_t Type_name;
    static const size_t Offs_name=0;
    typedef Tint Type_nlen;
    static const size_t Offs_nlen=4;
    typedef Tpointer_t Type_oldval;
    static const size_t Offs_oldval=8;
    typedef Tpointer_t Type_oldlenp;
    static const size_t Offs_oldlenp=12;
    typedef Tpointer_t Type_newval;
    static const size_t Offs_newval=16;
    typedef Tsize_t Type_newlen;
    static const size_t Offs_newlen=20;
  };
  static const Tint VCTL_KERN = 0x00000001;
  static const Tint VKERN_VERSION = 0x00000004;
  static const Tint V__NR__sysctl = 0x00001039;
  static const Tint V__NR_tee = 0x000010d2;
  class Tsigset_t{
   public:
    static const size_t Size_All=16;
    typedef Tulong Type_sig;
    static const size_t Offs_sig=0;
    static const size_t Step_sig=4;
    static const size_t Size_sig=4;
  };
  class Tsigaction{
   public:
    static const size_t Size_All=24;
    typedef Tuint Type_sa_flags;
    static const size_t Offs_sa_flags=0;
    typedef Tpointer_t Type_sa_handler;
    static const size_t Offs_sa_handler=4;
    typedef Tsigset_t Type_sa_mask;
    static const size_t Offs_sa_mask=8;
  };
};
