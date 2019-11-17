#ifndef VMEMH
#define VMEMH

#include <stdbool.h>

typedef struct vm vm;
typedef struct page page;
typedef struct frame frame;

struct page{
    unsigned frame_index;
    bool valid;
    bool accessed_once;
};

struct frame{
    unsigned referenced_page;
    unsigned last_access;
};

// Recebe o array de frames e páginas, remove um quadro do array de quadros de acordo
// com uma política, invalida a página correspondente e retorna o índice do quadro removido
typedef unsigned (*page_replacement_f)(frame* frames, page* pages, unsigned num_frames, unsigned num_pages, unsigned* entrance_order, unsigned* ent_size);

vm* create_vm(unsigned page_size, unsigned memory_size, page_replacement_f replacement_f);
void mem_access(vm* virtual_mem, unsigned address, char rw, unsigned time);
void delete_vm(vm* virtual_mem);

void get_statistics(vm* v_mem, unsigned* acesses, unsigned* used_pages, unsigned* faults, unsigned* hits);

#endif