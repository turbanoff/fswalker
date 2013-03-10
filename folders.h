#ifndef FOLDERS_H
#define	FOLDERS_H

#include <dirent.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    DIR* head;
} directory_head;

typedef struct {
   char name[256];
   int is_dir;
} directory_entry;

directory_head* open_folder(const char* folder_name) {
    DIR* dir = opendir(folder_name);
    if (dir == NULL)
        return NULL;
    directory_head* p = malloc(sizeof(directory_head));
    p->head = dir;
    return p;
}

const char* parent_ref = "..";
const char* self_ref = ".";

directory_entry* read_folder(directory_head* folder) {
    struct dirent* next_file;
    do {
        next_file = readdir(folder->head);
        if (next_file == NULL) {
            closedir(folder->head);
            return NULL;
        }
    } while (strcmp(next_file->d_name, parent_ref) == 0 || strcmp(next_file->d_name, self_ref) == 0);

    directory_entry* p = malloc(sizeof(directory_entry));
    strncpy(p->name, next_file->d_name, 256);
    p->is_dir = 0;
    return p;
}

#endif	/* FOLDERS_H */

