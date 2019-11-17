#include "vmem.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

struct vm{
    unsigned num_frames;
    unsigned page_size;
    unsigned max_pages;
    unsigned page_id_size;
    page_replacement_f replacement_f;

    unsigned page_hits;
    unsigned page_faults;
    unsigned memory_accesses;
    unsigned used_pages;
    
    unsigned avaliable_frame_index;

    unsigned* entrance_order; // ordem de entrada das paginas (usado no fifo e 2a chance)
    unsigned entrance_size; // tamanho de vetor de entradas

    frame* frame_array;
    page* page_array;
};


unsigned page_size_to_id_size(unsigned page_size){
  unsigned s, tmp;
  /* Derivar o valor de s: */
  tmp = page_size*1024;
  s = 0;
  while (tmp>1) {
    tmp = tmp>>1;
    s++;
  }
  return s;
}

vm* create_vm(unsigned page_size, unsigned memory_size, page_replacement_f replacement_f) {
    vm* new_vm = calloc(1, sizeof(vm));

    new_vm->replacement_f = replacement_f;

    new_vm->num_frames = memory_size/page_size;
    new_vm->frame_array = calloc(new_vm->num_frames, sizeof(frame));
    new_vm->avaliable_frame_index = 1;

    new_vm->page_size = page_size;
    new_vm->page_id_size = page_size_to_id_size(page_size);
    new_vm->max_pages = pow(2, 32-new_vm->page_id_size);
    new_vm->page_array = calloc(new_vm->max_pages, sizeof(page));

    new_vm->entrance_order = calloc(new_vm->max_pages, sizeof(page));
    new_vm->entrance_size = 0;

    printf("num_frames %u\n", new_vm->num_frames);
    printf("page_size %u\n", new_vm->page_size);
    printf("page_id_size %u\n", new_vm->page_id_size);
    printf("max_pages %u\n", new_vm->max_pages);

    return new_vm;
}

void mem_access(vm* v_mem, unsigned address, char rw, unsigned time) {
    unsigned page_id = address >> v_mem->page_id_size;
    page* p = &(v_mem->page_array[page_id]);

    if (!p->accessed_once){
        p->accessed_once = true;
        v_mem->used_pages++;
    }

    if (!p->valid) {
        v_mem->page_faults++;
        v_mem->entrance_order[v_mem->entrance_size] = page_id;
        v_mem->entrance_size++;
        
        // Ainda existem quadros não preenchidos
        if (v_mem->avaliable_frame_index > 0) {
            p->valid = true;
            p->frame_index = v_mem->avaliable_frame_index-1;
            v_mem->frame_array[p->frame_index].referenced_page = page_id;
            v_mem->frame_array[p->frame_index].last_access = time;
            
            v_mem->avaliable_frame_index++;
            if (v_mem->avaliable_frame_index >= v_mem->num_frames + 1)
                v_mem->avaliable_frame_index = 0;
        } 
        
        // Todos os quadros foram preenchidos, chama o algoritmo de reposição
        else {
            p->frame_index = v_mem->replacement_f(v_mem->frame_array, v_mem->page_array, 
                                                  v_mem->num_frames, v_mem->max_pages,
                                                  v_mem->entrance_order, &v_mem->entrance_size);
            p->valid = true;
            v_mem->frame_array[p->frame_index].referenced_page = page_id;
            v_mem->frame_array[p->frame_index].last_access = time;
        }
    } else {
        v_mem->page_hits++;
        v_mem->frame_array[p->frame_index].last_access = time;
    }

    v_mem->memory_accesses++;
}

void delete_vm(vm* v_mem) {
    free(v_mem->frame_array);
    free(v_mem->page_array);
}

void get_statistics(vm* v_mem, unsigned* acesses, unsigned* used_pages, unsigned* faults, unsigned* hits) {
    if (acesses) *acesses = v_mem->memory_accesses;
    if (faults) *faults = v_mem->page_faults;
    if (hits) *hits = v_mem->page_hits;
    if (used_pages) *used_pages = v_mem->used_pages;
}