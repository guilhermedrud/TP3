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

    struct vm_stats stats;
    
    unsigned time;
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

    return new_vm;
}

void mem_access(vm* v_mem, unsigned address, char rw) {
    unsigned page_id = address >> v_mem->page_id_size;
    page* p = &(v_mem->page_array[page_id]);

    if (!p->accessed_once){
        p->accessed_once = true;
        v_mem->stats.used_pages++;
    }

    if (!p->valid) {
        v_mem->stats.page_faults++;
        v_mem->entrance_order[v_mem->entrance_size] = page_id;
        v_mem->entrance_size++;
        
        // Ainda existem quadros não preenchidos
        if (v_mem->avaliable_frame_index > 0) {
            p->valid = true;
            p->last_access_mode = rw;
            p->frame_index = v_mem->avaliable_frame_index-1;
            v_mem->frame_array[p->frame_index].referenced_page = page_id;
            v_mem->frame_array[p->frame_index].last_access = v_mem->time;
            
            v_mem->avaliable_frame_index++;
            if (v_mem->avaliable_frame_index >= v_mem->num_frames + 1)
                v_mem->avaliable_frame_index = 0;
        } 
        
        // Todos os quadros foram preenchidos, chama o algoritmo de reposição
        else {
            p->frame_index = v_mem->replacement_f(v_mem->frame_array, v_mem->page_array, 
                                                  v_mem->num_frames, v_mem->max_pages,
                                                  v_mem->entrance_order, &v_mem->entrance_size);

            page* replaced_page = &v_mem->page_array[v_mem->frame_array[p->frame_index].referenced_page];
            replaced_page->valid = false;

            if (replaced_page->last_access_mode == 'W')
                v_mem->stats.dirty_pages_written++;

            p->valid = true;
            p->last_access_mode = rw;
            v_mem->frame_array[p->frame_index].referenced_page = page_id;
            v_mem->frame_array[p->frame_index].last_access = v_mem->time;
        }
    } else {
        v_mem->frame_array[p->frame_index].last_access = v_mem->time;
        v_mem->stats.page_hits++;
    }
    p->acessed_recently = true;
    v_mem->time++;
    if (rw == 'R')
        v_mem->stats.read_accesses++;
    else
        v_mem->stats.write_accesses++;
}

void delete_vm(vm* v_mem) {
    free(v_mem->frame_array);
    free(v_mem->page_array);
}

struct vm_stats get_statistics(vm* v_mem) {
    return v_mem->stats;
}