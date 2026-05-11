#include "../include/trace_helpers.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/ptrace.h>

int read_child_string(pid_t pid, unsigned long addr, char *buf, size_t bufsz)
{
    size_t copied = 0;

    if (bufsz == 0) {
        return -1;
    }

    while (copied + 1 < bufsz) {
        long word;
        size_t i;

        errno = 0;
        word = ptrace(PTRACE_PEEKDATA, pid, (void *)(addr + copied), NULL);
        if (word == -1 && errno != 0) {
            buf[0] = '\0';
            return -1;
        }

        for (i = 0; i < sizeof(word) && copied + 1 < bufsz; i++) {
            char c = ((char *)&word)[i];
            buf[copied++] = c;
            if (c == '\0') {
                return 0;
            }
        }
    }

    buf[bufsz - 1] = '\0';
    return 1;
}
