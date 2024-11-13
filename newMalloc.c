#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  // for uintptr_t
#define MAX_CAP 64000
#define MAX_ALLOC 1024

char heap[MAX_CAP];
size_t maxi = 0;

typedef struct chunk {
    void *ptr; 
    size_t size;
} Chunk; 

typedef struct chunkList {
    Chunk list[MAX_ALLOC];
    size_t count;
} ChunkList;

ChunkList allocated={0}; 
ChunkList freed={0}; 
Chunk ini={
    .ptr=heap,
    .size = MAX_CAP
};
void memDump(ChunkList *l) {
    for (int i = 0; i < l->count; i++) {
        printf("S.NO = %d = %p has size = %zu\n", i, l->list[i].ptr, l->list[i].size);
    }
}

int comp(const void *p1, const void *p2) {
    const Chunk *p = p1;
    const Chunk *q = p2;
    // Safely compare pointers
    return (p->ptr-  q->ptr);
}

int chunk_index_find(void *ptrs, ChunkList *l) {
   // printf("%p\n", ptrs);
    Chunk key = { .ptr = ptrs };
    Chunk *rs = bsearch(&key, l->list, l->count, sizeof(l->list[0]), comp);
    if (rs != 0) {
        int index = (rs - l->list);
        return index;
    }
    return -1;
}
void initialize(ChunkList *list)
{
    list->list[list->count]=ini;
    list->count=1;
    
}
void InsertList(ChunkList *l, void *ptr, size_t sizee) {
    size_t sz = l->count;
    Chunk *list = l->list;
    list[sz].ptr = ptr;
    list[sz].size = sizee;

    // Insert new chunk while maintaining sorted order by pointer value
    for (int i = sz - 1; i >= 0 && list[i].ptr > list[i + 1].ptr; i--) {
        Chunk temp = list[i];
        list[i] = list[i + 1];
        list[i + 1] = temp;
    }
    l->count = l->count + 1;
}

void removeChunk(ChunkList *l , int index)
{
  for(int i = index ; i < l->count -1 ; i++)
    {
        l->list[i] = l->list[i+1];
    }
    l->count = l->count -1 ;
}
void *alloc(size_t sz) {
   
    void *ptr = NULL ; 
    for(int i = 0 ; i < freed.count ; i++){
        if(freed.list[i].size >= sz){
            Chunk *c = &freed.list[i];
          // Chunk c = freed.list[i];
           ptr = c->ptr;
           c->size-= sz;
        removeChunk(&freed , i);
        //memDump(&freed);
        InsertList(&allocated,c->ptr,sz);
       if(c->size > 0)
       {
        InsertList(&freed , c->ptr + sz , c->size);
       }
        }
    }
    return ptr;
}
ChunkList temp ; 
void merge(ChunkList *list){
    if(list->count == 0)return ;
    Chunk c = list->list[0];
    for(int i= 1; i < list->count ; i++)
    {
        if(c.ptr + c.size == list->list[i].ptr)
        {   
            c.size+=list->list[i].size;
        }
        else{
                temp.list[temp.count] = c ;
                temp.count++;
                c = list->list[i];
            }
    }
    temp.list[temp.count]=c;
    temp.count++;

    freed = temp;
    temp.count =0;

    
}
void frees(void *ptr){
    
    int index = chunk_index_find(ptr  , &allocated);
    if(index >=0)
    {
        size_t sz = allocated.count;
        Chunk c  = allocated.list[index];
        for(int i = index ; i<sz -1; i++){
            allocated.list[i]=allocated.list[i+1];
        }
        InsertList(&freed,c.ptr,c.size);
        allocated.count = allocated.count-1;
    }
   merge(&freed);
}

int main() {
    char *arr[11];
    initialize(&freed);
    for(int i = 1 ; i <= 10 ; i++){
        
        arr[i] = alloc(i);

    }
    for(int i = 1 ; i <= 10 ; i++){
        if(i%2==0)frees(arr[i]);
    }
    
    for(int i = 1 ; i <= 10 ; i++){
        if(i%2!=0)frees(arr[i]);
    }
   
    memDump(&freed);
    
    return 0;
}
