/* 
 * File:   main.c
 * Author: turbanoff
 *
 * Created on March 4, 2013, 10:26 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "folders.h"

struct dir_entry {
    char filename[256];
    int is_dir;
    struct stack_entry* first_child;
};
typedef struct dir_entry dir_entry;

struct stack_entry {
    struct stack_entry* next;
    struct dir_entry dir;
};
typedef struct stack_entry stack_entry;

void add(stack_entry* tail, directory_entry* next_entry) {
    tail->next = malloc(sizeof(stack_entry));
    tail = tail->next;
    strncpy(tail->dir.filename, next_entry->name, 256);
    tail->dir.is_dir = next_entry->is_dir;
    tail->next = NULL;
}

void show_linked(stack_entry* first) {
    while (first != NULL) {
        printf("%s\n", first->dir.filename);
        first = first->next;
    }
}

void show_arr(dir_entry* arr, size_t size) {
    for (int i=0; i<size; i++) {
        printf("%s\n", arr[i].filename);
    }
}

int main(int argc, char** argv) {
    stack_entry head;
    head.dir.filename[0] = '\0';
    head.next = NULL;
    
    stack_entry* tail = &head;
    
    directory_head* next_folder = open_folder("/");
    directory_entry* next_entry;
    size_t dir_size = 0;
    while ((next_entry = read_folder(next_folder)) != NULL) {
        add(tail, next_entry);
        dir_size++;
        tail = tail->next;
    }
    
    dir_entry *arr = malloc(sizeof(dir_entry) * dir_size);
    stack_entry *iter = head.next;
    int dir_num = 0;
    while (iter != NULL) {
        arr[dir_num] = iter->dir;
        iter = iter->next;
        dir_num++;
    }
    
    show_linked(head.next);
    printf("----------------------------");
    show_arr(arr, dir_size);
    
    printf("----------------------------");
    return (EXIT_SUCCESS);
}

