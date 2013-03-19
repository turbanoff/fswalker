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

arr_elem * list_dir_elem(char *parent_full_path, arr_elem *parent, size_t *dir_size, unsigned long long *size_in_bytes) {
    //printf("list_dir_elem %s\n", parent_full_path);
    if (strcmp(parent_full_path, "/proc") == 0) {
        *dir_size = 0;
        *size_in_bytes = 0;
        return NULL;
    }
    DIR *dir = opendir(parent_full_path);
    if (dir == NULL) {
        *dir_size = 0;
        *size_in_bytes = 0;
        return NULL;
    }
    
    list_node first = {.elem = NULL, .next = NULL};
    
    size_t arr_size = 0;
    list_node *tail = &first;
    
    unsigned long long summary_dir_size = 0;
    
    size_t parent_path_size = strlen(parent_full_path);
    size_t alloc_size = parent_path_size + 8 + 1;
    char *full_name = malloc(sizeof(char) * alloc_size);
    strcpy(full_name, parent_full_path);
    if (parent_full_path[parent_path_size - 1] != '/') {
        full_name[parent_path_size] = '/';
        parent_path_size++;
    }
    
    struct dirent *ent;
    while ( (ent = readdir(dir)) != NULL ) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        //создаем элемент массива
        arr_elem *next_elem = malloc(sizeof(arr_elem));
        size_t next_file_len = strlen(ent->d_name);
        next_elem->filename = malloc(sizeof(char) * next_file_len + 1);
        strcpy(next_elem->filename, ent->d_name);
        //создать полный путь, чтобы проверить папка, или нет
        if (parent_path_size + next_file_len + 1 > alloc_size) { //1 - под завершающий ноль
            alloc_size = parent_path_size + next_file_len + 1;
            char* old_full_name = full_name;
            full_name = malloc(alloc_size * sizeof(char));
            strncpy(full_name, old_full_name, parent_path_size);
            free(old_full_name);
        }

        strcpy(full_name + parent_path_size, next_elem->filename);
        struct stat buf;
        lstat(full_name, &buf);
        if (S_ISDIR(buf.st_mode)) {
            next_elem->is_dir = 1;
            next_elem->size = 0;
        } else if (S_ISREG(buf.st_mode)) {
            next_elem->is_dir = 0;
            next_elem->size = buf.st_size;
            summary_dir_size += buf.st_size;
        } else {
            next_elem->is_dir = 0;
            next_elem->size = 0;
        }
        printf("is_dir=%d size=%9llu %s\n", next_elem->is_dir, next_elem->size, full_name);
        
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
    closedir(dir);
    return result;
}
