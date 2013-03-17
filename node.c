#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "node.h"

struct list_node {
    arr_elem *elem;
    struct list_node *next;
};
typedef struct list_node list_node;
typedef struct dirent dirent;

arr_elem * list_dir_elem(char *parent_full_path, arr_elem *parent, size_t *dir_size, size_t *size_in_bytes) {
    DIR *dir = opendir(parent_full_path);
    if (dir == NULL) {
        *dir_size = 0;
        return NULL;
    }
    
    list_node first;
    first.elem = NULL;
    first.next = NULL;
    
    size_t arr_size = 0;
    list_node *tail = &first;
    
    size_t summary_dir_size = 0;
    
    size_t parent_path_size = strlen(parent_full_path);
    size_t alloc_size = parent_path_size + 8 + 1;
    char *full_name = malloc(sizeof(char) * alloc_size);
    strcpy(full_name, parent_full_path);
    
    int ends_with_slash = (parent_full_path[parent_path_size - 1] == '/');
    
    dirent *ent;
    while ( (ent = readdir(dir)) != NULL ) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        //создаем элемент массива
        arr_elem *next_elem = malloc(sizeof(arr_elem));
        strcpy(next_elem->filename, ent->d_name);
        //создать полный путь - проверить папка, или нет
        size_t next_file_len = strlen(next_elem->filename);
        if (parent_path_size + next_file_len + 1 + ends_with_slash > alloc_size) { //2 - под слеш и завершающий ноль
            alloc_size = parent_path_size + next_file_len + 1 + ends_with_slash;
            free(full_name);
            full_name = malloc(alloc_size * sizeof(char));
            strcpy(full_name, parent_full_path);
        }
        if (!ends_with_slash)
            full_name[parent_path_size] = '/';

        strcpy(full_name + parent_path_size + !ends_with_slash, next_elem->filename);
        struct stat buf;
        lstat(full_name, &buf);
        if (S_ISDIR(buf.st_mode)) {
            next_elem->is_dir = 1;
            next_elem->size = 0;
        } else if (S_ISREG(buf.st_mode)) {
            next_elem->is_dir = 0;
            next_elem->size = buf.st_size;
            summary_dir_size += next_elem->size;
        } else {
            next_elem->is_dir = 0;
            next_elem->size = 0;
        }
        printf("is_dir=%d size=%9zd %s\n", next_elem->is_dir, next_elem->size, full_name);
        
        //добавляем на время в связный список
        list_node *next_list_node = malloc(sizeof(list_node));
        next_list_node->elem = next_elem;
        next_list_node->next = NULL;
        tail->next = next_list_node;
        tail = tail->next;
        arr_size++;
    }
    
    //преобразуем связанный список в массив
    arr_elem *result = malloc(arr_size * sizeof(arr_elem));
    arr_elem *result_part = result;
    tail = first.next;
    for (size_t i=0; i<arr_size; i++) {
        memcpy(result_part, tail->elem, sizeof(arr_elem));
        result_part++;
        list_node *for_free = tail;
        tail = tail->next;
        free(for_free);
    }
    *dir_size = arr_size;
    *size_in_bytes = summary_dir_size;
    free(full_name);
    return result;
}