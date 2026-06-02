#include "../include/trace_runtime.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#if !defined(__x86_64__)
#error "Este runtime didatico suporta apenas Linux x86_64."
#endif

static void fill_event_from_regs(pid_t pid,
                                 int entering,
                                 const struct user_regs_struct *regs,
                                 struct syscall_event *ev)
{
    /*
     * TODO Semana 4:
     *
     * Preencha struct syscall_event usando os registradores x86_64.
     *
     * Dicas:
     * - regs->orig_rax contem o numero da syscall.
     * - regs->rax contem o retorno, valido na saida.
     * - os seis argumentos ficam em rdi, rsi, rdx, r10, r8 e r9.
     * - ev->entering deve copiar o parametro entering.
     */
    memset(ev, 0, sizeof(*ev));
    ev->pid = pid;
    ev->entering = entering;
}

static pid_t launch_tracee(char *const argv[])
{
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        perror("erro ao criar fork");
        return -1;
    }

    if (pid == 0) {
        // estamos no filho ----

        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
            perror("ptrace(PTRACE_TRACEME)");
            _exit(1);
        }

        raise(SIGSTOP);

        execvp(argv[0], argv);

        perror("execvp");
        _exit(1);
    }

    // estamos no pai ----
    // retorna o pid do filho.
    return pid;
}

static int wait_for_initial_stop(pid_t child)
{
    int status;

    if (waitpid(child, &status, 0) < 0) {
        perror("waitpid");
        return -1;
    }

    if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGSTOP) {
        return 0;  

  }

    return -1;
}

static int configure_trace_options(pid_t child)
{
    /*
     * TODO Semana 3:
     * Configure PTRACE_O_TRACESYSGOOD com PTRACE_SETOPTIONS.
     */
    if (ptrace(PTRACE_SETOPTIONS, child, NULL, PTRACE_O_TRACESYSGOOD) < 0) {
        perror("ptrace(PTRACE_SETOPTIONS)");
        return -1;
    }
    return 0;
}

static int resume_until_next_syscall(pid_t child, int signal_to_deliver)
{
    /*
     * TODO Semana 3:
     * Use ptrace(PTRACE_SYSCALL, ...) para deixar o filho executar ate a
     * proxima entrada ou saida de syscall.
     */
    if (ptrace(PTRACE_SYSCALL, child, NULL, (void *)(long)signal_to_deliver) < 0) {
        perror("ptrace(PTRACE_SYSCALL)");
        return -1;
    }
    return 0;
}

static int wait_for_syscall_stop(pid_t child, int *status)
{
    /*
     * TODO Semana 3:
     * Espere o filho com waitpid().
     */
    if (waitpid(child, status, 0) < 0) {
        perror("waitpid");
        return -1;
    }

    if (WIFEXITED(*status) || WIFSIGNALED(*status)) {
        return 0;
    }

    if (WIFSTOPPED(*status)) {
        int sig = WSTOPSIG(*status);
        // Com PTRACE_O_TRACESYSGOOD, o sinal de parada da syscall terá o bit 0x80 (SIGTRAP | 0x80)
        if (sig == (SIGTRAP | 0x80)) {
            return 1;
        }
    }

    return -1;
}
int trace_program(char *const argv[],
                  trace_observer_fn observer,
                  void *userdata)
{
    pid_t child;
    int status = 0;
    int entering = 1;

    if (argv == NULL || argv[0] == NULL) {
        fprintf(stderr, "erro: programa alvo ausente\n");
        return -1;
    }

    child = launch_tracee(argv);
    if (child < 0) {
        return -1;
    }

    if (wait_for_initial_stop(child) < 0) {
        return -1;
    }
// ---------------------------------------------------------------------------
    if (configure_trace_options(child) < 0) {
        return -1;
    }

    if (resume_until_next_syscall(child, 0) < 0) {
        return -1;
    }

    while (1) {
        struct user_regs_struct regs;
        struct syscall_event ev;
        int stop_kind;

        stop_kind = wait_for_syscall_stop(child, &status);
        if (stop_kind < 0) {
            return -1;
        }
        if (stop_kind == 0) {
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            }
            if (WIFSIGNALED(status)) {
                return 128 + WTERMSIG(status);
            }
            return 0;
        }

        /*
         * TODO Semana 4:
         *
         * Use PTRACE_GETREGS para preencher regs.
         * Depois chame fill_event_from_regs() e observer().
         */
        memset(&regs, 0, sizeof(regs));
        fill_event_from_regs(child, entering, &regs, &ev);
        if (observer != NULL) {
            observer(&ev, userdata);
        }

        entering = !entering;

        if (resume_until_next_syscall(child, 0) < 0) {
            return -1;
        }
    }
}
