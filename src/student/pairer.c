#include "../../include/student_api.h"

int student_pair_syscall(struct syscall_pairer *pairer,
                         const struct syscall_event *ev,
                         struct syscall_event *out)
{
    /*
     * TODO Semana 2:
     *
     * O runtime chama esta funcao duas vezes para cada syscall:
     *
     * 1. uma vez antes da syscall executar
     * 2. uma vez depois da syscall terminar
     *
     * Na primeira parada, os argumentos estao disponiveis.
     * Na segunda parada, o retorno esta disponivel.
     *
     * Seu trabalho e produzir um evento completo apenas quando ja existirem
     * as duas metade da syscall.
     *
     * Dicas:
     * - ev->entering == 1 indica entrada de syscall.
     * - ev->entering == 0 indica saida de syscall.
     * - para comecar, assuma apenas um processo monitorado.
     *
     * Retorne:
     * 1 se out contem uma syscall completa
     * 0 se ainda nao ha syscall completa
     * -1 se a sequencia de eventos parece invalida
     */
    
    // Armazena o estado da última entrada de syscall (para um único processo)
    static struct syscall_event entry_ev;
    static int has_entry = 0;

    if (ev->entering == 1) {
        // Se já havia uma entrada guardada sem uma saída correspondente, a sequência é inválida
        if (has_entry) {
            return -1;
        }
        // Guarda os dados da entrada (argumentos) e aguarda a saída
        entry_ev = *ev;
        has_entry = 1;
        return 0;
    } 
    else if (ev->entering == 0) {
        // Se chegou uma saída mas não temos o registro da entrada correspondente, é inválido
        // Usando os nomes corretos do arquivo syscall_event.h: syscall_no e pid
        if (!has_entry || entry_ev.syscall_no != ev->syscall_no || entry_ev.pid != ev->pid) {
            return -1;
        }
        // Combina os dados: copia a entrada (com os argumentos) e atualiza com o retorno da saída
        *out = entry_ev;
        out->entering = 0;
        
        // Usando o nome correto do arquivo syscall_event.h: ret
        out->ret = ev->ret;
        
        // Reseta o estado para a próxima syscall
        has_entry = 0;
        return 1;
    }

    return -1;
}
