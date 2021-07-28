#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/hashtable.h"

table* createHashTable(size_t size) {
    if (size > 31) {
        printf("Error: size limited!\n");
        return NULL;
    }
    table* t = (table*)malloc(sizeof(table));
    size_t i = 0;
    t->bucketcount = 1 << size;
    t->bucket = (entry**)malloc(sizeof(entry*) * t->bucketcount);
    for (i = 0; i < t->bucketcount; ++i) {
        t->bucket[i] = NULL;
    }
    return t;
}

entry* createEntry(const void* key , const void* value, int key_type, int value_type) {
    entry* eptr = (entry*)malloc(sizeof(entry));

    eptr->key_type = key_type;
    eptr->value_type = value_type;

    if (TYPE_CHARARR == key_type) eptr->key = (void*)strdup((char*)key);
    else if (TYPE_INT == key_type) {
        eptr->key = malloc(sizeof(int));
        *(int*)(eptr->key) = *(int*)key;
    } 
    else if (TYPE_LUINT == key_type){
        eptr->key = malloc(sizeof(size_t));
        *(size_t*)(eptr->key) = *(size_t*)key;
    }


    if (TYPE_CHARARR == value_type) eptr->value = (void*)strdup((char*)value);
    else if (TYPE_INT == value_type) {
        eptr->value = malloc(sizeof(int));
        *(int*)(eptr->value) = *(int*)value;
    } 
    else if (TYPE_LUINT == value_type){
        eptr->value = malloc(sizeof(size_t));
        *(size_t*)(eptr->value) = *(size_t*)value;
    }
    return eptr;
}

entry* createPointerEntry(const void* key , void* value, int key_type) {
    entry* eptr = (entry*)malloc(sizeof(entry));

    eptr->key_type = key_type;
    eptr->value_type = TYPE_POINTER;

    if (TYPE_CHARARR == key_type) eptr->key = (void*)strdup((char*)key);
    else if (TYPE_INT == key_type) {
        eptr->key = malloc(sizeof(int));
        *(int*)(eptr->key) = *(int*)key;
    } 
    else if (TYPE_LUINT == key_type){
        eptr->key = malloc(sizeof(size_t));
        *(size_t*)(eptr->key) = *(size_t*)key;
    }
    eptr->value = value;
    return eptr;
}

void freeHashTable(table* t) {
    ssize_t i;
    entry *eptr,*ep;
    if (t == NULL) return;
    for (i = 0; i < t->bucketcount; ++i) {
        eptr = t->bucket[i];
        while (eptr != NULL) {
            ep = eptr;
            eptr = eptr->next;
            free(ep->key);
            free(ep->value);
            free(ep);
        }
    }
    free(t->bucket);
    free(t);
    t = NULL;
}

size_t hashFunction(table* t, const void* key) {
    size_t index = (size_t)((char*)key)[0];
    size_t i, len = strlen(((char*)key));
    for (i = 1; i < len; i++) {
        index <<= 2;
        index += (size_t)((char*)key)[i];
    }
    index &= (t->bucketcount - 1);
    return index;
}

int confirmKey(const void* k1 , const void* k2, int t1, int t2) {
    if (t1 == t2) {
        if (TYPE_CHARARR == t1) {
            if (strcmp((char*)k1 , (char*)k2) == 0) return 1;
        }
        else if (TYPE_INT == t1){
            if (*(int*)k1 == *(int*)k2) return 1;
        }
        else if (TYPE_LUINT == t1){
            if (*(size_t*)k1 == *(size_t*)k2) return 1;
        }
    }
    return 0;
}

int addValue(void* v1 , const void* v2, int t1, int t2) {
    if (t1 == t2) {
        if (TYPE_CHARARR == t2) v1 = (void*)strcat((char*)v1, (char*)v2);
        else if (TYPE_INT == t2) *(int*)v1 += *(int*)v2;
        else if (TYPE_LUINT == t2) *(size_t*)v1 += *(size_t*)v2;
    }
    else {
        free(v1);
        if (TYPE_CHARARR == t2) v1 = (void*)strdup((char*)v2);
        else if (TYPE_INT == t2) *(int*)v1 = *(int*)v2;
        else if (TYPE_LUINT == t2) *(size_t*)v1 = *(size_t*)v2;
    }
    return 0;
}

int addEntryE(table* t, const entry* eptr) {
    return addEntry(t, eptr->key, eptr->value, eptr->key_type, eptr->value_type);
}

