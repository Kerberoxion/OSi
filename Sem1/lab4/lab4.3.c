#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>

struct Heap{
    int size;
    struct Heap *next;
    void* pointer;
};

struct Heap* head = NULL;

int ceil_4096(int num){
    return num % 4096 == 0 ? num: num+4095;
}

void *my_malloc(int size){
    if(size < 0){
        printf("Error: size less than zero\n");
        return NULL;
    }
    if(!head){
        head = mmap(&head, sizeof(struct Heap), PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
        if(head == -1){
            perror("Error: ");
            return 0;
        }
        head->size = size;

        int size_mmap = ceil_4096(size)/4096;
        head->pointer = sbrk(size_mmap*4096);
        if(head->pointer == -1)
            return NULL;

        return head->pointer;
    }
    else{
        struct Heap* node = mmap(&head, sizeof(struct Heap), PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
        if(head == -1){
            perror("Error: ");
            return 0;
        }
        struct Heap* tmp = head->next;
        int size_mmap = ceil_4096(size)/4096;
        while(tmp && tmp->next) {
            tmp = tmp->next;
        }

        if(tmp){
            node->pointer = sbrk(size_mmap * 4096);
            if(node->pointer == -1){
                return NULL;
            }
            tmp->next = node;
        }
        else{
            node->pointer = sbrk(size_mmap * 4096);
            if(node->pointer == -1){
                return NULL;
            }
            head->next = node;
        }

        node->size = size;

        return node->pointer;
    }
}

struct Heap* Remove(struct Heap* first){
    struct Heap* heap = first->next;
    munmap(first, sizeof(struct Heap));
    return heap;
}

void my_free(void* pointer){
    if(pointer == head->pointer){
        int size = head->size;
        int size_mmap = ceil_4096(size)/4096;
        head->size = 0;
        int err = munmap(pointer, size_mmap * 4096);
        if(err == -1){
            perror("Error: ");
            return;
        }
        head = Remove(head);


   }
    else if(pointer){
        struct Heap* node = head->next;
        while(node->next && node->pointer!=pointer){
            node = node->next;
        }

        if(pointer == node->pointer){
            int size_mmap = ceil_4096(node->size)/4096;
            struct Heap* tmp = head->next;
            while(tmp->next && tmp->pointer !=pointer && tmp->next->pointer!=pointer ){
                tmp = tmp->next;
            }
            node->size = 0;

            if(tmp->next)
                tmp->next = node->next;
            else{
                head->next = node->next;
            }
            int err = munmap(pointer,size_mmap * 4096);
            if(err == -1){
                perror("Error: ");
                return;
            }
            Remove(node);
        }
    } 
}

int main (int argc, char* argv[])
{
    int* a = my_malloc(-1*sizeof(int));
   // a[3] = 100;
    printf("%p, %d\n", a, getpid());

    int* b = my_malloc(1024*1024*1024/4 * sizeof(int));
    if(b==NULL){
        return 0;
    }
    b[5] = 60;
    if(a==NULL){
        return 0;
    }

    printf("array %d\n", b[5]);

    my_free(b);
    my_free(a);

    printf ("Hello, World!\n");
    return 0;
}
