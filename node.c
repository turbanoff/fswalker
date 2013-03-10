#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>

#include "node.h"

struct list_node {
    arr_elem* elem;
    struct list_node* next;
};
typedef struct list_node list_node;
typedef struct dirent dirent;

arr_elem* list_dir_elem(arr_elem* parent, size_t* dir_size) {
    DIR* dir = opendir(parent->filename);
    if (dir == NULL) {
        *dir_size = 0;
        return NULL;
    }
    
    list_node first;
    first.elem = NULL;
    first.next = NULL;
    
    size_t arr_size = 0;
    list_node *tail = &first;
    
    dirent* ent;
    while ( (ent = readdir(dir)) != NULL ) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        //создаем элемент массива
        arr_elem *next_elem = malloc(sizeof(arr_elem));
        next_elem->is_dir = 1;
        next_elem->size = 0;
        strncpy(next_elem->filename, ent->d_name, 255);
        
        //добавляем на време в связный список
        list_node *next_list_node = malloc(sizeof(list_node));
        next_list_node->elem = next_elem;
        next_list_node->next = NULL;
        tail->next = next_list_node;
        tail = tail->next;
        arr_size++;
    }
    
    //преобразуем связанный список в массив
    arr_elem* result = malloc(arr_size * sizeof(arr_elem));
    arr_elem* result_part = result;
    tail = first.next;
    for (size_t i=0; i<arr_size; i++) {
        memcpy(result_part, tail->elem, sizeof(arr_elem));
        result_part++;
        list_node * for_free = tail;
        tail = tail->next;
        free(for_free);
    }
    *dir_size = arr_size;
    return result;
}