#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>

typedef struct _Node {
    char value[100];
    struct _Node* next;
    pthread_spinlock_t sync;
} Node;

typedef struct _Storage {
    Node *first;
    pthread_spinlock_t firstNodeLock;
} Storage;

int IncCnt = 0;
int DecCnt = 0;
int EqCnt = 0;
int SwapCnt = 0;


void destroyStorage(Storage *s) {
    if (s->first == NULL) return;

    while(s->first){
        Node *tmp = s->first;
        s->first = s->first->next;

        pthread_spin_destroy(&tmp->sync);
        free(tmp);

    }
    pthread_spin_destroy(&s->firstNodeLock);

}

void addNode(Storage *storage, char* value) {

    Node *new_node = malloc(sizeof(Node));
    if (!new_node) {
        printf("Cannot allocate memory for new node\n");
        abort();
    }

    strncpy(new_node->value, value, 100);
    new_node->value[99] = '\0';
    pthread_spin_init(&new_node->sync, 0);

    new_node->next = storage->first;
    storage->first = new_node;

}


void* countIncPairs(void* arg) {
    Storage* storage = (Storage*)arg;
    while (1) {
        int err = pthread_spin_lock(&storage->firstNodeLock);
        if (err) {
            printf("pthread_spin_lock() failed: %s\n", strerror(err));
            return NULL;
        }


        pthread_spin_lock(&storage->first->sync);

        Node* current = storage->first;
        pthread_spin_unlock(&storage->firstNodeLock);
        Node* next = NULL;

        while (current->next != NULL) {
            pthread_spin_lock(&current->next->sync);
            next = current->next;

            if (strlen(current->value) < strlen(next->value)) {
                IncCnt++;
            }

            pthread_spin_unlock(&current->sync);
            current = next;
        }
        pthread_spin_unlock(&current->sync);
    }
    return NULL;
}

void* countDecPairs(void* arg) {
    Storage* storage = (Storage*)arg;
    while (1) {
        int err = pthread_spin_lock(&storage->firstNodeLock);
        if (err) {
            printf(" pthread_spin_lock() failed: %s\n", strerror(err));
            return NULL;
        }

        pthread_spin_lock(&storage->first->sync);

        Node* current = storage->first;
        pthread_spin_unlock(&storage->firstNodeLock);
        Node* next = NULL;

        while (current->next != NULL) {
            pthread_spin_lock(&current->next->sync);
            next = current->next;

            if (strlen(current->value) > strlen(next->value)) {
                DecCnt++;
            }

            pthread_spin_unlock(&current->sync);
            current = next;
        }
        pthread_spin_unlock(&current->sync);
    }
    return NULL;
}

void* countEqPairs(void* arg) {
    Storage* storage = (Storage*)arg;
    while (1) {
        int err = pthread_spin_lock(&storage->firstNodeLock);
        if (err) {
            printf("writer: pthread_spin_lock() failed: %s\n", strerror(err));
            return NULL;
        }


        pthread_spin_lock(&storage->first->sync);

        Node* current = storage->first;
        pthread_spin_unlock(&storage->firstNodeLock);
        Node* next = NULL;

        while (current->next != NULL) {
            pthread_spin_lock(&current->next->sync);
            next = current->next;

            if (strlen(current->value) == strlen(next->value)) {
                EqCnt++;
            }

            pthread_spin_unlock(&current->sync);
            current = next;
        }
        pthread_spin_unlock(&current->sync);
    }
    return NULL;
}

void* swapNodes(void* arg) {
    Storage* storage = (Storage*)arg;
    while (1) {
        pthread_spin_lock(&storage->firstNodeLock);
        pthread_spin_lock(&storage->first->sync);

        Node* prev = storage->first;
        pthread_spin_unlock(&storage->firstNodeLock);
        Node* current = NULL;
        Node* next = NULL;

        while (prev->next != NULL) {
            pthread_spin_lock(&prev->next->sync);
            current = prev->next;

            if (current->next != NULL) {
                pthread_spin_lock(&current->next->sync);
                next = current->next;

                if (next != NULL && rand() % 100 < 50) {
                    prev->next = next;
                    current->next = next->next;
                    next->next = current;
                }

                SwapCnt++;
                pthread_spin_unlock(&next->sync);
            }
            pthread_spin_unlock(&prev->sync);
            prev = current;
        }
        pthread_spin_unlock(&prev->sync);
    }
    return NULL;
}

void* monitor(void* arg) {
    while (1) {
        printf("IncCnt = %d\nDecCnt = %d\nEqCnt = %d\nSwapCnt = %d\n",
                IncCnt,
                DecCnt,
                EqCnt,
                SwapCnt);
        sleep(1);
    }
}

void generateRandomString(char *str, int length) {
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < length - 1; i++) {
        int key = rand() % (int)(sizeof(charset) - 1);
        str[i] = charset[key];
    }
    str[length - 1] = '\0';
}

void generateList(Storage *storage, int numNodes) {
    
    int err = pthread_spin_init(&storage->firstNodeLock, 0);
    if(err){
        printf("spin_iinit failed: %s\n", strerror(err));
    }

    for (int i = 0; i < numNodes; i++) {
        char randomString[100];
        generateRandomString(randomString, rand() % 100);
        addNode(storage, randomString);
    }
}

int main() {
    Storage storage;
    storage.first = NULL;
    generateList(&storage, 10000);

    pthread_t inc_thread, dec_thread, eq_thread, swap_thread1, swap_thread2, swap_thread3, monitor_thread;
    pthread_create(&inc_thread, NULL, countIncPairs, (void* )&storage);
    pthread_create(&dec_thread, NULL, countDecPairs, (void* )&storage);
    pthread_create(&eq_thread, NULL, countEqPairs, (void* )&storage);
    pthread_create(&swap_thread1, NULL, swapNodes, (void* )&storage);
    pthread_create(&swap_thread2, NULL, swapNodes, (void* )&storage);
    pthread_create(&swap_thread3, NULL, swapNodes, (void* )&storage);
    pthread_create(&monitor_thread, NULL, monitor, (void* )&storage);

    pthread_join(inc_thread, NULL);
    pthread_join(dec_thread, NULL);
    pthread_join(eq_thread, NULL);
    pthread_join(swap_thread1, NULL);
    pthread_join(swap_thread2, NULL);
    pthread_join(swap_thread3, NULL);
    pthread_join(monitor_thread, NULL);

    destroyStorage(&storage);

    return 0;
}

