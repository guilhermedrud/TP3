#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "vmem.h"
#include "algorithms.h"

void print_parameters(char* input, int memory_size, int page_size, char* alg){
    printf("Arquivo de entrada: %s\nTamanho da memoria: %d KB\nTamanho das paginas: %d KB\nTecnica de reposicao: %s\n",input, memory_size, page_size, alg);
}

int main(int argc,char *argv[]){
    srand(time(NULL));
    
    char *algorithm = argv[1], *input = argv[2];
    int pagesize = atoi(argv[3]), total_memory = atoi(argv[4]) ;

    FILE* input_file;
    input_file = fopen(input,"r");
    unsigned logical_adress;
    char rw;
    int endereco;

    page_replacement_f f;
    if(strcmp(algorithm,"random")==0)
         f = random_page;
    else if(strcmp(algorithm,"lru")==0)
        f = lru;
    else if(strcmp(algorithm,"2a")==0)
        f = second_chance;
    else if(strcmp(algorithm,"fifo")==0)
        f = fifo;
    else {
        printf("Error: invalid algorithm (%s)\n", algorithm); exit(1);
    }
    
    vm* v_mem = create_vm(pagesize,total_memory, f);


    // Itera sobre entrada
    printf("Executando o simulador...\n");
    while (fscanf(input_file, "%x %c",&logical_adress, &rw)!=EOF) {
        mem_access(v_mem, logical_adress, rw);
    }

    print_parameters(input, total_memory, pagesize, algorithm);
    struct vm_stats stats = get_statistics(v_mem);
    printf("Total de leituras: %8u\n", stats.read_accesses);
    printf("Total de escritas: %8u\n", stats.write_accesses);
    printf("Total de acessos:  %8u\n",  stats.read_accesses + stats.write_accesses);
    printf("Total de page faults: %8u\n",  stats.page_faults);
    printf("Total de page hits:   %8u\n",  stats.page_hits);
    printf("Total de paginas sujas escritas: %u\n",  stats.dirty_pages_written);

    delete_vm(v_mem);
    return 0;
}