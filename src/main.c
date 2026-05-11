#include "../include/cli.h"
#include "../include/student_api.h"
#include "../include/trace_runtime.h"

#include <stdio.h>

struct trace_state {
    int raw_events;
    struct syscall_pairer pairer;
};

static void trace_observer(const struct syscall_event *ev, void *userdata)
{
    struct trace_state *state = userdata;
    struct syscall_event completed;
    char line[512];
    int ready;

    if (state->raw_events) {
        student_debug_raw_event(ev, line, sizeof(line));
        puts(line);
        return;
    }

    ready = student_pair_syscall(&state->pairer, ev, &completed);
    if (ready <= 0) {
        return;
    }

    student_format_event(&completed, line, sizeof(line));
    puts(line);
}

int main(int argc, char **argv)
{
    struct trace_options opts;
    struct trace_state state = {0};
    int rc;

    rc = parse_args(argc, argv, &opts);
    if (rc > 0) {
        return 0;
    }
    if (rc < 0) {
        return 2;
    }

    state.raw_events = opts.raw_events;
    rc = trace_program(opts.target_argv, trace_observer, &state);
    return rc < 0 ? 1 : rc;
}
