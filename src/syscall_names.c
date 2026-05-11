#include "../include/syscall_names.h"

#include <string.h>
#include <sys/syscall.h>

struct syscall_name_entry {
    long no;
    const char *name;
};

static const struct syscall_name_entry syscall_table[] = {
#ifdef SYS_read
    {SYS_read, "read"},
#endif
#ifdef SYS_write
    {SYS_write, "write"},
#endif
#ifdef SYS_open
    {SYS_open, "open"},
#endif
#ifdef SYS_openat
    {SYS_openat, "openat"},
#endif
#ifdef SYS_close
    {SYS_close, "close"},
#endif
#ifdef SYS_execve
    {SYS_execve, "execve"},
#endif
#ifdef SYS_exit
    {SYS_exit, "exit"},
#endif
#ifdef SYS_exit_group
    {SYS_exit_group, "exit_group"},
#endif
#ifdef SYS_fork
    {SYS_fork, "fork"},
#endif
#ifdef SYS_clone
    {SYS_clone, "clone"},
#endif
#ifdef SYS_wait4
    {SYS_wait4, "wait4"},
#endif
#ifdef SYS_brk
    {SYS_brk, "brk"},
#endif
#ifdef SYS_mmap
    {SYS_mmap, "mmap"},
#endif
#ifdef SYS_mprotect
    {SYS_mprotect, "mprotect"},
#endif
#ifdef SYS_munmap
    {SYS_munmap, "munmap"},
#endif
#ifdef SYS_access
    {SYS_access, "access"},
#endif
#ifdef SYS_newfstatat
    {SYS_newfstatat, "newfstatat"},
#endif
#ifdef SYS_arch_prctl
    {SYS_arch_prctl, "arch_prctl"},
#endif
#ifdef SYS_set_tid_address
    {SYS_set_tid_address, "set_tid_address"},
#endif
#ifdef SYS_set_robust_list
    {SYS_set_robust_list, "set_robust_list"},
#endif
#ifdef SYS_prlimit64
    {SYS_prlimit64, "prlimit64"},
#endif
#ifdef SYS_getrandom
    {SYS_getrandom, "getrandom"},
#endif
#ifdef SYS_rseq
    {SYS_rseq, "rseq"},
#endif
};

const char *syscall_name(long syscall_no)
{
    size_t i;

    for (i = 0; i < sizeof(syscall_table) / sizeof(syscall_table[0]); i++) {
        if (syscall_table[i].no == syscall_no) {
            return syscall_table[i].name;
        }
    }

    return "unknown";
}

long syscall_number_by_name(const char *name)
{
    size_t i;

    for (i = 0; i < sizeof(syscall_table) / sizeof(syscall_table[0]); i++) {
        if (strcmp(syscall_table[i].name, name) == 0) {
            return syscall_table[i].no;
        }
    }

    return -1;
}
