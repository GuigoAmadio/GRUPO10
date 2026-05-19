#include "../include/cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int find_separator(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--") == 0) {
            return i;
        }
    }

    return -1;
}

void print_usage(FILE *out, const char *prog)
{
    fprintf(out,
            "uso: %s trace [opcoes] -- programa [argumentos...]\n"
            "\n"
            "opcoes:\n"
            "  --raw-events    Semana 4: imprime eventos crus de entrada/saida\n"
            "\n"
            "exemplos:\n"
            "  %s trace -- /bin/echo oi\n"
            "  %s trace --raw-events -- ./tests/targets/hello_write\n",
            prog, prog, prog);
}

int parse_args(int argc, char **argv, struct trace_options *opts)
{
    int sep;
    int i;

    memset(opts, 0, sizeof(*opts));

    if (argc < 2 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage(stdout, argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "trace") != 0) {
        fprintf(stderr, "erro: comando desconhecido: %s\n", argv[1]);
        return -1;
    }

    sep = find_separator(argc, argv);
    if (sep < 0 || sep + 1 >= argc) {
        fprintf(stderr, "erro: informe o programa alvo depois de --\n");
        return -1;
    }

    for (i = 2; i < sep; i++) {
        if (strcmp(argv[i], "--raw-events") == 0) {
            opts->raw_events = 1;
        } else {
            fprintf(stderr, "erro: opcao desconhecida: %s\n", argv[i]);
            return -1;
        }
    }

    opts->target_argv = &argv[sep + 1];
    return 0;
}