int addEntry(table* t, const void* key , const void* value, int key_type, int value_type) {
    if (t == NULL || key == NULL || value == NULL) {
        printf("Error: table or key or value is NULL!");
        return -1;
    }

    size_t index = hashFunction(t, key);
    /***************************首次散列到此bucket***********************/
    if (t->bucket[index] == NULL) 
        t->bucket[index] = createEntry(key, value, key_type, value_type);
    /*******************************进行遍历****************************/
    else {
        entry *eptr, *leptr;
        eptr = t->bucket[index];
        while (eptr != NULL) {
            if (confirmKey(eptr->key , key, eptr->key_type, key_type)) {
                addValue(eptr->value, value, eptr->value_type, value_type);
                return index; //成功匹配
            }
            leptr = eptr;
            eptr = eptr->next;
        }
        /**没有在当前桶中找到 创建新条目加入**/
        leptr->next = createEntry(key, value, key_type, value_type);
    }
    return index;
}

int addPointerEntry(table* t, const void* key , void* value, int key_type) {
    if (t == NULL || key == NULL || value == NULL) {
        printf("Error: table or key or value is NULL!");
        return -1;
    }

    size_t index = hashFunction(t, key);
    /***************************首次散列到此bucket***********************/
    if (t->bucket[index] == NULL) 
        t->bucket[index] = createPointerEntry(key, value, key_type);
    /*******************************进行遍历****************************/
    else {
        entry *eptr, *leptr;
        eptr = t->bucket[index];
        while (eptr != NULL) {
            if (confirmKey(eptr->key , key, eptr->key_type, key_type)) {
                free(eptr->value);
                eptr->value = value;
                return index; //成功匹配
            }
            leptr = eptr;
            eptr = eptr->next;
        }
        /**没有在当前桶中找到 创建新条目加入**/
        leptr->next = createPointerEntry(key, value, key_type);
    }
    return index;
}

entry* getEntry(table* t, const void* key, int key_type) {
    if (t == NULL || key == NULL) {
        printf("Error: table or key is NULL!");
        return NULL;
    }

    size_t index = hashFunction(t, key);
    /***************************首次散列到此bucket***********************/
    if (t->bucket[index] == NULL) 
        return NULL;
    /*******************************进行遍历****************************/
    else {
        entry *eptr;
        eptr = t->bucket[index];
        while (eptr != NULL) {
            if (confirmKey(eptr->key , key, eptr->key_type, key_type)) {
                return eptr; //成功匹配
            }
            eptr = eptr->next;
        }
    }
    return NULL;
}

void printEntrys(table* t) {
    size_t i;
    entry* eptr;
    if (t == NULL)return;
    for (i = 0; i < t->bucketcount; ++i) {
        eptr = t->bucket[i];
        while (eptr != NULL) {
            printEntry(eptr);
            printf("\n");
            eptr = eptr->next;
        }
    }
    printf("\n");
}

void printTable(table* t) {
    size_t i;
    entry* eptr;
    if (t == NULL)return;
    for (i = 0; i < t->bucketcount; ++i) {
        eptr = t->bucket[i];
        if (NULL != eptr) printf("\nbucket[%lu]:\n" , i);
        while (eptr != NULL) {
            // printf("index:%lu\n", i);
            printEntry(eptr);
            eptr = eptr->next;
        }
    }
    printf("\n");
}

void printEnValues(table* t) {
    size_t i;
    entry* eptr;
    if (t == NULL)return;
    for (i = 0; i < t->bucketcount; ++i) {
        eptr = t->bucket[i];
        while (eptr != NULL) {
            // printf("index:%lu\n", i);
            printEntryValue(eptr);
            eptr = eptr->next;
            printf("  ");
        }
    }
    printf("\n");
}

void printEntry(entry* eptr) {
    if (NULL == eptr) return;

    if (TYPE_CHARARR == eptr->key_type) printf("<key: %s ", (char*)(eptr->key));
    else if (TYPE_INT == eptr->key_type) printf("<key: %d ", *(int*)(eptr->key));
    else if (TYPE_LUINT == eptr->key_type) printf("<key: %lu ", *(size_t*)(eptr->key));

    if (TYPE_CHARARR == eptr->value_type) printf(", value: %s> ", (char*)(eptr->value));
    else if (TYPE_INT == eptr->value_type) printf(", value: %d> ", *(int*)(eptr->value));
    else if (TYPE_LUINT == eptr->value_type) printf(", value: %lu> ", *(size_t*)(eptr->value));
    else if (TYPE_POINTER == eptr->value_type) printf(", value: point> ");
}

void printEntryValue(entry* eptr) {
    if (NULL == eptr) return;

    if (TYPE_CHARARR == eptr->key_type) printf("<%s ", (char*)(eptr->key));
    else if (TYPE_INT == eptr->key_type) printf("<%d ", *(int*)(eptr->key));
    else if (TYPE_LUINT == eptr->key_type) printf("<%lu ", *(size_t*)(eptr->key));

    if (TYPE_CHARARR == eptr->value_type) printf("%s>", (char*)(eptr->value));
    else if (TYPE_INT == eptr->value_type) printf("%d>", *(int*)(eptr->value));
    else if (TYPE_LUINT == eptr->value_type) printf("%lu>", *(size_t*)(eptr->value));
    else if (TYPE_POINTER == eptr->value_type) printf("point>");
}

