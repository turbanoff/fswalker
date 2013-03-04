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

struct stack_entry {
    struct stack_entry* next;
    char filename[256];
    int is_dir;
    struct stack_entry* first_child;
};

typedef struct stack_entry stack_entry;

void add(stack_entry* tail, directory_entry* next_entry) {
    tail->next = malloc(sizeof(stack_entry));
    tail = tail->next;
    strncpy(tail->filename, next_entry->name, 256);
    tail->is_dir = next_entry->is_dir;
    tail->next = NULL;
    tail->first_child = NULL;
}

void show(stack_entry* first) {
    while (first != NULL) {
        printf("%s\n", first->filename);
        first = first->next;
    }
}

int main(int argc, char** argv) {
    stack_entry head;
    head.filename[0] = '\0';
    head.next = NULL;
    
    stack_entry* tail = &head;
    
    directory_head* next_folder = open_folder("/");
    directory_entry* next_entry;
    while ((next_entry = read_folder(next_folder)) != NULL) {
        add(tail, next_entry);
        tail = tail->next;
        free(directory_entry);
    }
    
    show(head.next);
    
    printf("----------------------------");
    return (EXIT_SUCCESS);
}

