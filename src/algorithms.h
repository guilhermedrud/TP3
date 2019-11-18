#ifndef ALG_H
#define ALG_H

#include "vmem.h"
#include <time.h>
#include <stdlib.h>
#include <assert.h>

void remove_and_shift(unsigned* array, int position, unsigned* size){
    for(int i=position; i<*size-1; i++ ){
        array[i]=array[i+1];
    }
    *size--;
}

unsigned fifo(frame* frames, page* pages, unsigned num_frames, unsigned num_pages, unsigned* entrance_order, unsigned* ent_size){
    unsigned first_addres = entrance_order[0]; // pega primeiro da fila
    remove_and_shift(entrance_order,0,ent_size); //shifta fila
    return pages[first_addres].frame_index;
}

unsigned random_page(frame* frames, page* pages, unsigned num_frames, unsigned num_pages, unsigned* entrance_order, unsigned* ent_size){
    unsigned r;
    do{
       r = entrance_order[(rand() % *ent_size)];
    } while (pages[r].valid == false);
    
    remove_and_shift(entrance_order,*ent_size-1,ent_size);
    return pages[r].frame_index;
}

unsigned second_chance(frame* frames, page* pages, unsigned num_frames, unsigned num_pages, unsigned* entrance_order, unsigned* ent_size){
    unsigned current_page;
    for (unsigned i = 0; i < *ent_size; i++){
        current_page = entrance_order[i];
        if(pages[current_page].acessed_recently == true){
            pages[current_page].acessed_recently == false;
        }
        else{
            remove_and_shift(entrance_order,i,ent_size); //shifta fila
            return pages[current_page].frame_index;
        }
    }
    // se todos são acessed_recently, remove o primeiro:
    unsigned first_addres = entrance_order[0];
    remove_and_shift(entrance_order,0,ent_size);
    return pages[first_addres].frame_index;
}

unsigned lru(frame* frames, page* pages, unsigned num_frames, unsigned num_pages, unsigned* entrance_order, unsigned* ent_size){
    unsigned oldest = frames[0].last_access;
    unsigned oldest_index = 0;
    remove_and_shift(entrance_order,*ent_size-1,ent_size);
    for(int i=1; i<num_frames; i++){
        if((frames[i].last_access < oldest)){
            oldest = frames[i].last_access;
            oldest_index=i;
        }
    }
    return oldest_index;
}

#endif