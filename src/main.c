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
    if(strcmp(algorithm,"rand")==0){
         f = random_page;
    }
    else if(strcmp(algorithm,"lru")==0){
        f = lru;
    }
    else if(strcmp(algorithm,"2a")==0){
        f = second_chance;
    }
    else if(strcmp(algorithm,"fifo")==0){
        f = fifo;
    }
    //page_replacement_f f = random_page; //TODO: Selecionar qual algoritmo usar
    vm* v_mem = create_vm(pagesize,total_memory, f);


    // Itera sobre entrada
    printf("Executando o simulador...\n");
    unsigned count = 0;
    while (fscanf(input_file, "%x %c",&logical_adress, &rw)!=EOF) {
        //printf("%x %c\n",logical_adress, rw);
        count++;
        mem_access(v_mem, logical_adress, rw, count);
    }

    print_parameters(input, total_memory, pagesize, algorithm);
    unsigned acesses,used_pages,faults,hits;
    get_statistics(v_mem, &acesses, &used_pages, &faults, &hits);
    printf("Acessos: %u\nNum. Paginas usadas: %u\nPage faults: %10u\nPage hits:   %10u\n", acesses, used_pages, faults, hits);
}