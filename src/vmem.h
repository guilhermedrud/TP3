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
    char last_access_mode;
    bool acessed_recently;
};

struct frame{
    unsigned referenced_page;
    unsigned last_access;
};

struct vm_stats{
    unsigned page_hits;
    unsigned page_faults;
    unsigned read_accesses;
    unsigned write_accesses;
    unsigned dirty_pages_written;
    unsigned used_pages;
};

// Recebe o array de frames e páginas, remove um quadro do array de
// quadros de acordo com uma política e retorna o índice do quadro removido
typedef unsigned (*page_replacement_f)(frame* frames, page* pages, unsigned num_frames, unsigned num_pages, unsigned* entrance_order, unsigned* ent_size);

vm* create_vm(unsigned page_size, unsigned memory_size, page_replacement_f replacement_f);
void mem_access(vm* virtual_mem, unsigned address, char rw);
void delete_vm(vm* virtual_mem);

struct vm_stats get_statistics(vm* v_mem);

#endif